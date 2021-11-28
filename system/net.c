#include <assert.h>

#include "system/net.h"
#include "system/gpio.h"
#include "system/dbgio.h"

#include "osal/osal.h"

// TODO! Может быть все необходимые инклады перенести в system/usb_ecm?
#include "usb/usbd_def.h"
#include "usb/class/cdc_ecm/usbd_cdc_ecm_if.h"

#include "lwip/tcpip.h"
#include "netif/etharp.h"


struct netif main_netif;
extern USBD_HandleTypeDef USBD_Device; // extern не круто
static osal_thread_t thread = NULL;


static struct pbuf *ll_input(__attribute__((unused)) struct netif *pnetif)
{
    USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef *)USBD_Device.pClassData;
    struct pbuf *p = NULL;

    if ((hcdc->RxLength > 0) && (hcdc->RxState == 1)) {
        p = pbuf_alloc(PBUF_RAW, (uint16_t)hcdc->RxLength, PBUF_POOL);
    }

    if (p != NULL) {
        memcpy(p->payload, (uint8_t *)hcdc->RxBuffer, (uint16_t)hcdc->RxLength);
        p->len = (uint16_t)hcdc->RxLength;
    }

    return p;
}


static err_t ll_output_cb(__attribute__((unused)) struct netif *netif, struct pbuf *p)
{
    USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef *)USBD_Device.pClassData;
    uint8_t *pdata = hcdc->TxBuffer;
    uint32_t try = 1000000;

    /* Check if the TX State is not busy */
    while ((hcdc->TxState != 0) && (try > 0)) {
        try--;
    }

    /* If no success getting the TX state ready, return error */
    if (try == 0) {
        return ERR_USE;
    }

    hcdc->TxLength = 0;

    for (struct pbuf *curr = p; curr != NULL; curr = curr->next) {
        memcpy(pdata, curr->payload, curr->len);
        pdata += curr->len;
        hcdc->TxLength += curr->len;
    }

    USBD_CDC_ECM_SetTxBuffer(&USBD_Device, hcdc->TxBuffer, (uint16_t)hcdc->TxLength);

    if (USBD_CDC_ECM_TransmitPacket(&USBD_Device)) {
        return ERR_USE;
    }

    return ERR_OK;
}


static err_t netif_out_cb(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    return etharp_output(netif, q, ipaddr);
}


static err_t netif_init_cb(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    strcpy(netif->name, NETIF_NAME);

    netif->hwaddr_len = ETH_HWADDR_LEN;

    netif->hwaddr[0] = MAC_ADDR0;
    netif->hwaddr[1] = MAC_ADDR1;
    netif->hwaddr[2] = MAC_ADDR2;
    netif->hwaddr[3] = MAC_ADDR3;
    netif->hwaddr[4] = MAC_ADDR4;
    netif->hwaddr[5] = MAC_ADDR5;

    netif->mtu = 1500;

    netif->output = netif_out_cb; // можно использовать обёртку или непосредственно передать etharp_output()
    netif->linkoutput = ll_output_cb;

    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    return (err_t)ERR_OK;
}


static void main_netif_setup(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    netif_add(&main_netif, &ipaddr, &netmask, &gw, NULL, &netif_init_cb, &tcpip_input);
    netif_set_default(&main_netif);
    netif_set_up(&main_netif);
    netif_set_link_up(&main_netif);

    CONSOLE_LOG("Host static IP: %s",
        ip4addr_ntoa((const ip4_addr_t *)&main_netif.ip_addr));
}


static void net_event_loop(void const *arg)
{
    err_t err;
    struct pbuf *p;
    struct netif *pnetif = (struct netif *)arg;
    USBD_CDC_ECM_HandleTypeDef *hcdc = NULL;

    CONSOLE_LOG("Net event loop running");

    while(true) {
        uint32_t flags = osal_thread_notify_wait(NET_EVENT_ANY, OSAL_MAX_TIMEOUT);

        if (flags & NET_EVENT_LINK_UP) {
            netif_set_link_up(&main_netif);
            gpio_set(GPIO_GREEN_LED);
            hcdc = USBD_Device.pClassData;
        }

        if (flags & NET_EVENT_LINK_DOWN) {
            netif_set_link_down(&main_netif);
            gpio_reset(GPIO_GREEN_LED);
            hcdc = NULL;
        }

        if (flags & NET_EVENT_NEW_PACKET) {
            LOCK_TCPIP_CORE();

            p = ll_input(pnetif);
            if (p == NULL) {
                CONSOLE_ERROR("Can't read packet");
                goto proceed;
            }

            err = pnetif->input(p, pnetif);
            if (err) {
                CONSOLE_ERROR("lwip input error");
                pbuf_free(p);
                p = NULL;
            }

            UNLOCK_TCPIP_CORE();

        proceed:
            hcdc->RxLength = 0;
            hcdc->RxState = 0;

            USBD_LL_PrepareReceive(&USBD_Device, CDC_ECM_OUT_EP,
                                   (uint8_t *)hcdc->RxBuffer, hcdc->MaxPcktLen);
        }
    }
}


static void net_init_done(void *arg)
{
    CONSOLE_LOG("lwip stack init done");
}


void net_init(void)
{
    gpio_init(GPIO_ORANGE_LED);
    gpio_init(GPIO_GREEN_LED);

    // lwip stack init
    tcpip_init(net_init_done, NULL);

    main_netif_setup();

    thread = osal_thread_create("lwip_input",
                                net_event_loop,
                                1024,
                                THREAD_PRIORITY_NORMAL,
                                &main_netif);
    if (thread == NULL) {
        CONSOLE_ERROR("Can't create lwip_input thread");
        assert(0);
    }
}


void net_notify(NET_EVENT_FLAG_T event)
{
    osal_thread_notify(thread, event);
}

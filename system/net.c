#include <assert.h>

#include "lwip/tcpip.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"

#include "system/gpio.h"
#include "system/dbgio.h"
#include "osal/osal.h"

// TODO! Может быть все необходимые инклады перенести в system/usb_ecm?
#include "usb/usbd_def.h"
#include "usb/class/cdc_ecm/usbd_cdc_ecm_if.h"


struct netif main_netif;
extern USBD_HandleTypeDef USBD_Device; // extern не круто

static osal_semaphore_t sem;


static void low_level_init(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* Set MAC hardware address */
    netif->hwaddr[0] =  CDC_ECM_MAC_ADDR0;
    netif->hwaddr[1] =  CDC_ECM_MAC_ADDR1;
    netif->hwaddr[2] =  CDC_ECM_MAC_ADDR2;
    netif->hwaddr[3] =  CDC_ECM_MAC_ADDR3;
    netif->hwaddr[4] =  CDC_ECM_MAC_ADDR4;
    netif->hwaddr[5] =  CDC_ECM_MAC_ADDR5;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
}


static struct pbuf *low_level_input(struct netif *pnetif)
{
    struct pbuf *p = NULL;

    USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef*) (USBD_Device.pClassData);
    uint32_t temp = hcdc->RxState;

    UNUSED(pnetif);

    /* Get the length of the current buffer */
    if ((hcdc->RxLength > 0U) && (temp == 1U))
    {
        /* Allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, (uint16_t)hcdc->RxLength, PBUF_POOL);
    }

    /* Check that allocation was done correctly */
    if (p != NULL)
    {
        /* Copy the buffer data in the allocated buffer */
        (void)memcpy(p->payload, (uint8_t *)hcdc->RxBuffer, (uint16_t)hcdc->RxLength);
        p->len = (uint16_t)hcdc->RxLength;
    }

    return p;
}


static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    uint8_t *pdata;
    uint32_t Trials = CDC_ECM_MAX_TX_WAIT_TRIALS;
    USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef*) (USBD_Device.pClassData);

    UNUSED(netif);

    CONSOLE_LOG("Packet out");

    /* Check if the TX State is not busy */
    while ((hcdc->TxState != 0U) && (Trials > 0U))
    {
        Trials--;
    }

    /* If no success getting the TX state ready, return error */
    if (Trials == 0U)
    {
        return (err_t)ERR_USE;
    }

    pdata = hcdc->TxBuffer;
    hcdc->TxLength = 0;

    for(q = p; q != NULL; q = q->next)
    {
        (void)memcpy(pdata, q->payload, q->len);
        pdata += q->len;
        hcdc->TxLength += q->len;
    }

    /* Set the TX buffer information */
    (void)USBD_CDC_ECM_SetTxBuffer(&USBD_Device, hcdc->TxBuffer, (uint16_t)hcdc->TxLength);

    /* Start transmitting the Tx buffer */
    if(USBD_CDC_ECM_TransmitPacket(&USBD_Device) == (uint8_t)USBD_OK)
    {
        return (err_t)ERR_OK;
    }

    return (err_t)ERR_USE;
}


static void process_packet(void const *arg)
{
    err_t err;
    struct pbuf *p;
    struct netif *pnetif = (struct netif *)arg;

    CONSOLE_LOG("lwip thread running");

    while(true) {
        osal_semaphore_wait(sem, OSAL_MAX_TIMEOUT);

        // TODO Тред создаётся раньше, чем инициализируется USBD_Device.pClassData, по этому нельзя один раз
        //      запомнить адрес и использовать его. Нужно подумать, нормально ли это
        USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef*) (USBD_Device.pClassData);

        /* move received packet into a new pbuf */
        p = low_level_input(pnetif);

        /* no packet could be read, silently ignore this */
        if (p == NULL)
        {
            CONSOLE_LOG("Can't read packet");
            goto proceed;
        }

        /* entry point to the LwIP stack */
        err = pnetif->input(p, pnetif);

        if (err != (err_t)ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("IP input error\n"));
            (void)pbuf_free(p);
            p = NULL;
        }

proceed:
        /* Reset the Received buffer length to zero for next transfer */
        hcdc->RxLength = 0U;
        hcdc->RxState = 0U;

        /* Prepare Out endpoint to receive next packet in current/new frame */
        (void)USBD_LL_PrepareReceive(&USBD_Device, CDC_ECM_OUT_EP,
                                     (uint8_t*)(hcdc->RxBuffer), hcdc->MaxPcktLen);

        /* Free the allocated buffer :
            The allocated buffer is freed by low layer ethernet functions.
        */
    }
}


static void check_timeouts(const void *args)
{
    while (true) {
        sys_check_timeouts();
        osal_thread_sleep(10);
    }
}


static err_t out(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    CONSOLE_LOG("Outgoing packet");
    return etharp_output(netif, q, ipaddr);
}


err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

    CONSOLE_LOG("Net iface init");

    gpio_init(GPIO_ORANGE_LED);
    gpio_init(GPIO_GREEN_LED);

    netif->name[0] = 's';
    netif->name[1] = 't';

    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
    netif->output = out;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return (err_t)ERR_OK;
}


static void net_iface_setup(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    
    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    
    netif_add(&main_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    
    /*  Registers the default network interface. */
    netif_set_default(&main_netif);

    netif_set_up(&main_netif);
    netif_set_link_up(&main_netif);
    
    // if (netif_is_link_up(&main_netif))
    // {
    //     /* When the netif is fully configured this function must be called.*/
    //     netif_set_up(&main_netif);
    // }
    // else
    // {
    //     /* When the netif link is down this function must be called */
    //     netif_set_down(&main_netif);
    // }
}


static void net_init_done(void *arg)
{
    CONSOLE_LOG("lwip stack init done");
}


void net_init(void)
{
    sem = osal_semaphore_create();
    if (sem == NULL) {
        CONSOLE_LOG("Can't create semaphore");
        assert(0);
    }

    #if 0
    if (osal_thread_create("lwip", process_packet, 1024, THREAD_PRIORITY_NORMAL, &main_netif) == NULL) {
        CONSOLE_ERROR("Can't create lwip thread");
        assert(0);
    }

    if (osal_thread_create("lwip timeouts", check_timeouts, 256, THREAD_PRIORITY_NORMAL, NULL) == NULL) {
        CONSOLE_ERROR("Can't create lwip timeouts thread");
        assert(0);
    }
    #endif

    tcpip_init(net_init_done, NULL);
    net_iface_setup();
    CONSOLE_LOG("Host static IP: %s", ip4addr_ntoa((const ip4_addr_t *)&main_netif.ip_addr));
}


void ethernetif_notify_conn_changed(struct netif *netif)
{
    /* NOTE : This is function clould be implemented in user file
                when the callback is needed,
    */
    UNUSED(netif);
}


void ethernetif_update_config(struct netif *netif)
{
    CONSOLE_LOG("Update net iface callback");

    if(netif_is_link_up(netif) == 1U)
    {
        netif_set_up(netif);
        gpio_set(GPIO_GREEN_LED);
    }
    else
    {
        gpio_reset(GPIO_GREEN_LED);
    }

    ethernetif_notify_conn_changed(netif);
}


void ethernetif_ready_cb(void)
{
    osal_semaphore_release(sem);
}


// u32_t sys_now(void)
// {
//   return HAL_GetTick();
// }

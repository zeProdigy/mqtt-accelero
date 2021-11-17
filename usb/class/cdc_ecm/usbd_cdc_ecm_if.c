#include "usbd_cdc_ecm_if.h"
#include "system/net.h"


#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN static uint8_t UserRxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END; /* Received Data over USB are stored in this buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN  static uint8_t UserTxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END; /* Received Data over CDC_ECM (CDC_ECM interface) are stored in this buffer */

// TODO! избавиться от extern
extern USBD_HandleTypeDef USBD_Device;
extern struct netif main_netif;


static int8_t CDC_ECM_Itf_Init(void)
{
    USBD_UsrLog("CDC ECM Init");

    USBD_CDC_ECM_SetTxBuffer(&USBD_Device, UserTxBuffer, 0U);
    USBD_CDC_ECM_SetRxBuffer(&USBD_Device, UserRxBuffer);

    net_notify(NET_EVENT_LINK_UP);

    return 0;
}


static int8_t CDC_ECM_Itf_DeInit(void)
{
    USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef *)USBD_Device.pClassData;

    USBD_UsrLog("CDC ECM Deinit");

    hcdc->LinkStatus = 0U;

    net_notify(NET_EVENT_LINK_DOWN);

    return 0;
}



static int8_t CDC_ECM_Itf_Control(uint8_t cmd,
                                  __attribute__((unused)) uint8_t *pbuf,
                                  __attribute__((unused)) uint16_t length)
{
  USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef *)USBD_Device.pClassData;

  USBD_UsrLog("CDC ECM Control cmd = 0x%x", cmd);

  switch(cmd) {
        case CDC_ECM_SEND_ENCAPSULATED_COMMAND:
        break;

        case CDC_ECM_GET_ENCAPSULATED_RESPONSE:
        break;

        case CDC_ECM_SET_ETH_MULTICAST_FILTERS:
        break;

        case CDC_ECM_SET_ETH_PWRM_PATTERN_FILTER:
            break;

        case CDC_ECM_GET_ETH_PWRM_PATTERN_FILTER:
            break;

        // TODO! Не очень понятно, почему по этой команде есть некая бизнес логика
        case CDC_ECM_SET_ETH_PACKET_FILTER:
            /* Check if this is the first time we enter */
            if (hcdc->LinkStatus == 0) {
                hcdc->LinkStatus = 1U;

                if (hcdc->NotificationStatus == 0) {
                    /* Send notification: NETWORK_CONNECTION Event */
                    (void)USBD_CDC_ECM_SendNotification(&USBD_Device, NETWORK_CONNECTION,
                                                        CDC_ECM_NET_CONNECTED, NULL);

                    hcdc->NotificationStatus = 1U;
                }
            }
            break;

        case CDC_ECM_GET_ETH_STATISTIC:
            break;

        default:
            break;
    }

    return 0;
}


static int8_t CDC_ECM_Itf_Receive(__attribute__((unused)) uint8_t *Buf,
                                  __attribute__((unused)) uint32_t *Len)
{
    USBD_CDC_ECM_HandleTypeDef *hcdc = (USBD_CDC_ECM_HandleTypeDef *)USBD_Device.pClassData;

    hcdc->RxState = 1U;

    net_notify(NET_EVENT_NEW_PACKET);

    return 0;
}


static int8_t CDC_ECM_Itf_TransmitCplt(__attribute__((unused)) uint8_t *Buf,
                                       __attribute__((unused)) uint32_t *Len,
                                       __attribute__((unused)) uint8_t epnum)
{
    USBD_UsrLog("CDC ECM Transmit Complete");
    return 0;
}


static int8_t CDC_ECM_Itf_Process(__attribute__((unused)) USBD_HandleTypeDef *pdev)
{
    USBD_UsrLog("CDC ECM Process");
    return 0;
}


USBD_CDC_ECM_ItfTypeDef USBD_CDC_ECM_fops = {
    CDC_ECM_Itf_Init,
    CDC_ECM_Itf_DeInit,
    CDC_ECM_Itf_Control,
    CDC_ECM_Itf_Receive,
    CDC_ECM_Itf_TransmitCplt,
    CDC_ECM_Itf_Process,
    (uint8_t *)CDC_ECM_MAC_STR_DESC,
};

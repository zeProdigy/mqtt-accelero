/**
  ******************************************************************************
  * @file    Src/usbd_cdc_ecm_if_template.c
  * @author  MCD Application Team
  * @brief   Source file for USBD CDC_ECM interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usbd_cdc_ecm_if.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "system/net.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN static uint8_t UserRxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END; /* Received Data over USB are stored in this buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
#endif
__ALIGN_BEGIN  static uint8_t UserTxBuffer[CDC_ECM_ETH_MAX_SEGSZE + 100]__ALIGN_END; /* Received Data over CDC_ECM (CDC_ECM interface) are stored in this buffer */

static uint8_t CDC_ECMInitialized = 0U;

/* USB handler declaration */
extern USBD_HandleTypeDef USBD_Device;

// TODO! избавиться от extern
extern struct netif main_netif;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_ECM_Itf_Init(void);
static int8_t CDC_ECM_Itf_DeInit(void);
static int8_t CDC_ECM_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_ECM_Itf_Receive(uint8_t *pbuf, uint32_t *Len);
static int8_t CDC_ECM_Itf_TransmitCplt(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);
static int8_t CDC_ECM_Itf_Process(USBD_HandleTypeDef *pdev);

USBD_CDC_ECM_ItfTypeDef USBD_CDC_ECM_fops =
{
  CDC_ECM_Itf_Init,
  CDC_ECM_Itf_DeInit,
  CDC_ECM_Itf_Control,
  CDC_ECM_Itf_Receive,
  CDC_ECM_Itf_TransmitCplt,
  CDC_ECM_Itf_Process,
  (uint8_t *)CDC_ECM_MAC_STR_DESC,
};


static void CDC_ECM_NetifConfig(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  CONSOLE_LOG("Configure net iface");

  IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
  IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

  /* add the network interface */
  (void)netif_add(&main_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /*  Registers the default network interface */
  netif_set_default(&main_netif);

  if (netif_is_link_up(&main_netif)== 1U)
  {
    /* When the netif is fully configured this function must be called */
    CONSOLE_LOG("Up net iface");
    netif_set_up(&main_netif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&main_netif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&main_netif, ethernetif_update_config);
}


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_ECM_Itf_Init
  *         Initializes the CDC_ECM media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Init(void)
{
  USBD_UsrLog("CDC ECM Init");

  if (CDC_ECMInitialized == 0U)
  {
    /*
      Initialize the TCP/IP stack here
    */
    // lwip_init();

    // CDC_ECM_NetifConfig();
    
    CDC_ECMInitialized = 1U;
  }

  /* Set Application Buffers */
  (void)USBD_CDC_ECM_SetTxBuffer(&USBD_Device, UserTxBuffer, 0U);
  (void)USBD_CDC_ECM_SetRxBuffer(&USBD_Device, UserRxBuffer);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_DeInit
  *         DeInitializes the CDC_ECM media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_DeInit(void)
{
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(USBD_Device.pClassData);

  USBD_UsrLog("CDC ECM Deinit");

  CONSOLE_LOG("netif set down");
  netif_set_down(&main_netif);

  /* Notify application layer that link is down */
  hcdc_cdc_ecm->LinkStatus = 0U;

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Control
  *         Manage the CDC_ECM class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(USBD_Device.pClassData);

  USBD_UsrLog("CDC ECM Control cmd = 0x%x", cmd);

  switch (cmd)
  {
    case CDC_ECM_SEND_ENCAPSULATED_COMMAND:
      /* Add your code here */
      break;

    case CDC_ECM_GET_ENCAPSULATED_RESPONSE:
      /* Add your code here */
      break;

    case CDC_ECM_SET_ETH_MULTICAST_FILTERS:
      /* Add your code here */
      break;

  case CDC_ECM_SET_ETH_PWRM_PATTERN_FILTER:
      /* Add your code here */
      break;

  case CDC_ECM_GET_ETH_PWRM_PATTERN_FILTER:
      /* Add your code here */
      break;

  case CDC_ECM_SET_ETH_PACKET_FILTER:
      /* Check if this is the first time we enter */
      if (hcdc_cdc_ecm->LinkStatus == 0U)
      {
        /*
          Setup the Link up at TCP/IP level
        */
        CONSOLE_LOG("Set link up");
        // netif_set_link_up(&main_netif);

        hcdc_cdc_ecm->LinkStatus = 1U;

        /* Modification for MacOS which doesn't send SetInterface before receiving INs */
        if (hcdc_cdc_ecm->NotificationStatus == 0U)
        {
          CONSOLE_LOG("Send link up notify");
          
          /* Send notification: NETWORK_CONNECTION Event */
          (void)USBD_CDC_ECM_SendNotification(&USBD_Device, NETWORK_CONNECTION,
                                              CDC_ECM_NET_CONNECTED, NULL);

          /* Prepare for sending Connection Speed Change notification */
          hcdc_cdc_ecm->NotificationStatus = 1U;
        }
      }
      /* Add your code here */
      break;

  case CDC_ECM_GET_ETH_STATISTIC:
      /* Add your code here */
      break;

    default:
      break;
  }
  UNUSED(length);
  UNUSED(pbuf);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Receive
  *         Data received over USB OUT endpoint are sent over CDC_ECM interface
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Receive(uint8_t *Buf, uint32_t *Len)
{
  /* Get the CDC_ECM handler pointer */
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(USBD_Device.pClassData);

  /* Call Eth buffer processing */
  hcdc_cdc_ecm->RxState = 1U;

  ethernetif_ready_cb();

  UNUSED(Len);
  UNUSED(Buf);

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_TransmitCplt
  *         Data transmited callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_TransmitCplt(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  USBD_UsrLog("CDC ECM Transmit Complete");

  return (0);
}

/**
  * @brief  CDC_ECM_Itf_Process
  *         Data received over USB OUT endpoint are sent over CDC_ECM interface
  *         through this function.
  * @param  pdef: pointer to the USB Device Handle
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_ECM_Itf_Process(USBD_HandleTypeDef *pdev)
{
  /* Get the CDC_ECM handler pointer */
  USBD_CDC_ECM_HandleTypeDef *hcdc_cdc_ecm = (USBD_CDC_ECM_HandleTypeDef *)(pdev->pClassData);

  if ((hcdc_cdc_ecm != NULL) && (hcdc_cdc_ecm->LinkStatus != 0U))
  {
    /* Read a received packet from the Ethernet buffers and send it
      to the lwIP for handling */

    /* Handle timeouts */
    sys_check_timeouts();
  }

  return (0);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
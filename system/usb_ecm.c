#include "usb/usbd_desc.h"
#include "usb/usbd_conf.h"
#include "usb/class/cdc_ecm/usbd_cdc_ecm.h"
#include "usb/class/cdc_ecm/usbd_cdc_ecm_if.h"


USBD_HandleTypeDef USBD_Device;


int usb_ecm_init(void)
{
    int ret;

    ret = USBD_Init(&USBD_Device, &CDC_ECM_Desc, 0);
    if (ret) {
        return 1;
    }

    ret = USBD_RegisterClass(&USBD_Device, USBD_CDC_ECM_CLASS);
    if (ret) {
        return 1;
    }

    ret = USBD_CDC_ECM_RegisterInterface(&USBD_Device, &USBD_CDC_ECM_fops);
    if (ret) {
        return 1;
    }

    ret = USBD_Start(&USBD_Device);
    if (ret) {
        return 1;
    }

    return 0;
}


void usb_ecm_process(void)
{
    USBD_CDC_ECM_fops.Process(&USBD_Device);
}


void usb_ecm_irq_handler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

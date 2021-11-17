#pragma once

#include "usbd_cdc_ecm.h"

// TODO! Драйвер usb_cdc_ecm в Linux по этой строке назначает mac addr на интерфейс, он должен быть равен
//       тому, что задано у нас в плате? По логике нет...
#define CDC_ECM_MAC_STR_DESC                 (uint8_t *)"000202020000"

#define CDC_ECM_ETH_STATS_BYTE0                                 0U
/*(uint8_t)(CDC_ECM_XMIT_OK_VAL | CDC_ECM_RVC_OK_VAL | CDC_ECM_XMIT_ERROR_VAL | \
            CDC_ECM_RCV_ERROR_VAL | CDC_ECM_RCV_NO_BUFFER_VAL | CDC_ECM_DIRECTED_BYTES_XMIT_VAL | \
            CDC_ECM_DIRECTED_FRAMES_XMIT_VAL | CDC_ECM_MULTICAST_BYTES_XMIT_VAL) */

#define CDC_ECM_ETH_STATS_BYTE1                                 0U
/*(uint8_t)(CDC_ECM_MULTICAST_FRAMES_XMIT_VAL | CDC_ECM_BROADCAST_BYTES_XMIT_VAL | \
            CDC_ECM_BROADCAST_FRAMES_XMIT_VAL | CDC_ECM_DIRECTED_BYTES_RCV_VAL | \
            CDC_ECM_DIRECTED_FRAMES_RCV_VAL | CDC_ECM_MULTICAST_BYTES_RCV_VAL | \
            CDC_ECM_MULTICAST_FRAMES_RCV_VAL | CDC_ECM_BROADCAST_BYTES_RCV_VAL) */

#define CDC_ECM_ETH_STATS_BYTE2                                 0U
/*(uint8_t)(CDC_ECM_BROADCAST_FRAMES_RCV_VAL | CDC_ECM_RCV_CRC_ERROR_VAL | \
            CDC_ECM_TRANSMIT_QUEUE_LENGTH_VAL | CDC_ECM_RCV_ERROR_ALIGNMENT_VAL | \
            CDC_ECM_XMIT_ONE_COLLISION_VAL | CDC_ECM_XMIT_MORE_COLLISIONS_VAL | \
            CDC_ECM_XMIT_DEFERRED_VAL | CDC_ECM_XMIT_MAX_COLLISIONS_VAL) */

#define CDC_ECM_ETH_STATS_BYTE3                                 0U
/*(uint8_t)(CDC_ECM_RCV_OVERRUN_VAL | CDC_ECM_XMIT_UNDERRUN_VAL | CDC_ECM_XMIT_HEARTBEAT_FAILURE_VAL | \
            CDC_ECM_XMIT_TIMES_CRS_LOST_VAL | CDC_ECM_XMIT_LATE_COLLISIONS_VAL | CDC_ECM_ETH_STATS_RESERVED) */

/* Ethernet Maximum Segment size, typically 1514 bytes */
#define CDC_ECM_ETH_MAX_SEGSZE                                  1514U

/* Number of Ethernet multicast filters */
#define CDC_ECM_ETH_NBR_MACFILTERS                              0U

/* Number of wakeup power filters */
#define CDC_ECM_ETH_NBR_PWRFILTERS                              0U

#define CDC_ECM_CONNECT_SPEED_UPSTREAM                          0x004C4B40U /* 5Mbps */
#define CDC_ECM_CONNECT_SPEED_DOWNSTREAM                        0x004C4B40U /* 5Mbps */


extern USBD_CDC_ECM_ItfTypeDef USBD_CDC_ECM_fops;

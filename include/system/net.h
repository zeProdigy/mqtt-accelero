#pragma once

#include <assert.h>
#include <string.h>

#include "lwip/netif.h"


#define MAC_ADDR0 0x00
#define MAC_ADDR1 0x02
#define MAC_ADDR2 0x02
#define MAC_ADDR3 0x03
#define MAC_ADDR4 0x00
#define MAC_ADDR5 0x00

#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 0
#define IP_ADDR3 10

#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 0
#define GW_ADDR3 1

#define NETIF_NAME  "ak001"

_Static_assert(strlen(NETIF_NAME) + 1 <= NETIF_NAMESIZE, "Too long netif name");


typedef enum {
    NET_EVENT_LINK_UP       = 0x01,
    NET_EVENT_LINK_DOWN     = 0x02,
    NET_EVENT_NEW_PACKET    = 0x04,
} NET_EVENT_FLAG_T;


void net_init(void);
void net_notify(NET_EVENT_FLAG_T event);

#pragma once

#include "lwip/err.h"
#include "lwip/netif.h"

void net_init(void);
err_t ethernetif_init(struct netif *pnetif);
void ethernetif_update_config(struct netif *netif);
void ethernetif_ready_cb(void);
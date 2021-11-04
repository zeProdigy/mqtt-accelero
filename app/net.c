#include "lwip/tcpip.h"
#include "netif/etharp.h"


static struct netif main_netif;


int net_init()
{
    main_netif.name[0] = 's';
    main_netif.name[0] = 't';

    return 0;
}

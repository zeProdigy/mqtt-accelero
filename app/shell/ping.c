#include "system/dbgio.h"

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/inet_chksum.h"
#include "lwip/prot/ip4.h"


#define PING_RESULT(ping_ok)
#define PING_ID        0xAFAF
#define PING_DATA_SIZE 32
#define PING_DELAY     1000
#define PING_DEBUG     LWIP_DBG_ON


static bool is_init = false;
static const ip_addr_t *ping_target;
static struct raw_pcb *ping_pcb;
static u16_t ping_seq_num;

#ifdef LWIP_DEBUG
static u32_t ping_time;
#endif /* LWIP_DEBUG */


static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len)
{
    size_t i;
    size_t data_len = len - sizeof(struct icmp_echo_hdr);

    ICMPH_TYPE_SET(iecho, ICMP_ECHO);
    ICMPH_CODE_SET(iecho, 0);
    iecho->chksum = 0;
    iecho->id     = PING_ID;
    iecho->seqno  = lwip_htons(++ping_seq_num);

    /* fill the additional data buffer with some data */
    for(i = 0; i < data_len; i++) {
        ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
    }

    iecho->chksum = inet_chksum(iecho, len);
}


static void ping_send(struct raw_pcb *raw, const ip_addr_t *addr)
{
    struct pbuf *p;
    struct icmp_echo_hdr *iecho;
    size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

    LWIP_DEBUGF( PING_DEBUG, ("ping: send"));
    ip_addr_debug_print(PING_DEBUG, addr);
    LWIP_ASSERT("ping_size <= 0xffff", ping_size <= 0xffff);

    p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
    if (!p) {
        return;
    }

    if ((p->len == p->tot_len) && (p->next == NULL)) {
        iecho = (struct icmp_echo_hdr *)p->payload;

        ping_prepare_echo(iecho, (u16_t)ping_size);

        raw_sendto(raw, p, addr);
    #ifdef LWIP_DEBUG
        ping_time = sys_now();
    #endif /* LWIP_DEBUG */
    }

    pbuf_free(p);
}


static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
    struct icmp_echo_hdr *iecho;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_ASSERT("p != NULL", p != NULL);

    if ((p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr))) &&
        pbuf_remove_header(p, PBUF_IP_HLEN) == 0) {
            iecho = (struct icmp_echo_hdr *)p->payload;

            if ((iecho->id == PING_ID) && (iecho->seqno == lwip_htons(ping_seq_num))) {
                LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
                ip_addr_debug_print(PING_DEBUG, addr);
                LWIP_DEBUGF( PING_DEBUG, ("time: %"U32_F" ms\n", (sys_now()-ping_time)));

                /* do some ping result processing */
                PING_RESULT(1);
                pbuf_free(p);
                return 1; /* eat the packet */
            }
            /* not eaten, restore original packet */
            pbuf_add_header(p, PBUF_IP_HLEN);
    }

    return 0; /* don't eat the packet */
}


static void ping_send_now(void)
{
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);
    ping_send(ping_pcb, ping_target);
}


static void ping_init(void)
{
    ping_pcb = raw_new(IP_PROTO_ICMP);
    LWIP_ASSERT("ping_pcb != NULL", ping_pcb != NULL);

    raw_recv(ping_pcb, ping_recv, NULL);
    raw_bind(ping_pcb, IP_ADDR_ANY);
}


int ping_test(int argc, char *argv[])
{
    if (!is_init) {
        ping_init();
        is_init = true;
    }

    ip_addr_t ping_addr = IPADDR4_INIT_BYTES(192, 168, 0, 1);   //TODO адрес из аргументов
    ping_target = &ping_addr;

    ping_send_now();

    return 0;
}

#pragma once

// use lwip native API (no OS require)
#define NO_SYS 1


// memory opts
#define MEM_ALIGNMENT           4
#define MEM_SIZE                (10*1024)
#define MEMP_NUM_PBUF           10
#define MEMP_NUM_UDP_PCB        2
#define MEMP_NUM_TCP_PCB        2
#define MEMP_NUM_TCP_PCB_LISTEN 1
#define MEMP_NUM_TCP_SEG        8
#define MEMP_NUM_SYS_TIMEOUT    LWIP_NUM_SYS_TIMEOUT_INTERNAL

// pbufs
#define PBUF_POOL_SIZE          4
#define PBUF_POOL_BUFSIZE       1524

// ipv4
#define LWIP_IPV4                1

// tcp
#define LWIP_TCP                1
#define TCP_TTL                 255
#define TCP_QUEUE_OOSEQ         0
#define TCP_MSS                 (1500 - 40)
#define TCP_SND_BUF             (4*TCP_MSS)
#define TCP_SND_QUEUELEN        (2* TCP_SND_BUF/TCP_MSS)
#define TCP_WND                 (2*TCP_MSS)

// udp
#define LWIP_UDP                1
#define UDP_TTL                 255

// icmp
#define LWIP_ICMP                       1

// stats
#define LWIP_STATS 0

// link cb
#define LWIP_NETIF_LINK_CALLBACK        1

// checksums
#define CHECKSUM_GEN_IP                 1
#define CHECKSUM_GEN_UDP                1
#define CHECKSUM_GEN_TCP                1
#define CHECKSUM_CHECK_IP               1
#define CHECKSUM_CHECK_UDP              1
#define CHECKSUM_CHECK_TCP              1
#define CHECKSUM_GEN_ICMP               1

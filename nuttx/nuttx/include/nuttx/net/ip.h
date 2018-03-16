/****************************************************************************
 * include/nuttx/net/ip.h
 *
 * The uIP header file contains IP-related definitions for a number of C
 * macros that are used by applications as well as internally by the
 * OS networking logic.
 *
 *   Copyright (C) 2007-2012, 2014 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * This logic was leveraged from uIP which also has a BSD-style license:
 *
 *   Author Adam Dunkels <adam@dunkels.com>
 *   Copyright (c) 2001-2003, Adam Dunkels.
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_NUTTX_NET_IP_H
#define __INCLUDE_NUTTX_NET_IP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <queue.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <nuttx/net/netconfig.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* Values for the IP protocol field */

#define IP_PROTO_ICMP  1
#define IP_PROTO_IGMP  2
#define IP_PROTO_TCP   6
#define IP_PROTO_UDP   17
#define IP_PROTO_ICMP6 58

/* Header sizes */

#ifdef CONFIG_NET_IPv6
# define IP_HDRLEN     40    /* Size of IP header */
#else
# define IP_HDRLEN     20    /* Size of IP header */
#endif

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* Representation of an IP address */

typedef in_addr_t net_ip4addr_t;
typedef uint16_t net_ip6addr_t[8];

#ifdef CONFIG_NET_IPv6
typedef net_ip6addr_t net_ipaddr_t;
#else
typedef net_ip4addr_t net_ipaddr_t;
#endif

/* The IP header */

struct net_iphdr_s
{
#ifdef CONFIG_NET_IPv6

  /* IPv6 Ip header */

  uint8_t  vtc;             /* Bits 0-3: version, bits 4-7: traffic class (MS) */
  uint8_t  tcf;             /* Bits 0-3: traffic class (LS), 4-bits: flow label (MS) */
  uint16_t flow;            /* 16-bit flow label (LS) */
  uint8_t  len[2];          /* 16-bit Payload length */
  uint8_t  proto;           /*  8-bit Next header (same as IPv4 protocol field) */
  uint8_t  ttl;             /*  8-bit Hop limit (like IPv4 TTL field) */
  net_ip6addr_t srcipaddr;  /* 128-bit Source address */
  net_ip6addr_t destipaddr; /* 128-bit Destination address */

#else /* CONFIG_NET_IPv6 */

  /* IPv4 IP header */

  uint8_t  vhl;             /*  8-bit Version (4) and header length (5 or 6) */
  uint8_t  tos;             /*  8-bit Type of service (e.g., 6=TCP) */
  uint8_t  len[2];          /* 16-bit Total length */
  uint8_t  ipid[2];         /* 16-bit Identification */
  uint8_t  ipoffset[2];     /* 16-bit IP flags + fragment offset */
  uint8_t  ttl;             /*  8-bit Time to Live */
  uint8_t  proto;           /*  8-bit Protocol */
  uint16_t ipchksum;        /* 16-bit Header checksum */
  uint16_t srcipaddr[2];    /* 32-bit Source IP address */
  uint16_t destipaddr[2];   /* 32-bit Destination IP address */

#endif /* CONFIG_NET_IPv6 */
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* Construct an IPv4 address from four bytes.
 *
 * This function constructs an IPv4 address in network byte order.
 *
 *   addr  A pointer to a net_ipaddr_t variable that will be
 *         filled in with the IPv4 address.
 *   addr0 The first octet of the IPv4 address.
 *   addr1 The second octet of the IPv4 address.
 *   addr2 The third octet of the IPv4 address.
 *   addr3 The forth octet of the IPv4 address.
 */

#define net_ipaddr(addr, addr0, addr1, addr2, addr3) \
  do { \
    addr = HTONL((uint32_t)(addr0) << 24 | (uint32_t)(addr1) << 16 | \
                 (uint32_t)(addr2) << 8  | (uint32_t)(addr3)); \
  } while (0)

/* Convert an IPv4 address of the form uint16_t[2] to an in_addr_t */

#ifdef CONFIG_ENDIAN_BIG
#  define net_ip4addr_conv32(addr) (((in_addr_t)((uint16_t*)addr)[0] << 16) | (in_addr_t)((uint16_t*)addr)[1])
#else
#  define net_ip4addr_conv32(addr) (((in_addr_t)((uint16_t*)addr)[1] << 16) | (in_addr_t)((uint16_t*)addr)[0])
#endif

/* Extract individual bytes from a 32-bit IPv4 IP address that is in network
 * byte order.
 */

#ifdef CONFIG_ENDIAN_BIG
   /* Big-endian byte order: 11223344 */

#  define ip4_addr1(ipaddr) (((ipaddr) >> 24) & 0xff)
#  define ip4_addr2(ipaddr) (((ipaddr) >> 16) & 0xff)
#  define ip4_addr3(ipaddr) (((ipaddr) >>  8) & 0xff)
#  define ip4_addr4(ipaddr)  ((ipaddr)        & 0xff)
#else
   /* Little endian byte order: 44223311 */

#  define ip4_addr1(ipaddr)  ((ipaddr)        & 0xff)
#  define ip4_addr2(ipaddr) (((ipaddr) >>  8) & 0xff)
#  define ip4_addr3(ipaddr) (((ipaddr) >> 16) & 0xff)
#  define ip4_addr4(ipaddr) (((ipaddr) >> 24) & 0xff)
#endif

/* Construct an IPv6 address from eight 16-bit words.
 *
 * This function constructs an IPv6 address.
 */

#define ip6_addr(addr, addr0,addr1,addr2,addr3,addr4,addr5,addr6,addr7) \
  do { \
    ((uint16_t*)(addr))[0] = HTONS((addr0)); \
    ((uint16_t*)(addr))[1] = HTONS((addr1)); \
    ((uint16_t*)(addr))[2] = HTONS((addr2)); \
    ((uint16_t*)(addr))[3] = HTONS((addr3)); \
    ((uint16_t*)(addr))[4] = HTONS((addr4)); \
    ((uint16_t*)(addr))[5] = HTONS((addr5)); \
    ((uint16_t*)(addr))[6] = HTONS((addr6)); \
    ((uint16_t*)(addr))[7] = HTONS((addr7)); \
  } while (0)

/* Copy an IP address from one place to another.
 *
 * Example:
 *
 *   net_ipaddr_t ipaddr1, ipaddr2;
 *
 *   net_ipaddr(&ipaddr1, 192,16,1,2);
 *   net_ipaddr_copy(&ipaddr2, &ipaddr1);
 *
 * dest The destination for the copy.
 * src The source from where to copy.
 */

#ifndef CONFIG_NET_IPv6
#  define net_ipaddr_copy(dest, src) \
   do { \
     (dest) = (in_addr_t)(src); \
   } while (0)
#  define net_ipaddr_hdrcopy(dest, src) \
   do { \
     ((uint16_t*)(dest))[0] = ((uint16_t*)(src))[0]; \
     ((uint16_t*)(dest))[1] = ((uint16_t*)(src))[1]; \
   } while (0)
#else /* !CONFIG_NET_IPv6 */
#  define net_ipaddr_copy(dest, src)    memcpy(&dest, &src, sizeof(net_ip6addr_t))
#  define net_ipaddr_hdrcopy(dest, src) net_ipaddr_copy(dest, src)
#endif /* !CONFIG_NET_IPv6 */

/* Compare two IP addresses
 *
 * Example:
 *
 *   net_ipaddr_t ipaddr1, ipaddr2;
 *
 *   net_ipaddr(&ipaddr1, 192,16,1,2);
 *   if (net_ipaddr_cmp(ipaddr2, ipaddr1))
 *     {
 *       printf("They are the same");
 *     }
 *
 * addr1 The first IP address.
 * addr2 The second IP address.
 */

#ifndef CONFIG_NET_IPv6
#  define net_ipaddr_cmp(addr1, addr2) \
     (addr1 == addr2)
#  define net_ipaddr_hdrcmp(addr1, addr2) \
     net_ipaddr_cmp(net_ip4addr_conv32(addr1), net_ip4addr_conv32(addr2))
#else /* !CONFIG_NET_IPv6 */
#  define net_ipaddr_cmp(addr1, addr2) \
     (memcmp(&addr1, &addr2, sizeof(net_ip6addr_t)) == 0)
#  define net_ipaddr_hdrcmp(addr1, addr2) \
     net_ipaddr_cmp(addr, addr2)
#endif /* !CONFIG_NET_IPv6 */

/* Compare two IP addresses under a netmask.  The mask is used to mask
 * out the bits that are to be compared:  Buts within the mask much
 * match exactly; bits outside if the mask are ignored.
 *
 * Example:
 *
 *   net_ipaddr_t ipaddr1;
 *   net_ipaddr_t ipaddr2;
 *   net_ipaddr_t mask;
 *
 *   net_ipaddr(&mask, 255,255,255,0);
 *   net_ipaddr(&ipaddr1, 192,16,1,2);
 *   net_ipaddr(&ipaddr2, 192,16,1,3);
 *   if (net_ipaddr_maskcmp(ipaddr1, ipaddr2, &mask))
 *     {
 *       printf("They are the same");
 *     }
 *
 * addr1 The first IP address.
 * addr2 The second IP address.
 * mask The netmask.
 */

#ifndef CONFIG_NET_IPv6
#  define net_ipaddr_maskcmp(addr1, addr2, mask) \
  (((in_addr_t)(addr1) & (in_addr_t)(mask)) == \
   ((in_addr_t)(addr2) & (in_addr_t)(mask)))
#else
bool net_ipaddr_maskcmp(net_ipaddr_t addr1, net_ipaddr_t addr2,
                        net_ipaddr_t mask);
#endif

/* Mask out the network part of an IP address, given the address and
 * the netmask.
 *
 * Example:
 *
 *   net_ipaddr_t ipaddr1, ipaddr2, netmask;
 *
 *   net_ipaddr(&ipaddr1, 192,16,1,2);
 *   net_ipaddr(&netmask, 255,255,255,0);
 *   net_ipaddr_mask(&ipaddr2, &ipaddr1, &netmask);
 *
 * In the example above, the variable "ipaddr2" will contain the IP
 * address 192.168.1.0.
 *
 * dest Where the result is to be placed.
 * src The IP address.
 * mask The netmask.
 */

#define net_ipaddr_mask(dest, src, mask) \
  do { \
    (in_addr_t)(dest) = (in_addr_t)(src) & (in_addr_t)(mask); \
  } while (0)

#endif /* __INCLUDE_NUTTX_NET_IP_H */

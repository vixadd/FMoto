/****************************************************************************
 * include/nuttx/net/icmp.h
 * Header file for the uIP ICMP stack.
 *
 *   Copyright (C) 2007-2009, 2012, 2014 Gregory Nutt. All rights reserved.
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

#ifndef __INCLUDE_NUTTX_NET_ICMP_H
#define __INCLUDE_NUTTX_NET_ICMP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <nuttx/net/netconfig.h>
#include <nuttx/net/ip.h>
#include <nuttx/net/tcp.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* ICMP/ICMP6 definitions */

/* ICMP/ICMP6 Message Types */

#define ICMP_ECHO_REPLY              0    /* RFC 792 */
#define ICMP_DEST_UNREACHABLE        3    /* RFC 792 */
#define ICMP_SRC_QUENCH              4    /* RFC 792 */
#define ICMP_REDIRECT                5    /* RFC 792 */
#define ICMP_ALT_HOST_ADDRESS        6
#define ICMP_ECHO_REQUEST            8    /* RFC 792 */
#define ICMP_ROUTER_ADVERTISEMENT    9    /* RFC 1256 */
#define ICMP_ROUTER_SOLICITATION     10   /* RFC 1256 */
#define ICMP_TIME_EXCEEDED           11   /* RFC 792 */
#define ICMP_PARAMETER_PROBLEM       12
#define ICMP_TIMESTAMP_REQUEST       13
#define ICMP_TIMESTAMP_REPLY         14
#define ICMP_INFORMATION_REQUEST     15
#define ICMP_INFORMATION_REPLY       16
#define ICMP_ADDRESS_MASK_REQUEST    17
#define ICMP_ADDRESS_MASK_REPLY      18
#define ICMP_TRACEROUTE              30
#define ICMP_CONVERSION_ERROR        31
#define ICMP_MOBILE_HOST_REDIRECT    32
#define ICMP_IPV6_WHEREAREYOU        33
#define ICMP_IPV6_IAMHERE            34
#define ICMP_MOBILE_REGIS_REQUEST    35
#define ICMP_MOBILE_REGIS_REPLY      36
#define ICMP_DOMAIN_NAME_REQUEST     37
#define ICMP_DOMAIN_NAME_REPLY       38
#define ICMP_SKIP_DISCOVERY_PROTO    39
#define ICMP_PHOTURIS_SECURITY_FAIL  40
#define ICMP_EXP_MOBILE_PROTO        41   /* RFC 4065 */

/* ICMP6 Message Types */

#define ICMP6_ECHO_REPLY             129
#define ICMP6_ECHO_REQUEST           128
#define ICMP6_NEIGHBOR_SOLICITATION  135
#define ICMP6_NEIGHBOR_ADVERTISEMENT 136

#define ICMP6_FLAG_S (1 << 6)

#define ICMP6_OPTION_SOURCE_LINK_ADDRESS 1
#define ICMP6_OPTION_TARGET_LINK_ADDRESS 2

/* Header sizes */

#define ICMP_HDRLEN    4                         /* Size of ICMP header */
#define IPICMP_HDRLEN  (ICMP_HDRLEN + IP_HDRLEN) /* Size of IP + ICMP header */

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* The ICMP and IP headers */

struct icmp_iphdr_s
{
#ifdef CONFIG_NET_IPv6

  /* IPv6 Ip header */

  uint8_t  vtc;             /* Bits 0-3: version, bits 4-7: traffic class (MS) */
  uint8_t  tcf;             /* Bits 0-3: traffic class (LS), bits 4-7: flow label (MS) */
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

  /* ICMP header */

  uint8_t  type;            /* Defines the format of the ICMP message */
  uint8_t  icode;           /* Further qualifies the ICMP messsage */
  uint16_t icmpchksum;      /* Checksum of ICMP header and data */

  /* Data following the ICMP header contains the data specific to the
   * message type indicated by the Type and Code fields.
   */

#ifndef CONFIG_NET_IPv6

  /* ICMP_ECHO_REQUEST and ICMP_ECHO_REPLY data */

  uint16_t id;               /* Used to match requests with replies */
  uint16_t seqno;            /* "  " "" "   " "      " "  " "     " */

#else /* !CONFIG_NET_IPv6 */

  /* ICMP6_ECHO_REQUEST and ICMP6_ECHO_REPLY data */

  uint8_t flags;
  uint8_t reserved1;
  uint8_t reserved2;
  uint8_t reserved3;
  uint8_t icmp6data[16];
  uint8_t options[1];

#endif /* !CONFIG_NET_IPv6 */
};

/* The structure holding the ICMP statistics that are gathered if
 * CONFIG_NET_STATISTICS is defined.
 */

#ifdef CONFIG_NET_STATISTICS
struct icmp_stats_s
{
  net_stats_t drop;       /* Number of dropped ICMP packets */
  net_stats_t recv;       /* Number of received ICMP packets */
  net_stats_t sent;       /* Number of sent ICMP packets */
  net_stats_t typeerr;    /* Number of ICMP packets with a wrong type */
};
#endif

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: imcp_ping
 *
 * Description:
 *   Send a ECHO request and wait for the ECHO response
 *
 * Parameters:
 *   addr  - The IP address of the peer to send the ICMP ECHO request to
 *           in network order.
 *   id    - The ID to use in the ICMP ECHO request.  This number should be
 *           unique; only ECHO responses with this matching ID will be
 *           processed (host order)
 *   seqno - The sequence number used in the ICMP ECHO request.  NOT used
 *           to match responses (host order)
 *   dsecs - Wait up to this many deci-seconds for the ECHO response to be
 *           returned (host order).
 *
 * Return:
 *   seqno of received ICMP ECHO with matching ID (may be different
 *   from the seqno argument (may be a delayed response from an earlier
 *   ping with the same ID). Or a negated errno on any failure.
 *
 * Assumptions:
 *   Called from the user level with interrupts enabled.
 *
 ****************************************************************************/

int icmp_ping(net_ipaddr_t addr, uint16_t id, uint16_t seqno, uint16_t datalen,
              int dsecs);

#undef EXTERN
#ifdef __cplusplus
}
#endif
#endif /* __INCLUDE_NUTTX_NET_ICMP_H */

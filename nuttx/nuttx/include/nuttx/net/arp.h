/****************************************************************************
 * include/nuttx/net/arp.h
 * Macros and definitions for the ARP module.
 *
 *   Copyright (C) 2007, 2009-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Derived from uIP with has a similar BSD-styple license:
 *
 *   Author: Adam Dunkels <adam@dunkels.com>
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

#ifndef __INCLUDE_NUTTX_NET_ARP_H
#define __INCLUDE_NUTTX_NET_ARP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#include <stdint.h>

#include <netinet/in.h>
#include <net/ethernet.h>

#include <nuttx/net/netconfig.h>

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

/* Recognized values of the type bytes in the Ethernet header */

#define ETHTYPE_ARP     0x0806 /* Address resolution protocol */
#define ETHTYPE_IP      0x0800 /* IP protocol */
#define ETHTYPE_IP6     0x86dd /* IP protocol version 6 */

/* Size of the Ethernet header */

#define ETH_HDRLEN      14      /* Minimum size: 2*6 + 2 */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* The Ethernet header -- 14 bytes. The first two fields are type 'struct
 * ether_addr but are represented as a simple byte array here because
 * some compilers refuse to pack 6 byte structures.
 */

struct eth_hdr_s
{
  uint8_t  dest[6]; /* Ethernet destination address (6 bytes) */
  uint8_t  src[6];  /* Ethernet source address (6 bytes) */
  uint16_t type;    /* Type code (2 bytes) */
};

/* One entry in the ARP table (volatile!) */

struct arp_entry
{
  in_addr_t         at_ipaddr;   /* IP address */
  struct ether_addr at_ethaddr;  /* Hardware address */
  uint8_t           at_time;
};

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

#ifdef CONFIG_NET_ARP
/****************************************************************************
 * Name: arp_ipin
 *
 * Description:
 *   The arp_ipin() function should be called by Ethernet device drivers
 *   whenever an IP packet arrives from the network.  The function will
 *   check if the address is in the ARP cache, and if so the ARP cache entry
 *   will be refreshed. If no ARP cache entry was found, a new one is created.
 *
 *   This function expects that an IP packet with an Ethernet header is
 *   present in the d_buf buffer and that the length of the packet is in the
 *   d_len field.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_ARP_IPIN
void arp_ipin(struct net_driver_s *dev);
#else
# define arp_ipin(dev)
#endif

/****************************************************************************
 * Name: arp_arpin
 *
 * Description:
 *   This function should be called by the Ethernet device driver when an ARP
 *   packet has been received.   The function will act differently
 *   depending on the ARP packet type: if it is a reply for a request
 *   that we previously sent out, the ARP cache will be filled in with
 *   the values from the ARP reply.  If the incoming ARP packet is an ARP
 *   request for our IP address, an ARP reply packet is created and put
 *   into the d_buf[] buffer.
 *
 *   On entry, this function expects that an ARP packet with a prepended
 *   Ethernet header is present in the d_buf[] buffer and that the length of
 *   the packet is set in the d_len field.
 *
 *   When the function returns, the value of the field d_len indicates whether
 *   the device driver should send out the ARP reply packet or not. If d_len
 *   is zero, no packet should be sent; If d_len is non-zero, it contains the
 *   length of the outbound packet that is present in the d_buf[] buffer.
 *
 ****************************************************************************/

void arp_arpin(struct net_driver_s *dev);

/****************************************************************************
 * Name: arp_out
 *
 * Description:
 *   This function should be called before sending out an IP packet. The
 *   function checks the destination IP address of the IP packet to see
 *   what Ethernet MAC address that should be used as a destination MAC
 *   address on the Ethernet.
 *
 *   If the destination IP address is in the local network (determined
 *   by logical ANDing of netmask and our IP address), the function
 *   checks the ARP cache to see if an entry for the destination IP
 *   address is found.  If so, an Ethernet header is pre-pended at the
 *   beginning of the packet and the function returns.
 *
 *   If no ARP cache entry is found for the destination IP address, the
 *   packet in the d_buf[] is replaced by an ARP request packet for the
 *   IP address. The IP packet is dropped and it is assumed that the
 *   higher level protocols (e.g., TCP) eventually will retransmit the
 *   dropped packet.
 *
 *   Upon return in either the case, a packet to be sent is present in the
 *   d_buf[] buffer and the d_len field holds the length of the Ethernet
 *   frame that should be transmitted.
 *
 ****************************************************************************/

void arp_out(FAR struct net_driver_s *dev);

/****************************************************************************
 * Name: arp_update
 *
 * Description:
 *   Add the IP/HW address mapping to the ARP table -OR- change the IP
 *   address of an existing association.
 *
 *   NOTE: This is an internal interface withint the networking layer and
 *   should not be used by application software.  This prototype is here
 *   because there is already a violation of this structureing:  The
 *   apps/netutils/dhcpd logic currently calls arp_update() directly.  That
 *   is bad and needs to be replace with some ioctl interface perhaps hidden
 *   somewhere in apps/netutils/netlib.
 *
 * Input parameters:
 *   pipaddr - Refers to an IP address uint16_t[2] in network order
 *   ethaddr - Refers to a HW address uint8_t[IFHWADDRLEN]
 *
 * Assumptions
 *   Interrupts are disabled to assure exclusive access to the ARP table.
 *
 ****************************************************************************/

void arp_update(FAR uint16_t *pipaddr, FAR uint8_t *ethaddr);

#else /* CONFIG_NET_ARP */

/* If ARP is disabled, stub out all ARP interfaces */

# define arp_ipin(dev)
# define arp_arpin(dev)
# define arp_out(dev)
# define arp_find(ipaddr) NULL
# define arp_delete(ipaddr)
# define arp_update(pipaddr,ethaddr)

#endif /* CONFIG_NET_ARP */

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_NUTTX_NET_ARP_H */

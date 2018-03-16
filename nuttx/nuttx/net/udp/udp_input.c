/****************************************************************************
 * net/udp/udp_input.c
 * Handling incoming UDP input
 *
 *   Copyright (C) 2007-2009, 2011 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Adapted for NuttX from logic in uIP which also has a BSD-like license:
 *
 *   Original author Adam Dunkels <adam@dunkels.com>
 *   Copyright () 2001-2003, Adam Dunkels.
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#if defined(CONFIG_NET) && defined(CONFIG_NET_UDP)

#include <debug.h>

#include <nuttx/net/netconfig.h>
#include <nuttx/net/netdev.h>
#include <nuttx/net/udp.h>
#include <nuttx/net/netstats.h>

#include "devif/devif.h"
#include "utils/utils.h"
#include "udp/udp.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define UDPBUF ((struct udp_iphdr_s *)&dev->d_buf[NET_LL_HDRLEN])

/****************************************************************************
 * Public Variables
 ****************************************************************************/

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: udp_input
 *
 * Description:
 *   Handle incoming UDP input
 *
 * Parameters:
 *   dev - The device driver structure containing the received UDP packet
 *
 * Return:
 *   OK  The packet has been processed  and can be deleted
 *   ERROR Hold the packet and try again later. There is a listening socket
 *         but no receive in place to catch the packet yet.
 *
 * Assumptions:
 *   Called from the interrupt level or with interrupts disabled.
 *
 ****************************************************************************/

int udp_input(FAR struct net_driver_s *dev)
{
  FAR struct udp_conn_s *conn;
  FAR struct udp_iphdr_s *pbuf = UDPBUF;
  int ret = OK;

#ifdef CONFIG_NET_STATISTICS
  g_netstats.udp.recv++;
#endif

  /* UDP processing is really just a hack. We don't do anything to the UDP/IP
   * headers, but let the UDP application do all the hard work. If the
   * application sets d_sndlen, it has a packet to send.
   */

  dev->d_len    -= IPUDP_HDRLEN;
#ifdef CONFIG_NET_UDP_CHECKSUMS
  dev->d_appdata = &dev->d_buf[NET_LL_HDRLEN + IPUDP_HDRLEN];
  if (pbuf->udpchksum != 0 && udp_chksum(dev) != 0xffff)
    {
#ifdef CONFIG_NET_STATISTICS
      g_netstats.udp.drop++;
      g_netstats.udp.chkerr++;
#endif
      nlldbg("Bad UDP checksum\n");
      dev->d_len = 0;
    }
  else
#endif
    {
      /* Demultiplex this UDP packet between the UDP "connections". */

      conn = udp_active(pbuf);
      if (conn)
        {
          uint16_t flags;

          /* Set-up for the application callback */

          dev->d_appdata = &dev->d_buf[NET_LL_HDRLEN + IPUDP_HDRLEN];
          dev->d_snddata = &dev->d_buf[NET_LL_HDRLEN + IPUDP_HDRLEN];
          dev->d_sndlen  = 0;

          /* Perform the application callback */

          flags = udp_callback(dev, conn, UDP_NEWDATA);

          /* If the operation was successful, the UDP_NEWDATA flag is removed
           * and thus the packet can be deleted (OK will be returned).
           */

          if ((flags & UDP_NEWDATA) != 0)
            {
              /* No.. the packet was not processed now.  Return ERROR so
               * that the driver may retry again later.
               */

              ret = ERROR;
            }

          /* If the application has data to send, setup the UDP/IP header */

          if (dev->d_sndlen > 0)
            {
              udp_send(dev, conn);
            }
        }
      else
        {
          nlldbg("No listener on UDP port\n");
          dev->d_len = 0;
        }
    }

  return ret;
}

#endif /* CONFIG_NET && CONFIG_NET_UDP */

/****************************************************************************
 * examples/webserver/webserver_main.c
 *
 *   Copyright (C) 2007, 2009-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Based on uIP which also has a BSD style license:
 *
 *   Copyright (c) 2001, Adam Dunkels.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
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

#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <debug.h>

#include <net/if.h>
#include <netinet/in.h>

#include <nuttx/net/arp.h>
#include <apps/netutils/netlib.h>

#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
#include <arpa/inet.h>
#endif

/* Here we include the header file for the application(s) we use in
 * our project as defined in the config/<board-name>/defconfig file
 */

/* DHCPC may be used in conjunction with any other feature (or not) */

#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
# include <apps/netutils/dnsclient.h>
# include <apps/netutils/dhcpc.h>
#endif

/* Include uIP webserver definitions */

#include <apps/netutils/httpd.h>

#include "cgi.h"

/****************************************************************************
 * Definitions
 ****************************************************************************/

#ifdef CONFIG_CPP_HAVE_VARARGS
#  ifdef CONFIG_DEBUG
#    define message(...) lowsyslog(__VA_ARGS__)
#  else
#    define message(...) printf(__VA_ARGS__)
#  endif
#else
#  ifdef CONFIG_DEBUG
#    define message lowsyslog
#  else
#    define message (void)
#  endif
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * webserver_main
 ****************************************************************************/

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int webserver_main(int argc, char *argv[])
#endif
{
  struct in_addr addr;
#if defined(CONFIG_EXAMPLES_WEBSERVER_DHCPC) || defined(CONFIG_EXAMPLES_WEBSERVER_NOMAC)
  uint8_t mac[IFHWADDRLEN];
#endif
#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
  void *handle;
#endif

/* Many embedded network interfaces must have a software assigned MAC */

#ifdef CONFIG_EXAMPLES_WEBSERVER_NOMAC
  mac[0] = 0x00;
  mac[1] = 0xe0;
  mac[2] = 0xde;
  mac[3] = 0xad;
  mac[4] = 0xbe;
  mac[5] = 0xef;
  netlib_setmacaddr("eth0", mac);
#endif

  /* Set up our host address */

#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
  addr.s_addr = 0;
#else
  addr.s_addr = HTONL(CONFIG_EXAMPLES_WEBSERVER_IPADDR);
#endif
  netlib_sethostaddr("eth0", &addr);

  /* Set up the default router address */

  addr.s_addr = HTONL(CONFIG_EXAMPLES_WEBSERVER_DRIPADDR);
  netlib_setdraddr("eth0", &addr);

  /* Setup the subnet mask */

  addr.s_addr = HTONL(CONFIG_EXAMPLES_WEBSERVER_NETMASK);
  netlib_setnetmask("eth0", &addr);

#ifdef CONFIG_EXAMPLES_WEBSERVER_DHCPC
  /* Set up the resolver */

  dns_bind();

  /* Get the MAC address of the NIC */

  netlib_getmacaddr("eth0", mac);

  /* Set up the DHCPC modules */

  handle = dhcpc_open(&mac, IFHWADDRLEN);

  /* Get an IP address.  Note:  there is no logic here for renewing the address in this
   * example.  The address should be renewed in ds.lease_time/2 seconds.
   */

  printf("Getting IP address\n");
  if (handle)
    {
        struct dhcpc_state ds;
        (void)dhcpc_request(handle, &ds);
        netlib_sethostaddr("eth1", &ds.ipaddr);

        if (ds.netmask.s_addr != 0)
          {
            netlib_setnetmask("eth0", &ds.netmask);
          }

        if (ds.default_router.s_addr != 0)
          {
            netlib_setdraddr("eth0", &ds.default_router);
          }

        if (ds.dnsaddr.s_addr != 0)
          {
            dns_setserver(&ds.dnsaddr);
          }

        dhcpc_close(handle);
        printf("IP: %s\n", inet_ntoa(ds.ipaddr));
    }
#endif

#ifdef CONFIG_NET_TCP
  printf("Starting webserver\n");
  httpd_init();
  cgi_register();
  httpd_listen();
#endif

  while (1)
    {
      sleep(3);
      printf("webserver_main: Still running\n");
#if CONFIG_NFILE_DESCRIPTORS > 0
      fflush(stdout);
#endif
    }

  return 0;
}

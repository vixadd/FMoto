/*******************************************************************************************
 * include/net/if.h
 *
 *   Copyright (C) 2007, 2008, 2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************/

#ifndef __INCLUDE_NET_IF_H
#define __INCLUDE_NET_IF_H

/*******************************************************************************************
 * Included Files
 *******************************************************************************************/

#include <sys/socket.h>

/*******************************************************************************************
 * Pre-Processor Definitions
 *******************************************************************************************/

/* Sizing parameters */

#define IFNAMSIZ        6   /* Older naming standard */
#define IF_NAMESIZE     6   /* Newer naming standard */
#define IFHWADDRLEN     6

#define IFF_DOWN        (1 << 0)
#define IFF_UP          (1 << 1)
#define IFF_RUNNING     (1 << 2)
#define IFF_NOARP       (1 << 7)

/*******************************************************************************************
 * Public Type Definitions
 *******************************************************************************************/

/* Structure passed with the SIOCMIINOTIFY ioctl command to enable notification of
 * of PHY state changes.
 */

struct mii_iotcl_notify_s
{
  pid_t pid;     /* PID of the task to receive the signal.  Zero means "this task" */
  uint8_t signo; /* Signal number to use when signalling */
  FAR void *arg; /* An argument that will accompany the signal callback */
};

/* Structure passed to read from or write to the MII/PHY management interface via the
 * SIOCxMIIREG ioctl commands.
 */

struct mii_ioctl_data_s
{
  uint16_t phy_id;      /* PHY device address */
  uint16_t reg_num;     /* PHY register address */
  uint16_t val_in;      /* PHY input data */
  uint16_t val_out;     /* PHY output data */
};

/* This is the newer form if the I/F request structure that can be used with both IPv4
 * and IPv6.
 */

struct lifreq
{
  char                        lifr_name[IFNAMSIZ];      /* Network device name (e.g. "eth0") */
  union
  {
    struct sockaddr_storage   lifru_addr;               /* IP Address */
    struct sockaddr_storage   lifru_dstaddr;            /* P-to-P Address */
    struct sockaddr_storage   lifru_broadaddr;          /* Broadcast address */
    struct sockaddr_storage   lifru_netmask;            /* Netmask */
    struct sockaddr           lifru_hwaddr;             /* MAC address */
    int                       lifru_count;              /* Number of devices */
    int                       lifru_mtu;                /* MTU size */
    uint8_t                   lifru_flags;              /* Interface flags */
    struct mii_iotcl_notify_s llfru_mii_notify;         /* PHY event notification */
    struct mii_ioctl_data_s   lifru_mii_data;           /* MII request data */
  } lifr_ifru;
};

#define lifr_addr             lifr_ifru.lifru_addr      /* IP address */
#define lifr_dstaddr          lifr_ifru.lifru_dstaddr   /* P-to-P Address */
#define lifr_broadaddr        lifr_ifru.lifru_broadaddr /* Broadcast address */
#define lifr_netmask          lifr_ifru.lifru_netmask   /* Interface net mask */
#define lifr_hwaddr           lifr_ifru.lifru_hwaddr    /* MAC address */
#define lifr_mtu              lifr_ifru.lifru_mtu       /* MTU */
#define lifr_count            lifr_ifru.lifru_count     /* Number of devices */
#define lifr_flags            lifr_ifru.lifru_flags     /* interface flags */
#define lifr_mii_notify_pid   lifr_ifru.llfru_mii_notify.pid   /* PID to be notified */
#define lifr_mii_notify_signo lifr_ifru.llfru_mii_notify.signo /* Signal to notify with */
#define lifr_mii_notify_arg   lifr_ifru.llfru_mii_notify.arg   /* sigval argument */
#define lifr_mii_phy_id       lifr_ifru.lifru_mii_data.phy_id  /* PHY device address */
#define lifr_mii_reg_num      lifr_ifru.lifru_mii_data.reg_num /* PHY register address */
#define lifr_mii_val_in       lifr_ifru.lifru_mii_data.val_in  /* PHY input data */
#define lifr_mii_val_out      lifr_ifru.lifru_mii_data.val_out /* PHY output data */

/* This is the older I/F request that should only be used with IPv4.  However, since
 * NuttX only supports IPv4 or 6 (not both), we can force the older structure to
 * be compatible when IPv6 is enabled.
 */

#ifndef CONFIG_NET_IPv6
struct ifreq
{
  char                        ifr_name[IFNAMSIZ];       /* Network device name (e.g. "eth0") */
  union
  {
    struct sockaddr           ifru_addr;                /* IP Address */
    struct sockaddr           ifru_dstaddr;             /* P-to-P Address */
    struct sockaddr           ifru_broadaddr;           /* Broadcast address */
    struct sockaddr           ifru_netmask;             /* Netmask */
    struct sockaddr           ifru_hwaddr;              /* MAC address */
    int                       ifru_count;               /* Number of devices */
    int                       ifru_mtu;                 /* MTU size */
    uint8_t                   ifru_flags;               /* Interface flags */
    struct mii_iotcl_notify_s ifru_mii_notify;          /* PHY event notification */
    struct mii_ioctl_data_s   ifru_mii_data;            /* MII request data */
  } ifr_ifru;
};

#define ifr_addr              ifr_ifru.ifru_addr        /* IP address */
#define ifr_dstaddr           ifr_ifru.ifru_dstaddr     /* P-to-P Address */
#define ifr_broadaddr         ifr_ifru.ifru_broadaddr   /* Broadcast address */
#define ifr_netmask           ifr_ifru.ifru_netmask     /* Interface net mask */
#define ifr_hwaddr            ifr_ifru.ifru_hwaddr      /* MAC address */
#define ifr_mtu               ifr_ifru.ifru_mtu         /* MTU */
#define ifr_count             ifr_ifru.ifru_count       /* Number of devices */
#define ifr_flags             ifr_ifru.ifru_flags       /* interface flags */
#define ifr_mii_notify_pid    ifr_ifru.ifru_mii_notify.pid   /* PID to be notified */
#define ifr_mii_notify_signo  ifr_ifru.ifru_mii_notify.signo /* Signal to notify with */
#define ifr_mii_notify_arg    ifr_ifru.ifru_mii_notify.arg   /* sigval argument */
#define ifr_mii_phy_id        ifr_ifru.ifru_mii_data.phy_id  /* PHY device address */
#define ifr_mii_reg_num       ifr_ifru.ifru_mii_data.reg_num /* PHY register address */
#define ifr_mii_val_in        ifr_ifru.ifru_mii_data.val_in  /* PHY input data */
#define ifr_mii_val_out       ifr_ifru.ifru_mii_data.val_out /* PHY output data */

#else /* CONFIG_NET_IPv6 */

#define ifreq                 lifreq                    /* Replace ifreq with lifreq */
#define ifr_name              lifr_name                 /* Network device name */
#define ifr_addr              lifr_ifru.lifru_addr      /* IP address */
#define ifr_dstaddr           lifr_ifru.lifru_dstaddr   /* P-to-P Address */
#define ifr_broadaddr         lifr_ifru.lifru_broadaddr /* Broadcast address */
#define ifr_netmask           lifr_ifru.lifru_netmask   /* Interface net mask */
#define ifr_hwaddr            lifr_ifru.lifru_hwaddr    /* MAC address */
#define ifr_mtu               lifr_ifru.lifru_mtu       /* MTU */
#define ifr_count             lifr_ifru.lifru_count     /* Number of devices */
#define ifr_flags             lifr_ifru.lifru_flags     /* interface flags */
#define ifr_mii_phy_id        lifr_ifru.lifru_mii_data.phy_id  /* PHY device address */
#define ifr_mii_reg_num       lifr_ifru.lifru_mii_data.reg_num /* PHY register address */
#define ifr_mii_val_in        lifr_ifru.lifru_mii_data.val_in  /* PHY input data */
#define ifr_mii_val_out       lifr_ifru.lifru_mii_data.val_out /* PHY output data */

#endif /* CONFIG_NET_IPv6 */

/*******************************************************************************************
 * Public Function Prototypes
 *******************************************************************************************/

#endif /* __INCLUDE_NET_IF_H */

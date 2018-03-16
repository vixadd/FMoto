/****************************************************************************
 * net/udp/udp_conn.c
 *
 *   Copyright (C) 2007-2009, 2011-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Large parts of this file were leveraged from uIP logic:
 *
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#if defined(CONFIG_NET) && defined(CONFIG_NET_UDP)

#include <stdint.h>
#include <string.h>
#include <semaphore.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <arch/irq.h>

#include <nuttx/net/netconfig.h>
#include <nuttx/net/net.h>
#include <nuttx/net/netdev.h>
#include <nuttx/net/ip.h>
#include <nuttx/net/udp.h>

#include "devif/devif.h"
#include "udp/udp.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* The array containing all uIP UDP connections. */

struct udp_conn_s g_udp_connections[CONFIG_NET_UDP_CONNS];

/* A list of all free UDP connections */

static dq_queue_t g_free_udp_connections;
static sem_t g_free_sem;

/* A list of all allocated UDP connections */

static dq_queue_t g_active_udp_connections;

/* Last port used by a UDP connection connection. */

static uint16_t g_last_udp_port;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: _udp_semtake() and _udp_semgive()
 *
 * Description:
 *   Take/give semaphore
 *
 ****************************************************************************/

static inline void _udp_semtake(FAR sem_t *sem)
{
  /* Take the semaphore (perhaps waiting) */

  while (net_lockedwait(sem) != 0)
    {
      /* The only case that an error should occur here is if
       * the wait was awakened by a signal.
       */

      ASSERT(*get_errno_ptr() == EINTR);
    }
}

#define _udp_semgive(sem) sem_post(sem)

/****************************************************************************
 * Name: udp_find_conn()
 *
 * Description:
 *   Find the UDP connection that uses this local port number.  Called only
 *   from user user level code, but with interrupts disabled.
 *
 ****************************************************************************/

static FAR struct udp_conn_s *udp_find_conn(uint16_t portno)
{
  int i;

  /* Now search each connection structure.*/

  for (i = 0; i < CONFIG_NET_UDP_CONNS; i++)
    {
      if (g_udp_connections[ i ].lport == portno)
        {
          return &g_udp_connections[ i ];
        }
    }

  return NULL;
}

/****************************************************************************
 * Name: udp_select_port()
 *
 * Description:
 *   Select an unused port number.
 *
 *   NOTE that in principle this function could fail if there is no available
 *   port number.  There is no check for that case and it would actually
 *   in an infinite loop if that were the case.  In this simple, small UDP
 *   implementation, it is reasonable to assume that that error cannot happen
 *   and that a port number will always be available.
 *
 * Input Parameters:
 *   None
 *
 * Return:
 *   Next available port number
 *
 ****************************************************************************/

static uint16_t udp_select_port(void)
{
  uint16_t portno;

  /* Find an unused local port number.  Loop until we find a valid
   * listen port number that is not being used by any other connection.
   */

  net_lock_t flags = net_lock();
  do
    {
      /* Guess that the next available port number will be the one after
       * the last port number assigned.
       */

      ++g_last_udp_port;

      /* Make sure that the port number is within range */

      if (g_last_udp_port >= 32000)
        {
          g_last_udp_port = 4096;
        }
    }
  while (udp_find_conn(htons(g_last_udp_port)));

  /* Initialize and return the connection structure, bind it to the
   * port number
   */

  portno = g_last_udp_port;
  net_unlock(flags);

  return portno;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: udp_initialize()
 *
 * Description:
 *   Initialize the UDP connection structures.  Called once and only from
 *   the UIP layer.
 *
 ****************************************************************************/

void udp_initialize(void)
{
  int i;

  /* Initialize the queues */

  dq_init(&g_free_udp_connections);
  dq_init(&g_active_udp_connections);
  sem_init(&g_free_sem, 0, 1);

  for (i = 0; i < CONFIG_NET_UDP_CONNS; i++)
    {
      /* Mark the connection closed and move it to the free list */

      g_udp_connections[i].lport = 0;
      dq_addlast(&g_udp_connections[i].node, &g_free_udp_connections);
    }

  g_last_udp_port = 1024;
}

/****************************************************************************
 * Name: udp_alloc()
 *
 * Description:
 *   Allocate a new, uninitialized UDP connection structure.  This is
 *   normally something done by the implementation of the socket() API
 *
 ****************************************************************************/

FAR struct udp_conn_s *udp_alloc(void)
{
  FAR struct udp_conn_s *conn;

  /* The free list is only accessed from user, non-interrupt level and
   * is protected by a semaphore (that behaves like a mutex).
   */

  _udp_semtake(&g_free_sem);
  conn = (FAR struct udp_conn_s *)dq_remfirst(&g_free_udp_connections);
  if (conn)
    {
      /* Make sure that the connection is marked as uninitialized */

      conn->lport = 0;

      /* Enqueue the connection into the active list */

      dq_addlast(&conn->node, &g_active_udp_connections);
    }

  _udp_semgive(&g_free_sem);
  return conn;
}

/****************************************************************************
 * Name: udp_free()
 *
 * Description:
 *   Free a UDP connection structure that is no longer in use. This should be
 *   done by the implementation of close().
 *
 ****************************************************************************/

void udp_free(FAR struct udp_conn_s *conn)
{
  /* The free list is only accessed from user, non-interrupt level and
   * is protected by a semaphore (that behaves like a mutex).
   */

  DEBUGASSERT(conn->crefs == 0);

  _udp_semtake(&g_free_sem);
  conn->lport = 0;

  /* Remove the connection from the active list */

  dq_rem(&conn->node, &g_active_udp_connections);

  /* Free the connection */

  dq_addlast(&conn->node, &g_free_udp_connections);
  _udp_semgive(&g_free_sem);
}

/****************************************************************************
 * Name: udp_active()
 *
 * Description:
 *   Find a connection structure that is the appropriate
 *   connection to be used within the provided TCP/IP header
 *
 * Assumptions:
 *   This function is called from UIP logic at interrupt level
 *
 ****************************************************************************/

FAR struct udp_conn_s *udp_active(FAR struct udp_iphdr_s *buf)
{
  FAR struct udp_conn_s *conn =
    (FAR struct udp_conn_s *)g_active_udp_connections.head;

  while (conn)
    {
      /* If the local UDP port is non-zero, the connection is considered
       * to be used. If so, the local port number is checked against the
       * destination port number in the received packet. If the two port
       * numbers match, the remote port number is checked if the
       * connection is bound to a remote port. Finally, if the
       * connection is bound to a remote IP address, the source IP
       * address of the packet is checked.
       */

      if (conn->lport != 0 && buf->destport == conn->lport &&
          (conn->rport == 0 || buf->srcport == conn->rport) &&
            (net_ipaddr_cmp(conn->ripaddr, g_allzeroaddr) ||
             net_ipaddr_cmp(conn->ripaddr, g_alloneaddr) ||
             net_ipaddr_hdrcmp(buf->srcipaddr, &conn->ripaddr)))
        {
          /* Matching connection found.. return a reference to it */

          break;
        }

      /* Look at the next active connection */

      conn = (FAR struct udp_conn_s *)conn->node.flink;
    }

  return conn;
}

/****************************************************************************
 * Name: udp_nextconn()
 *
 * Description:
 *   Traverse the list of allocated UDP connections
 *
 * Assumptions:
 *   This function is called from UIP logic at interrupt level (or with
 *   interrupts disabled).
 *
 ****************************************************************************/

FAR struct udp_conn_s *udp_nextconn(FAR struct udp_conn_s *conn)
{
  if (!conn)
    {
      return (FAR struct udp_conn_s *)g_active_udp_connections.head;
    }
  else
    {
      return (FAR struct udp_conn_s *)conn->node.flink;
    }
}

/****************************************************************************
 * Name: udp_bind()
 *
 * Description:
 *   This function implements the UIP specific parts of the standard UDP
 *   bind() operation.
 *
 * Assumptions:
 *   This function is called from normal user level code.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv6
int udp_bind(FAR struct udp_conn_s *conn, FAR const struct sockaddr_in6 *addr)
#else
int udp_bind(FAR struct udp_conn_s *conn, FAR const struct sockaddr_in *addr)
#endif
{
  int ret = -EADDRINUSE;
  net_lock_t flags;

  /* Is the user requesting to bind to any port? */

  if (!addr->sin_port)
    {
      /* Yes.. Find an unused local port number */

      conn->lport = htons(udp_select_port());
      ret         = OK;
    }
  else
    {
      /* Interrupts must be disabled while access the UDP connection list */

      flags = net_lock();

      /* Is any other UDP connection bound to this port? */

      if (!udp_find_conn(addr->sin_port))
        {
          /* No.. then bind the socket to the port */

          conn->lport = addr->sin_port;
          ret         = OK;
        }

      net_unlock(flags);
    }

  return ret;
}

/****************************************************************************
 * Name: udp_connect()
 *
 * Description:
 *   This function sets up a new UDP connection. The function will
 *   automatically allocate an unused local port for the new
 *   connection. However, another port can be chosen by using the
 *   udp_bind() call, after the udp_connect() function has been
 *   called.
 *
 *   This function is called as part of the implementation of sendto
 *   and recvfrom.
 *
 * Input Parameters:
 *   conn - A reference to UDP connection structure
 *   addr - The address of the remote host.
 *
 * Assumptions:
 *   This function is called user code.  Interrupts may be enabled.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv6
int udp_connect(FAR struct udp_conn_s *conn,
                FAR const struct sockaddr_in6 *addr)
#else
int udp_connect(FAR struct udp_conn_s *conn,
                FAR const struct sockaddr_in *addr)
#endif
{
  /* Has this address already been bound to a local port (lport)? */

  if (!conn->lport)
    {
      /* No.. Find an unused local port number and bind it to the
       * connection structure.
       */

      conn->lport = htons(udp_select_port());
    }

  /* Is there a remote port (rport) */

  if (addr)
    {
      conn->rport = addr->sin_port;
      net_ipaddr_copy(conn->ripaddr, addr->sin_addr.s_addr);
    }
  else
    {
      conn->rport = 0;
      net_ipaddr_copy(conn->ripaddr, g_allzeroaddr);
    }

  conn->ttl = IP_TTL;
  return OK;
}

#endif /* CONFIG_NET && CONFIG_NET_UDP */

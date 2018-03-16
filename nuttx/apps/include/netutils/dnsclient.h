/****************************************************************************
 * apps/include/netutils/dnsclient.h
 * DNS resolver code header file.
 * Author Adam Dunkels <adam@dunkels.com>
 *
 *   Copyright (C) 2007-2009, 2011-2012, 2014 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Inspired by/based on uIP logic by Adam Dunkels:
 *
 *   Copyright (c) 2002-2003, Adam Dunkels.
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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

#ifndef __APPS_INCLUDE_NETUTILS_DNSCLIENT_H
#define __APPS_INCLUDE_NETUTILS_DNSCLIENT_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/net/netconfig.h>

#include <netinet/in.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: dns_bind_sock
 *
 * Description:
 *   Initialize the DNS resolver using the caller provided socket.
 *
 ****************************************************************************/

int dns_bind_sock(FAR int *sockfd);

/****************************************************************************
 * Name: dns_bind
 *
 * Description:
 *   Initialize the DNS resolver using an internal, share-able socket.
 *
 ****************************************************************************/

int dns_bind(void);

/****************************************************************************
 * Name: dns_free_sock
 *
 * Description:
 *   Release the DNS resolver by closing the socket.
 *
 ****************************************************************************/

int dns_free_sock(FAR int *sockfd);

/****************************************************************************
 * Name: dns_query_sock
 *
 * Description:
 *   Using the DNS resolver socket (sockfd), look up the the 'hostname', and
 *   return its IP address in 'ipaddr'
 *
 * Returned Value:
 *   Returns zero (OK) if the query was successful.
 *
 ****************************************************************************/

int dns_query_sock(int sockfd, FAR const char *hostname, FAR in_addr_t *ipaddr);

/****************************************************************************
 * Name: dns_query
 *
 * Description:
 *   Using the internal DNS resolver socket, look up the the 'hostname', and
 *   return its IP address in 'ipaddr'
 *
 * Returned Value:
 *   Returns zero (OK) if the query was successful.
 *
 ****************************************************************************/

int dns_query(FAR const char *hostname, FAR in_addr_t *ipaddr);

/****************************************************************************
 * Name: dns_setserver
 *
 * Description:
 *   Configure which DNS server to use for queries
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv6
void dns_setserver(FAR const struct in6_addr *dnsserver);
#else
void dns_setserver(FAR const struct in_addr *dnsserver);
#endif

/****************************************************************************
 * Name: dns_getserver
 *
 * Description:
 *   Obtain the currently configured DNS server.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv6
void dns_getserver(FAR const struct in_addr *dnsserver);
#else
void dns_getserver(FAR struct in_addr *dnsserver);
#endif

/****************************************************************************
 * Name: dns_whois_socket
 *
 * Description:
 *   Get the binding for 'name' using the DNS server accessed via 'sockfd'
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv6
int  dns_whois_socket(int sockfd, FAR const char *name,
                      FAR struct sockaddr_in6 *addr);
#else
int  dns_whois_socket(int sockfd, FAR const char *name,
                      FAR struct sockaddr_in *addr);
#endif

/****************************************************************************
 * Name: dns_whois
 *
 * Description:
 *   Get the binding for 'name' using the DNS server accessed via the DNS
 *   resolvers internal socket.
 *
 ****************************************************************************/

#ifdef CONFIG_NET_IPv6
int dns_whois(FAR const char *name, FAR struct sockaddr_in6 *addr);
#else
int dns_whois(FAR const char *name, FAR struct sockaddr_in *addr);
#endif

/****************************************************************************
 * Name: dns_gethostip
 *
 * Descriptions:
 *   Combines the operations of dns_bind_sock(), dns_query_sock(), and
 *   dns_free_sock() to obtain the the IP address ('ipaddr') associated with
 *   the 'hostname' in one operation.
 *
 ****************************************************************************/

int dns_gethostip(FAR const char *hostname, FAR in_addr_t *ipaddr);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __APPS_INCLUDE_NETUTILS_DNSCLIENT_H */

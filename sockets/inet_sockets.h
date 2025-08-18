/* The Internet Socket Domains Library that abstracts the regular and protocol independent part of Internet Domain Sockets */
#ifndef INET_SOCKETS_H
#define INET_SOCKETS_H

#include <sys/socket.h>
#include <netdb.h>

int inetConnect(const char *host, const char *service, int type);

int inetListen(const char *service, int backlog, socklen_t *addrlen);

int inetBind(const char *service, int type, socklen_t *addrlen);

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen);

#define IS_ADDR_STR_LEN 4096		/* suggested length for the string buffer that the caller should pass to inetAddressStr - must be greater than NI_MAXHOST + NI_MAXSERV + 4 */

#endif

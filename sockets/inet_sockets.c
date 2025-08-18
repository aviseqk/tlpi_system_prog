/* This is the Internet Domain Sockets Library with functions abstracted to do protocol-agnostic or repetitive steps. 
 * These functions and their tasks are following:
 *
 * Function 1: inetConnect() -> Protocol Agnostic (for both TCP and UDP) -> this creates a socket with said socket type, and connects it to address specified by host and service
 *		-> used for TCP or UDP clients to connect their socket to a server socket
 * Function 2: inetListen() -> This creates a listening Stream (SOCK_STREAM) bound to the wildcard IP address on the TCP port specified by the service.
 *		-> to be used by TCP servers
 * Function 3: inetBind() -> This function creates a socket of given type, bound to the wildcard IP address on the port specified by the service and type.
 *		-> for the UDP servers and clients to create a socket bound to a specific address
 * Function 4: inetAddressStr() -> This function converts an Internet Socket Address to a printable presentation form
 * 		-> returns a null terminated string in format (hostname, port-number)
 * */
/* NOTE: Majority of the steps of inetBind and inetListen are similar, so a single function called `inetPassiveSocket()` is created and used by both former functions */

#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "inet_sockets.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

int inetConnect(const char *host, const char *service, int type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;

	memset(&hints, 0, sizeof(struct addrinfo));
    	hints.ai_canonname = NULL;
    	hints.ai_addr = NULL;
    	hints.ai_next = NULL;
    	hints.ai_socktype = type;
    	hints.ai_family = AF_UNSPEC;    /* unspecified, so allows both IPv4 and IPv6 */

	s = getaddrinfo(host, service, &hints, &result);
	if (s != 0) {
		errno = ENOSYS;
		return -1;
	}

	/* walk through the socket addresses till we find a socket we can connect to */
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;

		close(sfd);
	}

	freeaddrinfo(result);

	return (rp == NULL) ? -1 : sfd;
}

static int	/* Public interfaces : inetBind() and inetListen() */
inetPassiveSocket(const char *service, int type, socklen_t *addrlen, bool doListen, int backlog) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, optval, s;
	
    	hints.ai_canonname = NULL;
    	hints.ai_addr = NULL;
    	hints.ai_next = NULL;
    	hints.ai_socktype = type;
    	hints.ai_family = AF_UNSPEC;    /* unspecified, so allows both IPv4 and IPv6 */
	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo(NULL, service, &hints, &result);
	if (s != 0)
		return -1;

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (doListen) {
			if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
				close(sfd);
				freeaddrinfo(result);
				return -1;
			}
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;
		close(sfd);
	}

	if (rp != NULL && doListen) {
		if (listen(sfd, backlog) == -1) {
			freeaddrinfo(result);
			return -1;
		}
	}

	if (rp != NULL && addrlen != NULL)
		*addrlen = rp->ai_addrlen;

	freeaddrinfo(result);
	return (rp == NULL) ? -1 : sfd;
}

int inetListen(const char *service, int backlog, socklen_t *addrlen) {
	return inetPassiveSocket(service, SOCK_STREAM, addrlen, true, backlog);
}

int inetBind(const char *service, int type, socklen_t *addrlen) {
	return inetPassiveSocket(service, type, addrlen, false, 0);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen, char *addrStr, int addrStrLen) {
	char host[NI_MAXHOST], service[NI_MAXSERV];

	if (getnameinfo(addr,addrlen, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV) ==0)
		snprintf(addrStr, addrStrLen, "(%s, %s)", host, service);
	else
		snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");

	return addrStr;
}

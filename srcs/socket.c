#include "server.h"

/*
** Open a socket with the protocol TCP and set the socket to reuse the address
** If successful, return fd. Otherwise, a -1 is returned to indicate an error.
*/

int			socket_int(void)
{
	int					num;
	int					fd;
	struct protoent				*proto;

	if ((proto = getprotobyname("TCP")) == 0)
	{
		perror("ERROR: Protocol");
		// return (-1);
	}
	if ((fd = socket(PF_INET, SOCK_STREAM, proto ? proto->p_proto : 0)) == -1)
	{
		perror("ERROR: Socket");
		return (-1);
	}
	num = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(int)) == -1)
	{
		perror("ERROR: Setsockopt(SO_REUSEADDR)");
		return (-1);
	}
	return (fd);
}

/*
** Assign a name to a socket
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

int			socket_bind(int fd, int port, char **address)
{
	struct sockaddr_in	bind_s;

	memset(&bind_s, 0, sizeof(struct sockaddr_in));
	bind_s.sin_family = AF_INET;
	bind_s.sin_port = htons(port);
	bind_s.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd, (struct sockaddr *)&bind_s, sizeof(bind_s)) == -1)
	{
		perror("ERROR: Bind");
		return (0);
	}
	*address = strdup(inet_ntoa(bind_s.sin_addr));
	return (1);
}

/*
** Set time-out of TIME_OUT seconds for the socket
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int	socket_timeout(int fd)
{
	socklen_t		size;
	struct timeval	delay;

	size = sizeof(struct timeval);
	memset(&delay, 0, sizeof(struct timeval));
	delay.tv_sec = TIME_OUT;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&delay, size) == -1)
	{
		perror("ERROR: Setsockopt(SO_RCVTIMEO)");
		return (0);
	}
	return (1);
}

/*
** Block SIGPIPE for the socket. If the client close the connection
** in the middle of transmission, the write cause a SIGPIPE.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int	socket_sigpipe(int fd)
{
#if !defined(SO_NOSIGPIPE)
	(void)fd;
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		perror("ERROR: signal(SIGPIPE, SIG_IGN)");
		return 0;
	}
#else
	int pipe = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &pipe, sizeof(int)) == -1)
	{
		perror("ERROR: Setsockopt(SO_NOSIGPIPE)");
		return (0);
	}
#endif /* !defined(SO_NOSIGPIPE) */
	return (1);
}

/*
** Accept new socket from incoming connection.
** If successful, return fd. Otherwise, a -1 is returned to indicate an error.
*/

int			socket_accept(int fd, char **address)
{
	int			sock;
	socklen_t		sock_len;
	struct sockaddr_in	sock_init;

	sock_len = sizeof(struct sockaddr_in);
	memset(&sock_init, 0, sizeof(struct sockaddr_in));
	if ((sock = accept(fd, (struct sockaddr *)&sock_init, &sock_len)) == -1)
	{
		perror("ERROR: Accept");
		return (-1);
	}
	if (socket_timeout(sock) == 0)
		return (-1);
	if (socket_sigpipe(sock) == 0)
		return (-1);
	*address = strdup(inet_ntoa(sock_init.sin_addr));
	return (sock);
}

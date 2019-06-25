#include <errno.h>
#include "server.h"

/*
** Add the connection into a new process
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		connection_add(int fd, char *address, uint16_t connect)
{
	int			status;
	pid_t		process;
	t_http		*data;

	data = NULL;
	if ((process = fork()) == 0)
	{
		printf("[%3d] %s\n", connect, address);
		if (request(fd, &data, &status) == 1)
			status = response(data, fd);
		printf("[%3d] %d %s\n", connect, status, get_reponse_message(status));
		_exit(close(fd));
	}
	close(fd);
	if (process < 0)
		perror("ERROR: Fork");
	strdel(&address);
	return ((process < 0) ? 0 : 1);
}

/*
** Accept the incoming connection
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		loop(void)
{
	int		fd;
	char		*address;
	uint16_t	connect;

	connect = 0;
	address = NULL;

	signal(SIGCHLD, sigchld);

	while ((fd = socket_accept(_g.fd, &address)) > 0)
	{
		if (connection_add(fd, address, connect) == 0)
			return (0);
		++connect;
	}
	if (fd == -1)
	{
		perror("ERROR: Accept");
		exit_server();
	}
	return (1);
}

struct s_globalstate		_g = {
	.fd = -1,
	.port = DEFAULT_PORT,
	.webdir = DEFAULT_WEBDIR_PATH,
	.argv0 = NULL,
};

/*
** Automatically called on program termination
*/
void				cleanup() {
	if (_g.fd != -1)
		close(_g.fd);
}

void				usage(FILE *fp, char *msg, ...) {
	static const char	usage_str[] =
		"\nUsage: %s [OPTIONS]\n"
		"\n"
		"OPTIONS:\n"
		"       -w <DIR>        Web server directory (default: site)\n"
		"       -p <PORT>       TCP port of server listen socket (default: 6060)\n"
		"       -h              Produces this information to stdout.\n";
	if (msg) {
		va_list ap;
		va_start(ap, msg);
		vfprintf(fp ? fp : stderr, msg, ap);
		va_end(ap);
	}
	fprintf(fp ? fp : stderr, usage_str, _g.argv0);
}

/*
 ** Start a HTTP server listening on the port DEFAULT_PORT (or an overriding _g.port)
 ** If successful, return 0. Otherwise, the program exits with EXIT_FAILURE.
 */

int				main(int argc, char **argv)
{
	char			*address;
	int			opt;

	atexit(cleanup);

	address = NULL;
	_g.argv0 = *argv;
	signal(SIGINT, sigstop);

	while ((opt = getopt(argc, argv, "hw:p:")) != -1) {
		char		*eptr;
		long		port;
		struct stat 	sbuf;

		switch (opt) {
			case 'w':
				if (stat(optarg, &sbuf) == -1 || (!S_ISDIR(sbuf.st_mode) ? (errno=ENOTDIR,1) : 0))
					usage(stderr, "%s: can not use '%s' as service directory: %s\n",
							*argv, optarg, strerror(errno)), exit_server();
				else
					_g.webdir = strdup(optarg);
				break;
			case 'p':
				port = strtol(optarg, &eptr, 10);
				if (*eptr != '\0' || port < 1 || port > 65535)
					usage(stderr, "%s: invalid TCP port specified (%ld).\n",
							*argv, port), exit_server();
				else
					_g.port = port;
				break;
			case 'h':
				usage(stdout, NULL);
				exit(EXIT_SUCCESS);
				break;
			default:
				usage(stderr, NULL);
				exit_server();
				break;
		}
	}

	if (optind < argc)
	{
		usage(stderr, "%s: error: superfluous command line arguments.\n", *argv);
		exit_server();
	}

	if ((_g.fd = socket_int()) == -1)
		exit(EXIT_FAILURE);
	if (socket_bind(_g.fd, _g.port, &address) == 0)
		exit_server();
	if (listen(_g.fd, CONNECTION_BACKLOG) == -1)
		exit_server();

	printf("Starting new Server\nAdrress: %s:%d\n", address, _g.port);
	strdel(&address);

	if (loop() == 0)
		exit_server();
	return (0);
}

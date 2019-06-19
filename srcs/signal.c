#include "server.h"

/*
** Exit the server.
*/

void		exit_server(void)
{
	close(g_fd);
	exit(EXIT_FAILURE);
}

/*
** Stop the server when signal SIGINT is send.
*/

void		sigstop(int sig)
{
	(void)sig;
	printf("Stoping Server\n");
	exit(close(g_fd));
}

/*
** Check for terminated processes and collect their exit status.
*/

void		sigchld(int sig)
{
	int	status;
	pid_t   pid;

	(void)sig;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			fprintf(stderr, "%s: abnormal termination of child PID %ju\n",
					__func__, (uintmax_t)pid);
}

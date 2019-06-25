#include "server.h"
#include "response.h"

/*
** Itoa function used on a long unsigned int
*/

static char				*ft_itoa(long unsigned int nb)
{
	long unsigned int	tmp;
	long unsigned int	pow;
	char				*str;

	tmp = nb;
	pow = 1;
	while (tmp /= 10)
		pow++;
	if ((str = (char *)malloc(pow + 1)) == NULL)
		return (NULL);
	str[pow] = 0;
	while (pow--)
	{
		str[pow] = (nb % 10) + '0';
		nb /= 10;
	}
	return (str);
}

/*
** Function used to remove any dprintf in the process of writing to the socket
** fd the HTTP Response Header
**
** Takes a variable number of string arguments (exactly nfields arguments) to
** be written out to fd in sequence.
**
** Returns 1 when successful, 0 otherwise.
*/

static int				ft_write(int fd, size_t nfields, ...)
{
	va_list ap;
	va_start(ap, nfields);
	while (nfields--)
	{
		const char *str = va_arg(ap, const char *);
		if (write(fd, str, strlen(str)) == -1) {
			perror("ft_write: write() error");
			return (0);
		}
	}
	va_end(ap);
	return (1);
}

/*
** Processing to print the Response HTML Header
*/

void					print_header(t_reponse *answer)
{
	char				*tmp;

	tmp = NULL;
	answer->protocol ? ft_write(answer->fd, 2, "HTTP/",
			protocol_version(answer->protocol)) : 0;
	tmp = ft_itoa((long unsigned int)answer->reponse);
	answer->reponse && tmp ? ft_write(answer->fd, 4, " ", tmp, " ",
			get_reponse_message(answer->reponse)) : 0;
	free(tmp);
	tmp = NULL;
	ft_write(answer->fd, 1, CRLF "Connection: close" CRLF);
	if (answer->date) {
		ft_write(answer->fd, 3, "Date: ", answer->date, CRLF);
		ft_write(answer->fd, 3, "Last-Modified: ", answer->date, CRLF);
	}
	answer->content_type ? ft_write(answer->fd, 3, "Content-Type: ",
			answer->content_type, CRLF) : 0;
	answer->file_size ?
		(tmp = ft_itoa((long unsigned int)answer->file_size)) : 0;
	answer->file_size ? ft_write(answer->fd, 3, "Content-Length: ", tmp,
			CRLF) : 0;
	if (tmp)
		free(tmp);
	write(answer->fd, CRLF, 2);
}

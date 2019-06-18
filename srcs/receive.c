#include "server.h"

/*
** Locate the boundary element in the content
** If successful, return a pointer to the boundary.
** Otherwise, a NULL pointer is returned to indicate an error.
*/

uint8_t			*locate(const char *content, size_t size, const char *boundary)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (i < size)
	{
		if (content[i] == boundary[0])
		{
			j = 0;
			while ((i + j) < size && content[i + j] == boundary[j])
				++j;
			if (boundary[j] == '\0')
				return ((uint8_t *)(content + i));
		}
		++i;
	}
	return (NULL);
}

/*
** Copy the buffer into a malloced pointer.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		copy(uint8_t *buff, uint8_t **data, uint8_t **end, ssize_t size)
{
	if ((*data = (uint8_t *)malloc(sizeof(uint8_t) * size)) == NULL)
		return (0);
	memcpy(*data, buff, size);
	*end = locate((const char *)(*data), size, CRLF CRLF);
	memset(*end, 0, sizeof(uint8_t) * 4);
	*end = *end + 4;
	return (1);
}

/*
** Receive the header from the client.
** If successful, the number of bytes actually read is returned.
** Upon reading end-of-file, zero is returned.
** Otherwise, a -1 is returned to indicate an error.
*/

static ssize_t	request_read(int fd, uint8_t **data, uint8_t **end, int *status)
{
	ssize_t		size;
	uint8_t		buff[PAGE_SIZE];

	memset(buff, 0, PAGE_SIZE);
	if ((size = recv(fd, buff, PAGE_SIZE, 0)) > 0)
	{
		if (locate((const char *)buff, size, CRLF CRLF) == NULL)
		{
			*status = response_error(fd, NULL, ENTITY_TOO_LARGE);
			return (0);
		}
		if (copy(buff, data, end, size) == 0)
		{
			*status = response_error(fd, NULL, INTERNAL_SERVER_ERROR);
			return (0);
		}
	}
	else if (size == 0)
		*status = response_error(fd, NULL, NO_CONTENT);
	else
		*status = response_error(fd, NULL, REQUEST_TIME_OUT);
	return (size);
}

/*
** Build the http header and send the appropriate response to the client.
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
**
** DEBUG: write(1, buf, (end - buf));
*/

int				request(int fd, t_http **data, int *status)
{
	int			ret;
	ssize_t		size;
	uint8_t		*buf;
	uint8_t		*end;

	ret = 1;
	if ((size = request_read(fd, &buf, &end, status)) <= 0)
		return (0);
	if ((*data = header(fd, (char *)buf, status)) == NULL)
		return (0);
	if (data && (*data)->method == METHOD_POST && (*data)->content_length)
	{
		size = size - (end - buf);
		ret = content(*data, end, size, status);
	}
	free(buf);
	return (ret);
}

#include "server.h"

/*
** Find the element search and then allocate the content into the variable key
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		get_element(char **str, char *search, char **key)
{
	int		i;
	char	*element;

	if ((element = strstr((const char *)*str, search)) == NULL)
		return (0);
	element = element + strlen(search);
	if (*element == '\"')
		++element;
	i = 0;
	while (element[i] && element[i] != '\"')
		++i;
	*key = strndup(element, i);
	if (element[i] == '\"')
		++i;
	*str = element + i;
	return (1);
}

/*
** Create a file and copy the content inside
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		parse_file(char *file, uint8_t *content, ssize_t size)
{
	int			fd;

	if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0660)) == -1)
		return (0);
	if (write(fd, content, size) != size)
	{
		close(fd);
		remove(file);
		return (0);
	}
	close(fd);
	return (1);
}

/*
** Parse the information contains in the boundary
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

static int		parse_boundary(t_http *data, uint8_t *buff, size_t size)
{
	char		*str;
	char		*key;
	char		*value;
	uint8_t		*content;

	str = (char *)buff;
	if ((content = locate((const char *)buff, size, CRLF CRLF)) == NULL)
		return (0);
	memset(content, 0, sizeof(uint8_t) * 4);
	content = content + 4;
	if (get_element(&str, "name=", &key) == 0)
		return (0);
	if (*str == '\0')
	{
		value = strndup((char *)content, size);
		if (content_add(data, key, value, 0) == 0)
			return (0);
		return (1);
	}
	if (get_element(&str, "filename=", &value) == 0)
		return (0);
	if (content_add(data, key, value, 1) == 0)
		return (0);
	return (parse_file(value, content, (size - (content - buff))));
}

/*
** Parse the multipart/form-data by locate the occurence of the boundary
** If successful, return 1. Otherwise, a 0 is returned to indicate an error.
*/

int				parse_multipart(t_http *data, const uint8_t *ptr, size_t size)
{
	size_t		len;
	char		*boundary;
	uint8_t		*start;
	uint8_t		*end;

	end = (uint8_t *)ptr;
	len = strlen(data->boundary);
	boundary = data->boundary;
	while ((start = locate((const char *)end, size, boundary)) != NULL)
	{
		if (size >= len + 2 && start[len] == '-' && start[len + 1] == '-')
			return (1);
		if ((end = locate((const char *)start + len, size, boundary)) == NULL)
			return (0);
		memset(end - 2, 0, sizeof(uint8_t) * 2);
		if (parse_boundary(data, start, ((end - 2) - start)) == 0)
			return (0);
		size -= (end - start);
	}
	return (0);
}

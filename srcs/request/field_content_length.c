#include "server.h"

int				field_content_length(char *str, t_http *data)
{
	size_t		len;

	len = strtoumax(str, &str, 10);
	if (*str != '\0')
		return (0);
	data->content_length = len;
	return (1);
}

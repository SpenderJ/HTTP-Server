#ifndef REQUEST_H
# define REQUEST_H

# include <stdlib.h>

# include "libft.h"
# include "http.h"

# define PROTOCOL_1_0	1
# define PROTOCOL_1_1	2
# define PROTOCOL_2_0	3

# define METHOD_NUMBER 	4
# define METHOD_GET		1
# define METHOD_HEAD	2
# define METHOD_POST	3
# define METHOD_PUT		4

# define POST_NUMBER	3
# define POST_TYPE_1	1
# define POST_TYPE_2	2
# define POST_TYPE_3	3

typedef struct		s_lookup
{
	char			*name;
	uint8_t			length;
	uint8_t			number;
}					t_lookup;

int					http_free(t_http *data);

t_http				*header(int fd, char *request, int *status);
uint8_t				method(char *method, t_http *data);
int					content(t_http *data, uint8_t *ptr,
						size_t size, int *status);

int					parse_multipart(t_http *data,
						const uint8_t *ptr, size_t size);
int					content_add(t_http *data, char *key,
						char *value, uint8_t flag);
t_content			*content_init(char *key, char *value, uint8_t flag);
void				content_free(t_content *node);
int					content_parse(t_http *data, char *content);
uint8_t				*locate(const char *content,
						size_t size, const char *boundary);

typedef struct		s_fields
{
	char			*name;
	int				(*fct) (char *str, t_http *data);
}					t_fields;

# define FIELDS_COUNT	5

uint8_t				fields_dispatch(char *str, t_http *data);

int					field_accept(char *str, t_http *data);
int					field_content_length(char *str, t_http *data);
int					field_content_type(char *str, t_http *data);
int					field_expect(char *str, t_http *data);
int					field_host(char *str, t_http *data);

#endif

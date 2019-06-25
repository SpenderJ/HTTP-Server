#ifndef RESPONSE_H
# define RESPONSE_H

# include <netdb.h>
# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <time.h>
# include <fcntl.h>

# include "libft.h"
# include "http.h"

# define STATUS_COUNT			20

typedef struct		s_response
{
	char			*complete_path;
	int			reponse;
	uint8_t			protocol;
	int			fd;
	int			file_fd;
	uint64_t		file_size;
	char			*content_type;
	char			*date;
}			t_reponse;

/*
** Environment variables
*/

# define IER				500
# define DEFAULT_WEBDIR_PATH		"site"
# define ERROR_DIR_PATH	        	"error_pages/"
# define API_DIR_PATH			"api"

/*
** SUCCESS
*/

# define OK				200
# define CREATED			201
# define ACCEPTED			202
# define NO_CONTENT			204
# define PARTIAL_CONTENT		206

/*
** Redirections
*/

# define MULTIPLE_CHOICES		300
# define MOVED_PERMANENTLY		301
# define FOUND				302
# define PERMANENT_REDIRECT		308

/*
** Request Error
*/

# define BAD_REQUEST			400
# define UNAUTHORIZED			401
# define FORBIDDEN			403
# define NOT_FOUND			404
# define METHOD_NOT_ALLOWED		405
# define REQUEST_TIME_OUT		408
# define ENTITY_TOO_LARGE		413

/*
** Server Error
*/

# define INTERNAL_SERVER_ERROR		500
# define NOT_IMPLEMENTED		501
# define BAD_GATEWAY 			502
# define SERVICE_UNAVAILABLE		503

/*
** Structure for error message depending return value
*/

typedef struct		s_status
{
	int			code;
	const char		*name;
}			t_status;

extern t_status		g_status[STATUS_COUNT];

/*
** Response function definition
*/

int				response(t_http *request, int fd);
char				*protocol_version(uint8_t protocol);
int				check_content_type(t_http *request, char *complete_path);
char				*get_content_type(t_http *request, char *complete_path);
int				response_error(int fd, t_http *data, int reponse);
int				end_connection_success(t_http *request, int reponse, int fd,
					t_reponse *answer);
int				end_connection_error(t_http *request, int reponse, int fd,
					t_reponse *answer);
const char			*get_reponse_message(int reponse);
void				print_header(t_reponse *answer);

/*
** Function used to create valid header
*/

t_reponse			*reponse_init(void);
char				*get_date(void);
int				reponse_free(t_reponse *answer);
int				ft_free(char *to_free);

/*
** API functions
*/

int				post_api(void);

#endif

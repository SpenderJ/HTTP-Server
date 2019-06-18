#include "server.h"
#include "api.h"
#include "response.h"

#include <time.h>
#include <fcntl.h>

/*
** Return value of the different method implemented
** Get method -> 1
** Head method -> 2
** Post method -> 3
** Put method -> 4
**
** Ip of the server of dev:
** 127.0.0.1:6060
**
** Function to get the current date for the response header
** In case of malloc error will return an error 500
** Returns an allocated char *
*/

char				*get_date(void)
{
	char			date[32];
	static const char	* const weekday[7] = { "Sun","Mon","Tue",
						"Wed","Thr","Fri","Sat" };
	static const char	* const month[12] = { "Jan","Feb","Mar","Apr",
						"May","Jun","Jul","Aug","Sep",
						"Oct","Nov","Dec" };
	time_t			t;
	struct tm		*tm;

	time(&t);
	tm = gmtime(&t);
	snprintf(date, sizeof date, "%s, %02d %s %04d %02d:%02d:%02d GMT",
			weekday[tm->tm_wday], tm->tm_mday, month[tm->tm_mon],
			tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
			tm->tm_sec);
	date[sizeof date-1] = '\0';
	return strdup(date);
}

/*
** Initializing response structure not in use right now
*/

t_reponse			*reponse_init(void)
{
	t_reponse	*answer;

	if ((answer = (t_reponse *)malloc(sizeof(t_reponse))) == NULL)
		return (NULL);
	memset(answer, 0, sizeof(t_reponse));
	answer->file_fd = -1;
	return (answer);
}

/*
** Print the reponse message linked to the value of the reponse sent
*/

t_status			g_status[STATUS_COUNT] = {

	{OK, "OK"},
	{CREATED, "Created"},
	{ACCEPTED, "Accepted"},
	{NO_CONTENT, "No Content"},
	{PARTIAL_CONTENT, "Partial Content"},

	{MULTIPLE_CHOICES, "Multiple Choices"},
	{MOVED_PERMANENTLY, "Moved Permanently"},
	{FOUND, "Found"},
	{PERMANENT_REDIRECT, "Permanent redirected (experimental)"},

	{BAD_REQUEST, "Bad Request"},
	{UNAUTHORIZED, "Unauthorized"},
	{FORBIDDEN, "Forbidden"},
	{NOT_FOUND, "Not Found"},
	{METHOD_NOT_ALLOWED, "Method Not Allowed"},
	{REQUEST_TIME_OUT, "Request Timeout"},
	{ENTITY_TOO_LARGE, "Request Entity Too Large"},

	{INTERNAL_SERVER_ERROR, "Internal Server Error"},
	{NOT_IMPLEMENTED, "Not Implemented"},
	{BAD_GATEWAY, "Bad Gateway"},
	{SERVICE_UNAVAILABLE, "Service Unavailable"},

};

/*
** Find the good response message depending on the value sent and return it
*/

char				*get_reponse_message(int reponse)
{
	int				i;

	i = -1;
	while (++i <= SERVICE_UNAVAILABLE)
		if (g_status[i].code == reponse)
			return (g_status[i].name);
	return ("Unhandled error");
}

/*
** Checking if the called URL is not present in the API folder
** Add your APIS in this array and modify the API_COUNT in api include file
*/

t_api				g_api[API_COUNT] = {
	{"demo", &demo}
};

/*
** For any API, prepare the response and launch the called API with the
** header sent by the HTTP and a structure answer containing most of your
** desired header
*/

static int			start_api_response(t_http *request, t_reponse *answer)
{
	int				n;
	int				i;
	int				fd;

	i = -1;
	fd = answer->fd;
	n = strlen(request->path);
	while (n > 0 && request->path[n] != '/')
		--n;
	n += 1;
	while (++i < API_COUNT)
		if (strcmp(&request->path[n], g_api[i].name) == 0
				&& (answer->reponse = g_api[i].fct(request, answer)))
			break ;
	if (i >= API_COUNT && reponse_free(answer) == 0)
		return (response_error(fd, request, NOT_FOUND));
	i = answer->reponse;
	http_free(request) ? reponse_free(answer) : reponse_free(answer);
	return (i);
}

/*
** Main thread for every call to the server, will transform the request to
** a reponse after having perfeomed it
** Returns an int matching the response answer.
*/

int					response(t_http *request, int fd)
{
	struct stat		sb;
	t_reponse		*answer;
	char			*concatted;

	if (!request)
		return (end_connection_error(request, BAD_REQUEST, fd, NULL));
	if ((answer = reponse_init()) == NULL)
		return (end_connection_error(request, IER, fd, NULL));
	answer->fd = fd;
	if (request->method == 0 || request->method > METHOD_PUT)
		return (end_connection_error(request, NOT_IMPLEMENTED, fd, answer));
	concatted = concat(WEBSITE_FOLDER_PATH, request->path);
	if ((!request->path || stat(concatted, &sb) == -1) && ft_free(concatted)
			== 0)
		return (start_api_response(request, answer));
	ft_free(concatted);
	return (end_connection_success(request, OK, fd, answer));
}

#ifndef SERVER_H
# define SERVER_H

# include "libft.h"
# include "http.h"
# include "request.h"
# include "response.h"
# include "api.h"

/*
**  Defined in server.c
*/
extern struct s_globalstate {
	int 			fd;
	unsigned		port;
	const char		*webdir;
	const char		*argv0;
}	_g;


#endif /* SERVER_H */

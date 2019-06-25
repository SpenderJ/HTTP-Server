# HTTP Server in C

This project is a [HTTP](https://tools.ietf.org/html/rfc2616) server in C.

## Getting Started

The goal of this project is to understand the HTTP protocol and its internals by implementing a whole server in C using only the standard C library.

### Installing

To build the server just go to the root directory of the cloned repository and:

```bash
$ git clone https://github.com/maxencejded/HTTP-Server.git server
$ cd server
$ make
$ ./server
Starting new Server
Adrress: 0.0.0.0:6060
```

The `Makefile` makes use of GNU-specific features. On BSDs make sure to install GNU make from pkgsrc/ports and invoke it as `gmake`.

If you want to add your website, you'll be expected to insert it in the `site` subdirectory. The file `index.html` should be at the root of the subdirectory.

### API
If you are willing to add APIs, you'll have to create them in the `api` subdirectory:

```bash
$ cd api/
```
The demo file is an api to show you how you should format your content.
Don't forget that you will have to print the HTTP header by yourself.
Finally, you are expected to add the name in the API array. Don't forget to increase the size of the number of ``API_COUNT``.

```bash
$ vim includes/api.h
```
```C
# define API_COUNT 1 /* NUMBER OF APIs */

int        demo(t_http *request, t_reponse *answer);
/* Add your functions here */
```

## Running Test

Tests will be available soon. They are being written.

## Deployment
### Command line options
```
Usage: ./server [OPTIONS]                                                       │
                                                                                │
OPTIONS:                                                                        │
       -w <DIR>        Web server directory (default: site)                     │
       -p <PORT>       TCP port of server listen socket (default: 6060)         │
       -h              Produces this information to stdout.
 ```
 
### Compatibility
So far tested to be compatible with macOS (`darwin x86_64`), Linux (both `x86_64` and ARMv7) and NetBSD as well as Android ([NDK](https://developer.android.com/ndk), [bionic libc](https://android.googlesource.com/platform/bionic).\
We are currently working on making it available for other UNIX systems.

## Built With

* [GCC](https://gcc.gnu.org/) and [clang](https://clang.llvm.org) - Tested compilers
* [42 Norme](https://github.com/Nqsir/The-Norm-42/blob/master/norme.fr.pdf) - Norme code guidelines for C

## Authors

* **Jules Spender** - *Initial work* - [spenderj](https://github.com/spenderj)
* **Maxence Jacques de Dixmude** - *Initial work* - [maxencejded](https://github.com/maxencejded)

See also the list of [contributors](CONTRIBUTORS.md) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## Acknowledgments

HTTP protocol, Parsing, Dynamic memory management, Optimization, Debugging, Error management


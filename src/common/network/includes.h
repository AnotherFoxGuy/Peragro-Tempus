#ifndef INCLUDES_H
#define INCLUDES_H

#undef FD_SETSIZE
#define FD_SETSIZE 128

#ifdef WIN32
  #include <winsock.h>
  #define socklen int
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <netdb.h>
  #include <sys/ioctl.h>
  #include <errno.h>
  #define socklen socklen_t
#endif

#endif

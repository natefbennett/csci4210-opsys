/* tcp-server-iterative.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXBUFFER 1024

int main()
{
  /* Create the listener socket as TCP socket (SOCK_STREAM) */
  int listener = socket( AF_INET, SOCK_STREAM, 0 );
    /* here, the listener is a socket descriptor (part of the fd table) */

  if ( listener == -1 ) { perror( "socket() failed" ); return EXIT_FAILURE; }

  /* populate the socket structure for bind() */
  struct sockaddr_in tcp_server;
  tcp_server.sin_family = AF_INET;  /* IPv4 */

  tcp_server.sin_addr.s_addr = htonl( INADDR_ANY );
    /* allow any IP address to connect */

  unsigned short port = 8123;

  /* htons() is host-to-network short for data marshalling */
  /* Internet is big endian; Intel is little endian; etc.  */
  tcp_server.sin_port = htons( port );

  int length = sizeof( tcp_server );

  /* attempt to bind (or associate) port 8123 with the socket */
  if ( bind( listener, (struct sockaddr *)&tcp_server, length ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }


  /* identify this port as a TCP listener port */
  if ( listen( listener, 5 ) == -1 )
  {
    perror( "listen() failed" );
    return EXIT_FAILURE;
  }

  printf( "SERVER: TCP listener socket (fd %d) bound to port %d\n", listener, port );


  while ( 1 )
  {
    struct sockaddr_in remote_client;
    int addrlen = sizeof( remote_client );

    printf( "SERVER: Blocked on accept()\n" );
    int newsd = accept( listener, (struct sockaddr *)&remote_client, (socklen_t *)&addrlen );
    if ( newsd == -1 ) { perror( "accept() failed" ); continue; }

    /* the listener variable above is the listener socket descriptor that
        we use to accept new incoming client connections on port 8123 */

    /* the newsd variable is the means of communicating via recv()/send()
        or read()/write() calls with the connected remote client */

    printf( "SERVER: Accepted new client connection on newsd %d\n", newsd );

    /* we have successfully established a TCP connection between server
        and a remote client; below implements the application protocol */

    int n;

    do
    {
      char buffer[MAXBUFFER+1];

      printf( "SERVER: Blocked on recv()\n" );

      /* recv() call will block until we receive data (n > 0)
          or an error occurs (n == -1)
           or the client closed its socket (n == 0) */
      n = recv( newsd, buffer, MAXBUFFER, 0 );   /* or read() */

      if ( n == -1 )
      {
        perror( "recv() failed" );
        return EXIT_FAILURE;
      }
      else if ( n == 0 )
      {
        printf( "SERVER: Rcvd 0 from recv(); closing descriptor %d...\n", newsd );
      }
      else /* n > 0 */
      {
        buffer[n] = '\0';   /* assume this is text data */
        printf( "SERVER: Rcvd message (%d bytes) from %s: [%s]\n", n,
                 inet_ntoa( (struct in_addr)remote_client.sin_addr ),
                 buffer );

        printf( "SERVER: Sending acknowledgement to client\n" );
        /* send OK message back to client */
        n = send( newsd, "OK\n", 3, 0 );
        if ( n == -1 ) { perror( "send() failed" ); return EXIT_FAILURE; }
      }
    }
    while ( n > 0 );

    /* at this point, the remote client side has closed its socket */

    close( newsd );
  }

  close( listener );

  return EXIT_SUCCESS;
}

#if 0

   SERVER                                     CLIENT
  +-------------+                           +-----------+
  |            ===80 (HTTP)                 |           |
  |             |                           |           |
  |     fd 3   ===8123                      |           |
  |             |  accept()                 |           |
  |             |                           |           |
  |             |                           |           |
  |             |                           +-----------+
  +-------------+


   SERVER                                     CLIENT
  +-------------+                           +-----------+
  |            ===80 (HTTP)                 |           |
  |             |                           |           |
  |     fd 3   ===8123 <==============41634===connect() |
  |             |  accept()                 |           |
  |             |                           |           |
  |             |                           |           |
  |             |                           +-----------+
  +-------------+


   SERVER                                     CLIENT
  +-------------+                           +-----------+
  |            ===80 (HTTP)                 |           |
  |             |                           |           |
  |     fd 3   ===8123                      |           |
  |             |  accept()                 |           |
  |             |                           |           |
  |  newsd 4   ===8123================41634===read()    |
  |             |                           | write()   |
  |             |                           | recv()    |
  |             |                           | send()    |
  |             |                           +-----------+
  +-------------+

 uniquely identify a connection:
   (TCP, SERVER IP, SERVER PORT 8123, CLIENT IP, CLIENT PORT 41634)

#endif

/* udp-server.c */

/* UDP SERVER example
   -- socket()             create a socket (endpoint) for communication
   -- bind()               bind to (assign) a specific port number
                           (or let the OS assign us a port number)
   -- getsockname()        get socket "name" -- IP address, port number, etc.
   -- recvfrom()/sendto()  receive/send datagrams
*/

/* To test this server, you can use the following
   command-line netcat tool:


   bash$ netcat -u linux02.cs.rpi.edu 41234
                   ^^^^^^
                   replace with your hostname

   Note that netcat will act as a client to this UDP server...

   The hostname (e.g., linux02.cs.rpi.edu) can also be
   localhost (127.0.0.1); and the port number must match what
   the server reports.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXBUFFER 512

int CountChar( char data[], int size, char target )
{
  int count = 0;

  for ( int i = 0; i < size; i++ )
  {
    if ( data[i] == target )
        count++;
  }

  return count;
}

int main( int argc, char * argv[] )
{
  // NOTE: for Submitty (auto-grader) use only
  setvbuf( stdout, NULL, _IONBF, 0 );

  int port = 0;

  // take in port number and validate
  // if no argument is supplied it will default to 0
  // and the system will assign a port number
  if ( argc > 2 || argc < 1 )
  {
    fprintf( stderr, "ERROR: Invalid argument\nUSAGE: a.out <port>\n" );
    return EXIT_FAILURE;
  }
  else if ( argc == 2 )
  {
    int argv1 = atoi( argv[1] );
    
    // validate as port number
    if ( argv1 >= 1024 && argv1 <= 65535 )
      port = argv1;
    else
      fprintf( stderr, "ERROR: Invalid port number, system will assign valid port...\n" );
  }

  int sd;  /* socket descriptor -- this is actually in the fd table! */

  /* create the socket (endpoint) on the server side */
  sd = socket( AF_INET, SOCK_DGRAM, 0 );
  if ( sd == -1 ) { perror( "socket() failed" ); return EXIT_FAILURE; }

  printf( "Server-side UDP socket created on descriptor %d\n", sd );

  struct sockaddr_in udp_server;
  int length = sizeof( udp_server );

  udp_server.sin_family = AF_INET;  /* IPv4 */

  udp_server.sin_addr.s_addr = htonl( INADDR_ANY );
           /* any remote IP can send us a datagram */


  /* specify the port number for the server */
  udp_server.sin_port = htons( port );  /* a 0 here means let the kernel assign
                                         us a port number to listen on */

  /* bind to a specific (OS-assigned) port number */
  if ( bind( sd, (struct sockaddr *)&udp_server, length ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }


  /* call getsockname() to obtain the port number that was just assigned */
  if ( getsockname( sd, (struct sockaddr *)&udp_server, (socklen_t *)&length ) == -1 )
  {
    perror( "getsockname() failed" );
    return EXIT_FAILURE;
  }

  printf( "UDP server bound to port number %d\n", ntohs( udp_server.sin_port ) );


  /* the code below implements the application protocol */
  while ( 1 )
  {
    char buffer[MAXBUFFER+1];
    struct sockaddr_in remote_client;
    int addrlen = sizeof( remote_client );

    /* read a datagram from the remote client side (BLOCKING) */
    int n = recvfrom( sd, buffer, MAXBUFFER, 0,
                      (struct sockaddr *)&remote_client, (socklen_t *)&addrlen );

    if ( n == -1 ) { perror( "recvfrom() failed" ); continue; }

    printf( "Rcvd datagram from %s port %d\n",
            inet_ntoa( remote_client.sin_addr ), ntohs( remote_client.sin_port ) );

    printf( "RCVD %d bytes\n", n );
    buffer[n] = '\0';   /* assume that its printable char[] data */
    printf( "RCVD: [%s]\n", buffer );
    
    // count number of 'G' present and data martial to network format
    int num_G = htonl( CountChar( buffer, n, 'G' ) );

    /* echo the first 4 bytes (at most) plus '\n' back to the sender/client */
    // if ( n > 4 ) { n = 5; buffer[4] = '\n'; }
    // sendto( sd, buffer, n, 0, (struct sockaddr *)&remote_client, addrlen );

    sendto( sd, &num_G, 4, 0, (struct sockaddr *)&remote_client, addrlen );
    /* TO DO: check the return value from sendto() */
  }

  close( sd );

  return EXIT_SUCCESS;
}
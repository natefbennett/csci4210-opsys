/* tcp-client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define MAXBUFFER 1024

int main( int argc, char * argv[] )
{
  setvbuf( stdout, NULL, _IONBF, 0 );
 
  if ( argc < 5 )
  {
    fprintf( stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <server-hostname> <server-port> <n> <int-value-1> ... <int-value-n>\n" );
    return EXIT_FAILURE;
  }

  // read in command line arguments
  char * server_hostname = argv[1];
  unsigned short server_port = atoi( argv[2] );
  int n = atoi( argv[3] );
  int data[n+1];
  data[0] = htonl( n );
  for ( int i = 0; i < n; i++ )
      data[i+1] = htonl( atoi( argv[i+4] ) );
  
  /* create TCP client socket (endpoint) */
  int sd = socket( AF_INET, SOCK_STREAM, 0 );
  if ( sd == -1 ) { perror( "socket() failed" ); exit( EXIT_FAILURE ); }


  struct hostent * hp = gethostbyname( server_hostname );  /* 127.0.0.1 */

  if ( hp == NULL )
  {
    fprintf( stderr, "ERROR: gethostbyname() failed\n" );
    return EXIT_FAILURE;
  }

  struct sockaddr_in tcp_server;
  tcp_server.sin_family = AF_INET;
  memcpy( (void *)&tcp_server.sin_addr, (void *)hp->h_addr, hp->h_length );
  // unsigned short server_port = 8123;
  tcp_server.sin_port = htons( server_port );

  // printf( "CLIENT: TCP server address is %s\n", inet_ntoa( tcp_server.sin_addr ) );


  
  if ( connect( sd, (struct sockaddr *)&tcp_server, sizeof( tcp_server ) ) == -1 )
  {
    perror( "connect() failed" );
    return EXIT_FAILURE;
  }
  printf( "CLIENT: Successfully connected to server\n" );


  /* The implementation of the application protocol is below... */

  // char * msg = "why not change the world?";
  if ( n == 1 )
    printf( "CLIENT: Sending 1 integer value\n" );
  else
    printf( "CLIENT: Sending %d integer values\n", n );

  int write_ret = write( sd, data, (n+1)*sizeof( int ) );    /* or use send()/recv() */
  if ( write_ret == -1 ) { perror( "write() failed" ); return EXIT_FAILURE; }

  int int_result;
  int read_ret = read( sd, &int_result, sizeof( int ) );    /* BLOCKING */

  char secret_message[MAXBUFFER+1];
  read_ret = read( sd, secret_message, MAXBUFFER );    /* BLOCKING */

  if ( read_ret == -1 )
  {
    perror( "read() failed" );
    return EXIT_FAILURE;
  }
  else if ( read_ret == 0 )
  {
    printf( "CLIENT: Rcvd no data; TCP server socket was closed\n" );
  }
  else  /* n > 0 */
  {
    printf( "CLIENT: Rcvd result: %d\n", ntohl( int_result ) );
    secret_message[read_ret] = '\0';    /* assume we rcvd text data */
    printf( "CLIENT: Rcvd secret message: \"%s\"\n", secret_message );
  }

  printf( "CLIENT: Disconnected from server\n" );

  close( sd );

  return EXIT_SUCCESS;
}
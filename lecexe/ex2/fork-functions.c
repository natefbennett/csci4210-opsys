
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

char lecex2_child( int n )
{
    // check if n is negative
    if ( n < 0 ) {
        perror( "ERROR: n is negative\n" );
        return EXIT_FAILURE; 
    }
    
    // open file
    int f_read = open( "data.txt", O_RDONLY );

    // check if file can be opened
    if ( f_read == -1 )
    {
        perror( "ERROR: open() failed\n" );
        return EXIT_FAILURE; 
    }

    // extract the nth char in file
    char cur_char;
    if ( lseek( f_read, n-1, SEEK_CUR ) )
    {
      if ( read( f_read, &cur_char, 1 ) )
      {
        if ( cur_char != '\n' || cur_char != '\0' )
        {
          close( f_read );
          return cur_char;
        }
      }
    }
 
    // all other cases result in failure
    perror( "ERROR: could not read nth character\n" );
    close( f_read );
    return EXIT_FAILURE;
}

char lecex2_parent()
{
  int status;
  waitpid(-1, &status, 0);
  
  if ( !WIFEXITED(status) || WEXITSTATUS(status) == 1 )
  {
    printf( "PARENT: child process terminated abnormally!\n" );
    return EXIT_FAILURE;
  }
  printf( "PARENT: child process returned \'%c\'\n", WEXITSTATUS(status) ); 
  return EXIT_SUCCESS;
}

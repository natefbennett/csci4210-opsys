
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
  int isdone = 0;
  pid_t child_pid, wait_return;

  printf( "PARENT: start here!\n" );
  fflush( stdout );

  child_pid = fork();
  wait_return = waitpid(child_pid, &isdone, WUNTRACED);

  // code the child process will run
  if ( child_pid == 0 )
  {
    printf( "CHILD: happy birthday to me!\n" );
    printf( "CHILD: idk......good-bye!\n" );
    //return EXIT_SUCCESS;
  }
  // code that will run for parent process
  else 
  {
    
    
    if ( wait_return == -1 ) 
    {
      fprintf( stderr, "error\n" );
    }
    else if ( wait_return == child_pid ) 
    {
      printf("PARENT: child process terminated.\n" );
      printf("PARENT: sigh, i'm gonna miss that little child process.\n" );
    }
  }

  return EXIT_SUCCESS;
}

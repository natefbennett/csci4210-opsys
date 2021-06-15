
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
  int isdone1, isdone2;
  isdone1 = isdone2 = 0;
  pid_t child_pid1, child_pid2, 
        wait_return1, wait_return2;

  printf( "PARENT: start here!\n" );
  fflush( stdout );
  
  // make child a and await return
  child_pid1 = fork();
  wait_return1 = waitpid(child_pid1, &isdone1, WUNTRACED);

  // code the child a will run
  if ( child_pid1 == 0 )
  {
    printf( "CHILD A: happy birthday to me!\n" );
    printf( "CHILD A: idk......good-bye!\n" );
  }
  else 
  {
    // make child b and await return
    child_pid2 = fork();
    wait_return2 = waitpid(child_pid2, &isdone2, WUNTRACED);
    
    // code child b will run
    if ( child_pid2 == 0 )
    {
      printf( "CHILD B: and happy birthday to me!\n" );
      printf( "CHILD B: nothing to do.\n" );
    }
    // code parent will run
    else 
    {
      if ( wait_return1 == -1 || wait_return2 == -1 ) 
      {
        fprintf( stderr, "error\n" );
      }
      else if ( wait_return1 == child_pid1 && wait_return2 == child_pid2  ) 
      {
        printf("PARENT: both child processes terminated.\n" );
        printf("PARENT: end here!\n" );
      }
    }
  }

  return EXIT_SUCCESS;
}

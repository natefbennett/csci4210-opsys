#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

struct stat fstat; // for getting size of file

int ChildPatternCount ( char * pattern, char * chunk )
{
    int count;


    return count;
}

int main ( int argc, char ** argv )
{
    setvbuf( stdout, NULL, _IONBF, 0 ); // for Submitty (auto-grader) use only

    // check command line arguments
    if ( argc < 3 )
    {
        fprintf( stderr, "ERROR: Too few arguments\n" );
        return EXIT_FAILURE;
    }
    
    // number of child processes
    int n = atoi( *(argv + 1) );
    // string to count
    char * s = *( argv + 2 );
    // input file name
    char * filename = "data.txt";

    // get size of input file
    stat( filename, &fstat );
    int filesize = fstat.st_size;
    // get number of bytes per child
    int childchunksize = filesize / n;
    int l_childchunksize = filesize - ( ( childchunksize - 1 ) * n );

    printf( "childchunksize :>> %d\n", childchunksize ); // DEBUG
    printf( "l_childchunksize :>> %d\n", l_childchunksize ); // DEBUG
    printf( "filesize :>> %d\n", filesize ); // DEBUG
    printf( "s :>> %s\n", s ); // DEBUG

    pid_t ** pidarray = calloc( n+1, sizeof( pid_t ) );

    // create n children
    for ( int i = 0; i < n; i++ )
    {
        pid_t pid = fork();
        
        if ( pid == -1 )
        {
            perror( "fork() failed" );
            return EXIT_FAILURE;
        }

        // child code
        if ( pid == 0 )
        {
            // open(), read(), lseek(), close()
            // "Found 4 occurrences of "th" in byte range 800-999"
        }
        // parent code
        else /* p > 0 */
        {
            // parent
            &(pidarray + i) = pid;
        }
    }

    // set last slot NULL
    *(pidarray+n) = NULL;
    int wstatus;

    // if parent wait for all children
    if ( getpid() > 0 )
    {
        pid_t ** itr = pidarray;
        while ( *itr != NULL )
        {
            waitpid( *itr, &wstatus, 0 );
            itr++;
        }
    }


    return EXIT_SUCCESS;
}
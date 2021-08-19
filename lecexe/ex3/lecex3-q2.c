#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * copy_file( void * arg );

int main ( int argc, char ** argv )
{
    setvbuf( stdout, NULL, _IONBF, 0 ); // for Submitty (auto-grader) use only

    int total_bytes_copied = 0;

    // count number of files to copy
    int num_files = 0;
    char ** itr = argv + 1;
    while ( *itr != NULL )
    {
        num_files++;
        itr++;
    }

    // track children ids
    pthread_t * tid = calloc( num_files, sizeof( pthread_t ) );

    // loop thourgh input files and create child threads
    itr = argv + 1;
    int rc, i = 0;
    while ( i < num_files )
    {
        rc = pthread_create( (pthread_t * )(tid+i), NULL, copy_file, *itr );
        
        if ( rc != 0 )
        {
            fprintf( stderr, "pthread_create() failed (%d)\n", rc );
        }

        printf( "MAIN: Creating thread to copy \"%s\"\n", *itr );
        i++;
        itr++;
    }

    // loop through threads and call pthread_join()
    i = 0;
    while ( i < num_files )
    {
        int * x;
        int status = pthread_join( *(tid+i), ( void ** )&x );
        if ( status != 0 )
        {
            perror( "pthread_join() failed" );
            return EXIT_FAILURE;
        }

        printf( "MAIN: Thread completed copying %d bytes for \"%s\"\n", *x, *( argv+1+i ) );
        total_bytes_copied += *x;
        i++;
    }

    free( tid );
    
    if ( num_files == 1 )
    {
        printf( "MAIN: Successfully copied %d bytes via 1 child thread\n", total_bytes_copied );
    }
    else
    {
        printf( "MAIN: Successfully copied %d bytes via %d child threads\n", total_bytes_copied, num_files );
    }
    
    return EXIT_SUCCESS;
}
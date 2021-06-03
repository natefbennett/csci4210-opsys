#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main( int argc, char ** argv )
{
    // check command line arguments
    if ( argc != 3 )
    {
        fprintf( stderr, "ERROR: Invalid arguments\n" );
        fprintf( stderr, "USAGE: a.out <n> <filename>\n" );
        return EXIT_FAILURE;
    }

    // get command line args
    int n = atoi(*(argv+1));
    char * filename = *(argv+2);

    // open file
    int f_read = open( filename, O_RDONLY );

    // test file open from class notes
    if ( f_read == -1 )
    {
        perror( "open() failed" );
        return EXIT_FAILURE; 
    }

    // read file and add every nth byte to buffer
    char * write_buffer = calloc( 1, sizeof( char ) );
    char * write_buffer_pos = write_buffer;
    char cur_char;
    int count = 1;
    int first_time = 1;

    // read chars from the file
    while ( read( f_read, &cur_char, 1 ) )
    {   
        // go to first occurence of nth byte
        if ( first_time ) 
        {
            first_time = 0;
            lseek( f_read, n-1, SEEK_SET ); // move position to first nth byte
        }

        // record last nth byte found and go to next
        else
        {
            *write_buffer_pos = cur_char;
            count++;

            // allocate more space for next nth byte
            write_buffer = realloc( write_buffer, count );
            write_buffer_pos = write_buffer + count - 1;

            lseek( f_read, n-1, SEEK_CUR ); // move position to next nth byte
        }
    }

    // close file
    close( f_read );

    // print results
    printf( "%s\n", write_buffer );
    fflush(stdout); // send buffered printf to stdout

    // free memory used
    free( write_buffer );

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

struct Cache
{
    void ** heap_data; // array of void pointers
    char * heap_type_meta_data; // '\0': null; 's': string; 'i': integer
    int size;
};

// checks if parameter is a "valid" string
// valid strings return 1 (true), invalid returns 0 (false)
int IsString ( char * data )
{
    char * itr = data;
    while ( *itr != '\0' )
    {
        if ( !isalpha(*itr) ) 
        {
            return 0;
        }
        itr++;
    }

    int length = itr - data;

    if ( length < 3 )
    {
        return 0;
    }

    return 1;
}

// checks if parameter is an integer
// valid integers return 1 (true), invalid returns 0 (false)
int IsInteger ( char * data )
{
    char * itr = data;
    while ( *itr != '\0' )
    {
        if ( !isdigit( *itr ) ) 
        {
            return 0;
        }
        itr++;
    }

    // check if length of int positive nonzero
    if ( itr - data )
    {
        return 1;
    }

    return 0;
}

// sum the ascii values of each char and find remainder when divided by cache size
int HashString ( char * data, int cache_size )
{
    char * itr = data;
    int sum = 0;
    while ( *itr != '\0' )
    {
        sum += ( int )( *itr ); // get ascii value of char and add to sum
        itr++;
    }

    int cache_index = sum % cache_size;
    
    return cache_index;
}

int HashInteger ( int data, int cache_size )
{
    int cache_index = data % cache_size;
    
    return cache_index;
}

// insert or update data in cache
// input: pointer to input data and pointer to cache
void CachePut ( char * data, struct Cache * cache )
{
    int cache_index;

    // check data type (string/int) if not throw error
    if ( IsString( data ) )
    {
        cache_index = HashString( data, cache->size );
        void ** index_ptr = cache->heap_data + cache_index;
        char * meta_ptr = cache->heap_type_meta_data + cache_index;

        // cache slot empty - calloc
        if ( *index_ptr == NULL ) {

            *index_ptr = calloc( strlen( data ) + 1, sizeof( char ) );
            strcpy( *index_ptr, data );
            printf( "Word \"%s\" ==> %d (calloc)\n", data, cache_index);
        }
        // collision - realloc
        else {
            *index_ptr = realloc( *index_ptr, strlen( data ) + 1 );
            strcpy( *index_ptr, data );
            printf( "Word \"%s\" ==> %d (realloc)\n", data, cache_index);
        }

        *meta_ptr = 's'; // set data for meta array to track type
    }
    else if ( IsInteger( data ) )
    {
        int data_int = atoi( data );
        cache_index = HashInteger( data_int, cache->size );
        void ** index_ptr = cache->heap_data + cache_index;
        char * meta_ptr = cache->heap_type_meta_data + cache_index;

        // cache slot empty - calloc
        if ( *index_ptr == NULL ) {
            *index_ptr = calloc( 1, sizeof( int ) );
            memcpy( *index_ptr, &data_int, sizeof( int ) );
            printf( "Integer \"%s\" ==> %d (calloc)\n", data, cache_index);
        }
        // collision - realloc
        else {
            *index_ptr = realloc( *index_ptr, sizeof( int ) );
            memcpy( *index_ptr, &data_int, sizeof( int ) );
            printf( "Integer \"%s\" ==> %d (realloc)\n", data, cache_index);
        }

        *meta_ptr = 'i';// set data for meta array to track type
    }

    fflush(stdout); // send buffered printf to stdout

}

void PrintCache ( struct Cache * cache )
{
    int itr = 0;
    while ( itr < cache->size )
    {
        if ( *(cache->heap_data + itr) != NULL ) 
        {
            // if string
            if ( *(cache->heap_type_meta_data + itr) == 's' ) 
            {
                char * data = *(cache->heap_data + itr);
                printf( "Cache index %d ==> \"%s\"\n", itr, data );
                fflush(stdout); // send buffered printf to stdout
            }
            // if integer
            else if ( *(cache->heap_type_meta_data + itr) == 'i' )
            {
                int * data = *(cache->heap_data + itr);
                printf( "Cache index %d ==> int %d\n", itr, *data );
                fflush(stdout); 
            }
        }

        itr++;
    }
    
}

// helper function to clear out buffer
void NullBuffer ( char * buffer )
{
    char * itr = buffer;
    int count = 0;
    while ( count < 128 )
    {
        *( itr ) = '\0';
        count++;
        itr++;
    }
}

// input: opened file and a cache instance
// goes over every char in file and identifies strings and integers
// puts found data in cache as it goes
void ParseFile ( int file, struct Cache * cache )
{
    char cur_char;
    char prev_char = '\0';
    int count = 1;
    int reading = 0;
    char * buffer = calloc( 128, sizeof( char ) ); // stores currents string or integer

    // read chars from the file
    while ( read( file, &cur_char, 1 ) )
    {
        if ( isalpha( cur_char ) || isdigit( cur_char ) )
        {
    
            // check if types match and last char wasnt a delimeter
            if ( isalpha( prev_char ) != isalpha( cur_char ) && prev_char != '\0' )
            {
                fprintf( stderr, "ERROR: Mix of letters and numbers encountered\n" );
                return exit(1);
            }

            reading = 1;

            // valid addition, add to buffer
            count++;
            *( buffer + count - 2 ) = cur_char;
            *( buffer + count - 1 ) = '\0';
            prev_char = cur_char;
            
        }
        // put buffer in cache
        else 
        {
            if ( reading )
            {
                CachePut( buffer, cache );
                prev_char = '\0';
                count = 1;
                reading = 0;
                NullBuffer(buffer);
            }
        }
    }

    free( buffer );
}

void FreeCache ( struct Cache * cache )
{
    int itr = 0;
    while ( itr < cache->size )
    {
        if ( *(cache->heap_data + itr) != NULL ) 
        {
            free( *(cache->heap_data + itr) );
        }

        itr++;
    }
    free( cache->heap_type_meta_data );
    free( cache->heap_data ); 
    free( cache );
}

int main( int argc, char ** argv )
{
    setvbuf( stdout, NULL, _IONBF, 0 ); // for Submitty (auto-grader) use only

    // check command line arguments
    if ( argc < 3 )
    {
        fprintf( stderr, "ERROR: Too few arguments\n" );
        return EXIT_FAILURE;
    }

    // get command line args without using brackets
    // get size of cache and check if valid
    int cache_size = atoi( *(argv + 1) );

    if ( cache_size <= 0 ) 
    {
        fprintf( stderr, "ERROR: Cache size argument must be positive\n" );
        return EXIT_FAILURE;
    }

    // initialize cache
    struct Cache * cache = calloc( 1, sizeof( struct Cache ) );
    cache->size = cache_size;
    cache->heap_data = calloc( cache_size, sizeof( void * ) );
    cache->heap_type_meta_data = calloc( cache_size, sizeof( char ) );

    // get loop through all input files
    char ** itr = argv + 2;
    while ( *itr != NULL)
    {
        // open file
        char * filename = *itr;
        int open_read = open( filename, O_RDONLY );

        // test file open from class notes
        if ( open_read == -1 )
        {
            perror( "ERROR: open() failed" );
            return EXIT_FAILURE; 
        }
        
        // load file data into cache
        ParseFile( open_read, cache );

        // close file
        close(open_read);

        itr++;
    }

    // print
    PrintCache( cache );

    // free cache
    FreeCache( cache );
    
    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// reverse function with dynamic memory allocation

char * reverse( char * s ) 
{
    size_t len = strlen( s );
    char * buffer = calloc( len, sizeof( char ) );
    
    if ( buffer != NULL )
    { 
        // build buffer of reversed characters
        char * b_itr = buffer;
        char * s_itr = s + len - 1; // go to end of string s
        while ( s_itr != s - 1 )
        {
            *b_itr = *s_itr; // set iterator value to reverse position of s
            s_itr-- ; b_itr++;
        }

        *b_itr = '\0'; // add null byte to end of buffer

        // refill s with reversed order from buffer
        b_itr = buffer;
        s_itr = s;
        while ( *b_itr != '\0' )
        {
            *s_itr = *b_itr;
            s_itr++ ; b_itr++;
        }
    }

    free(buffer);
    
    return s;
}


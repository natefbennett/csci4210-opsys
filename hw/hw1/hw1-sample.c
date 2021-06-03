/* hw1-sample.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  int cache_size = 17;

  void ** cache = calloc( cache_size, sizeof( void * ) );

  int cache_index;


  char * word = "Lion";   /* 5 bytes */
  cache_index = 11;

  /* use calloc() to dynamically allocate memory at index 11 ... */
  cache[cache_index] = calloc( strlen( word ) + 1, sizeof( char ) );
  strcpy( cache[cache_index], word );


  int value = 1248;
  cache_index = 7;

  /* use calloc() to dynamically allocate memory at index 7 ... */
  cache[cache_index] = calloc( 1, sizeof( int ) );

  /* cache[cache_index] = value; */
  int * ptr = *(cache+cache_index);
  *ptr = value;


  return EXIT_SUCCESS;
}

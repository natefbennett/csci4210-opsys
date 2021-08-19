#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int lecex3_q1_child ( int readfd )
{
    // get shared memory key from input
    key_t shmkey;
    read( readfd, &shmkey, sizeof( int ) );

    // get shared memory data size from input
    int datasize;
    read( readfd, &datasize, sizeof( int ) );

    close( readfd );

    // find ID of shared memory segment
    size_t shmsize = sizeof( char ) * datasize;
    int shmid = shmget( shmkey, shmsize, 0 );
    if ( shmid == -1 )
    {
        perror( "shmget() failed" );
        return EXIT_FAILURE;
    }

    // attach to shared memory with found id
    char * data = shmat( shmid, NULL, 0 );
    if ( data == (void *)-1 )
    {
        perror( "shmat() failed" );
        return EXIT_FAILURE;
    }

    // change all chars in shared memory segment to uppercase
    char * itr = data;
    while ( *itr != '\n' || *itr != '\0' )
    {
        *itr = toupper( *itr );
        itr++;
    }

    // detatch from shared memory segment
    int rc = shmdt( data );
    if ( rc == -1 )
    {
        perror( "shmdt() failed" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
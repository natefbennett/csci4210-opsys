#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{

    int rc;
    printf( "ONE-%d\n", getpid() );
    rc = fork();
    printf( "TWO-%d\n", getpid() );
    if ( rc == 0 ) { printf( "THREE-%d\n", getpid() ); }
    if ( rc > 0 ) { printf( "FOUR-%d\n", getpid() ); }
    return EXIT_SUCCESS;

}
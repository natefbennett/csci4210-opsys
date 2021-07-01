#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int p[2];
    close(0);
    open("data.txt", O_RDONLY);
    open("output.txt", O_WRONLY | O_CREAT, 0600);
    pipe(p);

    // print write end of pipe
    printf( "p[1] :>> %d\n", p[1] ); // DEBUG
}
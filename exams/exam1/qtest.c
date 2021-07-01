#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{

    int sum = (sizeof(int) + sizeof(float) + (sizeof(double*)*32)+(sizeof(double)*16));
    printf( "sum :>> %d\n", sum ); // DEBUG


    return EXIT_SUCCESS;
}
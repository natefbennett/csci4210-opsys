#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void summer(int s) { fprintf(stderr, "-SUMMER-"); }

int main(int argc, char **argv)
{
    
    signal(SIGUSR1, SIG_IGN);
    signal(SIGTERM, summer);
    printf("NO-MORE");

    sleep(10000);

    signal(SIGTERM, SIG_DFL);
    printf("ARCH");
    return EXIT_SUCCESS;

}
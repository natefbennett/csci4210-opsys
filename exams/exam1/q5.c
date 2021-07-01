#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void f1(int s)
{
    printf("4!3!2!1!");
    fflush(stdout);
}
void f2(int s) { fprintf(stderr, "==SU"); }

int main(int argc, char **argv)
{
    signal(SIGUSR1, f1);
    signal(SIGUSR2, f2);
    close(0);
    close(1);
    int *p = calloc(2, sizeof(int));
    pipe(p);
    // printf("NS--");
    // fflush(stdout);
    // // sleep(1);
    // fprintf(stderr, "<MEME>");
    // // sleep(1);
    char *buffer = calloc(8, sizeof(char));
    read(p[0], buffer, 4);
    fprintf(stderr, "%s", buffer);
    read(p[0], buffer, 2);
    fprintf(stderr, "in%s", buffer); /* ====== see part (b) below ====== */
    close(p[0]);
    close(p[1]);
    free(buffer);
    free(p);
    return EXIT_SUCCESS;
}
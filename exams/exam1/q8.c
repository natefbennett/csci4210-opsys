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
    int a = sizeof(int *);
    int *r = malloc(sizeof(int));
    float *c = malloc(sizeof(float));
    double **h = calloc(32, sizeof(double *));
    *c = 8.88;
    *r = sizeof(short);
    pid_t p3 = fork();
    printf("{%d}{%d}{%.4f}{%d}{", a, *r, *c, p3);
    // parent
    if (p3 > 0)
    {
        h[a] = calloc(16, sizeof(double));
        *r = a + *r;
        printf("summer %d}\n", a);
        waitpid(p3, NULL, 0);
        free( r );
        free( c );
    }
    // child
    else if (p3 == 0)
    {
        h[a] = calloc(8, sizeof(double));
        *r = a - *r;
        printf("square %d}\n", *r);
        h[a][*r] = 20.21;
        free( r );
        free( c );
    }

    printf("{%0.1lf}\n", h[a][a - 2]);
    free( h );
    return EXIT_SUCCESS;
}
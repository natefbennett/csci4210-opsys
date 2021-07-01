#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>



int main()
{
    int i = 0;
    pid_t buffer1[256];
    int buffer2[256];
    while (1)
    {
        int j = 0;
        pid_t p7 = fork();
        pid_t *careful = malloc(sizeof(pid_t));
        *careful = getpid();
        buffer1[i] = getpid();
        buffer2[j] = i;
        if (p7 > 0 && waitpid(p7, NULL, 0))
        {
            printf("PARENT: %d %d %d\n", getpid(), *careful, buffer1[i++]);
        }
        else if (p7 == 0)
        {
            printf("CHILD: %d %d\n", *careful, buffer2[j]);
            free(careful);
            break;
        }
        free(careful);
        // sleep(1);
    }
    return EXIT_SUCCESS;
}
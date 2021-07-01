#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int x = 2021;
    printf("PARENT: x is %d\n", x);
    
    pid_t pid = fork();
    printf("PARENT %d: forked...\n", pid);

    if (pid == 0)
    {
        printf("CHILD: happy birthday to %d\n", getpid());
        x += 19;
        printf("CHILD: %d\n", x);
    }
    else
    {
        waitpid(-1, NULL, 0);
        printf("PARENT %d: child completed\n", getpid());
        x -= 2000;
        printf("PARENT: %d\n", x);
    }
    return EXIT_SUCCESS;
}
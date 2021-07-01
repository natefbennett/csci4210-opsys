#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    close(2);
    printf("HELLO\n");

#if 0
close( 1 ); /* <== add this line later.... */
#endif

    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    printf("==> %d\n", fd);
    printf("WHAT?\n");
    fprintf(stderr, "ERROR\n");

    close(fd);
    
    return EXIT_SUCCESS;
}
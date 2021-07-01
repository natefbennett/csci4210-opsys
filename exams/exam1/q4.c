#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd = open("alpha.txt", O_RDONLY);
    lseek(fd, 2, SEEK_CUR);
    char *buffer = calloc(10, sizeof(char));
    read(fd, buffer, 1);
    read(fd, buffer + 1, 2);
    read(fd, buffer + 2, 4);
    buffer++;
    fprintf(stderr, ">> %c", *(buffer + 2)); /* ... */
    return EXIT_SUCCESS;
}
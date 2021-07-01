#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

int main(int argc, char **argv)
{

    char **dcc = calloc(17, sizeof(char *));
    *(dcc + 3) = malloc(27 * sizeof(char));
    dcc = realloc(dcc, 33 * sizeof(char *));
    *(dcc + 6) = calloc(27, sizeof(char)); /* how many bytes are allocated on the heap here? */

    return EXIT_SUCCESS;
}
#include "lexer.h"

char* craete_twin_buffer()
{
    char* buffer = malloc(1000000*sizeof(char));
    return buffer;
}

void clear_buffer(char* buffer)
{
    free(buffer);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"

// cчитывает строчку из файла произвольной длины и возвращает в выделенной строке
// capacity is buf size
// returns true on success
bool read_line(FILE *f, char ** buf, int * capacity)
{
    static const size_t CHUNK_SIZE = 128;
    size_t len = 0;
    if (!(*buf)) 
    {
        if (*capacity < CHUNK_SIZE)
            *capacity = CHUNK_SIZE;
        *buf = malloc(*capacity); 
    } else if (*capacity < CHUNK_SIZE) 
    {
        *capacity = CHUNK_SIZE;
        *buf = realloc(*buf, *capacity);
    }
    if (!*buf)
        return false;

    *buf[0] = '\0';

    char chunk[CHUNK_SIZE];
    while (fgets(chunk, sizeof(chunk), f))
    {
        size_t chunk_len = strlen(chunk);

        if (len + chunk_len + 1 > *capacity)
        {
            *capacity *= 2;
            char *tmp = realloc(*buf, *capacity);
            if (!tmp)
            {
                free(*buf);
                return NULL;
            }
            *buf = tmp;
        }

        memcpy(*buf + len, chunk, chunk_len + 1);
        len += chunk_len;

        if (chunk[chunk_len - 1] == '\n')
            break;
    }

    if (len == 0)
    {
        return false;
    }
    return true;
}
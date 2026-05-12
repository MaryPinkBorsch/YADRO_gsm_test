#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

// cчитывает строчку из файла произвольной длины и возвращает в выделенной строке
char *read_line(FILE *f)
{
    static const size_t CHUNK_SIZE = 128;
    size_t cap = CHUNK_SIZE;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf)
        return NULL;

    buf[0] = '\0';

    char chunk[CHUNK_SIZE];
    while (fgets(chunk, sizeof(chunk), f))
    {
        size_t chunk_len = strlen(chunk);

        if (len + chunk_len + 1 > cap)
        {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp)
            {
                free(buf);
                return NULL;
            }
            buf = tmp;
        }

        memcpy(buf + len, chunk, chunk_len + 1);
        len += chunk_len;

        if (chunk[chunk_len - 1] == '\n')
            break;
    }

    if (len == 0)
    {
        free(buf);
        return NULL;
    }
    return buf;
}
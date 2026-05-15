#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "util.h"

// cчитывает строчку из файла произвольной длины и возвращает в выделенной строке
// capacity is buf size
// returns true on success
bool read_line(FILE *f, char **buf, int *capacity)
{
    static const size_t CHUNK_SIZE = 128;
    size_t len = 0;
    if (!(*buf))
    {
        if (*capacity < CHUNK_SIZE)
            *capacity = CHUNK_SIZE;
        *buf = malloc(*capacity);
    }
    else if (*capacity < CHUNK_SIZE)
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



/**
 * Токенизация CSV строки по разделителю-запятой
 *
 * @param input - входная строка
 * @param tokens - выходной массив указателей на токены (должен быть предварительно выделен)
 * @param max_tokens - максимальное количество токенов
 * @return количество найденных токенов
 */
int csv_tokenize(const char *input, char **tokens, int max_tokens)
{
    if (input == NULL || tokens == NULL || max_tokens <= 0)
    {
        return 0;
    }

    int token_count = 0;
    const char *start = input;
    const char *current = input;

    while (*current != '\0' && token_count < max_tokens)
    {
        if (*current == ',')
        {
            // Нашли разделитель, извлекаем токен
            int length = current - start;

            // Выделяем память под токен
            tokens[token_count] = (char *)malloc(length + 1);
            if (tokens[token_count] == NULL)
            {
                // Очистка при ошибке
                for (int i = 0; i < token_count; i++)
                {
                    free(tokens[i]);
                    tokens[i] = NULL;
                }
                return 0;
            }

            // Копируем токен
            strncpy(tokens[token_count], start, length);
            tokens[token_count][length] = '\0';

            token_count++;
            start = current + 1;
        }
        current++;
    }

    // Последний токен (после последней запятой или вся строка)
    if (token_count < max_tokens)
    {
        int length = current - start;
        tokens[token_count] = (char *)malloc(length + 1);
        if (tokens[token_count] != NULL)
        {
            strncpy(tokens[token_count], start, length);
            tokens[token_count][length] = '\0';
            token_count++;
        }
    }

    return token_count;
}

/**
 * Улучшенная версия с обработкой пустых полей
 * (например: "a,,c" -> токены "a", "", "c")
 */
int csv_tokenize_advanced(const char *input, char **tokens, int max_tokens)
{
    if (input == NULL || tokens == NULL || max_tokens <= 0)
    {
        return 0;
    }

    int token_count = 0;
    const char *start = input;
    const char *current = input;

    while (*current != '\0' && token_count < max_tokens)
    {
        if (*current == ',')
        {
            // Нашли разделитель
            int length = current - start;

            // Выделяем память под токен (даже если пустой)
            tokens[token_count] = (char *)malloc(length + 1);
            if (tokens[token_count] == NULL)
            {
                for (int i = 0; i < token_count; i++)
                {
                    free(tokens[i]);
                    tokens[i] = NULL;
                }
                return 0;
            }

            // Копируем токен (для пустого поля length=0)
            strncpy(tokens[token_count], start, length);
            tokens[token_count][length] = '\0';

            token_count++;
            start = current + 1;
        }
        current++;
    }

    // Последний токен
    if (token_count < max_tokens)
    {
        int length = current - start;
        tokens[token_count] = (char *)malloc(length + 1);
        if (tokens[token_count] != NULL)
        {
            strncpy(tokens[token_count], start, length);
            tokens[token_count][length] = '\0';
            token_count++;
        }
    }

    return token_count;
}
 

/**
 * Освобождение памяти, выделенной под токены
 */
void free_tokens(char **tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (tokens[i] != NULL)
        {
            free(tokens[i]);
            tokens[i] = NULL;
        }
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

static bool is_number(const char *s)
{
    if (*s == '\0') return false;
    for (const char *p = s; *p; p++)
        if (!isdigit((unsigned char)*p)) return false;
    return true;
}

bool string_to_cell_and_index(const char *str, Cell ** out_cells, int *out_num_cells, int *out_index)
{
    int count = 0;
    int cap = 8;
    const char *start = str;
    const char *end;

    *out_cells = malloc(cap * sizeof(Cell));

    // parse index
    end = strchr(start, ',');
    *out_index = strtol(start, &end, 10);
    start = end + 1;

    while ((end = strchr(start, ',')) != NULL)
    {
        Cell tmp_cell;
        if (count == cap)
        {
            cap *= 2;
            Cell *tmp = realloc(*out_cells, cap * sizeof(Cell));
            if (!tmp) goto fail_toks;
            *out_cells = tmp;
        }
        if (*start == '=') 
        {
            // parse function
            tmp_cell.expression = malloc(end-start+1);
            memset(tmp_cell.expression, 0, end-start+1);
            strncpy(tmp_cell.expression,start, end-start); 
            tmp_cell.cell_type = EXPRESSION;
        }
        else 
        {
            // parse number
            tmp_cell.val = strtol(start, &end, 10);
            tmp_cell.cell_type = VALUE;
        }
        (*out_cells)[count] = tmp_cell;
        ++count;
        start = end + 1;
    }
    Cell tmp_cell;
    if (count == cap)
    {
        cap *= 2;
        Cell *tmp = realloc(*out_cells, cap * sizeof(Cell));
        if (!tmp) goto fail_toks;
        *out_cells = tmp;
    }
    if (*start == '=') 
    {
        // parse function
        tmp_cell.expression = malloc(end-start+1);
        memset(tmp_cell.expression, 0, end-start+1);
        strncpy(tmp_cell.expression,start, end-start); 
        tmp_cell.cell_type = EXPRESSION;
    }
    else 
    {
        // parse number
        tmp_cell.val = strtol(start, &end, 10);
        tmp_cell.cell_type = VALUE;
    }
    (*out_cells)[count] = tmp_cell;
    ++count;
    *out_num_cells = count;
    return true;

fail_toks:
    return false;
}
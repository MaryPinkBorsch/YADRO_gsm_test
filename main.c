#include <stdio.h>

#include "util.h"

typedef enum
{
    PROCESSING, // это если идет обработка в рекурсивном вызове
    VALUE,      // это если уже высчитали число и оно готово
    EXPRESSION  // это если ссылка на др клетку
} CELL_TYPE;

typedef struct
{
    CELL_TYPE cell_type;
    union
    {
        int val;
        char *expression;
    };
} Cell;

/*
структура ЦСВ:
- первая строка =  названия столбцов (клетка 0,0 всегда пустая), там будет N столбцов разделенных запятыми
- следующие строки это M строк c N+1 значениями, где первое значение в строке это индекс записи (может быть не по порядку)
*/

int main(int argc, char *argv[])
{
    // Check command line arguments
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <csv_filename>\n", argv[0]);
        return 1;
    }

    // Open the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }

    // структуры данных
    static const int MAX_HEADERS = 4096; // названия столбцов, N штук
    char *headers[MAX_HEADERS];
    int *indices;   // массив индексов строк, M штук
    Cell **data;    // двумерных массив ячеек, размера M в высоту x N в ширину

    char *headers_str = malloc(1024*1024);
    char *buf = NULL;

    int capacity = 0;

    bool success = read_line(file, &buf, &capacity);
    if (success)
    {
        strcpy(headers_str, buf);
    }
    while (success)
    {
        printf("%s", buf);
        success = read_line(file, &buf, &capacity);
    }

    int num_headers = csv_tokenize(headers_str, headers, MAX_HEADERS);

    printf("\nSTOLBZI:\n");
    for(int i =0; i < num_headers;i++)
        printf("%s\n", headers[i]);

    free_tokens(headers, num_headers);
    fclose(file);
    free(headers_str);
    free(buf);

    return 0;
}
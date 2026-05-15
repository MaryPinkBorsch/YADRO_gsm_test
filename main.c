#include <stdio.h>
#include <string.h>

#include "common.h"
#include "util.h"
#include "int_int_hash_table.h"
#include "str_int_hash_table.h"

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
    int *indices; // массив индексов строк, M штук
    Cell **data;  // двумерных массив ячеек, размера M в высоту x N в ширину

    char *headers_str = malloc(1024 * 1024);
    char *buf = NULL;
    IntIntHashTable i_ht;
    int_int_ht_init(&i_ht);

    StrIntHashTable str_ht;
    str_int_ht_init(&str_ht);

    int capacity = 0;

    bool success = read_line(file, &buf, &capacity);
    if (success)
    {
        strcpy(headers_str, buf);
    }
    int num_headers = csv_tokenize(headers_str, headers, MAX_HEADERS);
    for (int i = 0; i < num_headers; i++) 
    {
        printf("%s -> %d\n", headers[i], i);
        str_int_ht_set(&str_ht, headers[i], i);
    }

    int cap_rows = 8;
    indices = malloc(sizeof(int)*cap_rows);
    data = malloc(sizeof(Cell*)*cap_rows);
    int num_rows = 0;
    while (success)
    {
        if (num_rows == cap_rows)
        {
            cap_rows *= 2;
            int * tmp1 = realloc(indices, cap_rows * sizeof(int));
            if (!tmp1) goto big_fail;
            indices = tmp1;
            Cell * tmp2 = realloc(*data, cap_rows * sizeof(Cell*));
            if (!tmp2) goto big_fail;
            *data = tmp2;
        }

        printf("%s", buf);
        success = read_line(file, &buf, &capacity);

        int num_row_cells = 0;
        success = string_to_cell_and_index(buf,&(data[num_rows]), & num_row_cells, &(indices[num_rows]));
        int_int_ht_set(&i_ht, indices[num_rows], num_rows);
        success &= num_row_cells == num_headers-1; // first header is empty / doesn't count
        if (!success)
            goto big_fail;
        ++num_rows;
    }

    // int out = 0;
    // int_int_ht_get(&i_ht, 42, &out);
    // printf("\nHASH TEST:\n");
    // printf("%d\n", out);

    int_int_ht_destroy(&i_ht);
    free_tokens(headers, num_headers);
    fclose(file);
    free(headers_str);
    free(buf);

big_fail:    

    return 0;
}
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

typedef enum 
{
    PLUS,
    MINUS,
    DIV,
    MUL,
    INVALID_OP_TYPE
} OP_TYPE;

int calc_recursive(int *indices, Cell **data, IntIntHashTable *i_ht, StrIntHashTable *str_ht, int i, int j) 
{
    if (data[i][j].cell_type == VALUE)
        return data[i][j].val;

    if (data[i][j].cell_type == PROCESSING) 
    {
        printf("CYCLE!");
        exit(1);
    }

    char * tmp = malloc(1024);

    strcpy(tmp, data[i][j].expression);

    data[i][j].cell_type = PROCESSING;

    OP_TYPE op_type = INVALID_OP_TYPE;
    char * op_pos = NULL;

    if (op_pos = strchr(tmp, '+'))
        op_type = PLUS;
    else if (op_pos = strchr(tmp, '-'))
        op_type = MINUS;
    else if (op_pos = strchr(tmp, '*'))
        op_type = MUL;
    else if (op_pos = strchr(tmp, '/'))
        op_type = DIV;
    *op_pos = 0;
    char * arg1 = tmp + 1; // skip =
    char * arg2 = op_pos + 1; // skip op char
    int arg1val = -1;
    int arg2val = -1;

    char * parse_end = NULL;
    arg1val = strtol(arg1, &parse_end, 10);
    if (arg1val == 0 && parse_end == arg1) // не число! 
    {
        char *arg1idxpos = arg1;
        while (!isdigit(*arg1idxpos))
            ++arg1idxpos;

        int arg1idx_val = -1;
        arg1idx_val = strtol(arg1idxpos, &parse_end, 10);
        *arg1idxpos = 0;

        int row_idx = -1;
        int_int_ht_get(i_ht, arg1idx_val, &row_idx);
    
        int column_idx = -1;
        str_int_ht_get(str_ht, arg1, &column_idx);

        arg1val = calc_recursive(indices, data, i_ht, str_ht, row_idx, column_idx);
    }

    parse_end = NULL;
    arg2val = strtol(arg2, &parse_end, 10);
    if (arg2val == 0 && parse_end == arg2) // не число! 
    {
        char *arg2idxpos = arg2;
        while (!isdigit(*arg2idxpos))
            ++arg2idxpos;

        int arg2idx_val = -1;
        arg2idx_val = strtol(arg2idxpos, &parse_end, 10);
        *arg2idxpos = 0;

        int row_idx = -1;
        int_int_ht_get(i_ht, arg2idx_val, &row_idx);
    
        int column_idx = -1;
        str_int_ht_get(str_ht, arg2, &column_idx);

        arg2val = calc_recursive(indices, data, i_ht, str_ht, row_idx, column_idx);
    }

    int res = -1;
    switch(op_type) 
    {
        case PLUS:
            res = arg1val + arg2val;
            break;
        case MINUS:
            res = arg1val - arg2val;
            break;
        case DIV:
            res = arg1val / arg2val;
            break;
        case MUL:
            res = arg1val * arg2val;
            break;
    }

    data[i][j].val = res;
    data[i][j].cell_type = VALUE;

    free(tmp);
    return res;
}

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
    for (int i = 1; i < num_headers; i++) 
    {
        str_int_ht_set(&str_ht, headers[i], i-1); // пропуск первой колонки
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

        success = read_line(file, &buf, &capacity);
        if (!success)
            break;

        int num_row_cells = 0;
        success = string_to_cell_and_index(buf,&(data[num_rows]), & num_row_cells, &(indices[num_rows]));
        int_int_ht_set(&i_ht, indices[num_rows], num_rows);
        success &= num_row_cells == num_headers-1; // first header is empty / doesn't count
        if (!success)
            goto big_fail;
        ++num_rows;
    }

    for (int i = 0; i < num_headers; i++) 
    {
        printf("%s",headers[i]);
        if (i != num_headers-1)
            printf(",");
    }
    printf("\n");

    for (int i = 0; i < num_rows; i++) 
    {
        printf("%d,", indices[i]);
        for (int j = 0; j < num_headers - 1; j++) 
        {
            if (data[i][j].cell_type == VALUE)
                printf("%d", data[i][j].val);
            else if (data[i][j].cell_type == EXPRESSION) 
            {
                calc_recursive(indices,data, &i_ht, &str_ht, i,j);
                printf("%d", data[i][j].val);
            }
            if (j != num_headers - 2) // WTF
                printf(",");
        }        
        printf("\n");
    }
    printf("\n");

    int_int_ht_destroy(&i_ht);
    str_int_ht_destroy(&str_ht);
    free_tokens(headers, num_headers);
    fclose(file);
    free(headers_str);
    free(buf);

big_fail:    

    return 0;
}
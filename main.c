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
    char ** headers; // названия столбцов, N штук
    int * indices; // массив индексов строк, M штук 
    Cell ** data; // двумерных массив ячеек, размера M в высоту x N в ширину

    char * headers_str = NULL;
    int capacity;
    
    bool success = read_line(file, &headers_str, &capacity);
    printf("%s\n", headers_str);
    
    fclose(file);

    return 0;
}
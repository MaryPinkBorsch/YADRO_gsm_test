#pragma once

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
typedef enum
{
    PROCESSING, // это если идет обработка в рекурсивном вызове
    VALUE,      // это если уже высчитали число и оно готово
    EXPRESSION  // это если ссылка на др клетку
} CELL_TYPE;

typedef struct
{
    CELL_TYPE cell_type;
    int val;
    char *expr;
} Cell;

int main(int argc, char *argv[])
{
    Cell tmp;
    return 0;
}
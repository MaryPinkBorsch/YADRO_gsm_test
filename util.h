#include <stdio.h>
#include <stdbool.h>

// cчитывает строчку из файла произвольной длины и возвращает в выделенной строке
// capacity is buf size
// returns true on success
bool read_line(FILE *f, char ** buf, int * capacity);

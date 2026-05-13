#include <stdio.h>
#include <stdbool.h>

// cчитывает строчку из файла произвольной длины и возвращает в выделенной строке
// capacity is buf size
// returns true on success
bool read_line(FILE *f, char ** buf, int * capacity);


/**
 * Токенизация CSV строки по разделителю-запятой
 *
 * @param input - входная строка
 * @param tokens - выходной массив указателей на токены (должен быть предварительно выделен)
 * @param max_tokens - максимальное количество токенов
 * @return количество найденных токенов
 */
int csv_tokenize(const char *input, char **tokens, int max_tokens);

/**
 * Улучшенная версия с обработкой пустых полей
 * (например: "a,,c" -> токены "a", "", "c")
 */
int csv_tokenize_advanced(const char *input, char **tokens, int max_tokens);

/**
 * Освобождение памяти, выделенной под токены
 */
void free_tokens(char **tokens, int count);
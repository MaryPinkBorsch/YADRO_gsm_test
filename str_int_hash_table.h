#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STR_INT_BUCKETS 16384
#define STR_INT_CHUNK_CAP 4096

typedef struct StrIntEntry
{
    char * key;
    int value;
    struct StrIntEntry *next;
} StrIntEntry;

typedef struct StrIntChunk
{
    StrIntEntry entries[STR_INT_CHUNK_CAP];
    struct StrIntChunk *next;
} StrIntChunk;

typedef struct
{
    StrIntChunk *chunks;
    StrIntEntry *freelist;
} StrIntPool;

typedef struct
{
    StrIntEntry *buckets[STR_INT_BUCKETS];
    StrIntPool pool;
} StrIntHashTable;

// TODO: разделить публ инткрфейсы
void str_int_ht_pool_init(StrIntPool *p);
static bool str_int_ht_pool_grow(StrIntPool *p);
StrIntEntry *str_int_ht_pool_alloc(StrIntPool *p);
void str_int_ht_pool_free_entry(StrIntPool *p, StrIntEntry *e);
void str_int_ht_pool_destroy(StrIntPool *p);

void str_int_ht_init(StrIntHashTable *ht);
void str_int_ht_destroy(StrIntHashTable *ht);

static int str_int_ht_hash(const char* key);
bool str_int_ht_set(StrIntHashTable *ht, const char* key, int value);
bool str_int_ht_get(StrIntHashTable *ht, const char* key, int *out);
bool str_int_ht_delete(StrIntHashTable *ht, const char* key);
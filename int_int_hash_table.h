#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INT_INT_BUCKETS 16384
#define INT_INT_CHUNK_CAP 4096

typedef struct IntIntEntry
{
    int key;
    int value;
    struct IntIntEntry *next;
} IntIntEntry;

typedef struct IntIntChunk
{
    IntIntEntry entries[INT_INT_CHUNK_CAP];
    struct IntIntChunk *next;
} IntIntChunk;

typedef struct
{
    IntIntChunk *chunks;
    IntIntEntry *freelist;
} IntIntPool;

typedef struct
{
    IntIntEntry *buckets[INT_INT_BUCKETS];
    IntIntPool pool;
} IntIntHashTable;

// TODO: разделить публ инткрфейсы
void int_int_ht_pool_init(IntIntPool *p);
static bool int_int_ht_pool_grow(IntIntPool *p);
IntIntEntry *int_int_ht_pool_alloc(IntIntPool *p);
void int_int_ht_pool_free_entry(IntIntPool *p, IntIntEntry *e);
void int_int_ht_pool_destroy(IntIntPool *p);

void int_int_ht_init(IntIntHashTable *ht);
void int_int_ht_destroy(IntIntHashTable *ht);

static int int_int_ht_hash(int key);
bool int_int_ht_set(IntIntHashTable *ht, int key, int value);
bool int_int_ht_get(IntIntHashTable *ht, int key, int *out);
bool int_int_ht_delete(IntIntHashTable *ht, int key);
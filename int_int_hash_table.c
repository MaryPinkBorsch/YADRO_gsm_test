#include "int_int_hash_table.h"

void int_int_ht_pool_init(IntIntPool *p)
{
    p->chunks = NULL;
    p->freelist = NULL;
}

static bool int_int_ht_pool_grow(IntIntPool *p)
{
    IntIntChunk *c = malloc(sizeof(IntIntChunk));
    if (!c)
        return false;

    for (int i = 0; i < INT_INT_CHUNK_CAP - 1; i++)
        c->entries[i].next = &c->entries[i + 1];
    c->entries[INT_INT_CHUNK_CAP - 1].next = p->freelist;
    p->freelist = &c->entries[0];

    c->next = p->chunks;
    p->chunks = c;
    return true;
}

IntIntEntry *int_int_ht_pool_alloc(IntIntPool *p)
{
    if (!p->freelist && !int_int_ht_pool_grow(p))
        return NULL;
    IntIntEntry *e = p->freelist;
    p->freelist = e->next;
    return e;
}

void int_int_ht_pool_free_entry(IntIntPool *p, IntIntEntry *e)
{
    e->next = p->freelist;
    p->freelist = e;
}

void int_int_ht_pool_destroy(IntIntPool *p)
{
    IntIntChunk *c = p->chunks;
    while (c)
    {
        IntIntChunk *next = c->next;
        free(c);
        c = next;
    }
    p->chunks = NULL;
    p->freelist = NULL;
}

void int_int_ht_init(IntIntHashTable *ht)
{
    for (int i = 0; i < INT_INT_BUCKETS; i++)
        ht->buckets[i] = NULL;
    int_int_ht_pool_init(&ht->pool);
}

void int_int_ht_destroy(IntIntHashTable *ht)
{
    int_int_ht_pool_destroy(&ht->pool);
    // buckets are now dangling but pool_destroy freed everything
    for (int i = 0; i < INT_INT_BUCKETS; i++)
        ht->buckets[i] = NULL;
}

static int int_int_ht_hash(int key)
{
    // Use large prime numbers for multiplication and addition
    unsigned int hash = key * 2654435761u;  // Multiplication by a prime (golden ratio ~ 2^32/φ)
    hash = hash + 101u;                     // Add another prime
    return hash % INT_INT_BUCKETS; 
}

bool int_int_ht_set(IntIntHashTable *ht, int key, int value)
{
    int b = int_int_ht_hash(key);
    for (IntIntEntry *e = ht->buckets[b]; e; e = e->next)
    {
        if (e->key == key)
        {
            e->value = value;
            return true;
        }
    }
    IntIntEntry *e = int_int_ht_pool_alloc(&ht->pool);
    if (!e)
        return false;
    e->key = key;
    e->value = value;
    e->next = ht->buckets[b];
    ht->buckets[b] = e;
    return true;
}

bool int_int_ht_get(IntIntHashTable *ht, int key, int *out)
{
    int b = int_int_ht_hash(key);
    for (IntIntEntry *e = ht->buckets[b]; e; e = e->next)
    {
        if (e->key == key)
        {
            *out = e->value;
            return true;
        }
    }
    return false;
}

bool int_int_ht_delete(IntIntHashTable *ht, int key)
{
    int b = int_int_ht_hash(key);
    IntIntEntry **cur = &ht->buckets[b];
    while (*cur)
    {
        if ((*cur)->key == key)
        {
            IntIntEntry *dead = *cur;
            *cur = dead->next;
            int_int_ht_pool_free_entry(&ht->pool, dead);
            return true;
        }
        cur = &(*cur)->next;
    }
    return false;
}

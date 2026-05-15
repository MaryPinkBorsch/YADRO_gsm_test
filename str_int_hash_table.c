#include "str_int_hash_table.h"

void str_int_ht_pool_init(StrIntPool *p)
{
    p->chunks = NULL;
    p->freelist = NULL;
}

static bool str_int_ht_pool_grow(StrIntPool *p)
{
    StrIntChunk *c = malloc(sizeof(StrIntChunk));
    if (!c)
        return false;

    for (int i = 0; i < STR_INT_CHUNK_CAP - 1; i++)
        c->entries[i].next = &c->entries[i + 1];
    c->entries[STR_INT_CHUNK_CAP - 1].next = p->freelist;
    p->freelist = &c->entries[0];

    c->next = p->chunks;
    p->chunks = c;
    return true;
}

StrIntEntry *str_int_ht_pool_alloc(StrIntPool *p)
{
    if (!p->freelist && !str_int_ht_pool_grow(p))
        return NULL;
    StrIntEntry *e = p->freelist;
    p->freelist = e->next;
    return e;
}

void str_int_ht_pool_free_entry(StrIntPool *p, StrIntEntry *e)
{
    free(e->key);
    e->key = NULL;
    e->next = p->freelist;
    p->freelist = e;
}

void str_int_ht_pool_destroy(StrIntPool *p)
{
    StrIntChunk *c = p->chunks;
    while (c)
    {
        StrIntChunk *next = c->next;
        for (int i = 0; i < STR_INT_CHUNK_CAP; i++) 
        {
            if (c->entries[i].key) 
            {
                free(c->entries[i].key);
                c->entries[i].key = NULL;
            }
        }
        free(c);
        c = next;
    }
    p->chunks = NULL;
    p->freelist = NULL;
}

void str_int_ht_init(StrIntHashTable *ht)
{
    for (int i = 0; i < STR_INT_BUCKETS; i++)
        ht->buckets[i] = NULL;
    str_int_ht_pool_init(&ht->pool);
}

void str_int_ht_destroy(StrIntHashTable *ht)
{
    str_int_ht_pool_destroy(&ht->pool);
    // buckets are now dangling but pool_destroy freed everything
    for (int i = 0; i < STR_INT_BUCKETS; i++)
        ht->buckets[i] = NULL;
}

// hasher for null terminated string
static int str_int_ht_hash(const char* key)
{
    unsigned int hash = 5381;
    unsigned int prime = 31;  // Small prime multiplier
    
    while (*key) {
        hash = hash * prime + *key;  // Multiply by prime and add character
        // Rotate left by 5 bits (simple rotation)
        hash = (hash << 5) | (hash >> 27);
        key++;
    }    
    return hash; 
}

bool str_int_ht_set(StrIntHashTable *ht, const char* key, int value)
{
    int b = str_int_ht_hash(key);
    for (StrIntEntry *e = ht->buckets[b]; e; e = e->next)
    {
        if (strcmp(e->key, key) == 0)
        {
            e->value = value;
            return true;
        }
    }
    StrIntEntry *e = str_int_ht_pool_alloc(&ht->pool);
    if (!e)
        return false;
    e->key = malloc(strlen(key));
    strcpy(e->key,key);
    e->value = value;
    e->next = ht->buckets[b];
    ht->buckets[b] = e;
    return true;
}

bool str_int_ht_get(StrIntHashTable *ht, const char* key, int *out)
{
    int b = str_int_ht_hash(key);
    for (StrIntEntry *e = ht->buckets[b]; e; e = e->next)
    {
        if (strcmp(e->key, key) == 0)
        {
            *out = e->value;
            return true;
        }
    }
    return false;
}

bool str_int_ht_delete(StrIntHashTable *ht, const char* key)
{
    int b = str_int_ht_hash(key);
    StrIntEntry **cur = &ht->buckets[b];
    while (*cur)
    {
        if (strcmp((*cur)->key, key) == 0)
        {
            StrIntEntry *dead = *cur;
            *cur = dead->next;
            str_int_ht_pool_free_entry(&ht->pool, dead);
            return true;
        }
        cur = &(*cur)->next;
    }
    return false;
}

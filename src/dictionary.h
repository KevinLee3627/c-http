#ifndef DICTIONARY_H
#define DICTIONARY_H

struct Node
{
  char *key;
  char *value;
  struct Node *next;
};

struct HashTable
{
  struct Node **buckets;
  int bucket_count;
};

unsigned long hash(const char *key);
struct HashTable *init_hash_table(int count);
void insert(struct HashTable *hash_table, char *key, char *value);
char *get(struct HashTable *hash_table, char *key);
void free_table(struct HashTable *hash_table);
void print_table(struct HashTable *hash_table);
#endif
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUCKET_COUNT 16

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

// Hash function provided here: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(const char *key)
{
  unsigned long hash = 5381;
  int c;

  while (c = *key++)
  {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % BUCKET_COUNT;
  // return key[0] % BUCKET_COUNT;
}

struct HashTable *init_hash_table(int count)
{
  // Allocate space for the HashTable struct
  struct HashTable *hash_table = calloc(1, sizeof(struct HashTable));
  if (hash_table == NULL)
    return NULL;

  // Allocate space for the actual data in hash_table
  hash_table->buckets = calloc(count, sizeof(struct Node));
  if (hash_table->buckets == NULL)
  {
    free(hash_table);
    return NULL;
  }

  hash_table->bucket_count = count;
  return hash_table;
}

void insert(struct HashTable *hash_table, char *key, char *value)
{
  struct Node *new_node = malloc(sizeof(struct Node));
  if (new_node == NULL)
    return;

  new_node->key = key;
  new_node->value = value;
  new_node->next = NULL;

  unsigned long index = hash(key);

  if (hash_table->buckets[index] == NULL)
  {
    hash_table->buckets[index] = new_node;
    return;
  }

  new_node->next = hash_table->buckets[index];
  hash_table->buckets[index] = new_node;
}

char *get(struct HashTable *hash_table, char *key)
{
  unsigned long index = hash(key);
  while (hash_table->buckets[index] != NULL)
  {
    struct Node *tmp = hash_table->buckets[index]->next;
    if (strcmp(hash_table->buckets[index]->key, key) == 0)
    {
      return hash_table->buckets[index]->value;
    }
    hash_table->buckets[index] = tmp;
  }
  return NULL;
}

void free_table(struct HashTable *hash_table)
{
  for (int i = 0; i < hash_table->bucket_count; i++)
  {
    while (hash_table->buckets[i] != NULL)
    {
      struct Node *tmp = hash_table->buckets[i]->next;
      free(hash_table->buckets[i]);
      hash_table->buckets[i] = tmp;
    }
  }
  free(hash_table->buckets);
  free(hash_table);
}

void print_table(struct HashTable *hash_table)
{
  for (int i = 0; i < hash_table->bucket_count; i++)
  {
    struct Node *current_bucket = hash_table->buckets[i];
    while (current_bucket != NULL)
    {
      struct Node *tmp = current_bucket->next;
      printf("(%i): %p -> %s: %s\n", i, current_bucket, current_bucket->key, current_bucket->value);
      current_bucket = tmp;
    }
  }
}

int main(void)
{
  struct HashTable *hash_table = init_hash_table(BUCKET_COUNT);
  insert(hash_table, "Content-Type", "asdf");
  insert(hash_table, "Host", "whatevs");
  insert(hash_table, "Accept", "whatevs");
  insert(hash_table, "Test", "whatevs");
  insert(hash_table, "test 7", "whatevs");
  insert(hash_table, "SLASD", "whatevs");
  // print_table(hash_table);
  printf("%s\n", get(hash_table, "Host"));
  free_table(hash_table);
  return 0;
}

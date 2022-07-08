#include <stdlib.h>
#include <stdio.h>

struct Entry
{
  char *value;
  int key;
};

struct Dictionary
{
};

struct Node
{
  int key;
  char *value;
  struct Node *left;
  struct Node *right;
  int balance_factor; // -1, 0, +1
  int height;
};

struct AVLTree
{
  struct Node *root;
};

struct Node *create_node(int key, char *value)
{
  struct Node *node = malloc(sizeof(struct Node));
  node->key = key;
  node->value = value;
  node->left = NULL;
  node->right = NULL;
  node->height = 0;
  return node;
}

void insert(struct Node **root, int key, char *value, int height)
{
  if (*root == NULL)
  {
    struct Node *new_node = create_node(key, value);
    new_node->height = height;
    *root = new_node;
    return;
  }

  if (key < (*root)->key)
  {
    insert(&(*root)->left, key, value, (*root)->height + 1);
  }
  else if (key > (*root)->key)
  {
    insert(&(*root)->right, key, value, (*root)->height + 1);
  }
}

void free_tree(struct Node *node)
{
  if (node->left != NULL)
  {
    free_tree(node->left);
  }
  if (node->right != NULL)
  {
    free_tree(node->right);
  }
  free(node);
}

void print_tree(struct Node *root)
{
  if (root != NULL)
  {
    print_tree(root->left);
    for (int i = 0; i <= root->height; i++)
    {
      printf("---");
    }
    printf("%i -> %s\n", root->key, root->value);
    print_tree(root->right);
  }
}

int main(void)
{
  struct Node *root = malloc(sizeof(struct Node));
  root->key = 100;
  root->value = "ROOT";
  root->height = 0;

  insert(&root, 200, "whtaever", root->height);
  insert(&root, 50, "test", root->height);
  insert(&root, 37, "test", root->height);
  insert(&root, 75, "test", root->height);
  insert(&root, 25, "test", root->height);
  insert(&root, 45, "test", root->height);
  insert(&root, 52, "test", root->height);

  print_tree(root);
  free_tree(root);
  return 0;
}
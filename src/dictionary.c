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
  int depth;
};

struct AVLTree
{
  struct Node *root;
};
void print_tree(struct Node *root, int max_height);

struct Node *create_node(int key, char *value)
{
  struct Node *node = malloc(sizeof(struct Node));
  node->key = key;
  node->value = value;
  node->left = NULL;
  node->right = NULL;
  node->height = 0;
  node->depth = 0;
  return node;
}

int max(int a, int b)
{
  return (a > b) ? a : b;
}
// Get the height of a tree starting from a node
int get_height(struct Node *node)
{
  if (node == NULL)
  {
    return 0;
  }
  return 1 + max(get_height(node->left), get_height(node->right));
}

int get_balance_factor(struct Node *node)
{
  if (node == NULL)
  {
    return 0;
  }
  return get_height(node->left) - get_height(node->right);
}

struct Node *left_rotate(struct Node *current_root)
{
  struct Node *new_root = current_root->right;
  struct Node *leaf_to_move = new_root->left;

  new_root->left = current_root;
  current_root->right = leaf_to_move;
  print_tree(new_root, new_root->height);
  return new_root;
}
struct Node *right_rotate(struct Node *y)
{
  /*
         Y (2)                   X
        / \                    /   \
       /   \                  /     \
       X     3 ---->          A      Y
      /\                    / \      /\
     /  \                  /   \    /  \
    A    B                 1    2  B    3
   /\
  /  \
 1    2
    X is the new root. Y becomes the right sub-tree of X
    A stays as the left subtree of the new root, X
    If X had a right subtree (B), it would be the new left subtree of Y
  */
  printf("Current node: %i\n", y->key);
  // Becomes the new root
  struct Node *x = y->left;
  struct Node *b = x->right;

  // Y should be the new root (X)'s new right subtree
  x->right = y;
  //  the right subtree of X, aka B, should be Y's new left subtree
  y->left = b;

  // Update the heights of the moved nodes
  y->height = get_height(y);
  x->height = get_height(x);

  // Return the new root
  print_tree(x, x->height);
  return x;
}

struct Node *insert(struct Node *node, int key, char *value, int height)
{
  if (node == NULL)
  {
    struct Node *new_node = create_node(key, value);
    new_node->depth = height;
    return new_node;
  }

  if (key < node->key)
  {
    node->left = insert(node->left, key, value, node->depth + 1);
  }
  else if (key > (node)->key)
  {
    node->right = insert(node->right, key, value, node->depth + 1);
  }
  else
  {
    return node;
  }

  node->height = get_height(node);

  int balance_factor = get_balance_factor(node);
  if (balance_factor > 1 && key < node->left->key)
  {
    printf("doing right\n");
    return right_rotate(node);
  }
  if (balance_factor < -1 && key < node->right->key)
  {
    printf("doing left\n");
    return left_rotate(node);
  }
  if (balance_factor > 1 && key > node->left->key)
  {
    printf("doing left-right\n");
    node->left = left_rotate(node->left);
    return right_rotate(node);
  }
  if (balance_factor < -1 && key < node->right->key)
  {
    printf("doing right-left\n");
    node->right = right_rotate(node->right);
    return left_rotate(node);
  }
  return node;
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

void print_tree(struct Node *root, int max_height)
{
  if (root != NULL)
  {
    print_tree(root->right, max_height);
    for (int i = get_height(root); i <= max_height; i++)
    {
      printf("-----");
    }
    printf("%i (%i, %i, %i)|\n", root->key, get_balance_factor(root), get_height(root), root->depth);
    print_tree(root->left, max_height);
  }
}

int main(void)
{
  struct Node *root = malloc(sizeof(struct Node));
  root->key = 30;
  root->value = "ROOT";
  root->depth = 0;
  insert(root, 10, "test", root->depth);
  insert(root, 20, "test", root->depth);
  // insert(root, 50, "test", root->depth);
  // insert(root, 75, "test", root->depth);
  // insert(root, 85, "test", root->depth);
  // insert(root, 28, "test", root->depth);

  print_tree(root, root->height);
  free_tree(root);
  return 0;
}
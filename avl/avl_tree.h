#pragma once

#include <stdbool.h>
#include <stdlib.h>

struct avl_node {
  int height;
  int key;
};

struct avl_tree {
  unsigned int capacity;
  unsigned int size;
  struct avl_node* tree;
};

struct avl_tree avl_create(unsigned int capacity);
void avl_insert(struct avl_tree* avl, int key);
bool avl_search(struct avl_tree* avl, int key);


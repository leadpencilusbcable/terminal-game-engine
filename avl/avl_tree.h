#pragma once

#include <stdlib.h>

#include "../tge_asset.h"

struct avl_node {
  int height;
  unsigned int key;
  struct tge_data data;
};

struct avl_tree {
  unsigned int capacity;
  unsigned int size;
  struct avl_node* tree;
};

struct avl_tree avl_create(unsigned int capacity);
void avl_insert(struct avl_tree* avl, unsigned int key, struct tge_data value);
struct tge_data* avl_search(struct avl_tree* avl, int key);


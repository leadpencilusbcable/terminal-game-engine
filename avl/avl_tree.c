#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "avl_tree.h"

//TODO allow for realloc upon needing space

static inline unsigned int left_index(unsigned int index){
  return index * 2 + 1;
}

static inline unsigned int right_index(unsigned int index){
  return index * 2 + 2;
}

static struct avl_node default_node = {
  .height = -1,
  .key = 0
};

struct avl_tree avl_create(unsigned int capacity){
  struct avl_tree avl = { .capacity = capacity };

  avl.tree = malloc(capacity * sizeof(struct avl_node));

  for(unsigned int i = 0; i < capacity; i++){
    avl.tree[i] = default_node;
  }

  return avl;
}

static int calculate_balance_factor(struct avl_tree* avl, unsigned int node_index){
   struct avl_node* left = &avl->tree[left_index(node_index)];
   struct avl_node* right = &avl->tree[right_index(node_index)];

   return left->height - right->height;
}

static void left_rotate(struct avl_tree* avl, unsigned int grandparent_index, struct avl_node* grandparent_node, struct avl_node* parent_node, struct avl_node* inserted_node){
  unsigned int grandparent_key = grandparent_node->key;

  grandparent_node->key = parent_node->key;
  parent_node->key = inserted_node->key;

  unsigned int grandparent_left_index = left_index(grandparent_index);
  struct avl_node* new_left_child = &avl->tree[grandparent_left_index];
  new_left_child->key = grandparent_key;

  new_left_child->height = 0;
  inserted_node->height = -1;
  parent_node->height = 0;
  grandparent_node->height = 1;
}

static void right_rotate(struct avl_tree* avl, unsigned int grandparent_index, struct avl_node* grandparent_node, struct avl_node* parent_node, struct avl_node* inserted_node){
  unsigned int grandparent_key = grandparent_node->key;

  grandparent_node->key = parent_node->key;
  parent_node->key = inserted_node->key;

  unsigned int grandparent_right_index = right_index(grandparent_index);
  struct avl_node* new_right_child = &avl->tree[grandparent_right_index];
  new_right_child->key = grandparent_key;

  new_right_child->height = 0;
  inserted_node->height = -1;
  parent_node->height = 0;
  grandparent_node->height = 1;
}

static void left_right_rotate(struct avl_tree* avl, unsigned int grandparent_index, struct avl_node* grandparent_node, unsigned int parent_index, struct avl_node* parent_node, struct avl_node* inserted_node){
  //move inserted node to left of parent node
  struct avl_node* parent_node_left = &avl->tree[left_index(parent_index)];
  parent_node_left->height = -1;
  parent_node_left->key = parent_node->key;
  parent_node->key = inserted_node->key;
  inserted_node->height = -1;

  right_rotate(avl, grandparent_index, grandparent_node, parent_node, parent_node_left);
}

static void right_left_rotate(struct avl_tree* avl, unsigned int grandparent_index, struct avl_node* grandparent_node, unsigned int parent_index, struct avl_node* parent_node, struct avl_node* inserted_node){
  //move inserted node to left of parent node
  struct avl_node* parent_node_right = &avl->tree[right_index(parent_index)];
  parent_node_right->height = -1;
  parent_node_right->key = parent_node->key;
  parent_node->key = inserted_node->key;
  inserted_node->height = -1;

  left_rotate(avl, grandparent_index, grandparent_node, parent_node, parent_node_right);
}

static void avl_balance(struct avl_tree* avl, unsigned int* visited_indices, size_t visited_indices_count){
  if(visited_indices_count <= 2){
    return;
  }

  //the node 2 above inserted
  unsigned int inserted_node_grandparent_index = visited_indices[visited_indices_count - 3];
  struct avl_node* inserted_node_grandparent = &avl->tree[inserted_node_grandparent_index];

  //the node 1 above inserted
  unsigned int inserted_node_parent_index = visited_indices[visited_indices_count - 2];
  struct avl_node* inserted_node_parent = &avl->tree[inserted_node_parent_index];

  unsigned int inserted_node_index = visited_indices[visited_indices_count - 1];
  struct avl_node* inserted_node = &avl->tree[inserted_node_index];

  int balance_factor = calculate_balance_factor(avl, inserted_node_grandparent_index);

  //tree is left heavy and was a left insertion
  if(balance_factor > 1 && inserted_node_parent->key > inserted_node->key){
    right_rotate(avl, inserted_node_grandparent_index, inserted_node_grandparent, inserted_node_parent, inserted_node);
  }
  //tree is left heavy and was a right insertion
  else if(balance_factor > 1){
    left_right_rotate(avl, inserted_node_grandparent_index, inserted_node_grandparent, inserted_node_parent_index, inserted_node_parent, inserted_node);
  }

  //tree is right heavy and was a right insertion
  else if(balance_factor < -1 && inserted_node_parent->key < inserted_node->key){
    left_rotate(avl, inserted_node_grandparent_index, inserted_node_grandparent, inserted_node_parent, inserted_node);
  }
  //tree is right heavy and was a left insertion
  else if(balance_factor < -1){
    right_left_rotate(avl, inserted_node_grandparent_index, inserted_node_grandparent, inserted_node_parent_index, inserted_node_parent, inserted_node);
  }
}

void avl_insert(struct avl_tree* avl, unsigned int key, struct tge_data value){
  struct avl_node* cur_node;

  unsigned int visited_indices[avl->tree[0].height + 2];
  size_t visited_indices_count = 0;

  unsigned int i = 0;

  while(i < avl->capacity){
    cur_node = &avl->tree[i];
    visited_indices[visited_indices_count] = i;
    visited_indices_count++;

    if(cur_node->height == -1){
      cur_node->key = key;
      cur_node->height = 0;

      avl->size++;

      for(size_t j = 1; j < visited_indices_count - 1; j++){
        unsigned int visited_index = visited_indices[j];
        struct avl_node* visited_node = &avl->tree[visited_index];

        //if the current node already has a height greater than
        //the tree after the new insertion, stop incrementing heights
        if(visited_node->height >= j){
          break;
        }

        avl->tree[visited_indices[j]].height++;
      }

      avl_balance(avl, visited_indices, visited_indices_count);

      break;
    } else if(cur_node->key == key){
      cur_node->data = value;
    } else if(cur_node->key > key){
      i = left_index(i);
    } else if(cur_node->key < key){
      i = right_index(i);
    }
  }
}

struct tge_data* avl_search(struct avl_tree* avl, int key){
  struct avl_node* cur_node;

  unsigned int i = 0;

  while(i < avl->capacity){
    cur_node = &avl->tree[i];

    if(cur_node->height == -1){
      return NULL;
    } else if(cur_node->key == key){
      return &cur_node->data;
    } else if(cur_node->key > key){
      i = left_index(i);
    } else if(cur_node->key < key){
      i = right_index(i);
    }
  }

  return NULL;
}


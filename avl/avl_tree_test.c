#include "avl_tree.c"
#include "avl_tree.h"
#include "test.h"

void test_general(){
  puts("general testing");
  struct avl_tree avl = avl_create(10);

  expect_uint(avl.capacity, 10, "capacity");
  expect_uint(avl.size, 0, "size");

  expect_int(avl.tree[0].height, -1, "empty node has height of -1");

  avl_insert(&avl, 5);

  expect_int(avl.tree[0].height, 0, "filled node has height of 0");

  expect_int(avl_search(&avl, 5), 1, "5 inserted, 5 found");
  expect_int(avl_search(&avl, 1), 0, "1 not inserted, 1 not found");

  avl_insert(&avl, 4);

  expect_int(avl_search(&avl, 4), 1, "4 inserted, 4 found");
  expect_int(avl.tree[left_index(0)].key, 4, "4 to the left of 5");

  avl_insert(&avl, 7);

  expect_int(avl_search(&avl, 7), 1, "7 inserted, 7 found");
  expect_int(avl.tree[right_index(0)].key, 7, "7 to the right of 5");

  avl_insert(&avl, 10);

  expect_int(avl_search(&avl, 10), 1, "10 inserted, 10 found");
  expect_int(avl.tree[right_index(right_index(0))].key, 10, "10 to the right of 7");
}

void test_right_rotate(){
  puts("testing R rotation");
  struct avl_tree avl = avl_create(10);

  avl_insert(&avl, 3);
  avl_insert(&avl, 2);

  expect_int(avl.tree[0].key, 3, "3 at root");
  expect_int(avl.tree[left_index(0)].key, 2, "2 to the left of 3");

  //this should make tree unbalanced and cause a right rotation
  avl_insert(&avl, 1);

  expect_int(avl.tree[0].key, 2, "2 at root");
  expect_int(avl.tree[left_index(0)].key, 1, "1 to the left of 2");
  expect_int(avl.tree[right_index(0)].key, 3, "3 to the right of 2");
}

void test_left_rotate(){
  puts("testing L rotation");
  struct avl_tree avl = avl_create(10);

  avl_insert(&avl, 1);
  avl_insert(&avl, 2);

  expect_int(avl.tree[0].key, 1, "1 at root");
  expect_int(avl.tree[right_index(0)].key, 2, "2 to the right of 1");

  //this should make tree unbalanced and cause a left rotation
  avl_insert(&avl, 3);

  expect_int(avl.tree[0].key, 2, "2 at root");
  expect_int(avl.tree[left_index(0)].key, 1, "1 to the left of 2");
  expect_int(avl.tree[right_index(0)].key, 3, "3 to the right of 2");
}

void test_left_right_rotate(){
  puts("testing LR rotation");
  struct avl_tree avl = avl_create(10);

  avl_insert(&avl, 3);
  avl_insert(&avl, 1);

  expect_int(avl.tree[0].key, 3, "3 at root");
  expect_int(avl.tree[left_index(0)].key, 1, "1 to the left of 3");

  //this should make tree unbalanced and cause a left right rotation
  avl_insert(&avl, 2);

  expect_int(avl.tree[0].key, 2, "2 at root");
  expect_int(avl.tree[left_index(0)].key, 1, "1 to the left of 2");
  expect_int(avl.tree[right_index(0)].key, 3, "3 to the right of 2");
}

void test_right_left_rotate(){
  puts("testing RL rotation");
  struct avl_tree avl = avl_create(10);

  avl_insert(&avl, 1);
  avl_insert(&avl, 3);

  expect_int(avl.tree[0].key, 1, "1 at root");
  expect_int(avl.tree[right_index(0)].key, 3, "3 to the right of 1");

  //this should make tree unbalanced and cause a right left rotation
  avl_insert(&avl, 2);

  expect_int(avl.tree[0].key, 2, "2 at root");
  expect_int(avl.tree[left_index(0)].key, 1, "1 to the left of 2");
  expect_int(avl.tree[right_index(0)].key, 3, "3 to the right of 2");
}

void test_case1(){
  struct avl_tree avl = avl_create(100);

  avl_insert(&avl, 1);
  avl_insert(&avl, 3);
}

int main(){
  test_general();
  test_right_rotate();
  test_left_rotate();
  test_left_right_rotate();
  test_right_left_rotate();

  return 0;
}


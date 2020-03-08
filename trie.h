/*
CptS 471/571 Assignment Cover Sheet

Assignment: Programming Project 1
Authors: Cassidy, Ace
Authors: Stoddard, Miranda

https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Typeof.html#Typeof
https://stackoverflow.com/questions/3161054/static-and-external-variables


I certify that I have listed above all the sources that I consulted regarding
this assignment, and that I have not received or given any assistance that is
contrary to the letter or the spirit of the collaboration guidelines for this
assignment.

Dated: January, 31, 2020
*/

#pragma once

// INCUDES //
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// MACROS //
#define NOEXIST -1

// TYPES //
typedef struct node {
  int str_depth;
  struct node **children;
  int edge_start, edge_end;
  struct node *suff_link;
  int id;
} node;

typedef struct tree {
  node *root;
} tree;

// GLOBALS //
static tree *T;
static node *PARENT, *GRANDPARENT;
static char *SEQ;
static int SEQLEN;
static int SYMTABLE[128]; // Number of ascii characters
static int SYMSIZE;
static int LEAFID;
static int INTERNALID;

// FUNCTIONS //

// FILE PARSING //

// read_SEQS: parse the given file for the first two sequence found
bool read_seq(FILE *f) {
  char line[2000];
  fseek(f, 0L, SEEK_END);
  int sz = ftell(f);
  rewind(f);
  char *s = (char *)calloc(sizeof(char), sz);
  while (fgets(line, 2000, f)) {
    if (line[0] == '>')
      continue;
    if (line[strlen(line) - 1] == '\n')
      line[strlen(line) - 1] = 0;
    strcat(s, line);
  }
  strcat(s, "$");
  SEQLEN = strlen(s);
  LEAFID = 0;
  INTERNALID = SEQLEN;
  if (SEQLEN == 0)
    return false;
  SEQ = s;
  return true;
}

bool read_symtable(FILE *f) {
  for (int i = 0; i < 128; i++)
    SYMTABLE[i] = NOEXIST;

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET); /* same as rewind(f); */
  char *string = malloc(fsize + 1);
  fread(string, 1, fsize, f);
  string[fsize] = 0; // null terminate
  SYMTABLE[(int)'$'] = 0;
  int id = 1; // $ has already taken 0th index
  char *sym;
  sym = strtok(string, " \n\t");
  while (sym != 0) {
    SYMTABLE[(int)sym[0]] = id++;
    sym = strtok(NULL, " \n\t");
  }
  SYMSIZE = id;
  return true;
}

// TRIE OPERATIONS
node *create_node() {
  node *n = (node *)malloc(sizeof(node));
  *n = (node){
      .edge_start = 0,
      .edge_end = 0,
      .str_depth = 0,
  };
  n->children = (node **)malloc(sizeof(node *) * SYMSIZE);
  for (int i = 0; i < SYMSIZE; i++)
    n->children[i] = NULL;
  return n;
};

int edge_cmp(node *n, int cur_s) {
  int length = n->edge_end - n->edge_start;
  for (int i = 0; i < length; i++) {
    if (SEQ[n->edge_start + i] != SEQ[cur_s + i]) {
      return i;
    }
  }
  return length;
}

node *find_path(node *n, int index) {

  char c = SEQ[index];
  int d = SYMTABLE[(int)c];
  if (d == NOEXIST) {
    printf("%c isn't within the accepted alphabet.", c);
    return NULL;
  }
  node *next = n->children[d];
  if (next == NULL) {
    // NEW LEAF NODE
    node *new_node = create_node();
    n->children[d] = new_node;
    *new_node = (node){.edge_start = index,
                       .edge_end = SEQLEN - 1,
                       .str_depth = n->str_depth + ((SEQLEN - 1) - index),
                       .id = LEAFID++};
    GRANDPARENT = PARENT;
    PARENT = n;
    return new_node;
  } else {
    // INTERNAL NODE
    // check if child node edge path deviates from s
    int break_count = edge_cmp(next, index);
    if (break_count != n->edge_end - n->edge_start) {
      node *new_child = create_node();
      // change parent to point to new child
      n->children[d] = new_child;
      // set new childs edge label
      new_child->edge_start = next->edge_start;
      new_child->edge_end = next->edge_start + break_count - 1;
      // change og childs edge label to be shorter
      next->edge_start = new_child->edge_end + 1;
      // set new childs child to og child
      new_child->children[SYMTABLE[SEQ[next->edge_start]]] = next;
      // set id
      new_child->id = INTERNALID++;
      // set strdepth
      new_child->str_depth = n->str_depth + break_count;
      // set next to child
      next = new_child;
    }

    GRANDPARENT = PARENT;
    PARENT = n;
    return find_path(next, index);
  }
}

void BWT(node *n) {
  if (n->children != NULL){
    for (int i = 0; i < SYMSIZE; i++) {    
      if (n->children[i] != NULL)
        BWT(n->children[i]);
    }
  }
  
  else{
    printf("%c", SEQ[(n->id - 1) < 0 ? SEQLEN - 1 : n->id - 1]);
  }
}

tree *create_tree(char *s) {
  free(T);
  T = (tree *)malloc(sizeof(tree));
  T->root = create_node();
  T->root->id = INTERNALID++;
  for (int i = 0; i < SEQLEN; i++)
    find_path(T->root, i);

  return T;
}

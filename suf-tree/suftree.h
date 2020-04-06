/*
CptS 471/571 Assignment Cover Sheet

Assignment: Programming Project 2
Authors: Cassidy, Ace
Authors: Stoddard, Miranda

https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Typeof.html#Typeof
https://stackoverflow.com/questions/3161054/static-and-external-variables


I certify that I have listed above all the sources that I consulted regarding
this assignment, and that I have not received or given any assistance that is
contrary to the letter or the spirit of the collaboration guidelines for this
assignment.

Dated: March, 18, 2020
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

typedef struct int_tuple {
  int top, bottom;
} int_tuple;

typedef struct edge_ref {
  int_tuple ref;
  int seq_index;
} edge_ref;

typedef struct node {
  int str_depth;
  int id;
  edge_ref edge;
  struct node *parent;
  struct node *suff_link;
  struct node **children;
  bool mixed;
} node;

typedef struct tree {
  node *root;
} tree;

// GLOBALS //
static node *ROOT;
static char *SEQARR[100];
static int SEQID = -1;
static int CURSEQLEN;
static int SYMTABLE[128]; // Number of ascii characters
static int SYMSIZE;
static int LEAFID = 0;
static int INTERNALID = 0;

// FUNCTIONS //
bool read_symtable(FILE *);
int read_seq(FILE *);
int edge_cmp(node *, int);
node *create_node();
node *find_path(node *, int);
node *node_hops(node *, edge_ref);
node *suffix_cases(node *);
tree *create_tree();
tree *insert_seq(tree *t);
tree *test();
void BWT(node *);
void print_deepest();
void print_post_order(node *n);
void print_tree(node *);
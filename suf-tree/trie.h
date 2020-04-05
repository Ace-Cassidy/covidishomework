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

typedef struct node {
  int str_depth;
  int id;
  int_tuple edge_label;
  struct node *parent;
  struct node *suff_link;
  struct node **children;
} node;

typedef struct tree {
  node *root;
} tree;

// GLOBALS //
tree *T;

static char *SEQ;
static int SEQLEN;
static int SYMTABLE[128]; // Number of ascii characters
static int SYMSIZE;
static int LEAFID;
static int INTERNALID;

// FUNCTIONS //
bool read_seq(FILE *);
bool read_symtable(FILE *);
int edge_cmp(node *, int);
node *create_node();
node *find_path(node *, int);
node *node_hops(node *, int_tuple);
node *suffix_cases(node *);
tree *create_tree(char *sequence, char *alphabet);
tree *test();
void BWT(node *);
void print_deepest();
void print_post_order(node *n);
void print_tree(node *);

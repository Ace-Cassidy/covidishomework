
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

typedef struct int_tupl {
  int start, end;
} int_tuple;
typedef struct node {
  int str_depth;
  int id;
  int_tuple edge_label;
  struct node *parent;
  struct node **children;
  struct node *suff_link;
} node;

typedef struct tree {
  node *root;
} tree;

// GLOBALS //
static tree *T;

static char *SEQ;
static int SEQLEN;
static int SYMTABLE[128]; // Number of ascii characters
static int SYMSIZE;
static int LEAFID;
static int INTERNALID;

// FUNCTIONS //
tree *create_tree(char *s);
node *suffix_cases(node *leaf);
void BWT(node *n);
node *node_hops(node *start, int_tuple edge_label);
node *find_path(node *n, int index);
void print_tree(node *start);
int edge_cmp(node *n, int cur_s);
node *create_node();
bool read_symtable(FILE *f);
bool read_seq(FILE *f);

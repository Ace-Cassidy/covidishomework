/*
CptS 471/571 Assignment Cover Sheet

Assignment: Programming Project 3
Authors: Cassidy, Ace
Authors: Stoddard, Miranda

I certify that I have listed above all the sources that I consulted regarding
this assignment, and that I have not received or given any assistance that is
contrary to the letter or the spirit of the collaboration guidelines for this
assignment.

Dated: April, 5, 2020
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
static int CURSEQID = -1;
static int CURSEQLEN;
static int SYMTABLE[128]; // Number of ascii characters
static int SYMSIZE;
static int NODEID = 0;
static FILE *DOTFILE;

// FUNCTIONS //
int read_seq(FILE *f);
bool read_symtable(FILE *f);
node *find_path(node *n,int index);
node *node_hops(node *start,edge_ref b);
node *suffix_cases(node *leaf);
tree *create_tree();
tree *insert_seq(tree *t);
void dfs(node *n,void(*func)(node *));
int edge_len(edge_ref e);
int edge_cmp(node *n,int seq_index);
node *create_node();
node **create_children();
node *find_deepest_internal(node *curr_node);
void print_edge(node *n);
void print_node(node *n);
void print_tree(tree *t);
void print_dots(node *n);
void print_dotfile(tree *t);
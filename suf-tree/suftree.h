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
#include "../utility/utility.h"
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
static int FINGERPRINTSEQ;
static node TOPMIXED;
static node BOTMIXED;
static char LASTCHAR;

// FUNCTIONS //
bool read_symtable(FILE *f);
char **get_fingerprints(tree *t);
char *get_pathlabel(node *n);
int edge_cmp(node *n, int seq_index);
int edge_len(edge_ref e);
int read_seq(FILE *f);
node **create_children();
node *create_node();
node *dfs_lowest_mixed2(node *curr_node);
node *dfs_set_mixed(node *curr_node);
node *dfs(node *n, node *(*func)(node *));
node *find_deepest_internal(node *curr_node);
node *find_path(node *n, int index);
node *lcs(tree *t);
node *node_hops(node *start, edge_ref b);
node *print_dots(node *n);
node *print_node(node *n);
node *suffix_cases(node *leaf);
tree *create_tree();
tree *insert_seq(tree *t, int seqid);
void print_dotfile(tree *t);
void print_edge(node *n);
void print_tree(tree *t);
void free_tree(tree *t);
int get_SYMSIZE();

node *get_ROOT();
char **get_SEQARR();
int get_CURSEQID();
int get_CURSEQLEN();
int *get_SYMTABLE();
int get_NODEID();
FILE *get_DOTFILE();
int get_FINGERPRINTSEQ();
node get_TOPMIXED();
node get_BOTMIXED();
char get_LASTCHAR();
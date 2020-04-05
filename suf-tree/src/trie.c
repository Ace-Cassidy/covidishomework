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

#include "trie.h"

// FUNCTIONS //

// file parsing //

// read_seq: parse the given file for the first two sequence found
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

// read_symtable: parse the file for alphabet
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

// utility //

// edge_len: return the length (inclusive) of an int tuple
int edge_len(int_tuple e) { return e.bottom - e.top + 1; }

// trie operations //

// create_node: malloc and init node struct
node *create_node() {
  node *n = (node *)malloc(sizeof(node));
  // init fields
  *n = (node){.str_depth = 0,
              .id = NOEXIST,
              .edge_label = (int_tuple){0, 0},
              .parent = NULL,
              .suff_link = NULL};
  // init children
  n->children = (node **)malloc(sizeof(node *) * SYMSIZE);
  for (int i = 0; i < SYMSIZE; i++)
    n->children[i] = NULL;
  return n;
}

// edge_cmp: return the index of first mismatch between edge and seq_index
// returns edge_len if no mismatches (one past last index)
int edge_cmp(node *n, int seq_index) {
  int length = edge_len(n->edge_label);
  for (int i = 0; i < length; i++) {
    if (SEQ[n->edge_label.top + i] != SEQ[seq_index + i]) {
      return i;
    }
  }
  return length;
}

void print_edge(int_tuple edge) {
  for (int i = edge.top; i <= edge.bottom; i++) {
    putchar(SEQ[i]);
  }
}

// print_node: pretty print the given node
void print_node(node *n) {
  if (n->children != NULL) {
    print_edge(n->edge_label);
    printf(" (%d)\n", n->str_depth);
    printf("PARENT %-3d <== NODE %-3d ==> CHILDREN ", n->parent->id, n->id);
    for (int i = 0; i < SYMSIZE; i++) {
      if (n->children[i] != NULL) {
        printf("%-3d ", n->children[i]->id);
      }
    }
    printf("\nString Depth %d\n", n->str_depth);
    printf("\n");
  }
}

// print_tree: recursively prints nodes via DFS
void print_tree(node *n) {
  print_node(n);
  if (n->children != NULL) {
    for (int i = 0; i < SYMSIZE; i++) {
      if (n->children[i] != NULL) {
        print_tree(n->children[i]);
      }
    }
  }
}

void print_post_order(node *n) {
  if (n->children != NULL) {
    for (int i = SYMSIZE - 1; i >= 0; i--) {
      if (n->children[i] != NULL) {
        print_tree(n->children[i]);
      }
    }
  }
  print_node(n);
}

// find_path: find/insert suffix and return leaf node
node *find_path(node *n, int seq_index) {

  // LOCATE NEXT NODE
  char c = SEQ[seq_index];
  int child_index = SYMTABLE[(int)c];
  if (child_index == NOEXIST) {
    printf("%c isn't within the accepted alphabet.", c);
    return NULL;
  }
  node *next = n->children[child_index];

  if (next == NULL) {
    // NEW LEAF NODE
    node *new_leaf = create_node();
    n->children[child_index] = new_leaf;
    new_leaf->edge_label.top = seq_index;
    new_leaf->edge_label.bottom = SEQLEN - 1;
    new_leaf->str_depth = n->str_depth + ((SEQLEN)-seq_index);
    new_leaf->id = LEAFID++;
    new_leaf->parent = n;
    return new_leaf;
  } else {
    // check if next node edge label deviates from seq_index
    int mismatch = edge_cmp(next, seq_index);
    if (mismatch != edge_len(next->edge_label)) {
      // NEW INTERNAL NODE
      node *new_child = create_node();
      // change parent to point to new child
      n->children[child_index] = new_child;
      new_child->parent = n;
      next->parent = new_child;
      // set new childs edge label
      new_child->edge_label.top = next->edge_label.top;
      new_child->edge_label.bottom = next->edge_label.top + mismatch - 1;
      // change old childs edge label to be shorter
      next->edge_label.top = new_child->edge_label.bottom + 1;
      // set new childs child to old child
      new_child->children[SYMTABLE[SEQ[next->edge_label.top]]] = next;
      // set id
      new_child->id = INTERNALID++;
      // set strdepth
      new_child->str_depth = n->str_depth + mismatch;
      // set next to child
      next = new_child;
    }
    // RECURSE DOWN
    seq_index += mismatch;
    return find_path(next, seq_index);
  }
}

// node_hops: quick traverse tree by assuming that each hop is correct
// create internal node if last hop ends in edge middle
node *node_hops(node *start, int_tuple b) {

  // edge case //
  if (b.top > b.bottom)
    return T->root;

  int child_index;
  node *cur_node;
  cur_node = start;
  // descend to v //
  while (edge_len(b) > 0) {
    child_index = SYMTABLE[(int)SEQ[b.top]];
    // go to child node
    cur_node = cur_node->children[child_index];
    // decrement by path length of child
    b.top += edge_len(cur_node->edge_label);
  }

  // found v //
  if (edge_len(b) == 0)
    return cur_node;

  // otherwise create v //
  // go back up one node
  node *par_node = cur_node->parent;
  // calculate depth to insert new node, note len of b is negative
  int break_depth = edge_len(cur_node->edge_label) + edge_len(b);
  // create internal node
  node *new_node = create_node();
  par_node->children[child_index] = new_node;
  // set parents
  new_node->parent = par_node;
  cur_node->parent = new_node;
  // set new nodes edge label
  new_node->edge_label.top = cur_node->edge_label.top;
  new_node->edge_label.bottom = cur_node->edge_label.top + break_depth - 1;
  // shorten cur nodes edge label
  cur_node->edge_label.top = new_node->edge_label.bottom + 1;
  // set new nodes child to cur_node
  new_node->children[SYMTABLE[SEQ[cur_node->edge_label.top]]] = cur_node;
  // set new_node id
  new_node->id = INTERNALID++;
  // set strdepth
  new_node->str_depth = par_node->str_depth + break_depth;
  return new_node;
}

// BWT: semi unique encoding of leaf ordering for tree verification
void BWT(node *n) {
  for (int i = 0; i < SYMSIZE; i++) {
    if (n->children[i] != NULL)
      BWT(n->children[i]);
  }
  if (n->id < SEQLEN)
    printf("%c\n", SEQ[(n->id - 1) < 0 ? SEQLEN - 1 : n->id - 1]);
}

// suffix_cases: return suffix link of given leaf's parent
node *suffix_cases(node *leaf) {
  node *u = leaf->parent, *u_prime = u->parent, *v_prime = u_prime->suff_link;
  node *v = NULL;
  if (u->suff_link) { // SL(u) is known
    // CASE IA - SL(u) is known and u is not root
    // CASE IB   - SL(u) is known and u is root
    v = u->suff_link;
  } else { // SL(u) is unknown
    // CASE IIA - SL(u) is unknown and u' is not root
    int_tuple edge = u->edge_label;
    // CASE IIB - SL(u) is unknown and u' is root
    if (u->parent == T->root) {
      edge.top += 1;
    }
    v = node_hops(v_prime, edge);
    u->suff_link = v;
  }
  return v;
}

int count_internal_nodes(node *curr_node) {
  int count = 0;
  if (curr_node == NULL) {
    return count;
  }
  if (curr_node->id < SEQLEN) {
    return count;
  }
  for (int i = 0; i < SYMSIZE; i++) {
    count += count_internal_nodes(curr_node->children[i]);
  }
  return count + 1;
}

int count_leaf_nodes(node *curr_node) {
  int count = 0;
  if (curr_node == NULL) {
    return count;
  }
  for (int i = 0; i < SYMSIZE; i++) {
    count += count_leaf_nodes(curr_node->children[i]);
  }
  if (curr_node->id < SEQLEN) {
    return count + 1;
  } else {
    return count;
  }
}

node *find_deepest_internal(node *curr_node) {
  node *max = curr_node;
  node *temp_max;
  if (curr_node == NULL) {
    return 0;
  }
  if (curr_node->id < SEQLEN) {
    return 0;
  }
  for (int i = 0; i < SYMSIZE; i++) {
    temp_max = find_deepest_internal(curr_node->children[i]);
    if (temp_max != NULL) {
      if (temp_max->str_depth > max->str_depth) {
        max = temp_max;
      }
    }
  }
  return max;
}

int add_depths(node *curr_node) {
  int total = 0;
  if (curr_node == NULL) {
    return 0;
  }
  if (curr_node->id < SEQLEN) {
    return 0;
  }
  for (int i = 0; i < SYMSIZE; i++) {
    total += add_depths(curr_node->children[i]);
  }

  return total + curr_node->str_depth;
}

float find_average_depth() {
  int total = add_depths(T->root);
  int node_count = count_internal_nodes(T->root);
  return (float)total / node_count;
}

// create_tree: insert all suffixes of sequence into tree
tree *create_tree(char *sequence, char *alphabet) {
  free(T);
  T = (tree *)malloc(sizeof(tree));
  T->root = create_node();
  T->root->str_depth = 0;
  T->root->id = INTERNALID++;
  T->root->edge_label = (int_tuple){NOEXIST, NOEXIST};
  T->root->parent = T->root;
  T->root->suff_link = T->root;

  node *leaf;
  node *v = T->root;
  for (int i = 0; i < SEQLEN; i++) {
    leaf = find_path(v, i + v->str_depth);
    v = suffix_cases(leaf);
  }
  return T;
}

tree *test() {
  T = create_tree(SEQ, "");
  int internal_count = count_internal_nodes(T->root);
  printf("Internal Nodes: %d\n", internal_count);
  int leaf_count = count_leaf_nodes(T->root);
  printf("Leaf Nodes: %d\n", leaf_count);
  printf("Total Nodes: %d\n", internal_count + leaf_count);
  node *deepest_internal = find_deepest_internal(T->root);
  printf("Deepest Internal Node: %d\n", deepest_internal->str_depth);
  float average_internal = find_average_depth(T->root);
  printf("Average Depth of Internal Nodes: %f\n", average_internal);
  printf("Size of Tree: %ld\n",
         (leaf_count + internal_count) * (sizeof(node) + (SYMSIZE * 4) + 8));
  return T;
}

void print_deepest() {
  node *deep = find_deepest_internal(T->root);
  int start = deep->edge_label.bottom - deep->str_depth + 1;
  int end = deep->edge_label.bottom;
  int_tuple longest;
  longest.top = start;
  longest.bottom = end;
  print_edge(longest);
  printf("\n");
}
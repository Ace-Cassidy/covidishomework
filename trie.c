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

//
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

// trie operations //

//
node *create_node() {
  node *n = (node *)malloc(sizeof(node));
  *n = (node){.edge_label.top = 0,
              .edge_label.bottom = 0,
              .str_depth = 0,
              .suff_link = NULL};
  n->children = (node **)malloc(sizeof(node *) * SYMSIZE);
  for (int i = 0; i < SYMSIZE; i++)
    n->children[i] = NULL;
  return n;
};

//
int edge_cmp(node *n, int cur_s) {
  int length = n->edge_label.bottom - n->edge_label.top;
  for (int i = 0; i < length; i++) {
    if (SEQ[n->edge_label.top + i] != SEQ[cur_s + i]) {
      return i;
    }
  }
  return length;
}

//
void print_node(node *n) {
  if (n->children != NULL) {
    printf(" PARENT %-3d <== NODE %-3d at STRDEPTH %-3d ==> CHILDREN ",
           n->parent->id, n->id, n->str_depth);
    for (int i = 0; i < SYMSIZE; i++) {
      if (n->children[i] != NULL) {
        printf("%-3d ", n->children[i]->id);
      }
    }
    printf("\n");
  }
}

//
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

//
node *find_path(node *n, int strindex) {

  char c = SEQ[strindex];
  int child_no = SYMTABLE[(int)c];
  if (child_no == NOEXIST) {
    printf("%c isn't within the accepted alphabet.", c);
    return NULL;
  }
  node *next = n->children[child_no];
  if (next == NULL) {
    // NEW LEAF NODE
    node *new_leaf = create_node();
    n->children[child_no] = new_leaf;
    new_leaf->edge_label.top = strindex;
    new_leaf->edge_label.bottom = SEQLEN - 1;
    new_leaf->str_depth = n->str_depth + ((SEQLEN)-strindex);
    new_leaf->id = LEAFID++;
    new_leaf->parent = n;
    return new_leaf;
  } else {
    // INTERNAL NODE
    // check if child node edge path deviates from s
    int break_count = edge_cmp(next, strindex);
    if (break_count != n->edge_label.bottom - n->edge_label.top) {
      node *new_child = create_node();
      // change parent to point to new child
      n->children[child_no] = new_child;
      new_child->parent = n;
      next->parent = new_child;
      // set new childs edge label
      new_child->edge_label.top = next->edge_label.top;
      new_child->edge_label.bottom = next->edge_label.top + break_count - 1;
      // change og childs edge label to be shorter
      next->edge_label.top = new_child->edge_label.bottom + 1;
      // set new childs child to og child
      new_child->children[SYMTABLE[SEQ[next->edge_label.top]]] = next;
      // set id
      new_child->id = INTERNALID++;
      // set strdepth
      new_child->str_depth = n->str_depth + break_count;
      // set next to child
      next = new_child;
    }
    strindex += break_count;
    return find_path(next, strindex);
  }
}

//
int edge_len(int_tuple e) { return e.bottom - e.top + 1; }

//
node *node_hops(node *start, int_tuple b) {

  // edge case //
  if (b.top > b.bottom)
    return T->root;

  int child_index;
  node *cur_node;
  // descend to v //
  while (edge_len(b) > 0) {
    child_index = SYMTABLE[(int)SEQ[b.top]];
    // go to child node
    cur_node = start->children[child_index];
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

//
void BWT(node *n) {
  for (int i = 0; i < SYMSIZE; i++) {
    if (n->children[i] != NULL)
      BWT(n->children[i]);
  }
  if (n->id < SEQLEN)
    printf("%c", SEQ[(n->id - 1) < 0 ? SEQLEN - 1 : n->id - 1]);
}

//
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

//
tree *create_tree(char *s) {
  free(T);
  T = (tree *)malloc(sizeof(tree));
  T->root = create_node();
  T->root->str_depth = 0;
  T->root->id = INTERNALID++;
  T->root->edge_label = (int_tuple){0, 0};
  T->root->parent = T->root;
  T->root->suff_link = T->root;

  node *leaf;
  node *v = T->root;
  for (int i = 0; i < SEQLEN; i++) {
    printf("INSERT %s\n", SEQ + i);
    leaf = find_path(v, i + v->str_depth);
    v = suffix_cases(leaf);
    print_tree(T->root);
    BWT(T->root);
    printf("\n");
  }
  return T;
}

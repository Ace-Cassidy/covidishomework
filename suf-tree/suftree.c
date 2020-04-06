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

#include "suftree.h"

// FUNCTIONS //

// file parsing //

// read_seq: parse the given file for the first sequence found
int read_seq(FILE *f) {
  SEQID++;
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
  CURSEQLEN = strlen(s);
  LEAFID = INTERNALID;
  INTERNALID += CURSEQLEN;
  if (CURSEQLEN == 0)
    return false;
  SEQARR[SEQID] = s;
  return SEQID;
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
int edge_len(edge_ref e) { return e.ref.bottom - e.ref.top + 1; }

// trie operations //

// create_node: malloc and init node struct
node *create_node() {
  node *n = (node *)malloc(sizeof(node));
  // init fields
  *n = (node){.str_depth = NOEXIST,
              .id = NOEXIST,
              .edge = (edge_ref){NOEXIST, NOEXIST, SEQID},
              .parent = NULL,
              .suff_link = NULL,
              .children = NULL,
              .mixed = false};
  return n;
}

node **create_children() {
  node **children = (node **)malloc(sizeof(node *) * SYMSIZE);
  for (int i = 0; i < SYMSIZE; i++)
    children[i] = NULL;
  return children;
}

// edge_cmp: return the index of first mismatch between edge and seq_index
// returns edge_len if no mismatches (one past last index)
int edge_cmp(node *n, int seq_index) {
  int length = edge_len(n->edge);
  for (int i = 0; i < length; i++) {
    if (SEQARR[n->edge.seq_index][n->edge.ref.top + i] !=
        SEQARR[SEQID][seq_index + i]) {
      return i;
    }
  }
  return length;
}

// find_path: find/insert suffix and return leaf node
node *find_path(node *n, int index) {
  int mismatch;
  while (true) {

    // FOUND NODE
    if (index == CURSEQLEN) {
      return n;
    }

    // LOCATE NEXT NODE
    char c = SEQARR[SEQID][index];
    int child_index = SYMTABLE[(int)c];
    if (child_index == NOEXIST) {
      printf("%c isn't within the accepted alphabet.\n", c);
      return NULL;
    }
    node *next = n->children[child_index];

    // NEW LEAF NODE
    if (next == NULL) {
      node *new_leaf = create_node();
      n->children[child_index] = new_leaf;
      new_leaf->edge.ref.top = index;
      new_leaf->edge.ref.bottom = CURSEQLEN - 1;
      new_leaf->str_depth = n->str_depth + ((CURSEQLEN)-index);
      new_leaf->id = LEAFID++;
      new_leaf->parent = n;
      return new_leaf;
    }

    // check if next node edge label deviates from seq_index
    mismatch = edge_cmp(next, index);
    if (mismatch != edge_len(next->edge)) {
      // NEW INTERNAL NODE
      node *new_child = create_node();
      new_child->children = create_children();
      // change parent to point to new child
      n->children[child_index] = new_child;
      new_child->parent = n;
      next->parent = new_child;
      // set new childs edge label
      new_child->edge.ref.top = next->edge.ref.top;
      new_child->edge.ref.bottom = next->edge.ref.top + mismatch - 1;
      // change old childs edge label to be shorter
      next->edge.ref.top = new_child->edge.ref.bottom + 1;
      // set new childs child to old child
      new_child->children[SYMTABLE[SEQARR[next->edge.seq_index][next->edge.ref.top]]] =
          next;
      // set id
      new_child->id = INTERNALID++;
      // set strdepth
      new_child->str_depth = n->str_depth + mismatch;
      // set next to child
      next = new_child;
    }

    // RECURSE DOWN
    index += mismatch;
    n = next;
  }
}

// node_hops: quick traverse tree by assuming that each hop is correct
// create internal node if last hop ends in edge middle
node *node_hops(node *start, edge_ref b) {

  // edge case //
  if (b.ref.top > b.ref.bottom)
    return ROOT;

  int child_index;
  node *cur_node;
  cur_node = start;
  // descend to v //
  while (edge_len(b) > 0) {
    child_index = SYMTABLE[(int)SEQARR[b.seq_index][b.ref.top]];
    // go to child node
    cur_node = cur_node->children[child_index];
    // decrement by path length of child
    b.ref.top += edge_len(cur_node->edge);
  }

  // found v //
  if (edge_len(b) == 0)
    return cur_node;

  // otherwise create v //
  // go back up one node
  node *par_node = cur_node->parent;
  // calculate depth to insert new node, note len of b is negative
  int break_depth = edge_len(cur_node->edge) + edge_len(b);
  // create internal node
  node *new_node = create_node();
  new_node->children = create_children();
  par_node->children[child_index] = new_node;
  // set parents
  new_node->parent = par_node;
  cur_node->parent = new_node;
  // set new nodes edge label
  new_node->edge.ref.top = cur_node->edge.ref.top;
  new_node->edge.ref.bottom = cur_node->edge.ref.top + break_depth - 1;
  // shorten cur nodes edge label
  cur_node->edge.ref.top = new_node->edge.ref.bottom + 1;
  // set new nodes child to cur_node
  new_node
      ->children[SYMTABLE[SEQARR[cur_node->edge.seq_index][cur_node->edge.ref.top]]] =
      cur_node;
  // set new_node id
  new_node->id = INTERNALID++;
  // set strdepth
  new_node->str_depth = par_node->str_depth + break_depth;
  return new_node;
}

// suffix_cases: return suffix link of given leaf's parent
node *suffix_cases(node *leaf) {
  node *u = leaf->parent, *u_prime = u->parent, *v_prime = u_prime->suff_link;
  node *v = NULL;
  if (u->suff_link) { // SL(u) is known
    v = u->suff_link;
  } else { // SL(u) is unknown
    edge_ref edge = u->edge;
    if (u->parent == ROOT) {
      edge.ref.top += 1;
    }
    v = node_hops(v_prime, edge);
    u->suff_link = v;
  }
  return v;
}

// create_tree: insert all suffixes of sequence into tree
tree *create_tree() {
  tree *t = (tree *)malloc(sizeof(tree));
  t->root = create_node();
  t->root->children = create_children();
  t->root->str_depth = 0;
  t->root->id = INTERNALID++;
  t->root->edge.ref = (int_tuple){NOEXIST, NOEXIST};
  t->root->parent = t->root;
  t->root->suff_link = t->root;
  return t;
}

// insert_seq: inserts currently read in seq
tree *insert_seq(tree *t) {
  ROOT = t->root;
  node *leaf;
  node *v = t->root;
  for (int i = 0; i < CURSEQLEN; i++) {
    leaf = find_path(ROOT, i);
    // v = suffix_cases(leaf);
  }
  return t;
}

// stats //

node *find_deepest_internal(node *curr_node) {
  node *max = curr_node;
  node *temp_max;
  if (curr_node == NULL) {
    return 0;
  }
  if (curr_node->id < CURSEQLEN) {
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
  if (curr_node->id < CURSEQLEN) {
    return 0;
  }
  for (int i = 0; i < SYMSIZE; i++) {
    total += add_depths(curr_node->children[i]);
  }

  return total + curr_node->str_depth;
}

// print //

void print_edge(node *edge) {
  for (int i = edge->edge.ref.top; i <= edge->edge.ref.bottom; i++) {
    putchar(SEQARR[edge->edge.seq_index][i]);
  }
}

// print_node: pretty print the given node
void print_node(node *n) {
  if (n->children != NULL) {
    print_edge(n);
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
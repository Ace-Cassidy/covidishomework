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

#include "suftree.h"

// FILE PARSING //

// read_seq: parse the given file for the first sequence found
int read_seq(FILE *f) {
  CURSEQID++;
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
  if (CURSEQLEN == 0)
    return false;
  SEQARR[CURSEQID] = s;
  return CURSEQID;
}

// read_symtable: parse the file for alphabet
bool read_symtable(FILE *f) {
  for (int i = 0; i < 128; i++)
    SYMTABLE[i] = NOEXIST;

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET); /* same as rewind(f); */
  char *string = malloc(fsize + 1);
  fread(string, 1, fsize, f) == fsize ?: printf("fail to read symtable");
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

// TRIE OPERATIONS //

// find_path: find/insert suffix and return leaf node
node *find_path(node *n, int index) {
  int mismatch;
  while (true) {

    // FOUND NODE
    if (index == CURSEQLEN) {
      n->mixed = true;
      return n;
    }

    // LOCATE NEXT NODE
    char c = SEQARR[CURSEQID][index];
    int childno = SYMTABLE[(int)c];
    if (childno == NOEXIST) {
      printf("%c not in alphabet.\n", c);
      exit(1);
    }
    node *next = n->children[childno];

    // NEW LEAF NODE
    if (next == NULL) {
      node *new_leaf = create_node();
      n->children[childno] = new_leaf;
      new_leaf->edge.ref.top = index;
      new_leaf->edge.ref.bottom = CURSEQLEN - 1;
      new_leaf->str_depth = n->str_depth + ((CURSEQLEN)-index);
      new_leaf->id = NODEID++;
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
      n->children[childno] = new_child;
      new_child->parent = n;
      next->parent = new_child;
      // set new childs edge label
      new_child->edge.ref.top = index;
      new_child->edge.ref.bottom = index + mismatch - 1;
      // change old childs edge label to be shorter
      next->edge.ref.top = next->edge.ref.top + mismatch;
      // set new childs child to old child
      new_child->children
          [SYMTABLE[SEQARR[next->edge.seq_index][next->edge.ref.top]]] = next;
      // set id
      new_child->id = NODEID++;
      // set strdepth
      new_child->str_depth = n->str_depth + mismatch;
      // set next to child
      next = new_child;
    }

    // ITERATE DOWN
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
  new_node->children
      [SYMTABLE[SEQARR[cur_node->edge.seq_index][cur_node->edge.ref.top]]] =
      cur_node;
  // set new_node id
  new_node->id = NODEID++;
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
  t->root->id = NODEID++;
  t->root->edge.ref = (int_tuple){0, -1};
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
    leaf = find_path(v, i + v->str_depth);
    // v = suffix_cases(leaf);
  }
  return t;
}

node *fingerprint(node *n) {
  // n is a mixed node shallower than last seen
  if (n->str_depth <= TOPMIXED.str_depth && n->mixed) {
    for (int i = 0; i < SYMSIZE; i++) {
      // n has a child from our seq and the child is not mixed
      if (n->children && n->children[i] &&
          n->children[i]->edge.seq_index == FINGERPRINTSEQ &&
          !n->children[i]->mixed) {
        LASTCHAR = SEQARR[FINGERPRINTSEQ][n->children[i]->edge.ref.top];
        TOPMIXED = *n;
        break;
      }
    }
  }
  return &TOPMIXED;
}

node *get_topmixed(node *n, int curr_seq){
  if (n->children != NULL) {
    for (int i = 0; i < SYMSIZE; i++) {
      if (n->children[i] != NULL) {
        if (n->mixed && !n->children[i]->mixed && n->edge.seq_index == curr_seq){
          if (n->str_depth < TOPMIXED.str_depth){
            TOPMIXED = *n;
          }
        }
        else{
          get_topmixed(n->children[i], curr_seq);
        }        
      }
    }
  }
  return &TOPMIXED;
}

char **get_fingerprints(tree *t) {
  static char *fingerprints[100];
  for (int i = 0; i <= CURSEQID; i++) {
    FINGERPRINTSEQ = i;
    TOPMIXED = (node){.str_depth = INT_MAX};
    dfs(t->root, fingerprint);
    // get_topmixed(t->root, FINGERPRINTSEQ);
    char *fp = NULL;
    if (TOPMIXED.str_depth == INT_MAX) {
      fp = SEQARR[FINGERPRINTSEQ];
      fingerprints[FINGERPRINTSEQ] = fp;
      continue;
    }
    fp = get_pathlabel(&TOPMIXED);
    fp[TOPMIXED.str_depth] = LASTCHAR;
    fp[TOPMIXED.str_depth + 1] = '\0';
    fingerprints[FINGERPRINTSEQ] = fp;
  }
  return fingerprints;
}

// UTILITY //

void set_mixed(node *curr_node) {
  int seq = curr_node->edge.seq_index;
  for (int i = 0; i < SYMSIZE; i++) {
    if (curr_node->children && curr_node->children[i]) {
      set_mixed(curr_node->children[i]);
      if (curr_node->children[i]->mixed == true) {
        curr_node->mixed = true;
      } else if (curr_node->children[i]->edge.seq_index !=
                 curr_node->edge.seq_index) {
        curr_node->mixed = true;
      }
    }
  }
}

tree *label_tree(tree *t) {
  set_mixed(t->root);
  return t;
}

node *dfs_mixed(node * curr_node){
  for (int i = 0; i < SYMSIZE; i++) {
    if (curr_node->children && curr_node->children[i]) {
      if (curr_node->children[i]->mixed == true) {
        curr_node->mixed = true;
      } else if (curr_node->children[i]->edge.seq_index !=
                 curr_node->edge.seq_index) {
        curr_node->mixed = true;
      }
    }
  }
}

node *dfs(node *n, node *(*func)(node *)) {
  if (n->children != NULL) {
    for (int i = 0; i < SYMSIZE; i++) {
      if (n->children[i] != NULL) {
        dfs(n->children[i], func);
      }
    }
  }
  return func(n);
}

// edge_len: return the length (inclusive) of an int tuple
int edge_len(edge_ref e) { 
  int length = (e.ref.bottom - e.ref.top) + 1;
  return length;
}

// edge_cmp: return the index of first mismatch between edge and seq_index
// returns edge_len if no mismatches (one past last index)
int edge_cmp(node *n, int seq_index) {
  int length = edge_len(n->edge);
  for (int i = 0; i < length; i++) {
    if (SEQARR[n->edge.seq_index][n->edge.ref.top + i] !=
        SEQARR[CURSEQID][seq_index + i]) {
      return i;
    }
  }
  return length;
}

// create_node: malloc and init node struct to default values
node *create_node() {
  node *n = (node *)malloc(sizeof(node));
  *n = (node){.str_depth = NOEXIST,
              .id = NOEXIST,
              .edge = (edge_ref){NOEXIST, NOEXIST, CURSEQID},
              .parent = NULL,
              .suff_link = NULL,
              .children = NULL,
              .mixed = false};
  return n;
}
// create_children: malloc and init array of child node pointers
node **create_children() {
  node **children = (node **)malloc(sizeof(node *) * SYMSIZE);
  for (int i = 0; i < SYMSIZE; i++)
    children[i] = NULL;
  return children;
}

// PRINT //

char *get_pathlabel(node *n) {
  char *label =
      (char *)calloc((n->str_depth + 1), sizeof(char));
  while (n != ROOT) {
    char *temp = (char *)calloc(edge_len(n->edge), sizeof(char));
    strncpy(temp, &(SEQARR[n->edge.seq_index][n->edge.ref.top]), edge_len(n->edge));
    rev_str(temp);
    strcat(label, temp);
    n = n->parent;
    free(temp);
  }
  rev_str(label);
  return label;
}

// print_edge: print the path label
void print_edge(node *n) {
  for (int i = n->edge.ref.top; i <= n->edge.ref.bottom; i++) {
    putchar(SEQARR[n->edge.seq_index][i]);
  }
}

// print_node: pretty print the given node
node *print_node(node *n) {
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
  return n;
}

// print_tree: recursively prints nodes via DFS
void print_tree(tree *t) { dfs(t->root, print_node); }

// print_dots: print the dotfile line for given node
node *print_dots(node *n) {
  if (n == ROOT)
    return n;
  char *seq = SEQARR[n->edge.seq_index];
  char *label = seq + n->edge.ref.top;
  int len = edge_len(n->edge);
  if (len < 15)
    fprintf(DOTFILE, "%d -> %d[label=\"%.*s\"];\n", n->parent->id, n->id, len,
            label);
  else
    fprintf(DOTFILE, "%d -> %d[label=\"%.*s...%.*s(%d)\"];\n", n->parent->id,
            n->id, 5, label, 5, label + len - 5, len);
  return n;
}

// print_dotfile: creates a dotfile called graph.dot
void print_dotfile(tree *t) {
  DOTFILE = fopen("graph.dot", "w");
  fprintf(DOTFILE, "digraph {\n");
  dfs(t->root, print_dots);
  fprintf(DOTFILE, "}\n");
  fclose(DOTFILE);
}
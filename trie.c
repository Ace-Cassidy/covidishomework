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

// trie operations //

node *create_node() {
  node *n = (node *)malloc(sizeof(node));
  *n = (node){.edge_label.start = 0,
              .edge_label.end = 0,
              .str_depth = 0,
              .suff_link = NULL};
  n->children = (node **)malloc(sizeof(node *) * SYMSIZE);
  for (int i = 0; i < SYMSIZE; i++)
    n->children[i] = NULL;
  return n;
};

int edge_cmp(node *n, int cur_s) {
  int length = n->edge_label.end - n->edge_label.start;
  for (int i = 0; i < length; i++) {
    if (SEQ[n->edge_label.start + i] != SEQ[cur_s + i]) {
      return i;
    }
  }
  return length;
}

void print_tree(node *start) {
  printf("Crnt %d StrD %d\n", start->id, start->str_depth);
  if (start->children != NULL) {
    for (int i = 0; i < SYMSIZE; i++) {
      if (start->children[i] != NULL) {
        print_tree(start->children[i]);
      }
    }
  }
}

void print_children(node *start) {
  if (start->children != NULL) {
    printf("Cur %d", start->id);
    for (int i = 0; i < SYMSIZE; i++) {
      if (start->children[i] != NULL) {
        printf("%d, ", start->children[i]->id);
      }
    }
    printf("\n");
  }
}

node *find_path(node *n, int index) {

  char c = SEQ[index];
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
    *new_leaf = (node){.edge_label.start = index,
                       .edge_label.end = SEQLEN - 1,
                       .str_depth = n->str_depth + ((SEQLEN - 1) - index),
                       .id = LEAFID++,
                       .parent = n};
    return new_leaf;
  } else {
    // INTERNAL NODE
    // check if child node edge path deviates from s
    int break_count = edge_cmp(next, index);
    if (break_count != n->edge_label.end - n->edge_label.start) {
      node *new_child = create_node();
      // change parent to point to new child
      n->children[child_no] = new_child;
      new_child->parent = n;
      next->parent = new_child;
      // set new childs edge label
      new_child->edge_label.start = next->edge_label.start;
      new_child->edge_label.end = next->edge_label.start + break_count - 1;
      // change og childs edge label to be shorter
      next->edge_label.start = new_child->edge_label.end + 1;
      // set new childs child to og child
      new_child->children[SYMTABLE[SEQ[next->edge_label.start]]] = next;
      // set id
      new_child->id = INTERNALID++;
      // set strdepth
      new_child->str_depth = n->str_depth + break_count;
      // set next to child
      next = new_child;
    }
    index += break_count;
    return find_path(next, index); 
  }
}

node *node_hops(node *start, int_tuple edge_label) {
  char c;
  int child_no;
  int b_len = edge_label.end - edge_label.start + 1;
  node *next;
  if (edge_label.start > edge_label.end) {
    return T->root;
  }
  while (b_len > 0) { // if b_len is positive
    c = SEQ[edge_label.start];
    child_no = SYMTABLE[(int)c];
    // go to child node
    next = start->children[child_no];
    // decrement by path length of child
    b_len -= next->edge_label.end - next->edge_label.start + 1;
  }
  if (b_len == 0) {
    // found v exactly
    return next;
  } else {
    // need to make v
    // backtrack a node
    node *n = next->parent;
    // create internal node
    node *new_child = create_node();
    // set b_len to parent
    n->children[child_no] = new_child;
    b_len += next->edge_label.end - next->edge_label.start + 1;
    // change parent to point to new child
    new_child->parent = n;
    next->parent = new_child;
    // set new childs edge label
    new_child->edge_label.start = next->edge_label.start;
    new_child->edge_label.end = next->edge_label.start + b_len - 1;
    // change of childs edge label to be shorter
    next->edge_label.start = new_child->edge_label.end + 1;
    // set new childs child to og child
    new_child->children[SYMTABLE[SEQ[next->edge_label.start]]] = next;
    // set id
    new_child->id = INTERNALID++;
    // set strdepth
    new_child->str_depth = n->str_depth + b_len;
    return new_child;
  }

  // jump to next node based on first letter
  // check if gone too far
  // if not jump again
  // else if jumped exactly far enough return node
  // else if jumped too far go back one and .... create node????
}

void BWT(node *n) {
  if (n->children != NULL) {
    for (int i = 0; i < SYMSIZE; i++) {
      if (n->children[i] != NULL)
        BWT(n->children[i]);
    }
  } else {
    printf("%c", SEQ[(n->id - 1) < 0 ? SEQLEN - 1 : n->id - 1]);
  }
}

node *suffix_cases(node *leaf) {
  node *u = leaf->parent;
  node *sl_u = u->suff_link;
  node *v;
  if (sl_u) { // sl(u) is known
    if (u != T->root) {
      // CASE IA   - sl(u) is known and u is not the root
      v = u->suff_link;
    } else {
      // CASE IB   - sl(u) is known and u is the root
      v = u;
    }
  } else { // sl(u) is unknown
    // got to u'
    node *u_prime = u->parent;
    int_tuple edge;
    edge.start = u->edge_label.start + 1;
    edge.end = u->edge_label.end;
    if (u_prime != T->root) {
      // CASE IIA  - sl(u) is unknown and u' is not the root
      v = node_hops(u_prime->suff_link, edge);
      u->suff_link = v;
    } else {
    }
    // CASE IIB  - sl(u) is unknown and u' is the root
    v = node_hops(u_prime->suff_link, edge);
    u->suff_link = v;
  }
  return v;
}

tree *create_tree(char *s) {
  free(T);
  T = (tree *)malloc(sizeof(tree));
  T->root = create_node();
  T->root->id = INTERNALID++;
  T->root->suff_link = T->root;
  T->root->parent = T->root;
  node *leaf;
  node *v = T->root;
  int offset = 0;
  for (int i = 0; i < SEQLEN; i++) {
    leaf = find_path(v, i + v->str_depth);
    v = suffix_cases(leaf);
    print_tree(T->root);
    printf("\n");
    BWT(T->root);
    printf("\n");
  }
  return T;
}

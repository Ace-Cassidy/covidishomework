#include "trie.h"

int main(int argc, char const *argv[]) {
  FILE *f = fopen("./inputs/s1.fas", "r");
  read_seq(f);
  fclose(f);

  f = fopen("./inputs/English_alphabet.txt", "r");
  read_symtable(f);
  fclose(f);

  tree *T = create_tree(SEQ);

  printf("Tree created\n");

  return 0;
}

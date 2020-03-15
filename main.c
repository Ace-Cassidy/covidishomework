#include "trie.h"

int main(int argc, char const *argv[]) {
  argc == 3 ?: (printf("bad invocation\n"), exit(1));

  FILE *f = fopen(argv[1], "r");
  read_seq(f);
  fclose(f);

  f = fopen(argv[2], "r");
  read_symtable(f);
  fclose(f);

  tree *T = create_tree(SEQ);

  printf("Tree created\n");

  return 0;
}

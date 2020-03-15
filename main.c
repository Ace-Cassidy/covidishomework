#include "trie.h"

int main(int argc, char const *argv[]) {
  argc == 3 ?: (printf("bad invocation\n"), exit(1));

  FILE *f = fopen(argv[1], "r");
  f ?: (printf("failed to open input sequence file\n"), exit(1));
  read_seq(f);
  fclose(f);

  f = fopen(argv[2], "r");
  f ?: (printf("failed to open alphabet file\n"), exit(1));
  read_symtable(f);
  fclose(f);

  tree *T = create_tree(SEQ);

  printf("Tree created\n");

  return 0;
}

#include "trie.h"

int main(int argc, char const *argv[]) {
  argc == 4 ?: (printf("bad invocation\n"), exit(1));

  FILE *f = fopen(argv[1], "r");
  f ?: (printf("failed to open input sequence file\n"), exit(1));
  read_seq(f);
  fclose(f);

  f = fopen(argv[2], "r");
  f ?: (printf("failed to open alphabet file\n"), exit(1));
  read_symtable(f);
  fclose(f);

  if (strcmp(argv[3], "--bwt") == 0){
    tree *T = create_tree();
    BWT(T->root);
  }
  else if (strcmp(argv[3], "--debug") == 0){
    tree *T = test();
  }
  else if (strcmp(argv[3], "--dfs") == 0){
    tree *T = create_tree();
    print_tree(T->root);
  }
  return 0;
}

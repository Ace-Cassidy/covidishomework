#include "trie.h"

int main(int argc, char const *argv[]) {
  argc<5 && argc> 2 ?: (printf("bad invocation\n"), exit(1));

  FILE *f = fopen(argv[1], "r");
  f ?: (printf("failed to open input sequence file\n"), exit(1));
  read_seq(f);
  fclose(f);

  f = fopen(argv[2], "r");
  f ?: (printf("failed to open alphabet file\n"), exit(1));
  read_symtable(f);
  fclose(f);

  if (argc == 3) {
    create_tree(SEQ, "");
    return 0;
  }

  if (strcmp(argv[3], "--bwt") == 0) {
    create_tree(SEQ, "");
    BWT(T->root);
  } else if (strcmp(argv[3], "--stats") == 0) {
    test();
  } else if (strcmp(argv[3], "--dfs") == 0) {
    create_tree(SEQ, "");
    print_tree(T->root);
  } else if (strcmp(argv[3], "--repeat") == 0) {
    create_tree(SEQ, "");
    print_deepest(T->root);
  } else if (strcmp(argv[3], "--post") == 0) {
    create_tree(SEQ, "");
    print_post_order(T->root);
  }
  return 0;
}

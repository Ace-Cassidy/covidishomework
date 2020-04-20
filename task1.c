#include "align-table/alignment.h"
#include "suf-tree/suftree.h"
#include <stdio.h>

int main(int argc, char const *argv[]) {
  printf("begin task1\n");
  FILE *f = fopen(argv[1], "r");
  read_symtable(f);
  fclose(f);
  int seq_id;
  tree *t = create_tree();
  for (int i = 2; i < argc; i++) {
    f = fopen(argv[i], "r");
    seq_id = read_seq(f);
    insert_seq(t, i - 2);
    fclose(f);
  }

  dfs(t->root, dfs_set_mixed);
  char **seq_fps = get_fingerprints(t);
  for (int i = 0; i <= seq_id; i++) {
    printf("fingerprint %d: %s\n", i, seq_fps[i]);
    printf("fingerprint %d len: %ld\n", i, strlen(seq_fps[i]));
  }
  printf("complete task1\n");
  return 0;
}

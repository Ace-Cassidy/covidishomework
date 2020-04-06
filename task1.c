#include "align-table/alignment.h"
#include "suf-tree/suftree.h"
#include <stdio.h>

int main(int argc, char const *argv[]) {
  printf("begin task1\n");
  FILE *f = fopen(argv[1], "r");
  read_symtable(f);
  fclose(f);
  tree * t = create_tree();
  for (int i = 2; i < argc; i++) {
    f = fopen(argv[i], "r");
    int seq_index = read_seq(f);
    insert_seq(t);
    fclose(f);
  }

  printf("hello world\n");
  return 0;
}

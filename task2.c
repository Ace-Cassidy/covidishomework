#include "align-table/alignment.h"
#include "suf-tree/suftree.h"

int main(int argc, char const *argv[]) {

  // read symbol table from first arg
  printf("begin task2\n");
  FILE *f = fopen(argv[1], "r");
  read_symtable(f);
  fclose(f);

  // read in all sequences
  int num_sequences = argc - 2;
  for (int i = 2; i < argc; i++) {
    f = fopen(argv[i], "r");
    read_seq(f);
    fclose(f);
  }

  // make a table which is numsequences x numsequences large
  int **seq_table;
  seq_table = (int **)malloc(num_sequences * sizeof(int));
  for (int i = 0; i < num_sequences; i++) {
    seq_table[i] = (int *)malloc(num_sequences * sizeof(int));
  }

  // for each pair of strings
  tree *t;
  node *n;
  for (int i = 0; i < num_sequences; i++) {
    for (int j = i + 1; j < num_sequences; j++) {
      // 1.
      // call strings s_i and s_j, build GST
      t = create_tree();
      insert_seq(t, i);
      insert_seq(t, j);

      // 2.
      // identify longest common subsequence
      n = lcs(t);
      char *pl = get_pathlabel(n);
      printf("seq lcs (i: %d j: %d) = %s\n\n\n", i, j, pl);
      // find indices s_i_start, s_i_end
      // find indices s_j_start, s_j_end
      // find length B for LCS of s_i s_j

      //  For all alignments, use the following alignment parameters:
      // match score = +1, mismatch penalty = -2,
      //  opening gap (h) = -5, gap extension (g) = -1.

      // 3.
      // extract left sides of s_i and s_j
      // reverse left sides
      // do global alignment of left sides
      // find max in table for left sides, this is the left score

      // 4.
      // extract right sides of s_i and s_j
      // do global alignment of right sides
      // find max in table for right sides, this is the right score

      // 5.
      // add scores for left + right + longest common
      // similarity value for the string pair (s_i, s_j) is equal to:
      // a+b+c.

      // 6.
      // add score to table
      // Set D[i,j] = a+b+c;

      free_tree(t);
    }
  }

  printf("task2 complete\n");
  return 0;
}

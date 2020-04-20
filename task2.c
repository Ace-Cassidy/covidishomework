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
  int symsize = get_SYMSIZE();

  // make a table which is numsequences x numsequences large
  int **seq_score_table;
  seq_score_table = (int **)malloc(num_sequences * sizeof(int *));
  for (int i = 0; i < num_sequences; i++) {
    seq_score_table[i] = (int *)malloc(num_sequences * sizeof(int));
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

      // find indices s_i_start, s_i_end
      // find indices s_j_start, s_j_end
      // find length B for LCS of s_i s_j
      int lcs_len = n->str_depth;
      printf("Strlen[%d][%d] = %d\n", i, j, lcs_len);
      int index = n->edge.seq_index;
      edge_ref lcs_i;
      lcs_i.seq_index = i;
      edge_ref lcs_j;
      lcs_j.seq_index = j;
      edge_ref *other;
      if (index == i) {
        lcs_i.ref.top = n->edge.ref.bottom - n->str_depth + 1;
        lcs_i.ref.bottom = n->edge.ref.bottom;
        other = &lcs_j;
      } else {
        lcs_j.ref.top = n->edge.ref.bottom - n->str_depth + 1;
        lcs_j.ref.bottom = n->edge.ref.bottom;
        other = &lcs_i;
      }
      for (int k = 0; k < symsize; k++) {
        if (n->children[k] &&
            n->children[k]->edge.seq_index == other->seq_index) {
          other->ref.top =
              n->children[k]->edge.ref.bottom - n->children[k]->str_depth + 1;
          other->ref.bottom = n->children[k]->edge.ref.top - 1;
          break;
        }
      }

      // 3.
      // extract left sides of s_i and s_j
      char *string_i_left = (char *)calloc(lcs_i.ref.top, sizeof(char));
      char *string_j_left = (char *)calloc(lcs_j.ref.top, sizeof(char));
      strncpy(string_i_left, get_SEQARR()[i], lcs_i.ref.top);
      strncpy(string_j_left, get_SEQARR()[j], lcs_j.ref.top);
      // reverse left sides
      rev_str(string_i_left);
      rev_str(string_j_left);
      TABLE *T1 = (TABLE *)malloc(sizeof(TABLE));
      set_S1(string_i_left);
      set_S2(string_j_left);
      set_ISLOCAL(false);
      //  For all alignments, use the following alignment parameters:
      set_MATCH(1), set_MISMATCH(-2), set_GAP(-1), set_OPEN(-5);
      allocate_table(T1) ?: (puts("fail to allocate memory\n"), exit(1));
      // do global alignment of left sides
      align(*T1);
      // find max in table for left sides, this is the left score
      int left_max_score = get_TABLEMAX(T1);
      free_table(T1);
      free(string_i_left);
      free(string_j_left);

      // 4.
      // extract right sides of s_i and s_j
      // do global alignment of right sides
      // find max in table for right sides, this is the right score
      char *string_i_right = (char *)calloc(
          strlen(get_SEQARR()[i]) - lcs_i.ref.bottom, sizeof(char));
      char *string_j_right = (char *)calloc(
          (strlen(get_SEQARR()[j]) - lcs_j.ref.bottom), sizeof(char));
      strncpy(string_i_right, &(get_SEQARR()[i][lcs_i.ref.bottom + 1]),
              strlen(get_SEQARR()[i]) - lcs_i.ref.bottom);
      strncpy(string_j_right, &(get_SEQARR()[j][lcs_j.ref.bottom + 1]),
              strlen(get_SEQARR()[j]) - lcs_j.ref.bottom);
      // reverse left sides
      TABLE *T2 = (TABLE *)malloc(sizeof(TABLE));
      set_S1(string_i_right);
      set_S2(string_j_right);
      set_ISLOCAL(false);
      //  For all alignments, use the following alignment parameters:
      set_MATCH(1), set_MISMATCH(-2), set_GAP(-1), set_OPEN(-5);
      allocate_table(T2) ?: (puts("fail to allocate memory\n"), exit(1));
      // do global alignment of left sides
      align(*T2);
      // find max in table for left sides, this is the left score
      int right_max_score = get_TABLEMAX(T2);
      free_table(T2);
      free(string_i_right);
      free(string_j_right);

      // 5.
      // add scores for left + right + longest common
      // similarity value for the string pair (s_i, s_j) is equal to:
      // a+b+c.
      int total_score = left_max_score + right_max_score + lcs_len;

      // 6.
      // add score to table
      // Set D[i,j] = a+b+c;
      seq_score_table[i][j] = total_score;
      printf("score[%d][%d] = %d\n", i, j, seq_score_table[i][j]);
      free_tree(t);
    }
  }

  printf("task2 complete\n");
  return 0;
}

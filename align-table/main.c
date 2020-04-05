#include "alignment.h"

/*
CptS 471/571 Assignment Cover Sheet

Assignment: Programming Project 1
Name: Cassidy, Ace

https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Typeof.html#Typeof
https://stackoverflow.com/questions/3161054/static-and-external-variables


I certify that I have listed above all the sources that I consulted regarding
this assignment, and that I have not received or given any assistance that is
contrary to the letter or the spirit of the collaboration guidelines for this
assignment.

Dated: January, 31, 2020
*/

// MAIN //
#pragma region
int main(int argc, char const *argv[]) {
  // ARG HANDLING
  argc >= 3 || argc <= 4  
      ?: (puts("Usage:<executable><fasta><0:global 1:local>[config]"), exit(1));
  // read seqs
  FILE *f1 = fopen(argv[1], "r");
  f1 ?: (puts("Fail to open fasta file"), exit(1));
  read_SEQS(f1);
  fclose(f1);
  // set local/global
  set_ISLOCAL(*argv[2] == '1' ? true : false);
  // set scores
  if (argc == 4) {
    FILE *f2 = fopen(argv[3], "r");
    f2 ?: (puts("Fail to open file"), exit(1));
    read_CNFG(f2);
    fclose(f2);
  } else
    set_MATCH(1), set_MISMATCH(-2), set_GAP(-2), set_OPEN(-5);

  // ALIGNMENT
  printf("Begin %s alignment\n", *argv[2] == '1' ? "local" : "global");
  TABLE T;
  allocate_table(&T) ?: (puts("fail to allocate memory\n"), exit(1));
  align(T);
  print_result();
  printf("Alignment Complete\n");
  return 0;
}
#pragma endregion
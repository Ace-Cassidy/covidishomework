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

#include "alignment.h"

// GLOBAL SETTERS/GETTERS //

// Sequences
char *set_S1(char *s1) { return S1_i = s1; }
char *set_S2(char *s2) { return S2_j = s2; }

// Flags
bool set_ISLOCAL(bool islocal) { return ISLOCAL = islocal; }

// Scoring
int set_OPEN(const int open) { return OPEN = open; }
int set_GAP(const int gap) { return GAP = gap; }
int set_MATCH(int match) { return MATCH = match; }
int set_MISMATCH(int mismatch) { return MISMATCH = mismatch; }

// Stats
int get_OPTSCORE(void) { return OPTSCORE; }
int get_NMATCHES(void) { return NMATCHES; }
int get_NMISMATCHES(void) { return NMISMATCHES; }
int get_NGAPS(void) { return NGAPS; }
int get_NOPENINGS(void) { return NOPENINGS; }

// FILE PARSING //

bool read_CNFG(FILE *f) {
  char line[64], *k, *v;
  while (fgets(line, 64, f)) {
    k = strtok(line, " \t\n");
    v = strtok(NULL, "\t\n");
    if (!strcmp(k, "match")) {
      set_MATCH(atoi(v));
    } else if (!strcmp(k, "mismatch")) {
      set_MISMATCH(atoi(v));
    } else if (!strcmp(k, "h")) {
      set_OPEN(atoi(v));
    } else if (!strcmp(k, "g")) {
      set_GAP(atoi(v));
    }
  }
}

// read_SEQS: parse the given file for the first two sequence found
bool read_SEQS(FILE *f) {
  char s[2000];
  fseek(f, 0L, SEEK_END);
  int sz = ftell(f);
  rewind(f);
  char *s1 = (char *)calloc(sizeof(char), sz);
  char *s2 = (char *)calloc(sizeof(char), sz);
  for (int i = 0; i < 3; i++) {
    while (fgets(s, 2000, f)) {
      if (s[0] == '>')
        break;
      if (s[strlen(s) - 1] == '\n')
        s[strlen(s) - 1] = 0;
      if (i == 1)
        strcat(s1, s);
      else if (i == 2) {
        strcat(s2, s);
      }
      if (i == 2 && strlen(s) == 0)
        break;
    }
  }
  if (strlen(s1) == 0 || strlen(s2) == 0)
    return false;
  set_S1(s1);
  set_S2(s2);
  return true;
}

// print_result: print to result.txt
void print_result() {
  printf("Scoring: match = %d, mismatch = %d, open = %d, gap = %d\n\n", MATCH,
         MISMATCH, OPEN, GAP);

  printf("S1 length = %d characters\n", (int)strlen(S1_i));
  printf("S2 length = %d characters\n\n", (int)strlen(S2_j));

  char *s1 = TRACE_S1, *s2 = TRACE_S2, *sc = TRACE_COMPARE;
  int width = 76;
  while (*s1 && *s2 && *sc) {
    printf("S1 %.*s\n", width, s1);
    printf("   %.*s\n", width, sc);
    printf("s2 %.*s\n\n", width, s2);
    s1 += width, s2 += width, sc += width;
  }

  printf("%s optimal score: %d\n", ISLOCAL ? "Local" : "Global", OPTSCORE);

  printf("matches = %d, mismatches = %d, gaps = %d, opening gaps = %d\n",
         NMATCHES, NMISMATCHES, NGAPS, NOPENINGS);
  int len = strlen(TRACE_COMPARE);
  ;
  printf("Identities = %d/%d (%.3lf), Gaps = %d/%d (%.3lf)\n", NMATCHES, len,
         ((float)NMATCHES / len), NGAPS, len, ((float)NGAPS / len));
}

// UTILITY //

// M: calculate match or  mismatch from table indices
int matchiness(int i, int j) {
  char a = S1_i[i - 1];
  char b = S2_j[j - 1];
  return a == b ? MATCH : MISMATCH;
};

// rev_str: reverse string in place
void rev_str(char *s) {
  char c, *right = s + strlen(s), *left = s;
  while (--right > left) {
    c = *left;
    *left++ = *right;
    *right = c;
  }
}

// ALIGNMENT FUNCTIONS //

// allocate_table: dynamically allocate 2D array
bool allocate_table(TABLE *T) {
  T->rows_i = strlen(S1_i) + 1, T->cols_j = strlen(S2_j) + 1;
  if (!(T->cell = (CELL **)malloc(T->rows_i * sizeof(CELL *))))
    return false;
  for (int i = 0; i < T->rows_i; i++)
    if (!(T->cell[i] = (CELL *)malloc(T->cols_j * sizeof(CELL))))
      return false;
  return true;
}

// init_global:
// S(0,0) = 0, S(i, 0) = NEG_INF, S(0, j) = NEG_INF
// D(0,0) = NEG_INF, D(0, j) = NEG_INF, D(i, 0) = h + i * g
// I(0,0) = NEG_INF, I(0, J) = h + j * g, I(i,0) = NEG_INF
bool init_global(TABLE T) {
  T.cell[0][0].S = 0;
  T.cell[0][0].D = NEG_INF;
  T.cell[0][0].I = NEG_INF;

  for (int i = 1; i < T.rows_i; i++) {
    T.cell[i][0].S = T.cell[i][0].I = NEG_INF;
    T.cell[i][0].D = OPEN + (i * GAP);
  }

  for (int j = 1; j < T.cols_j; j++) {
    T.cell[0][j].S = T.cell[0][j].D = NEG_INF;
    T.cell[0][j].I = OPEN + (j * GAP);
  }
}

// init_local:
// S(0,0) = S(i, 0) = S(0, j) = 0
// D(0,0) = D(0, j) = D(i, 0) = 0
// I(0,0) = I(0, J) = I(i, 0) = 0
bool init_local(TABLE T) {
  T.cell[0][0].S = 0;
  T.cell[0][0].D = 0;
  T.cell[0][0].I = 0;

  for (int i = 1; i < T.rows_i; i++)
    T.cell[i][0].S = T.cell[i][0].I = T.cell[i][0].D = 0;

  for (int j = 1; j < T.cols_j; j++)
    T.cell[0][j].S = T.cell[0][j].D = T.cell[0][j].I = 0;
}

// populate_global:
// S(i, j)= MAX( S(i-1, j-1),   D(i-1, j-1),       I(i-1, j-1)) + m(a, b)
// D(i, j)= MAX( D(i-1, j) + g, S(i-1, j) + h + g, I(i-1, j) + h + g )
// I(i, j)= MAX( I(i, j-1) + g, S(i, j-1) + h + g, D(i, j-1) + h + g )
bool populate_global(TABLE T) {
  CELL *curt, *zont, *diag, *vert;
  for (int i = 1; i < T.rows_i; i++) {
    for (int j = 1; j < T.cols_j; j++) {
      curt = &T.cell[i][j];

      diag = &T.cell[i - 1][j - 1];
      curt->S = MAX3(diag->S, diag->D, diag->I) + matchiness(i, j);

      vert = &T.cell[i - 1][j];
      curt->D = MAX3(vert->D + GAP, vert->S + OPEN + GAP, vert->I + OPEN + GAP);

      zont = &T.cell[i][j - 1];
      curt->I = MAX3(zont->I + GAP, zont->S + OPEN + GAP, zont->D + OPEN + GAP);
    }
  }
  // Set last cell to be the optimal for global alignment
  int o = MAX3(curt->D, curt->I, curt->S);
  OPTSCORE = o, OPTIMAL_I = T.rows_i - 1, OPTIMAL_J = T.cols_j - 1;
}

// populate_local:
// S(i, j)= MAX( S(i-1, j-1),   D(i-1, j-1),       I(i-1, j-1)) + m(a, b) or 0
// D(i, j)= MAX( D(i-1, j) + g, S(i-1, j) + h + g, I(i-1, j) + h + g) or 0
// I(i, j)= MAX( I(i, j-1) + g, S(i, j-1) + h + g, D(i, j-1) + h + g) or 0
bool populate_local(TABLE T) {
  CELL *cur, *zont, *diag, *vert;
  for (int i = 1; i < T.rows_i; i++) {
    for (int j = 1; j < T.cols_j; j++) {
      cur = &T.cell[i][j];

      diag = &T.cell[i - 1][j - 1];
      cur->S = MAX2(MAX3(diag->S, diag->D, diag->I) + matchiness(i, j), 0);

      vert = &T.cell[i - 1][j];
      cur->D =
          MAX4(vert->D + GAP, vert->S + OPEN + GAP, vert->I + OPEN + GAP, 0);

      zont = &T.cell[i][j - 1];
      cur->I =
          MAX4(zont->I + GAP, zont->S + OPEN + GAP, zont->D + OPEN + GAP, 0);

      int o = MAX4(cur->D, cur->I, cur->S, OPTSCORE);
      if (o > OPTSCORE)
        OPTSCORE = o, OPTIMAL_I = i, OPTIMAL_J = j;
    }
  }
}

// trace: retrace the optimal alignment
void *trace(TABLE T) {
  NMATCHES = 0, NMISMATCHES = 0, NGAPS = 0, NOPENINGS = 0;
  TRACE_S1 = (char *)calloc(sizeof(char), T.rows_i + T.cols_j);
  TRACE_S2 = (char *)calloc(sizeof(char), T.rows_i + T.cols_j);
  TRACE_COMPARE = (char *)calloc(sizeof(char), T.rows_i + T.cols_j);
  int i, j, pos = 0, m = 0;

  if (ISLOCAL)
    i = OPTIMAL_I, j = OPTIMAL_J;
  else
    i = T.rows_i - 1, j = T.cols_j - 1;

  CELL *cur = &T.cell[i][j], *prev;
  int score = MAX3(cur->S, cur->D, cur->I);
  while (i > 0 | j > 0) {
    if (ISLOCAL && score <= 0)
      break;
    if (cur->S == score) { // substitution / diag
      m = matchiness(i, j);
      m == MATCH ? NMATCHES++ : NMISMATCHES++;
      TRACE_S1[pos] = S1_i[i - 1];
      TRACE_S2[pos] = S2_j[j - 1];
      TRACE_COMPARE[pos] = m == MATCH ? '|' : ' ';

      i--, j--;
      prev = &T.cell[i][j];
      score = MAX3(prev->D, prev->S, prev->I);

    } else if (cur->D == score) { // deletion / vert
      TRACE_S1[pos] = S1_i[i - 1];
      TRACE_S2[pos] = '_';
      TRACE_COMPARE[pos] = ' ';
      NGAPS++;

      i--;
      prev = &T.cell[i][j];

      if (cur->D == (prev->D + GAP)) {
        score = prev->D;
      } else if (cur->D == (prev->S + OPEN + GAP)) {
        score = prev->S;
        NOPENINGS++;
      } else {
        score = prev->I;
        NOPENINGS++;
      }

    } else { // insertion / zont
      TRACE_S1[pos] = '_';
      TRACE_S2[pos] = S2_j[j - 1];
      TRACE_COMPARE[pos] = ' ';
      NGAPS++;

      j--;
      prev = &T.cell[i][j];

      if (cur->I == (prev->D + GAP + OPEN)) {
        score = prev->D;
        NOPENINGS++;
      } else if (cur->I == (prev->S + OPEN + GAP)) {
        score = prev->S;
        NOPENINGS++;
      } else {
        score = prev->I;
      }
    }
    pos++;
    cur = prev;
  }
  // null terminate
  TRACE_S1[pos] = TRACE_S2[pos] = TRACE_COMPARE[pos] = 0;
  // reverse
  rev_str(TRACE_S1), rev_str(TRACE_S2), rev_str(TRACE_COMPARE);
}

void align(TABLE T) {
  ISLOCAL ? init_local(T) : init_global(T);
  ISLOCAL ? populate_local(T) : populate_global(T);
  trace(T);
}

// DEBUG //

// debug_print: prettyish print to csv file
void debug_print(TABLE T) {
  FILE *f = fopen("table.csv", "w");
  CELL *cur;
  fprintf(f, "%c |", ' ');

  for (int j = 0; j < T.cols_j; j++)
    if (j != 0)
      fprintf(f, "%-*c|", 15, S2_j[j - 1]);
    else
      fprintf(f, "%-*c|", 15, '0');

  for (int i = 0; i < T.rows_i; i++) {
    if (i != 0)
      fprintf(f, "\n\n%c |", S1_i[i - 1]);
    else
      fprintf(f, "\n%c |", '0');

    for (int j = 0; j < T.cols_j; j++) {
      cur = &T.cell[i][j];
      cur->D == NEG_INF ? fprintf(f, "D%-4c", '!')
                        : fprintf(f, "D%-4d", cur->D);
      cur->S == NEG_INF ? fprintf(f, "S%-4c", '!')
                        : fprintf(f, "S%-4d", cur->S);
      cur->I == NEG_INF ? fprintf(f, "I%-4c|", '!')
                        : fprintf(f, "I%-4d|", cur->I);
    }
  }
  fclose(f);
}
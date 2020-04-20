/*
CptS 471/571 Assignment Cover Sheet

Assignment: Programming Project 3
Authors: Cassidy, Ace
Authors: Stoddard, Miranda

I certify that I have listed above all the sources that I consulted regarding
this assignment, and that I have not received or given any assistance that is
contrary to the letter or the spirit of the collaboration guidelines for this
assignment.

Dated: April, 5, 2020
*/

#pragma once

// INCUDES //
#pragma region
#include "../utility/utility.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#pragma endregion

// MACROS //
#pragma region
// typeof safety
#define MAX2(a, b)                                                             \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })
#define MAX3(a, b, c) MAX2(MAX2(a, b), c)
#define MAX4(a, b, c, d) MAX2(MAX3(a, b, c), d);
// add a bit to prevent underflow
#define NEG_INF INT_MIN + 1000
#pragma endregion

// TYPES //
#pragma region
typedef struct CELL {
  int D, S, I;
} CELL;

typedef struct TABLE {
  CELL **cell;
  int rows_i, cols_j;
} TABLE;
#pragma endregion

// GLOBALS //
#pragma region
// make static so not externally linked

// flags
static bool ISLOCAL = false;
// scoring
static int GAP = 0, OPEN = 0, MATCH = 0, MISMATCH = 0;
// alignment stats
static int OPTSCORE = NEG_INF, OPTIMAL_J = NEG_INF, OPTIMAL_I = NEG_INF,
           NMATCHES, NMISMATCHES, NGAPS, NOPENINGS, TABLEMAX = INT_MIN;
// sequences
static char *S1_i = NULL, *S2_j = NULL, *TRACE_S1 = NULL, *TRACE_S2 = NULL,
            *TRACE_COMPARE = NULL;
#pragma endregion

// FUNCTIONS //
#pragma region
void print_result();

void align(TABLE T);

bool allocate_table(TABLE *T);
bool free_table(TABLE *T);
bool read_SEQS(FILE *f);
bool read_CNFG(FILE *f);

int get_NOPENINGS(void);
int get_NGAPS(void);
int get_NMISMATCHES(void);
int get_NMATCHES(void);
int get_OPTSCORE(void);
int get_TABLEMAX(TABLE *T);

int set_MISMATCH(int mismatch);
int set_MATCH(int match);
int set_GAP(const int gap);
int set_OPEN(const int open);
bool set_ISLOCAL(bool local);

char *set_S2(char *s2);
char *set_S1(char *s1);
#pragma endregion
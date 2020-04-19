#include "utility.h"

// rev_str: reverse string in place
void rev_str(char *s) {
  char c, *right = s + strlen(s), *left = s;
  while (--right > left) {
    c = *left;
    *left++ = *right;
    *right = c;
  }
}

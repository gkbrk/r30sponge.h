#include "r30sponge.h"
#include <time.h>
#include <stdio.h>

int main() {
  void *sp = r30s_new_default();
  size_t n;

  time_t t = time(NULL);
  r30s_absorb(sp, &t, sizeof(t));

  r30s_squeeze_buf(sp, &n, sizeof(n));
  printf("Random number: %zu\n", n);
  
  return 0;
}

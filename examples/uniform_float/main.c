#include <stdio.h>
#include <math.h>
#include "r30sponge.h"

double random_uniform(void *sponge) {
  unsigned long n;
  r30s_squeeze_buf(sponge, &n, sizeof(n));
  return ((double)n / (double)ULONG_MAX) * 2.0 - 1.0;
}

int main() {
  void *sp = r30s_new_default();
  size_t i;

  r30s_absorb_cstr(sp, "This should be seeded randomly");

  for (i = 0; i < 10; i++) {
    printf("Random float between -1 and 1: %f\n", random_uniform(sp));
  }
  
  return 0;
}

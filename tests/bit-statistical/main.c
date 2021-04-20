#include "r30sponge.h"
#include <stdio.h>
#include <time.h>

int main(void) {
  void *sp = r30s_new(64, 20);
  size_t even = 0;
  size_t odd = 0;
  size_t i;

  time_t t = time(0);
  r30s_absorb_cstr(sp, "Epic seed time");
  r30s_absorb(sp, &t, sizeof(t));

  for (i = 0; i < 5000; i++) {
    if (r30s_squeeze_bit(sp) == 0) {
      even++;
    } else {
      odd++;
    }
  }
  free(sp);

  printf("Even: %ld Odd: %ld\n", even, odd);
  printf("Ratio: %f\n", (double)even / (double)odd);

  if (1 - (double)even / (double)odd > 0.05) {
    return 1;
  }
  
  return 0;
}

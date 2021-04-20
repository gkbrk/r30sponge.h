#include "r30sponge.h"
#include <stdio.h>

int main(void) {
  size_t i;
  void *sp = r30s_new_default();

  r30s_absorb_cstr(sp, "Let's hash some data");
  r30s_absorb_cstr(sp, "You can read the data in chunks of arbitrary length.");

  for (i = 0; i < 8; i++) {
    printf("%x", r30s_squeeze_byte(sp));
  }
  printf("\n");
  
  free(sp);
  return 0;
}

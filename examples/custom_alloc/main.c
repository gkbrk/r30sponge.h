#define R30S_NOMALLOC
#include "r30sponge.h"

#include <stdio.h>

int main() {
  char buf[2048];
  void *sponge = buf;
  size_t i;
  
  r30s_init_buf(512, 20, buf);
  r30s_absorb_cstr(sponge, "Test message");

  for (i = 0; i < 8; i++)
    printf("%x", r30s_squeeze_byte(sponge));
  printf("\n");
  
  return 0;
}

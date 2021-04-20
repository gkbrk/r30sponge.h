#include <cstdio>
#include <cstdlib>
#include <cstdint>

extern "C" {
  void *r30s_new_default();
  void r30s_absorb_cstr(void*, const char*);
  uint8_t r30s_squeeze_byte(void*);
}

int main() {
  auto sp = r30s_new_default();
  r30s_absorb_cstr(sp, "Test message");
  printf("%d\n", r30s_squeeze_byte(sp));
  free(sp);
  
  return 0;
}

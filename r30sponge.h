/* r30sponge.h - Sponge function based on the Rule 30 cellular automaton */

/* Copyright (c) 2021, Gokberk Yaltirakli <opensource@gkbrk.com> */
/* See the bottom of the file for the license. */

/* This is a header library that tries to provide some basic cryptographic
functionality with very minimal code that can be understood and ported to other
languages quickly. The library is written in C89 / ANSI C style in order to be
compatible with more environments. */

/* DESCRIPTION

This library implements a sponge function using the Rule 30 elementary cellular
automaton.

A sponge function is a cryptographic function that can take an input of any
length and produce an output of any desired length. You can check
https://en.wikipedia.org/wiki/Sponge_function for the wikipedia article or
https://www.gkbrk.com/2021/03/md5-sponge/ for my blog post on some things you
can do with sponge functions.

Rule 30 is an elementary cellular automaton that produces random and chaotic
patterns from simple inputs. You can read more about Rule 30 at
https://en.wikipedia.org/wiki/Rule_30.

*/

/* COMMON USE CASES

Common use cases of sponge functions include:

- Random number generators
- Hash functions
- Message authentication codes
- Stream ciphers
- Block ciphers

*/

#ifndef R30S_H_
#define R30S_H_

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifdef R30S_NONSTATIC
#define R30S_STATIC
#else
#define R30S_STATIC static
#endif

#ifndef R30S_NOMALLOC
#include <stdlib.h>
#endif

/* This is the actual sponge struct. The library functions work with opaque
 * pointers in order to discourage exposing the implementation details. */
typedef struct {
  size_t size;
  size_t steps;
  uint8_t *state;
  uint8_t *other;
} r30s_sponge;

R30S_STATIC size_t r30s_sizeof(size_t bits) {
  return sizeof(r30s_sponge) + bits + bits;
}

R30S_STATIC void r30s_init_buf(size_t bits, size_t steps, void *ptr) {
  r30s_sponge *sp = ptr;
  uint8_t *buf = ptr;
  size_t i;

  sp->size = bits;
  sp->steps = steps;

  sp->state = buf + sizeof(r30s_sponge);
  sp->other = buf + sizeof(r30s_sponge) + bits;

  /* Zero out the state buffer. There is no need to clean the "other" buffer
     since we don't use the old data from that buffer. */
  for (i = 0; i < bits; i++) {
    sp->state[i] = 0;
  }

  /* Set the middle bit to 1. This ensures that even without absorbing any bits,
     we create some random-looking output. */
  sp->state[bits / 2] = 1;
}

#ifndef R30S_NOMALLOC

/**
 * Constructs a new Rule 30 Sponge.
 *
 * In general, the performance of the function depends on the number of bits and
 * the number of steps.
 *
 * @param bits The size of the internal state in bits
 * @param steps The number of Rule 30 steps to take for each bit
 * @return Opaque pointer to the sponge
 */
R30S_STATIC void *r30s_new(size_t bits, size_t steps) {
  void *ptr = malloc(r30s_sizeof(bits));
  r30s_init_buf(bits, steps, ptr);
  return ptr;
}

/**
 * Construct a Rule-30 sponge with default parameters.
 *
 * Keep in mind that the default parameters can change between different
 * versions of this library. If you will be interacting with another Rule 30
 * sponge, it is recommended to use `r30s_new` with fixed values.
 *
 * @return Opaque pointer to the sponge
 * @remark The default parameters might change between versions
 */
R30S_STATIC void *r30s_new_default() { return r30s_new(512, 20); }

#endif

/**
 * Performs a single iteration of Rule 30.
 *
 * The internal state is a fixed-size bit array. The left and right edges of the
 * array wrap around when calculating the next state.
 *
 * @param ptr Pointer to the sponge
 * @remark This function modifies the sponge in-place
 */
R30S_STATIC void r30s_step(void *ptr) {
  uint8_t *temp;
  size_t i;
  r30s_sponge *sp = ptr;

  for (i = 0; i < sp->size; i++) {
    if (i == 0) {
      sp->other[i] = sp->state[sp->size - 1] ^ (sp->state[0] | sp->state[1]);
    } else if (i == sp->size - 1) {
      sp->other[i] =
          sp->state[sp->size - 2] ^ (sp->state[sp->size - 1] | sp->state[0]);
    } else {
      sp->other[i] = sp->state[i - 1] ^ (sp->state[i + 1] | sp->state[i]);
    }
  }

  temp = sp->state;
  sp->state = sp->other;
  sp->other = temp;
}

R30S_STATIC void r30s_steps(void *ptr) {
  size_t i;
  r30s_sponge *sp = ptr;
  for (i = 0; i < sp->steps; i++) {
    r30s_step(ptr);
  }
}

/**
 * Absorb a single bit into the sponge.
 * @param ptr Sponge pointer
 * @param bit The bit to absorb. Valid values are 0 and 1.
 */
R30S_STATIC void r30s_absorb_bit(void *ptr, uint8_t bit) {
  r30s_sponge *sp = ptr;
  sp->state[sp->size / 2 + sp->steps] = bit;
  r30s_steps(ptr);
}

/**
 * Absorb a byte into the sponge.
 * @param ptr Sponge pointer
 * @param byte Byte to absorb.
 */
R30S_STATIC void r30s_absorb_byte(void *ptr, uint8_t byte) {
  size_t i;
  for (i = 0; i < CHAR_BIT; i++) {
    r30s_absorb_bit(ptr, (byte >> i) & 1);
  }
}

/**
 * Absorb a buffer into the sponge.
 * @param ptr Sponge pointer
 * @param buf Buffer pointer
 * @param size Number of bytes to absorb
 */
R30S_STATIC void r30s_absorb(void *ptr, const void *buf, size_t size) {
  size_t i;
  const uint8_t *ch = buf;
  for (i = 0; i < size; i++) {
    r30s_absorb_byte(ptr, ch[i]);
  }
}

/** Absorbs a null-terminated string into the sponge.
 * @param ptr Pointer to a sponge
 * @param str Null-terminated string
 */
R30S_STATIC void r30s_absorb_cstr(void *ptr, const char *str) {
  const char *c;
  for (c = str; *c != '\0'; c++) {
    r30s_absorb_byte(ptr, *c);
  }
}

/**
 * Squeeze a single bit from the sponge.
 * @param ptr Sponge pointer
 * @return A value of 0 or 1
 */
R30S_STATIC uint8_t r30s_squeeze_bit(void *ptr) {
  r30s_sponge *sp = ptr;
  uint8_t b = sp->state[sp->size / 2];
  r30s_steps(ptr);
  return b;
}

/**
 * Squeeze a byte from the sponge.
 * @param ptr Sponge pointer
 * @return The byte
 */
R30S_STATIC uint8_t r30s_squeeze_byte(void *ptr) {
  uint8_t b = 0;
  size_t i;
  for (i = 0; i < CHAR_BIT; i++) {
    if (r30s_squeeze_bit(ptr) == 1) {
      b |= 1 << i;
    }
  }
  return b;
}

/**
 * Fill a buffer with the output of the sponge.
 *
 * @param prt Sponge pointer
 * @param buf Buffer pointer
 * @param n The number of bytes to write
 */
R30S_STATIC void r30s_squeeze_buf(void *ptr, uint8_t *buf, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    buf[i] = r30s_squeeze_byte(ptr);
  }
}

#endif

/*
Copyright (c) 2021, Gokberk Yaltirakli

--------------------------------------------------------------------------------
This software is available under two licenses. Choose whichever you prefer.
--------------------------------------------------------------------------------
LICENSE 1 - ISC License

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright notice
and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
--------------------------------------------------------------------------------
LICENSE 2 - Public Domain

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

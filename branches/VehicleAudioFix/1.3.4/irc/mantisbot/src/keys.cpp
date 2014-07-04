/*
 * Copyright (c) 2007, Alberto Alonso Pinto
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions
 *       and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *       and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Mantis Bot nor the names of its contributors may be used to endorse or
 *       promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <string.h>
#include "keys.h"

static const char base64int_table[] = {
  '$', '.', 'A', 'B', 'C', 'D', 'E', 'F',
  'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
  'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
  'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
  'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
  '2', '3', '4', '5', '6', '7', '8', '9'
};

static const uint32_t intbase64_table[] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 000-009 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 010-019 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 020-029 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 030-039 */
   0,  0,  0,  0,  0,  0,  1,  0, 54, 55, /* 040-049 */
  56, 57, 58, 59, 60, 61, 62, 63,  0,  0, /* 050-059 */
   0,  0,  0,  0,  0,  2,  3,  4,  5,  6, /* 060-069 */
   7,  8,  9, 10, 11, 12, 13, 14, 15, 16, /* 070-079 */
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, /* 080-089 */
  27,  0,  0,  0,  0,  0,  0, 28, 29, 30, /* 090-099 */
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 100-109 */
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, /* 110-119 */
  51, 52, 53,  0,  0,  0,  0,  0,  0,  0, /* 120-129 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 130-139 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 140-149 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 150-159 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 160-169 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 170-179 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 180-189 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 190-199 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 200-209 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 210-219 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 220-229 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 230-239 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 240-249 */
   0,  0,  0,  0,  0,  0                  /* 250-255 */
};

static void base64encode(char* dest, uint32_t v)
{
  int i = 6;

  while (i > 0)
  {
    dest[--i] = base64int_table[(v & 63)];
    v >>= 6;
  }
}

static uint32_t base64decode(const char* s)
{
  uint32_t n = intbase64_table[(unsigned char)*s];
  s++;
  for (int i = 1; i < 6; i++)
  {
    n <<= 6;
    n += intbase64_table[(unsigned char)*s];
    s++;
  }
  return n;
}

void encodeXtea(uint32_t* v, uint32_t* w, uint32_t* k) {
    register uint32_t v0=v[0], v1=v[1], i, sum=0;
    register uint32_t delta=0x9E3779B9;
    for(i=0; i<32; i++) {
       v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
    }
    w[0]=v0; w[1]=v1;
}
 
void decodeXtea(uint32_t* v, uint32_t* w, uint32_t* k) {
    register uint32_t v0=v[0], v1=v[1], i, sum=0xC6EF3720;
    register uint32_t delta=0x9E3779B9;
    for(i=0; i<32; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
    }
    w[0]=v0; w[1]=v1;
}

char* keysEncode(const char* password, char* dest)
{
  const char* p = password;
  char* r = dest;
  uint32_t k[4];
  uint32_t v[2];
  uint32_t w[2];
  int n;

  /* Prepare tea */
  k[0] = MANTISBOT_KEY1;
  k[1] = MANTISBOT_KEY2;
  k[2] = MANTISBOT_KEY3;
  k[3] = MANTISBOT_KEY4;
  w[0] = 0;
  w[1] = 0;
  n = strlen(p) / sizeof(uint32_t) + !!(strlen(p) % sizeof(uint32_t));

  /* Encode! */
  for (int i = 0; i < n; i++)
  {
    v[0] = (*(uint32_t *)p) ^ (w[0] + w[1]);
    v[1] = w[0] ^ w[1];
    p += sizeof(uint32_t);
    encodeXtea(v, w, k);
    base64encode(r, w[0]);
    base64encode(r + 6, w[1]);
    r += 12;
  }

  *r = '\0';
  return dest;
}

char* keysDecode(const char* encodedPassword, char* dest)
{
  char* r;
  const char* p = encodedPassword;
  size_t len;
  uint32_t v[2];
  uint32_t w[2];
  uint32_t k[4];
  uint32_t save = 0;

  /* Prepare tea */
  k[0] = MANTISBOT_KEY1;
  k[1] = MANTISBOT_KEY2;
  k[2] = MANTISBOT_KEY3;
  k[3] = MANTISBOT_KEY4;
  w[0] = 0;
  w[1] = 0;
  r = dest;
  len = strlen(p);

  /* Decode! */
  for (int i = 0; i < (len / 12); i++)
  {
    v[0] = base64decode(p);
    v[1] = base64decode(p + 6);
    p += 12;
    decodeXtea(v, w, k);
    w[0] = w[0] ^ save;
    memcpy(r, &w[0], sizeof(uint32_t));
    save = v[0] + v[1];
    r += sizeof(uint32_t);
  }

  *r = '\0';
  return dest;
}


/*
 * Copyright (C) 2001 Edmund Grimley Evans <edmundo@rano.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Modified to make consistent unsigned to prevent warnings.
int utf8_mbtowc(unsigned int *pwc, const char *s, int n)
{
  unsigned char c;
  int wc, i, k;

  if (!n || !s)
    return 0;

  c = *s;
  if (c < 0x80) {
    if (pwc)
      *pwc = c;
    return c ? 1 : 0;
  }
  else if (c < 0xc2)
    return -1;
  else if (c < 0xe0) {
    if (n >= 2 && (s[1] & 0xc0) == 0x80) {
      if (pwc)
        *pwc = ((c & 0x1f) << 6) | (s[1] & 0x3f);
      return 2;
    }
    else
      return -1;
  }
  else if (c < 0xf0)
    k = 3;
  else if (c < 0xf8)
    k = 4;
  else if (c < 0xfc)
    k = 5;
  else if (c < 0xfe)
    k = 6;
  else
    return -1;

  if (n < k)
    return -1;
  wc = *s++ & ((1 << (7 - k)) - 1);
  for (i = 1; i < k; i++) {
    if ((*s & 0xc0) != 0x80)
      return -1;
    wc = (wc << 6) | (*s++ & 0x3f);
  }
  if (wc < (1 << (5 * k - 4)))
    return -1;
  if (pwc)
    *pwc = wc;
  return k;
}

// Modified to accept s as NULL
int utf8_wctomb(char *s, int wc1)
{
  unsigned int wc = wc1;

  if (wc < (1 << 7)) {
    if (s)
      *s++ = wc;
    return 1;
  }
  else if (wc < (1 << 11)) {
    if (s)
    {
      *s++ = 0xc0 | (wc >> 6);
      *s++ = 0x80 | (wc & 0x3f);
    }
    return 2;
  }
  else if (wc < (1 << 16)) {
    if (s)
    {
      *s++ = 0xe0 | (wc >> 12);
      *s++ = 0x80 | ((wc >> 6) & 0x3f);
      *s++ = 0x80 | (wc & 0x3f);
    }
    return 3;
  }
  else if (wc < (1 << 21)) {
    if (s)
    {
      *s++ = 0xf0 | (wc >> 18);
      *s++ = 0x80 | ((wc >> 12) & 0x3f);
      *s++ = 0x80 | ((wc >> 6) & 0x3f);
      *s++ = 0x80 | (wc & 0x3f);
    }
    return 4;
  }
  else if (wc < (1 << 26)) {
    if (s)
    {
      *s++ = 0xf8 | (wc >> 24);
      *s++ = 0x80 | ((wc >> 18) & 0x3f);
      *s++ = 0x80 | ((wc >> 12) & 0x3f);
      *s++ = 0x80 | ((wc >> 6) & 0x3f);
      *s++ = 0x80 | (wc & 0x3f);
    }
    return 5;
  }
  else if (wc < (1 << 31)) {
    if (s)
    {
      *s++ = 0xfc | (wc >> 30);
      *s++ = 0x80 | ((wc >> 24) & 0x3f);
      *s++ = 0x80 | ((wc >> 18) & 0x3f);
      *s++ = 0x80 | ((wc >> 12) & 0x3f);
      *s++ = 0x80 | ((wc >> 6) & 0x3f);
      *s++ = 0x80 | (wc & 0x3f);
    }
    return 6;
  }
  else
    return -1;
}

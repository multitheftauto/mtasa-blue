// Copyright (c) 2005, Google Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// Author: Craig Silverstein
//
// This tests <google/sparsetable>
//
// Since sparsetable is templatized, it's important that we test every
// function in every class in this file -- not just to see if it
// works, but even if it compiles.

#include "config.h"
#include <string>
#include <stdio.h>
#include <string.h>         // for memcmp()
#include <stdlib.h>         // defines unlink() on some windows platforms(?)
#ifdef HAVE_UNISTD_H
#include <unistd.h>         // for unlink()
#include <sys/types.h>      // for size_t
#include <string>
#endif
#include <google/sparsetable>

using STL_NAMESPACE::string;
using GOOGLE_NAMESPACE::sparsetable;

// Many sparsetable operations return a size_t.  Rather than have to
// use PRIuS everywhere, we'll just cast to a "big enough" value.
#define UL(x)    ( static_cast<unsigned long>(x) )


static char outbuf[10240];       // big enough for these tests
static char* out = outbuf;       // where to write next
#define LEFT (outbuf + sizeof(outbuf) - out)

#define TEST(cond)  out += snprintf(out, LEFT, #cond "? %s\n", \
                                    (cond) ? "yes" : "no");

inline string AsString(int n) {
  const int N = 64;
  char buf[N];
  snprintf(buf, N, "%d", n);
  return string(buf);
}

// Test sparsetable with a POD type, int.
void TestInt() {
  out += snprintf(out, LEFT, "int test\n");
  sparsetable<int> x(7), y(70), z;
  x.set(4, 10);
  y.set(12, -12);
  y.set(47, -47);
  y.set(48, -48);
  y.set(49, -49);

  const sparsetable<int> constx(x);
  const sparsetable<int> consty(y);

  // ----------------------------------------------------------------------
  // Test the plain iterators

  for ( sparsetable<int>::iterator it = x.begin(); it != x.end(); ++it ) {
    out += snprintf(out, LEFT, "x[%lu]: %d\n", UL(it - x.begin()), int(*it));
  }
  for ( sparsetable<int>::const_iterator it = x.begin(); it != x.end(); ++it ) {
    out += snprintf(out, LEFT, "x[%lu]: %d\n", UL(it - x.begin()), *it);
  }
  for ( sparsetable<int>::reverse_iterator it = x.rbegin(); it != x.rend(); ++it ) {
    out += snprintf(out, LEFT, "x[%lu]: %d\n", UL(x.rend()-1 - it), int(*it));
  }
  for ( sparsetable<int>::const_reverse_iterator it = constx.rbegin(); it != constx.rend(); ++it ) {
    out += snprintf(out, LEFT, "x[%lu]: %d\n", UL(constx.rend()-1 - it), *it);
  }
  for ( sparsetable<int>::iterator it = z.begin(); it != z.end(); ++it ) {
    out += snprintf(out, LEFT, "z[%lu]: %d\n", UL(it - z.begin()), int(*it));
  }

  {                                          // array version
    out += snprintf(out, LEFT, "x[3]: %d\n", int(x[3]));
    out += snprintf(out, LEFT, "x[4]: %d\n", int(x[4]));
    out += snprintf(out, LEFT, "x[5]: %d\n", int(x[5]));
  }
  {
    sparsetable<int>::iterator it;           // non-const version
    out += snprintf(out, LEFT, "x[4]: %d\n", int(x.begin()[4]));
    it = x.begin() + 4;          // should point to the non-zero value
    out += snprintf(out, LEFT, "x[4]: %d\n", int(*it));
    it--;
    --it;
    it += 5;
    it -= 2;
    it++;
    ++it;
    it = it - 3;
    it = 1 + it;                 // now at 5
    out += snprintf(out, LEFT, "x[3]: %d\n", int(it[-2]));
    out += snprintf(out, LEFT, "x[4]: %d\n", int(it[-1]));
    *it = 55;
    out += snprintf(out, LEFT, "x[5]: %d\n", int(it[0]));
    out += snprintf(out, LEFT, "x[5]: %d\n", int(*it));
    int *x6 = &(it[1]);
    *x6 = 66;
    out += snprintf(out, LEFT, "x[6]: %d\n", int(*(it + 1)));
    // Let's test comparitors as well
    TEST(it == it);
    TEST(!(it != it));
    TEST(!(it < it));
    TEST(!(it > it));
    TEST(it <= it);
    TEST(it >= it);

    sparsetable<int>::iterator it_minus_1 = it - 1;
    TEST(!(it == it_minus_1));
    TEST(it != it_minus_1);
    TEST(!(it < it_minus_1));
    TEST(it > it_minus_1);
    TEST(!(it <= it_minus_1));
    TEST(it >= it_minus_1);
    TEST(!(it_minus_1 == it));
    TEST(it_minus_1 != it);
    TEST(it_minus_1 < it);
    TEST(!(it_minus_1 > it));
    TEST(it_minus_1 <= it);
    TEST(!(it_minus_1 >= it));

    sparsetable<int>::iterator it_plus_1 = it + 1;
    TEST(!(it == it_plus_1));
    TEST(it != it_plus_1);
    TEST(it < it_plus_1);
    TEST(!(it > it_plus_1));
    TEST(it <= it_plus_1);
    TEST(!(it >= it_plus_1));
    TEST(!(it_plus_1 == it));
    TEST(it_plus_1 != it);
    TEST(!(it_plus_1 < it));
    TEST(it_plus_1 > it);
    TEST(!(it_plus_1 <= it));
    TEST(it_plus_1 >= it);
  }
  {
    sparsetable<int>::const_iterator it;    // const version
    out += snprintf(out, LEFT, "x[4]: %d\n", int(x.begin()[4]));
    it = x.begin() + 4;          // should point to the non-zero value
    out += snprintf(out, LEFT, "x[4]: %d\n", *it);
    it--;
    --it;
    it += 5;
    it -= 2;
    it++;
    ++it;
    it = it - 3;
    it = 1 + it;                 // now at 5
    out += snprintf(out, LEFT, "x[3]: %d\n", it[-2]);
    out += snprintf(out, LEFT, "x[4]: %d\n", it[-1]);
    out += snprintf(out, LEFT, "x[5]: %d\n", *it);
    out += snprintf(out, LEFT, "x[6]: %d\n", *(it + 1));
    // Let's test comparitors as well
    TEST(it == it);
    TEST(!(it != it));
    TEST(!(it < it));
    TEST(!(it > it));
    TEST(it <= it);
    TEST(it >= it);

    sparsetable<int>::const_iterator it_minus_1 = it - 1;
    TEST(!(it == it_minus_1));
    TEST(it != it_minus_1);
    TEST(!(it < it_minus_1));
    TEST(it > it_minus_1);
    TEST(!(it <= it_minus_1));
    TEST(it >= it_minus_1);
    TEST(!(it_minus_1 == it));
    TEST(it_minus_1 != it);
    TEST(it_minus_1 < it);
    TEST(!(it_minus_1 > it));
    TEST(it_minus_1 <= it);
    TEST(!(it_minus_1 >= it));

    sparsetable<int>::const_iterator it_plus_1 = it + 1;
    TEST(!(it == it_plus_1));
    TEST(it != it_plus_1);
    TEST(it < it_plus_1);
    TEST(!(it > it_plus_1));
    TEST(it <= it_plus_1);
    TEST(!(it >= it_plus_1));
    TEST(!(it_plus_1 == it));
    TEST(it_plus_1 != it);
    TEST(!(it_plus_1 < it));
    TEST(it_plus_1 > it);
    TEST(!(it_plus_1 <= it));
    TEST(it_plus_1 >= it);
  }

  TEST(x.begin() == x.begin() + 1 - 1);
  TEST(x.begin() < x.end());
  TEST(z.begin() < z.end());
  TEST(z.begin() <= z.end());
  TEST(z.begin() == z.end());


  // ----------------------------------------------------------------------
  // Test the non-empty iterators

  for ( sparsetable<int>::nonempty_iterator it = x.nonempty_begin(); it != x.nonempty_end(); ++it ) {
    out += snprintf(out, LEFT, "x[??]: %d\n", *it);
  }
  for ( sparsetable<int>::const_nonempty_iterator it = y.nonempty_begin(); it != y.nonempty_end(); ++it ) {
    out += snprintf(out, LEFT, "y[??]: %d\n", *it);
  }
  for ( sparsetable<int>::reverse_nonempty_iterator it = y.nonempty_rbegin(); it != y.nonempty_rend(); ++it ) {
    out += snprintf(out, LEFT, "y[??]: %d\n", *it);
  }
  for ( sparsetable<int>::const_reverse_nonempty_iterator it = consty.nonempty_rbegin(); it != consty.nonempty_rend(); ++it ) {
    out += snprintf(out, LEFT, "y[??]: %d\n", *it);
  }
  for ( sparsetable<int>::nonempty_iterator it = z.nonempty_begin(); it != z.nonempty_end(); ++it ) {
    out += snprintf(out, LEFT, "z[??]: %d\n", *it);
  }

  {
    sparsetable<int>::nonempty_iterator it;           // non-const version
    out += snprintf(out, LEFT, "first non-empty y: %d\n", *y.nonempty_begin());
    out += snprintf(out, LEFT, "first non-empty x: %d\n", *x.nonempty_begin());
    it = x.nonempty_begin();
    ++it;                        // should be at end
    --it;
    out += snprintf(out, LEFT, "first non-empty x: %d\n", *it++);
    it--;
    out += snprintf(out, LEFT, "first non-empty x: %d\n", *it++);
  }
  {
    sparsetable<int>::const_nonempty_iterator it;           // non-const version
    out += snprintf(out, LEFT, "first non-empty y: %d\n", *y.nonempty_begin());
    out += snprintf(out, LEFT, "first non-empty x: %d\n", *x.nonempty_begin());
    it = x.nonempty_begin();
    ++it;                        // should be at end
    --it;
    out += snprintf(out, LEFT, "first non-empty x: %d\n", *it++);
    it--;
    out += snprintf(out, LEFT, "first non-empty x: %d\n", *it++);
  }

  TEST(x.begin() == x.begin() + 1 - 1);
  TEST(z.begin() != z.end());

  // ----------------------------------------------------------------------
  // Test sparsetable functions
  out += snprintf(out, LEFT, "x has %lu/%lu buckets, "
                  "y %lu/%lu, z %lu/%lu\n",
                  UL(x.num_nonempty()), UL(x.size()),
                  UL(y.num_nonempty()), UL(y.size()),
                  UL(z.num_nonempty()), UL(z.size()));
  
  y.resize(48);              // should get rid of 48 and 49
  y.resize(70);              // 48 and 49 should still be gone
  out += snprintf(out, LEFT, "y shrank and grew: it's now %lu/%lu\n",
                  UL(y.num_nonempty()), UL(y.size()));
  out += snprintf(out, LEFT, "y[12] = %d, y.get(12) = %d\n", int(y[12]), y.get(12));
  y.erase(12);
  out += snprintf(out, LEFT, "y[12] cleared.  y now %lu/%lu.  "
                  "y[12] = %d, y.get(12) = %d\n",
                  UL(y.num_nonempty()), UL(y.size()), int(y[12]), y.get(12));

  swap(x, y);

  y.clear();
  TEST(y == z);

  y.resize(70);
  for ( int i = 10; i < 40; ++i )
    y[i] = -i;
  y.erase(y.begin() + 15, y.begin() + 30);
  y.erase(y.begin() + 34);
  y.erase(12);
  y.resize(38);
  y.resize(10000);
  y[9898] = -9898;
  for ( sparsetable<int>::const_iterator it = y.begin(); it != y.end(); ++it ) {
    if ( y.test(it) )
      out += snprintf(out, LEFT, "y[%lu] is set\n", UL(it - y.begin()));
  }
  out += snprintf(out, LEFT, "That's %lu set buckets\n", UL(y.num_nonempty()));

  out += snprintf(out, LEFT, "Starting from y[32]...\n");
  for ( sparsetable<int>::const_nonempty_iterator it = y.get_iter(32);
        it != y.nonempty_end(); ++it )
    out += snprintf(out, LEFT, "y[??] = %d\n", *it);

  out += snprintf(out, LEFT, "From y[32] down...\n");
  for ( sparsetable<int>::nonempty_iterator it = y.get_iter(32);
        it != y.nonempty_begin(); )
    out += snprintf(out, LEFT, "y[??] = %d\n", *--it);

  // ----------------------------------------------------------------------
  // Test I/O
  string filestr = "/tmp/#sparsetable.test";
  const char *file = filestr.c_str();
  FILE *fp = fopen(file, "wb");
  if ( fp == NULL ) {
    // maybe we can't write to /tmp/.  Try the current directory
    file = "#sparsetable.test";
    fp = fopen(file, "wb");
  }
  if ( fp == NULL ) {
    out += snprintf(out, LEFT, "Can't open %s, skipping disk write...\n", file);
  } else {
    y.write_metadata(fp);          // only write meta-information
    y.write_nopointer_data(fp);
    fclose(fp);
  }
  fp = fopen(file, "rb");
  if ( fp == NULL ) {
    out += snprintf(out, LEFT, "Can't open %s, skipping disk read...\n", file);
  } else {
    sparsetable<int> y2;
    y2.read_metadata(fp);
    y2.read_nopointer_data(fp);
    fclose(fp);

    for ( sparsetable<int>::const_iterator it = y2.begin(); it != y2.end(); ++it ) {
      if ( y2.test(it) )
        out += snprintf(out, LEFT, "y2[%lu] is %d\n", UL(it - y2.begin()), *it);
    }
    out += snprintf(out, LEFT, "That's %lu set buckets\n", UL(y2.num_nonempty()));
  }
  unlink(file);
}

// Test sparsetable with a non-POD type, std::string
void TestString() {
  out += snprintf(out, LEFT, "string test\n");
  sparsetable<string> x(7), y(70), z;
  x.set(4, "foo");
  y.set(12, "orange");
  y.set(47, "grape");
  y.set(48, "pear");
  y.set(49, "apple");

  // ----------------------------------------------------------------------
  // Test the plain iterators

  for ( sparsetable<string>::iterator it = x.begin(); it != x.end(); ++it ) {
    out += snprintf(out, LEFT, "x[%lu]: %s\n",
                    UL(it - x.begin()), static_cast<string>(*it).c_str());
  }
  for ( sparsetable<string>::iterator it = z.begin(); it != z.end(); ++it ) {
    out += snprintf(out, LEFT, "z[%lu]: %s\n",
                    UL(it - z.begin()), static_cast<string>(*it).c_str());
  }

  TEST(x.begin() == x.begin() + 1 - 1);
  TEST(x.begin() < x.end());
  TEST(z.begin() < z.end());
  TEST(z.begin() <= z.end());
  TEST(z.begin() == z.end());

  // ----------------------------------------------------------------------
  // Test the non-empty iterators
    for ( sparsetable<string>::nonempty_iterator it = x.nonempty_begin(); it != x.nonempty_end(); ++it ) {
    out += snprintf(out, LEFT, "x[??]: %s\n", it->c_str());
  }
  for ( sparsetable<string>::const_nonempty_iterator it = y.nonempty_begin(); it != y.nonempty_end(); ++it ) {
    out += snprintf(out, LEFT, "y[??]: %s\n", it->c_str());
  }
  for ( sparsetable<string>::nonempty_iterator it = z.nonempty_begin(); it != z.nonempty_end(); ++it ) {
    out += snprintf(out, LEFT, "z[??]: %s\n", it->c_str());
  }

  // ----------------------------------------------------------------------
  // Test sparsetable functions
  out += snprintf(out, LEFT, "x has %lu/%lu buckets, y %lu/%lu, z %lu/%lu\n",
                  UL(x.num_nonempty()), UL(x.size()),
                  UL(y.num_nonempty()), UL(y.size()),
                  UL(z.num_nonempty()), UL(z.size()));

  y.resize(48);              // should get rid of 48 and 49
  y.resize(70);              // 48 and 49 should still be gone
  out += snprintf(out, LEFT, "y shrank and grew: it's now %lu/%lu\n",
                  UL(y.num_nonempty()), UL(y.size()));
  out += snprintf(out, LEFT, "y[12] = %s, y.get(12) = %s\n",
                  static_cast<string>(y[12]).c_str(), y.get(12).c_str());
  y.erase(12);
  out += snprintf(out, LEFT, "y[12] cleared.  y now %lu/%lu.  "
                  "y[12] = %s, y.get(12) = %s\n",
                  UL(y.num_nonempty()), UL(y.size()),
                  static_cast<string>(y[12]).c_str(),
                  static_cast<string>(y.get(12)).c_str());
  swap(x, y);

  y.clear();
  TEST(y == z);

  y.resize(70);
  for ( int i = 10; i < 40; ++i )
    y.set(i, AsString(-i));
  y.erase(y.begin() + 15, y.begin() + 30);
  y.erase(y.begin() + 34);
  y.erase(12);
  y.resize(38);
  y.resize(10000);
  y.set(9898, AsString(-9898));
  for ( sparsetable<string>::const_iterator it = y.begin(); it != y.end(); ++it ) {
    if ( y.test(it) )
      out += snprintf(out, LEFT, "y[%lu] is set\n", UL(it - y.begin()));
  }
  out += snprintf(out, LEFT, "That's %lu set buckets\n", UL(y.num_nonempty()));

  out += snprintf(out, LEFT, "Starting from y[32]...\n");
  for ( sparsetable<string>::const_nonempty_iterator it = y.get_iter(32);
        it != y.nonempty_end(); ++it )
    out += snprintf(out, LEFT, "y[??] = %s\n", it->c_str());

  out += snprintf(out, LEFT, "From y[32] down...\n");
  for ( sparsetable<string>::nonempty_iterator it = y.get_iter(32);
        it != y.nonempty_begin(); )
    out += snprintf(out, LEFT, "y[??] = %s\n", (*--it).c_str());
}

// The expected output from all of the above: TestInt() and TestString()
static const char g_expected[] = (
    "int test\n"
    "x[0]: 0\n"
    "x[1]: 0\n"
    "x[2]: 0\n"
    "x[3]: 0\n"
    "x[4]: 10\n"
    "x[5]: 0\n"
    "x[6]: 0\n"
    "x[0]: 0\n"
    "x[1]: 0\n"
    "x[2]: 0\n"
    "x[3]: 0\n"
    "x[4]: 10\n"
    "x[5]: 0\n"
    "x[6]: 0\n"
    "x[6]: 0\n"
    "x[5]: 0\n"
    "x[4]: 10\n"
    "x[3]: 0\n"
    "x[2]: 0\n"
    "x[1]: 0\n"
    "x[0]: 0\n"
    "x[6]: 0\n"
    "x[5]: 0\n"
    "x[4]: 10\n"
    "x[3]: 0\n"
    "x[2]: 0\n"
    "x[1]: 0\n"
    "x[0]: 0\n"
    "x[3]: 0\n"
    "x[4]: 10\n"
    "x[5]: 0\n"
    "x[4]: 10\n"
    "x[4]: 10\n"
    "x[3]: 0\n"
    "x[4]: 10\n"
    "x[5]: 55\n"
    "x[5]: 55\n"
    "x[6]: 66\n"
    "it == it? yes\n"
    "!(it != it)? yes\n"
    "!(it < it)? yes\n"
    "!(it > it)? yes\n"
    "it <= it? yes\n"
    "it >= it? yes\n"
    "!(it == it_minus_1)? yes\n"
    "it != it_minus_1? yes\n"
    "!(it < it_minus_1)? yes\n"
    "it > it_minus_1? yes\n"
    "!(it <= it_minus_1)? yes\n"
    "it >= it_minus_1? yes\n"
    "!(it_minus_1 == it)? yes\n"
    "it_minus_1 != it? yes\n"
    "it_minus_1 < it? yes\n"
    "!(it_minus_1 > it)? yes\n"
    "it_minus_1 <= it? yes\n"
    "!(it_minus_1 >= it)? yes\n"
    "!(it == it_plus_1)? yes\n"
    "it != it_plus_1? yes\n"
    "it < it_plus_1? yes\n"
    "!(it > it_plus_1)? yes\n"
    "it <= it_plus_1? yes\n"
    "!(it >= it_plus_1)? yes\n"
    "!(it_plus_1 == it)? yes\n"
    "it_plus_1 != it? yes\n"
    "!(it_plus_1 < it)? yes\n"
    "it_plus_1 > it? yes\n"
    "!(it_plus_1 <= it)? yes\n"
    "it_plus_1 >= it? yes\n"
    "x[4]: 10\n"
    "x[4]: 10\n"
    "x[3]: 0\n"
    "x[4]: 10\n"
    "x[5]: 55\n"
    "x[6]: 66\n"
    "it == it? yes\n"
    "!(it != it)? yes\n"
    "!(it < it)? yes\n"
    "!(it > it)? yes\n"
    "it <= it? yes\n"
    "it >= it? yes\n"
    "!(it == it_minus_1)? yes\n"
    "it != it_minus_1? yes\n"
    "!(it < it_minus_1)? yes\n"
    "it > it_minus_1? yes\n"
    "!(it <= it_minus_1)? yes\n"
    "it >= it_minus_1? yes\n"
    "!(it_minus_1 == it)? yes\n"
    "it_minus_1 != it? yes\n"
    "it_minus_1 < it? yes\n"
    "!(it_minus_1 > it)? yes\n"
    "it_minus_1 <= it? yes\n"
    "!(it_minus_1 >= it)? yes\n"
    "!(it == it_plus_1)? yes\n"
    "it != it_plus_1? yes\n"
    "it < it_plus_1? yes\n"
    "!(it > it_plus_1)? yes\n"
    "it <= it_plus_1? yes\n"
    "!(it >= it_plus_1)? yes\n"
    "!(it_plus_1 == it)? yes\n"
    "it_plus_1 != it? yes\n"
    "!(it_plus_1 < it)? yes\n"
    "it_plus_1 > it? yes\n"
    "!(it_plus_1 <= it)? yes\n"
    "it_plus_1 >= it? yes\n"
    "x.begin() == x.begin() + 1 - 1? yes\n"
    "x.begin() < x.end()? yes\n"
    "z.begin() < z.end()? no\n"
    "z.begin() <= z.end()? yes\n"
    "z.begin() == z.end()? yes\n"
    "x[??]: 10\n"
    "x[??]: 55\n"
    "x[??]: 66\n"
    "y[??]: -12\n"
    "y[??]: -47\n"
    "y[??]: -48\n"
    "y[??]: -49\n"
    "y[??]: -49\n"
    "y[??]: -48\n"
    "y[??]: -47\n"
    "y[??]: -12\n"
    "y[??]: -49\n"
    "y[??]: -48\n"
    "y[??]: -47\n"
    "y[??]: -12\n"
    "first non-empty y: -12\n"
    "first non-empty x: 10\n"
    "first non-empty x: 10\n"
    "first non-empty x: 10\n"
    "first non-empty y: -12\n"
    "first non-empty x: 10\n"
    "first non-empty x: 10\n"
    "first non-empty x: 10\n"
    "x.begin() == x.begin() + 1 - 1? yes\n"
    "z.begin() != z.end()? no\n"
    "x has 3/7 buckets, y 4/70, z 0/0\n"
    "y shrank and grew: it's now 2/70\n"
    "y[12] = -12, y.get(12) = -12\n"
    "y[12] cleared.  y now 1/70.  y[12] = 0, y.get(12) = 0\n"
    "y == z? no\n"
    "y[10] is set\n"
    "y[11] is set\n"
    "y[13] is set\n"
    "y[14] is set\n"
    "y[30] is set\n"
    "y[31] is set\n"
    "y[32] is set\n"
    "y[33] is set\n"
    "y[35] is set\n"
    "y[36] is set\n"
    "y[37] is set\n"
    "y[9898] is set\n"
    "That's 12 set buckets\n"
    "Starting from y[32]...\n"
    "y[??] = -32\n"
    "y[??] = -33\n"
    "y[??] = -35\n"
    "y[??] = -36\n"
    "y[??] = -37\n"
    "y[??] = -9898\n"
    "From y[32] down...\n"
    "y[??] = -31\n"
    "y[??] = -30\n"
    "y[??] = -14\n"
    "y[??] = -13\n"
    "y[??] = -11\n"
    "y[??] = -10\n"
    "y2[10] is -10\n"
    "y2[11] is -11\n"
    "y2[13] is -13\n"
    "y2[14] is -14\n"
    "y2[30] is -30\n"
    "y2[31] is -31\n"
    "y2[32] is -32\n"
    "y2[33] is -33\n"
    "y2[35] is -35\n"
    "y2[36] is -36\n"
    "y2[37] is -37\n"
    "y2[9898] is -9898\n"
    "That's 12 set buckets\n"
    "string test\n"
    "x[0]: \n"
    "x[1]: \n"
    "x[2]: \n"
    "x[3]: \n"
    "x[4]: foo\n"
    "x[5]: \n"
    "x[6]: \n"
    "x.begin() == x.begin() + 1 - 1? yes\n"
    "x.begin() < x.end()? yes\n"
    "z.begin() < z.end()? no\n"
    "z.begin() <= z.end()? yes\n"
    "z.begin() == z.end()? yes\n"
    "x[??]: foo\n"
    "y[??]: orange\n"
    "y[??]: grape\n"
    "y[??]: pear\n"
    "y[??]: apple\n"
    "x has 1/7 buckets, y 4/70, z 0/0\n"
    "y shrank and grew: it's now 2/70\n"
    "y[12] = orange, y.get(12) = orange\n"
    "y[12] cleared.  y now 1/70.  y[12] = , y.get(12) = \n"
    "y == z? no\n"
    "y[10] is set\n"
    "y[11] is set\n"
    "y[13] is set\n"
    "y[14] is set\n"
    "y[30] is set\n"
    "y[31] is set\n"
    "y[32] is set\n"
    "y[33] is set\n"
    "y[35] is set\n"
    "y[36] is set\n"
    "y[37] is set\n"
    "y[9898] is set\n"
    "That's 12 set buckets\n"
    "Starting from y[32]...\n"
    "y[??] = -32\n"
    "y[??] = -33\n"
    "y[??] = -35\n"
    "y[??] = -36\n"
    "y[??] = -37\n"
    "y[??] = -9898\n"
    "From y[32] down...\n"
    "y[??] = -31\n"
    "y[??] = -30\n"
    "y[??] = -14\n"
    "y[??] = -13\n"
    "y[??] = -11\n"
    "y[??] = -10\n"
    );

// defined at bottom of file for ease of maintainence
int main(int argc, char **argv) {          // though we ignore the args
  TestInt();
  TestString();

  // Finally, check to see if our output (in out) is what it's supposed to be.
  const size_t r = sizeof(g_expected) - 1;
  if ( r != out - outbuf ||               // output not the same size
       memcmp(outbuf, g_expected, r) ) {  // or bytes differed
    fprintf(stderr, "TESTS FAILED\n\nEXPECTED:\n\n%s\n\nACTUAL:\n\n%s\n\n",
            g_expected, outbuf);
    return 1;
  } else {
    printf("PASS.\n");
    return 0;
  }
}

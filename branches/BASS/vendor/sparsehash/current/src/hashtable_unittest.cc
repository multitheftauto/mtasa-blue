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
// This tests <google/sparsehash/densehashtable.h>
// This tests <google/dense_hash_set>
// This tests <google/dense_hash_map>
// This tests <google/sparsehash/sparsehashtable.h>
// This tests <google/sparse_hash_set>
// This tests <google/sparse_hash_map>

// Since {dense,sparse}hashtable is templatized, it's important that
// we test every function in every class in this file -- not just to
// see if it works, but even if it compiles.

#include "config.h"
#include <stdio.h>
#include <sys/stat.h>          // for stat()
#ifdef HAVE_UNISTD_H
#include <unistd.h>            // for unlink()
#endif
#include <string.h>
#include <time.h>              // for silly random-number-seed generator
#include <math.h>              // for sqrt()
#include <map>
#include <set>
#include <iterator>            // for insert_iterator
#include <iostream>
#include <iomanip>             // for setprecision()
#include <string>
#include HASH_FUN_H            // defined in config.h
#include <google/type_traits.h>
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#include <google/sparsehash/densehashtable.h>
#include <google/sparse_hash_map>
#include <google/sparse_hash_set>
#include <google/sparsehash/sparsehashtable.h>

// Otherwise, VC++7 warns about size_t -> int in the cout logging lines
#ifdef _MSC_VER
#pragma warning(disable:4267)
#endif

using GOOGLE_NAMESPACE::sparse_hash_map;
using GOOGLE_NAMESPACE::dense_hash_map;
using GOOGLE_NAMESPACE::sparse_hash_set;
using GOOGLE_NAMESPACE::dense_hash_set;
using GOOGLE_NAMESPACE::sparse_hashtable;
using GOOGLE_NAMESPACE::dense_hashtable;
using STL_NAMESPACE::map;
using STL_NAMESPACE::set;
using STL_NAMESPACE::pair;
using STL_NAMESPACE::make_pair;
using STL_NAMESPACE::string;
using STL_NAMESPACE::insert_iterator;
using STL_NAMESPACE::allocator;
using STL_NAMESPACE::equal_to;
using STL_NAMESPACE::ostream;

#define LOGF  STL_NAMESPACE::cout   // where we log to; LOGF is a historical name

#define CHECK(cond)  do {                       \
  if (!(cond)) {                                \
    LOGF << "Test failed: " #cond "\n";         \
    exit(1);                                    \
  }                                             \
} while (0)

#define CHECK_EQ(a, b)  CHECK((a) == (b))
#define CHECK_LT(a, b)  CHECK((a) < (b))

#ifndef WIN32   // windows defines its own version
static string TmpFile(const char* basename) {
  return string("/tmp/") + basename;
}
#endif

const char *words[] = {"Baffin\n",        // in /usr/dict/words
                       "Boffin\n",        // not in
                       "baffin\n",        // not in
                       "genial\n",        // last word in
                       "Aarhus\n",        // first word alphabetically
                       "Zurich\n",        // last word alphabetically
                       "Getty\n",
};

const char *nwords[] = {"Boffin\n",
                        "baffin\n",
};

const char *default_dict[] = {"Aarhus\n",
                              "aback\n",
                              "abandon\n",
                              "Baffin\n",
                              "baffle\n",
                              "bagged\n",
                              "congenial\n",
                              "genial\n",
                              "Getty\n",
                              "indiscreet\n",
                              "linens\n",
                              "pence\n",
                              "reassure\n",
                              "sequel\n",
                              "zoning\n",
                              "zoo\n",
                              "Zurich\n",
};

// Apparently identity is not stl-standard, so we define our own
template<class Value>
struct Identity {
  Value& operator()(Value& v) const { return v; }
  const Value& operator()(const Value& v) const { return v; }
};

// Likewise, it's not standard to hash a string.  Luckily, it is a char*
struct StrHash {
  size_t operator()(const string& s) const {
    return SPARSEHASH_HASH<const char*>()(s.c_str());
  }
};

// Let us log the pairs that make up a hash_map
template<class P1, class P2>
ostream& operator<<(ostream& s, const pair<P1, P2>& p) {
  s << "pair(" << p.first << ", " << p.second << ")";
  return s;
}

struct strcmp_fnc {
  bool operator()(const char* s1, const char* s2) const {
    return ((s1 == 0 && s2 == 0) ||
            (s1 && s2 && *s1 == *s2 && strcmp(s1, s2) == 0));
  }
};

namespace {

template <class T, class H, class I, class C, class A>
void set_empty_key(sparse_hashtable<T,T,H,I,C,A> *ht, T val) {
}

template <class T, class H, class C>
void set_empty_key(sparse_hash_set<T,H,C> *ht, T val) {
}

template <class K, class V, class H, class C>
void set_empty_key(sparse_hash_map<K,V,H,C> *ht, K val) {
}

template <class T, class H, class I, class C, class A>
void set_empty_key(dense_hashtable<T,T,H,I,C,A> *ht, T val) {
  ht->set_empty_key(val);
}

template <class T, class H, class C>
void set_empty_key(dense_hash_set<T,H,C> *ht, T val) {
  ht->set_empty_key(val);
}

template <class K, class V, class H, class C>
void set_empty_key(dense_hash_map<K,V,H,C> *ht, K val) {
  ht->set_empty_key(val);
}

template <class T, class H, class I, class C, class A>
bool clear_no_resize(sparse_hashtable<T,T,H,I,C,A> *ht) {
  return false;
}

template <class T, class H, class C>
bool clear_no_resize(sparse_hash_set<T,H,C> *ht) {
  return false;
}

template <class K, class V, class H, class C>
bool clear_no_resize(sparse_hash_map<K,V,H,C> *ht) {
  return false;
}

template <class T, class H, class I, class C, class A>
bool clear_no_resize(dense_hashtable<T,T,H,I,C,A> *ht) {
  ht->clear_no_resize();
  return true;
}

template <class T, class H, class C>
bool clear_no_resize(dense_hash_set<T,H,C> *ht) {
  ht->clear_no_resize();
  return true;
}

template <class K, class V, class H, class C>
bool clear_no_resize(dense_hash_map<K,V,H,C> *ht) {
  ht->clear_no_resize();
  return true;
}

template <class T, class H, class I, class C, class A>
void insert(dense_hashtable<T,T,H,I,C,A> *ht, T val) {
  ht->insert(val);
}

template <class T, class H, class C>
void insert(dense_hash_set<T,H,C> *ht, T val) {
  ht->insert(val);
}

template <class K, class V, class H, class C>
void insert(dense_hash_map<K,V,H,C> *ht, K val) {
  ht->insert(pair<K,V>(val,V()));
}

template <class T, class H, class I, class C, class A>
void insert(sparse_hashtable<T,T,H,I,C,A> *ht, T val) {
  ht->insert(val);
}

template <class T, class H, class C>
void insert(sparse_hash_set<T,H,C> *ht, T val) {
  ht->insert(val);
}

template <class K, class V, class H, class C>
void insert(sparse_hash_map<K,V,H,C> *ht, K val) {
  ht->insert(pair<K,V>(val,V()));
}

template <class HT, class Iterator>
void insert(HT *ht, Iterator begin, Iterator end) {
  ht->insert(begin, end);
}

// For hashtable's and hash_set's, the iterator insert works fine (and
// is used). But for the hash_map's, the iterator insert expects the
// iterators to point to pair's. So by looping over and calling insert
// on each element individually, the code below automatically expands
// into inserting a pair.
template <class K, class V, class H, class C, class Iterator>
void insert(dense_hash_map<K,V,H,C> *ht, Iterator begin, Iterator end) {
  while (begin != end) {
    insert(ht, *begin);
    ++begin;
  }
}

template <class K, class V, class H, class C, class Iterator>
void insert(sparse_hash_map<K,V,H,C> *ht, Iterator begin, Iterator end) {
  while (begin != end) {
    insert(ht, *begin);
    ++begin;
  }
}

// A version of insert that uses the insert_iterator.  But insert_iterator
// isn't defined for the low level hashtable classes, so we just punt to insert.

template <class T, class H, class I, class C, class A>
void iterator_insert(dense_hashtable<T,T,H,I,C,A>* ht, T val,
                            insert_iterator<dense_hashtable<T,T,H,I,C,A> >* ) {
  ht->insert(val);
}

template <class T, class H, class C>
void iterator_insert(dense_hash_set<T,H,C>* , T val,
                     insert_iterator<dense_hash_set<T,H,C> >* ii) {
  *(*ii)++ = val;
}

template <class K, class V, class H, class C>
void iterator_insert(dense_hash_map<K,V,H,C>* , K val,
                     insert_iterator<dense_hash_map<K,V,H,C> >* ii) {
  *(*ii)++ = pair<K,V>(val,V());
}

template <class T, class H, class I, class C, class A>
void iterator_insert(sparse_hashtable<T,T,H,I,C,A>* ht, T val,
                     insert_iterator<sparse_hashtable<T,T,H,I,C,A> >* ) {
  ht->insert(val);
}

template <class T, class H, class C>
void iterator_insert(sparse_hash_set<T,H,C>* , T val,
                     insert_iterator<sparse_hash_set<T,H,C> >* ii) {
  *(*ii)++ = val;
}

template <class K, class V, class H, class C>
void iterator_insert(sparse_hash_map<K,V,H,C> *, K val,
                     insert_iterator<sparse_hash_map<K,V,H,C> >* ii) {
  *(*ii)++ = pair<K,V>(val,V());
}


void write_item(FILE *fp, const char *val) {
  fwrite(val, strlen(val), 1, fp);   // \n serves to separate
}

// The weird 'const' declarations are desired by the compiler. Yucko.
void write_item(FILE *fp, const pair<char*const,int> &val) {
  fwrite(val.first, strlen(val.first), 1, fp);
}

void write_item(FILE *fp, const string &val) {
  fwrite(val.data(), val.length(), 1, fp);   // \n serves to separate
}

// The weird 'const' declarations are desired by the compiler. Yucko.
void write_item(FILE *fp, const pair<const string,int> &val) {
  fwrite(val.first.data(), val.first.length(), 1, fp);
}

char* read_line(FILE* fp, char* line, int linesize) {
  if ( fgets(line, linesize, fp) == NULL )
    return NULL;
  // normalize windows files :-(
  const size_t linelen = strlen(line);
  if ( linelen >= 2 && line[linelen-2] == '\r' && line[linelen-1] == '\n' ) {
    line[linelen-2] = '\n';
    line[linelen-1] = '\0';
  }
  return line;
}

void read_item(FILE *fp, char*const* val) {
  char line[1024];
  read_line(fp, line, sizeof(line));
  char **p = const_cast<char**>(val);
  *p = strdup(line);
}

void read_item(FILE *fp, pair<char*const,int> *val) {
  char line[1024];
  read_line(fp, line, sizeof(line));
  char **p = const_cast<char**>(&val->first);
  *p = strdup(line);
}

void read_item(FILE *fp, const string* val) {
  char line[1024];
  read_line(fp, line, sizeof(line));
  new(const_cast<string*>(val)) string(line);   // need to use placement new
}

void read_item(FILE *fp, pair<const string,int> *val) {
  char line[1024];
  read_line(fp, line, sizeof(line));
  new(const_cast<string*>(&val->first)) string(line);
}

void free_item(char*const* val) {
  free(*val);
}

void free_item(pair<char*const,int> *val) {
  free(val->first);
}

int get_int_item(int int_item) {
  return int_item;
}

int get_int_item(pair<int, int> val) {
  return val.first;
}

}  // end anonymous namespace

// Performs tests where the hashtable's value type is assumed to be int.
template <class htint>
void test_int() {
  htint x;
  htint y(1000);
  htint z(64);
  set_empty_key(&x, 0xefefef);
  set_empty_key(&y, 0xefefef);
  set_empty_key(&z, 0xefefef);

  CHECK(y.empty());
  insert(&y, 1);
  CHECK(!y.empty());
  insert(&y, 11);
  insert(&y, 111);
  insert(&y, 1111);
  insert(&y, 11111);
  insert(&y, 111111);
  insert(&y, 1111111);     // 1M, more or less
  insert(&y, 11111111);
  insert(&y, 111111111);
  insert(&y, 1111111111);  // 1B, more or less
  for ( int i = 0; i < 64; ++i )
    insert(&z, i);
  // test the second half of the insert with an insert_iterator
  insert_iterator<htint> insert_iter(z, z.begin());
  for ( int i = 32; i < 64; ++i )
    iterator_insert(&z, i, &insert_iter);

  // only perform the following CHECKs for
  // dense{hashtable, _hash_set, _hash_map}
  if (clear_no_resize(&x)) {
    // make sure x has to increase its number of buckets
    typename htint::size_type empty_bucket_count = x.bucket_count();
    int last_element = 0;
    while (x.bucket_count() == empty_bucket_count) {
      insert(&x, last_element);
      ++last_element;
    }
    // if clear_no_resize is supported (i.e. htint is a
    // dense{hashtable,_hash_set,_hash_map}), it should leave the bucket_count
    // as is.
    typename htint::size_type last_bucket_count = x.bucket_count();
    clear_no_resize(&x);
    CHECK(last_bucket_count == x.bucket_count());
    CHECK(x.empty());
    LOGF << "x has " << x.bucket_count() << " buckets\n";
    LOGF << "x size " << x.size() << "\n";
    // when inserting the same number of elements again, no resize should be
    // necessary
    for (int i = 0; i < last_element; ++i) {
      insert(&x, i);
      CHECK(x.bucket_count() == last_bucket_count);
    }
  }

  for ( typename htint::const_iterator it = y.begin(); it != y.end(); ++it )
    LOGF << "y: " << get_int_item(*it) << "\n";
  z.insert(y.begin(), y.end());
  swap(y,z);
  for ( typename htint::iterator it = y.begin(); it != y.end(); ++it )
    LOGF << "y+z: " << get_int_item(*it) << "\n";
  LOGF << "z has " << z.bucket_count() << " buckets\n";
  LOGF << "y has " << y.bucket_count() << " buckets\n";
  LOGF << "z size: " << z.size() << "\n";

  for (int i = 0; i < 64; ++i)
    CHECK(y.find(i) != y.end());

  CHECK(z.size() == 10);
  z.set_deleted_key(1010101010);      // an unused value
  z.erase(11111);
  CHECK(z.size() == 9);
  insert(&z, 11111);                  // should retake deleted value
  CHECK(z.size() == 10);
  // Do the delete/insert again.  Last time we probably resized; this time no
  z.erase(11111);
  insert(&z, 11111);                  // should retake deleted value
  CHECK(z.size() == 10);

  z.erase(-11111);                    // shouldn't do anything
  CHECK(z.size() == 10);
  z.erase(1);
  CHECK(z.size() == 9);
  typename htint::iterator itdel = z.find(1111);
  z.erase(itdel);
  CHECK(z.size() == 8);
  itdel = z.find(2222);               // should be end()
  z.erase(itdel);                     // shouldn't do anything
  CHECK(z.size() == 8);
  for ( typename htint::const_iterator it = z.begin(); it != z.end(); ++it )
    LOGF << "y: " << get_int_item(*it) << "\n";
  z.set_deleted_key(1010101011);      // a different unused value
  for ( typename htint::const_iterator it = z.begin(); it != z.end(); ++it )
    LOGF << "y: " << get_int_item(*it) << "\n";
  LOGF << "That's " << z.size() << " elements\n";
  z.erase(z.begin(), z.end());
  CHECK(z.empty());

  y.clear();
  CHECK(y.empty());
  LOGF << "y has " << y.bucket_count() << " buckets\n";
}

// Performs tests where the hashtable's value type is assumed to be char*.
// The read_write parameters specifies whether the read/write tests
// should be performed. Note that densehashtable::write_metdata is not
// implemented, so we only do the read/write tests for the
// sparsehashtable varieties.
template <class ht>
void test_charptr(bool read_write) {
  ht w;
  set_empty_key(&w, (char*) NULL);
  insert(&w, const_cast<char **>(nwords),
         const_cast<char **>(nwords) + sizeof(nwords) / sizeof(*nwords));
  LOGF << "w has " << w.size() << " items\n";
  CHECK(w.size() == 2);
  CHECK(w == w);

  ht x;
  set_empty_key(&x, (char*) NULL);
  long dict_size = 1;        // for size stats -- can't be 0 'cause of division

  map<string, int> counts;
  // Hash the dictionary
  {
    // automake says 'look for all data files in $srcdir.'  OK.
    string filestr = (string(getenv("srcdir") ? getenv("srcdir") : ".") +
                      "/src/words");
    const char* file = filestr.c_str();
    FILE *fp = fopen(file, "rb");
    if ( fp == NULL ) {
      LOGF << "Can't open " << file << ", using small, built-in dict...\n";
      for (int i = 0; i < sizeof(default_dict)/sizeof(*default_dict); ++i) {
        insert(&x, strdup(default_dict[i]));
        counts[default_dict[i]] = 0;
      }
    } else {
      char line[1024];
      while ( read_line(fp, line, sizeof(line)) ) {
        insert(&x, strdup(line));
        counts[line] = 0;
      }
      LOGF << "Read " << x.size() << " words from " << file << "\n";
      fclose(fp);
      struct stat buf;
      stat(file, &buf);
      dict_size = buf.st_size;
      LOGF << "Size of " << file << ": " << buf.st_size << " bytes\n";
    }
    for (char **word = const_cast<char **>(words);
          word < const_cast<char **>(words) + sizeof(words) / sizeof(*words);
          ++word ) {
      if (x.find(*word) == x.end()) {
        CHECK(w.find(*word) != w.end());
      } else {
        CHECK(w.find(*word) == w.end());
      }
    }
  }
  CHECK(counts.size() == x.size());

  // Save the hashtable.
  if (read_write) {
    const string file_string = TmpFile("#hashtable_unittest_dicthash");
    const char* file = file_string.c_str();
    FILE *fp = fopen(file, "wb");
    if ( fp == NULL ) {
      // maybe we can't write to /tmp/.  Try the current directory
      file = "#hashtable_unittest_dicthash";
      fp = fopen(file, "wb");
    }
    if ( fp == NULL ) {
      LOGF << "Can't open " << file << " skipping hashtable save...\n";
    } else {
      x.write_metadata(fp);        // this only writes meta-information
      int write_count = 0;
      for ( typename ht::iterator it = x.begin(); it != x.end(); ++it ) {
        write_item(fp, *it);
        free_item(&(*it));
        ++write_count;
      }
      LOGF << "Wrote " << write_count << " words to " << file << "\n";
      fclose(fp);
      struct stat buf;
      stat(file, &buf);
      LOGF << "Size of " << file << ": " << buf.st_size << " bytes\n";
      LOGF << STL_NAMESPACE::setprecision(3)
           << "Hashtable overhead "
           << (buf.st_size - dict_size) * 100.0 / dict_size
           << "% ("
           << (buf.st_size - dict_size) * 8.0 / write_count
           << " bits/entry)\n";
      x.clear();

      // Load the hashtable
      fp = fopen(file, "rb");
      if ( fp == NULL ) {
        LOGF << "Can't open " << file << " skipping hashtable reload...\n";
      } else {
        x.read_metadata(fp);      // reads metainformation
        LOGF << "Hashtable size: " << x.size() << "\n";
        int read_count = 0;
        for ( typename ht::iterator it = x.begin(); it != x.end(); ++it ) {
          read_item(fp, &(*it));
          ++read_count;
        }
        LOGF << "Read " << read_count << " words from " << file << "\n";
        fclose(fp);
        unlink(file);
        for ( char **word = const_cast<char **>(words);
              word < const_cast<char **>(words) + sizeof(words) / sizeof(*words);
              ++word ) {
          if (x.find(*word) == x.end()) {
            CHECK(w.find(*word) != w.end());
          } else {
            CHECK(w.find(*word) == w.end());
          }
        }
      }
    }
  }
  for ( typename ht::iterator it = x.begin(); it != x.end(); ++it ) {
    free_item(&(*it));
  }
}

// Perform tests where the hashtable's value type is assumed to
// be string.
// TODO(austern): factor out the bulk of test_charptr and test_string
// into a common function.
template <class ht>
void test_string(bool read_write) {
  ht w;
  set_empty_key(&w, string("-*- empty key -*-"));
  const int N = sizeof(nwords) / sizeof(*nwords);
  string* nwords1 = new string[N];
  for (int i = 0; i < N; ++i)
    nwords1[i] = nwords[i];
  insert(&w, nwords1, nwords1 + N);
  delete[] nwords1;
  LOGF << "w has " << w.size() << " items\n";
  CHECK(w.size() == 2);
  CHECK(w == w);

  ht x;
  set_empty_key(&x, string("-*- empty key -*-"));
  long dict_size = 1;        // for size stats -- can't be 0 'cause of division

  map<string, int> counts;
  // Hash the dictionary
  {
    // automake says 'look for all data files in $srcdir.'  OK.
    string filestr = (string(getenv("srcdir") ? getenv("srcdir") : ".") +
                      "/src/words");
    const char* file = filestr.c_str();
    FILE *fp = fopen(file, "rb");
    if ( fp == NULL ) {
      LOGF << "Can't open " << file << ", using small, built-in dict...\n";
      for (int i = 0; i < sizeof(default_dict)/sizeof(*default_dict); ++i) {
        insert(&x, string(default_dict[i]));
        counts[default_dict[i]] = 0;
      }
    } else {
      char line[1024];
      while ( fgets(line, sizeof(line), fp) ) {
        insert(&x, string(line));
        counts[line] = 0;
      }
      LOGF << "Read " << x.size() << " words from " << file << "\n";
      fclose(fp);
      struct stat buf;
      stat(file, &buf);
      dict_size = buf.st_size;
      LOGF << "Size of " << file << ": " << buf.st_size << " bytes\n";
    }
    for ( const char* const* word = words;
          word < words + sizeof(words) / sizeof(*words);
          ++word ) {
      if (x.find(*word) == x.end()) {
        CHECK(w.find(*word) != w.end());
      } else {
        CHECK(w.find(*word) == w.end());
      }
    }
  }
  CHECK(counts.size() == x.size());
  {
    // verify that size() works correctly
    int xcount = 0;
    for ( typename ht::iterator it = x.begin(); it != x.end(); ++it ) {
      ++xcount;
    }
    CHECK(x.size() == xcount);
  }

  // Save the hashtable.
  if (read_write) {
    const string file_string = TmpFile("#hashtable_unittest_dicthash_str");
    const char* file = file_string.c_str();
    FILE *fp = fopen(file, "wb");
    if ( fp == NULL ) {
      // maybe we can't write to /tmp/.  Try the current directory
      file = "#hashtable_unittest_dicthash_str";
      fp = fopen(file, "wb");
    }
    if ( fp == NULL ) {
      LOGF << "Can't open " << file << " skipping hashtable save...\n";
    } else {
      x.write_metadata(fp);        // this only writes meta-information
      int write_count = 0;
      for ( typename ht::iterator it = x.begin(); it != x.end(); ++it ) {
        write_item(fp, *it);
        ++write_count;
      }
      LOGF << "Wrote " << write_count << " words to " << file << "\n";
      fclose(fp);
      struct stat buf;
      stat(file, &buf);
      LOGF << "Size of " << file << ": " << buf.st_size << " bytes\n";
      LOGF << STL_NAMESPACE::setprecision(3)
           << "Hashtable overhead "
           << (buf.st_size - dict_size) * 100.0 / dict_size
           << "% ("
           << (buf.st_size - dict_size) * 8.0 / write_count
           << " bits/entry)\n";
      x.clear();

      // Load the hashtable
      fp = fopen(file, "rb");
      if ( fp == NULL ) {
        LOGF << "Can't open " << file << " skipping hashtable reload...\n";
      } else {
        x.read_metadata(fp);      // reads metainformation
        LOGF << "Hashtable size: " << x.size() << "\n";
        int count = 0;
        for ( typename ht::iterator it = x.begin(); it != x.end(); ++it ) {
          read_item(fp, &(*it));
          ++count;
        }
        LOGF << "Read " << count << " words from " << file << "\n";
        fclose(fp);
        unlink(file);
        for ( const char* const* word = words;
              word < words + sizeof(words) / sizeof(*words);
              ++word ) {
          if (x.find(*word) == x.end()) {
            CHECK(w.find(*word) != w.end());
          } else {
            CHECK(w.find(*word) == w.end());
          }
        }
      }
    }
  }

  // ensure that destruction is done properly in clear_no_resize()
  if (!clear_no_resize(&w)) w.clear();
}

// The read_write parameters specifies whether the read/write tests
// should be performed. Note that densehashtable::write_metdata is not
// implemented, so we only do the read/write tests for the
// sparsehashtable varieties.
template<class ht, class htstr, class htint>
void test(bool read_write) {
  test_int<htint>();
  test_string<htstr>(read_write);
  test_charptr<ht>(read_write);
}

// For data types with trivial copy-constructors and destructors, we
// should use an optimized routine for data-copying, that involves
// memmove.  We test this by keeping count of how many times the
// copy-constructor is called; it should be much less with the
// optimized code.

class Memmove {
 public:
  Memmove(): i_(0) {}
  explicit Memmove(int i): i_(i) {}
  Memmove(const Memmove& that) {
    this->i_ = that.i_;
    num_copies_++;
  }

  int i_;
  static int num_copies_;
};
int Memmove::num_copies_ = 0;


// This is what tells the hashtable code it can use memmove for this class:
_START_GOOGLE_NAMESPACE_
template<> struct has_trivial_copy<Memmove> : true_type { };
template<> struct has_trivial_destructor<Memmove> : true_type { };
_END_GOOGLE_NAMESPACE_

class NoMemmove {
 public:
  NoMemmove(): i_(0) {}
  explicit NoMemmove(int i): i_(i) {}
  NoMemmove(const NoMemmove& that) {
    this->i_ = that.i_;
    num_copies_++;
  }

  int i_;
  static int num_copies_;
};
int NoMemmove::num_copies_ = 0;

void TestSimpleDataTypeOptimizations() {
  {
    sparse_hash_map<int, Memmove> memmove;
    sparse_hash_map<int, NoMemmove> nomemmove;

    Memmove::num_copies_ = 0;  // reset
    NoMemmove::num_copies_ = 0;  // reset
    for (int i = 10000; i > 0; i--) {
      memmove[i] = Memmove(i);
    }
    for (int i = 10000; i > 0; i--) {
      nomemmove[i] = NoMemmove(i);
    }
    LOGF << "sparse_hash_map copies for unoptimized/optimized cases: "
         << NoMemmove::num_copies_ << "/" << Memmove::num_copies_ << "\n";
    CHECK(NoMemmove::num_copies_ > Memmove::num_copies_);
  }
  // Same should hold true for dense_hash_map
  {
    dense_hash_map<int, Memmove> memmove;
    dense_hash_map<int, NoMemmove> nomemmove;
    memmove.set_empty_key(0);
    nomemmove.set_empty_key(0);

    Memmove::num_copies_ = 0;  // reset
    NoMemmove::num_copies_ = 0;  // reset
    for (int i = 10000; i > 0; i--) {
      memmove[i] = Memmove(i);
    }
    for (int i = 10000; i > 0; i--) {
      nomemmove[i] = NoMemmove(i);
    }
    LOGF << "dense_hash_map copies for unoptimized/optimized cases: "
         << NoMemmove::num_copies_ << "/" << Memmove::num_copies_ << "\n";
    CHECK(NoMemmove::num_copies_ > Memmove::num_copies_);
  }
}

void TestShrinking() {
  // We want to make sure that when we create a hashtable, and then
  // add and delete one element, the size of the hashtable doesn't
  // change.
  {
    sparse_hash_set<int> s;
    s.set_deleted_key(0);
    const int old_bucket_count = s.bucket_count();
    s.insert(4);
    s.erase(4);
    s.insert(4);
    s.erase(4);
    CHECK_EQ(old_bucket_count, s.bucket_count());
  }
  {
    dense_hash_set<int> s;
    s.set_deleted_key(0);
    s.set_empty_key(1);
    const int old_bucket_count = s.bucket_count();
    s.insert(4);
    s.erase(4);
    s.insert(4);
    s.erase(4);
    CHECK_EQ(old_bucket_count, s.bucket_count());
  }
  {
    sparse_hash_set<int> s(2);        // start small: only expects 2 items
    CHECK_LT(s.bucket_count(), 32);   // verify we actually do start small
    s.set_deleted_key(0);
    const int old_bucket_count = s.bucket_count();
    s.insert(4);
    s.erase(4);
    s.insert(4);
    s.erase(4);
    CHECK_EQ(old_bucket_count, s.bucket_count());
  }
  {
    dense_hash_set<int> s(2);   // start small: only expects 2 items
    CHECK_LT(s.bucket_count(), 32);   // verify we actually do start small
    s.set_deleted_key(0);
    s.set_empty_key(1);
    const int old_bucket_count = s.bucket_count();
    s.insert(4);
    s.erase(4);
    s.insert(4);
    s.erase(4);
    CHECK_EQ(old_bucket_count, s.bucket_count());
  }
}

class TestHashFcn : public SPARSEHASH_HASH<int> {
 public:
  explicit TestHashFcn(int i)
      : id_(i) {
  }

  int id() const {
    return id_;
  }

 private:
  int id_;
};

class TestEqualTo : public equal_to<int> {
 public:
  explicit TestEqualTo(int i)
      : id_(i) {
  }

  int id() const {
    return id_;
  }

 private:
  int id_;
};

template <template <class V, class H, class E, class A> class Hash>
void TestHash() {
  typedef Hash<int, TestHashFcn, TestEqualTo, allocator<int> > TheHash;
  const TestHashFcn fcn(1);
  const TestEqualTo eqt(2);
  {
    const TheHash simple(0, fcn, eqt);
    CHECK(fcn.id() == simple.hash_funct().id());
    CHECK(eqt.id() == simple.key_eq().id());
  }
  {
    const set<int> input;
    const TheHash iterated(input.begin(), input.end(), 0, fcn, eqt);
    CHECK(fcn.id() == iterated.hash_funct().id());
    CHECK(eqt.id() == iterated.key_eq().id());
  }
}

static void TestHashes() {
  TestHash<sparse_hash_set>();
  TestHash<dense_hash_set>();
}

template <template <class K, class T, class H, class E, class A> class Map>
void TestMap() {
  typedef Map<int, int, TestHashFcn, TestEqualTo, allocator<int> > TheMap;
  const TestHashFcn fcn(1);
  const TestEqualTo eqt(2);
  {
    const TheMap simple(0, fcn, eqt);
    CHECK(fcn.id() == simple.hash_funct().id());
    CHECK(eqt.id() == simple.key_eq().id());
  }
  {
    const map<int, int> input;
    const TheMap iterated(input.begin(), input.end(), 0, fcn, eqt);
    CHECK(fcn.id() == iterated.hash_funct().id());
    CHECK(eqt.id() == iterated.key_eq().id());
  }
}

static void TestMaps() {
  TestMap<sparse_hash_map>();
  TestMap<dense_hash_map>();
}

static void TestOperatorEquals() {
  {
    dense_hash_set<int> sa, sb;
    sa.set_empty_key(-1);
    sb.set_empty_key(-1);
    sa.set_deleted_key(-2);
    sb.set_deleted_key(-2);
    CHECK(sa == sb);
    sa.insert(1);
    CHECK(sa != sb);
    sa.insert(2);
    CHECK(sa != sb);
    sb.insert(2);
    CHECK(sa != sb);
    sb.insert(1);
    CHECK(sa == sb);
    sb.erase(1);
    CHECK(sa != sb);
  }
  {
    dense_hash_map<int, string> sa, sb;
    sa.set_empty_key(-1);
    sb.set_empty_key(-1);
    sa.set_deleted_key(-2);
    sb.set_deleted_key(-2);
    CHECK(sa == sb);
    sa.insert(make_pair(1, "a"));
    CHECK(sa != sb);
    sa.insert(make_pair(2, "b"));
    CHECK(sa != sb);
    sb.insert(make_pair(2, "b"));
    CHECK(sa != sb);
    sb.insert(make_pair(1, "a"));
    CHECK(sa == sb);
    sa[1] = "goodbye";
    CHECK(sa != sb);
    sb.erase(1);
    CHECK(sa != sb);
  }
}

// Test the interface for setting the resize parameters in a
// sparse_hash_set or dense_hash_set.
template<class HS>
static void TestResizingParameters() {
  const int kSize = 16536;
  // Check growing past various thresholds and then shrinking below
  // them.
  for (float grow_threshold = 0.2;
       grow_threshold <= 0.8;
       grow_threshold += 0.2) {
    HS hs;
    hs.set_deleted_key(-1);
    set_empty_key(&hs, -2);
    hs.set_resizing_parameters(0.0, grow_threshold);
    hs.resize(kSize);
    size_t bucket_count = hs.bucket_count();
    // Erase and insert an element to set consider_shrink = true,
    // which should not cause a shrink because the threshold is 0.0.
    hs.insert(1);
    hs.erase(1);
    for (int i = 0;; ++i) {
      hs.insert(i);
      if (static_cast<float>(hs.size())/bucket_count < grow_threshold) {
        CHECK(hs.bucket_count() == bucket_count);
      } else {
        CHECK(hs.bucket_count() > bucket_count);
        break;
      }
    }
    // Now set a shrink threshold 1% below the current size and remove
    // items until the size falls below that.
    const float shrink_threshold = static_cast<float>(hs.size()) /
        hs.bucket_count() - 0.01;
    hs.set_resizing_parameters(shrink_threshold, 1.0);
    bucket_count = hs.bucket_count();
    for (int i = 0;; ++i) {
      hs.erase(i);
      // A resize is only triggered by an insert, so add and remove a
      // value every iteration to trigger the shrink as soon as the
      // threshold is passed.
      hs.erase(i+1);
      hs.insert(i+1);
      if (static_cast<float>(hs.size())/bucket_count > shrink_threshold) {
        CHECK(hs.bucket_count() == bucket_count);
      } else {
        CHECK(hs.bucket_count() < bucket_count);
        break;
      }
    }
  }
}

int main(int argc, char **argv) {
  TestOperatorEquals();

  // SPARSEHASH_HASH is defined in sparseconfig.h.  It resolves to the
  // system hash function (usually, but not always, named "hash") on
  // whatever system we're on.

  // First try with the low-level hashtable interface
  LOGF << "\n\nTEST WITH DENSE_HASHTABLE\n\n";
  test<dense_hashtable<char *, char *, SPARSEHASH_HASH<const char *>,
                       Identity<char *>, strcmp_fnc, allocator<char *> >,
       dense_hashtable<string, string, StrHash,
                       Identity<string>, equal_to<string>, allocator<string> >,
       dense_hashtable<int, int, SPARSEHASH_HASH<int>,
                       Identity<int>, equal_to<int>, allocator<int> > >(
                         false);

  // Now try with hash_set, which should be equivalent
  LOGF << "\n\nTEST WITH DENSE_HASH_SET\n\n";
  test<dense_hash_set<char *, SPARSEHASH_HASH<const char *>, strcmp_fnc>,
       dense_hash_set<string, StrHash>,
       dense_hash_set<int> >(false);

  TestResizingParameters<dense_hash_set<int> >();

  // Now try with hash_map, which differs only in insert()
  LOGF << "\n\nTEST WITH DENSE_HASH_MAP\n\n";
  test<dense_hash_map<char *, int, SPARSEHASH_HASH<const char *>, strcmp_fnc>,
       dense_hash_map<string, int, StrHash>,
       dense_hash_map<int, int> >(false);

  // First try with the low-level hashtable interface
  LOGF << "\n\nTEST WITH SPARSE_HASHTABLE\n\n";
  test<sparse_hashtable<char *, char *, SPARSEHASH_HASH<const char *>,
                       Identity<char *>, strcmp_fnc, allocator<char *> >,
       sparse_hashtable<string, string, StrHash,
                       Identity<string>, equal_to<string>, allocator<string> >,
       sparse_hashtable<int, int, SPARSEHASH_HASH<int>,
                       Identity<int>, equal_to<int>, allocator<int> > >(
                         true);

  // Now try with hash_set, which should be equivalent
  LOGF << "\n\nTEST WITH SPARSE_HASH_SET\n\n";
  test<sparse_hash_set<char *, SPARSEHASH_HASH<const char *>, strcmp_fnc>,
       sparse_hash_set<string, StrHash>,
       sparse_hash_set<int> >(true);

  TestResizingParameters<sparse_hash_set<int> >();

  // Now try with hash_map, which differs only in insert()
  LOGF << "\n\nTEST WITH SPARSE_HASH_MAP\n\n";
  test<sparse_hash_map<char *, int, SPARSEHASH_HASH<const char *>, strcmp_fnc>,
       sparse_hash_map<string, int, StrHash>,
       sparse_hash_map<int, int> >(true);

  // Test that we use the optimized routines for simple data types
  LOGF << "\n\nTesting simple-data-type optimizations\n";
  TestSimpleDataTypeOptimizations();

  // Test shrinking to very small sizes
  LOGF << "\n\nTesting shrinking behavior";
  TestShrinking();

  // Test that the hashers and key_equals are used properly in hash tables and
  // hash maps.
  LOGF << "\n\nTesting hashers and key_equals\n";
  TestHashes();
  TestMaps();

  LOGF << "\nAll tests pass.\n";
  return 0;
}

//*********************************************************************
//* Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//*
//* Enhancements by Stanley Yamane:
//*     o reverse lookup table for the decode function
//*     o reserve string buffer space in advance
//*
//*
//* Reformated by James W. Anderson.  Added isPrintable method.
//*********************************************************************

#include <ctype.h>

#include "base64.h"

using std::string;

const char Base64::fillchar = '=';
const string::size_type Base64::np = string::npos;

                               // 0000000000111111111122222222223333333333444444444455555555556666
                               // 0123456789012345678901234567890123456789012345678901234567890123
const string Base64::Base64Table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

// Decode Table gives the index of any valid base64 character in the Base64 table
// 65 == A, 97 == a, 48 == 0, 43 == +, 47 == /


const string::size_type Base64::DecodeTable[] =
//0  1  2  3  4  5  6  7  8  9
{np,np,np,np,np,np,np,np,np,np,  // 0 - 9
 np,np,np,np,np,np,np,np,np,np,  //10 -19
 np,np,np,np,np,np,np,np,np,np,  //20 -29
 np,np,np,np,np,np,np,np,np,np,  //30 -39
 np,np,np,62,np,np,np,63,52,53,  //40 -49
 54,55,56,57,58,59,60,61,np,np,  //50 -59
 np,np,np,np,np, 0, 1, 2, 3, 4,  //60 -69
 5, 6, 7, 8, 9,10,11,12,13,14,  //70 -79
 15,16,17,18,19,20,21,22,23,24,  //80 -89
 25,np,np,np,np,np,np,26,27,28,  //90 -99
 29,30,31,32,33,34,35,36,37,38,  //100 -109
 39,40,41,42,43,44,45,46,47,48,  //110 -119
 49,50,51,np,np,np,np,np,np,np,  //120 -129
 np,np,np,np,np,np,np,np,np,np,  //130 -139
 np,np,np,np,np,np,np,np,np,np,  //140 -149
 np,np,np,np,np,np,np,np,np,np,  //150 -159
 np,np,np,np,np,np,np,np,np,np,  //160 -169
 np,np,np,np,np,np,np,np,np,np,  //170 -179
 np,np,np,np,np,np,np,np,np,np,  //180 -189
 np,np,np,np,np,np,np,np,np,np,  //190 -199
 np,np,np,np,np,np,np,np,np,np,  //200 -209
 np,np,np,np,np,np,np,np,np,np,  //210 -219
 np,np,np,np,np,np,np,np,np,np,  //220 -229
 np,np,np,np,np,np,np,np,np,np,  //230 -239
 np,np,np,np,np,np,np,np,np,np,  //240 -249
 np,np,np,np,np,np               //250 -256
};


string Base64::encode(const string& data) {
  string::size_type i;
  char c;
  string::size_type len = data.length();
  string ret;

  ret.reserve(len * 2);

  for (i = 0; i < len; ++i)
  {
    c = (data[i] >> 2) & 0x3f;
    ret.append(1, Base64Table[c]);
    c = (data[i] << 4) & 0x3f;
    if (++i < len)
      c |= (data[i] >> 4) & 0x0f;

    ret.append(1, Base64Table[c]);
    if (i < len)
    {
      c = (data[i] << 2) & 0x3f;
      if (++i < len)
	c |= (data[i] >> 6) & 0x03;

      ret.append(1, Base64Table[c]);
    }
    else
    {
      ++i;
      ret.append(1, fillchar);
    }

    if (i < len)
    {
      c = data[i] & 0x3f;
      ret.append(1, Base64Table[c]);
    }
    else
    {
      ret.append(1, fillchar);
    }
  }

  return(ret);
} // encode

string Base64::decode(const string& data, string& ret) {
  string::size_type i;
  char c;
  char c1;
  string::size_type len = data.length();
  
  ret.reserve(len);

  for (i = 0; i < len; ++i)
  {
    c = (char) DecodeTable[(unsigned char)data[i]];
    ++i;
    c1 = (char) DecodeTable[(unsigned char)data[i]];
    c = (c << 2) | ((c1 >> 4) & 0x3);
    ret.append(1, c);
    if (++i < len)
    {
      c = data[i];
      if (fillchar == c)
	break;

      c = (char) DecodeTable[(unsigned char)data[i]];
      c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
      ret.append(1, c1);
    }

    if (++i < len)
    {
      c1 = data[i];
      if (fillchar == c1)
	break;

      c1 = (char) DecodeTable[(unsigned char)data[i]];
      c = ((c << 6) & 0xc0) | c1;
      ret.append(1, c);
    }
  }

  return(ret);
} // decode

bool Base64::isPrintable(const string& s) {
  size_t i = 0;
  while (i < s.size() && (isprint(s[i]) || isspace(s[i]))) {
    i++;
  }
  return (i == s.size());
} // isPrintable

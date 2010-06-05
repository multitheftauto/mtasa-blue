//*********************************************************************
//* C_Base64 - a simple base64 encoder and decoder.
//*
//*     Copyright (c) 1999, Bob Withers - bwit@pobox.com
//*
//* This code may be freely used for any purpose, either personal
//* or commercial, provided the authors copyright notice remains
//* intact.
//
//* Reformated by James W. Anderson.  Added declarations to use
//* enhanced version.
//*********************************************************************

#ifndef BASE64_H
#define BASE64_H

#include <string>

class Base64 {
public:
  static std::string encode(const std::string& data);
  static std::string decode(const std::string& data, std::string& ret);

  static bool isPrintable(const std::string& s);

  static const std::string Base64Table;

private:
  static const std::string::size_type DecodeTable[];
  static const char fillchar;
  static const std::string::size_type np;
};

#endif // BASE64_H

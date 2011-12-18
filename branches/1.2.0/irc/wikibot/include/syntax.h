#pragma once

#include <sys/types.h>

extern "C" bool colorizeSyntax ( const char* source, char* dest, size_t len );
extern "C" void setReservedWordsPath ( const char* filepath );

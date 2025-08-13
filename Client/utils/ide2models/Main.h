/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/ide2models/Main.h
 *  PURPOSE:     IDE to models converter
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdio>
using namespace std;

int  main(int iArgumentCount, char* szArgs[]);
int  Convert(FILE* pInput, FILE* pOutput);
bool Parse(const char* szLine, FILE* pOutput);

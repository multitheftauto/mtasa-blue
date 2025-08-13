/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        utils/map2xml/Main.h
 *  PURPOSE:     .map (race) to .xml (new) converter utility
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CConfig.h"
#include <cstdio>
using namespace std;

int  main(int iArgumentCount, char* szArgs[]);
int  Convert(CConfig* pInput, FILE* pOutput);
bool Parse(const char* szLine, FILE* pOutput);

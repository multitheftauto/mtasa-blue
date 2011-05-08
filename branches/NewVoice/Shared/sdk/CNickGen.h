/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.1
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CNickGen.h
*  PURPOSE:     Generate hillariously poor nicknames for players
*  DEVELOPERS:  Talidan
*
*****************************************************************************/

#ifndef __CNICKGEN_H
#define __CNICKGEN_H

#define NICKGEN_NUM_ADJECTIVES      1048
#define NICKGEN_NUM_NOUNS           934

class CNickGen
{
    public:
        static const char*                 m_szAdjectives [ NICKGEN_NUM_ADJECTIVES ];
        static const char*                 m_szNouns [ NICKGEN_NUM_NOUNS ];
        static SString                     GetRandomNickname    ( void );
};

#include "CNickGen.cpp"

#endif

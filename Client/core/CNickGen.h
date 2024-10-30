/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.1
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CNickGen.h
 *  PURPOSE:     Generate hillariously poor nicknames for players
 *
 *****************************************************************************/

#pragma once

#define NICKGEN_NUM_ADJECTIVES      1040
#define NICKGEN_NUM_NOUNS           921

class CNickGen
{
public:
    static const char* const m_szAdjectives[NICKGEN_NUM_ADJECTIVES];
    static const char* const m_szNouns[NICKGEN_NUM_NOUNS];
    static SString           GetRandomNickname();
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.1
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CNickGen.h
 *  PURPOSE:     Generate hillariously poor nicknames for players
 *
 *****************************************************************************/

#pragma once
#include <array>

#define NICKGEN_NUM_ADJECTIVES      1040
#define NICKGEN_NUM_NOUNS           921

class CNickGen
{
public:
    static const std::array<const char*, NICKGEN_NUM_ADJECTIVES> m_szAdjectives;
    static const std::array<const char*, NICKGEN_NUM_NOUNS> m_szNouns;
    static SString           GetRandomNickname();
};

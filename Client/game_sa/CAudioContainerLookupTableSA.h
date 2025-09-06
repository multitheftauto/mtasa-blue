/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAudioContainerLookupTableSA.h
 *  PURPOSE:     Audio container lookup table reader
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CAudioContainerSA.h"

struct SAudioLookupEntrySA;

class CAudioContainerLookupTableSA
{
public:
    CAudioContainerLookupTableSA(const SString& strPath);
    ~CAudioContainerLookupTableSA();

    int                  CountIndex(eAudioLookupIndex index);
    SAudioLookupEntrySA* GetEntry(eAudioLookupIndex lookupIndex, uint8 bankIndex);

private:
    std::vector<SAudioLookupEntrySA*> m_Entries[9];
};

struct SAudioLookupEntrySA
{
    uint8  index;
    uint8  filter[3];
    uint32 offset;
    uint32 length;
};            // size = 12 = 0xC
static_assert(sizeof(SAudioLookupEntrySA) == 0xC, "Invalid size for SAudioLookupEntrySA");

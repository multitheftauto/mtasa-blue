/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAudioContainerLookupTableSA.h
 *  PURPOSE:     Audio container lookup table reader
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <fstream>

typedef unsigned char  uint8;
typedef unsigned short uint16;

struct SAudioLookupEntrySA;

class CAudioContainerLookupTableSA
{
public:
    CAudioContainerLookupTableSA(const SString& strPath);
    ~CAudioContainerLookupTableSA();

    int                  CountIndex(eAudioLookupIndex index);
    SAudioLookupEntrySA* GetEntry(eAudioLookupIndex lookupIndex, uint8 bankIndex);
    // SAudioLookupEntrySA* Next       ( bool goToNext );

private:
    std::vector<SAudioLookupEntrySA*> m_Entries[9];
    // std::vector<SAudioLookupEntrySA*>::iterator   m_CurrentEntry;
};

struct SAudioLookupEntrySA
{
    uint8  index;
    uint8  filter[3];
    uint32 offset;
    uint32 length;
};            // size = 12 = 0xC
static_assert(sizeof(SAudioLookupEntrySA) == 0xC, "Invalid size for SAudioLookupEntrySA");

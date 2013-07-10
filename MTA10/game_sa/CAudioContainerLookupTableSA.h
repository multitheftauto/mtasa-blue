/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioContainerLookupTableSA.h
*  PURPOSE:     Audio container lookup table reader
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*  NOTE:        Big thanks to pdescobar for making SAAT. Because of him I saved a lot of time with reversing
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAUDIOCONTAINERLOOKUPTABLESA_H
#define __CAUDIOCONTAINERLOOKUPTABLESA_H

#include <fstream>
#include <algorithm>

typedef unsigned char uint8;
typedef unsigned short uint16;

struct SAudioLookupEntrySA;

class CAudioContainerLookupTableSA
{
public:
    CAudioContainerLookupTableSA    ( const SString& strPath );
    ~CAudioContainerLookupTableSA   ( void );

    int CountIndex                  ( eAudioLookupIndex index );
    SAudioLookupEntrySA* GetEntry   ( eAudioLookupIndex lookupIndex, uint8 bankIndex );
    //SAudioLookupEntrySA* Next       ( bool goToNext );

private:
    std::vector<SAudioLookupEntrySA*>               m_Entries[9];
    //std::vector<SAudioLookupEntrySA*>::iterator   m_CurrentEntry;
};

struct SAudioLookupEntrySA
{
    uint8 index;
    uint8 filter[3];
    uint32 offset;
    uint32 length;
}; // size = 12 = 0xC

#endif

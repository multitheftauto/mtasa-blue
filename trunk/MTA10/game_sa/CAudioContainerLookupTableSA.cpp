/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioContainerLookupTableSA.h
*  PURPOSE:     Audio container lookup table reader
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CAudioContainerLookupTableSA.h"

CAudioContainerLookupTableSA::CAudioContainerLookupTableSA( const SString& strPath )
{
    std::ifstream fileHandle ( strPath, std::ios::binary );
    if ( !fileHandle )
        return;

    while (true)
    {
        SAudioLookupEntrySA* entry = new SAudioLookupEntrySA;
        if ( !fileHandle.read ( reinterpret_cast<char*> ( entry ), sizeof ( SAudioLookupEntrySA ) ) )
            break;

        m_Entries[entry->index].push_back ( entry );
    }

    fileHandle.close ();
}

CAudioContainerLookupTableSA::~CAudioContainerLookupTableSA(void)
{
    for ( int i = 0; i < 9; i++ )
    {
        for ( std::vector<SAudioLookupEntrySA*>::iterator it = m_Entries[i].begin (); it != m_Entries[i].end (); it++ )
        {
           if ( *it != NULL )
               delete *it;
        }
    }
}

int CAudioContainerLookupTableSA::CountIndex ( eAudioLookupIndex index )
{
    if ( index < 0 || index > 8 )
        return 0;

    return m_Entries[index].size ();
}

SAudioLookupEntrySA* CAudioContainerLookupTableSA::GetEntry ( eAudioLookupIndex lookupIndex, uint8 bankIndex )
{
    if ( lookupIndex < 0 || lookupIndex > 8 )
        return NULL;

    if ( bankIndex+1 > m_Entries[lookupIndex].size () )
        return NULL;

    return m_Entries[lookupIndex][bankIndex];
}

/*
SAudioLookupEntrySA* CAudioContainerLookupTableSA::Next ( bool goToNext )
{
    if ( goToNext )
        return *++m_CurrentEntry;

    return *m_CurrentEntry++;
}
*/

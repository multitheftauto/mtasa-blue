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

#include "StdInc.h"
#include <fstream>
#include "CAudioContainerLookupTableSA.h"

CAudioContainerLookupTableSA::CAudioContainerLookupTableSA(const SString& strPath)
{
    std::ifstream fileHandle(FromUTF8(strPath), std::ios::binary);

    if (!fileHandle)
        return;

    while (true)
    {
        SAudioLookupEntrySA* entry = new SAudioLookupEntrySA;

        if (!fileHandle.read(reinterpret_cast<char*>(entry), sizeof(SAudioLookupEntrySA)))
        {
            delete entry;
            break;
        }

        m_Entries[entry->index].push_back(entry);
    }

    fileHandle.close();
}

CAudioContainerLookupTableSA::~CAudioContainerLookupTableSA()
{
    for (int i = 0; i < 9; i++)
    {
        for (std::vector<SAudioLookupEntrySA*>::iterator it = m_Entries[i].begin(); it != m_Entries[i].end(); it++)
        {
            if (*it != nullptr)
                delete *it;
        }
    }
}

int CAudioContainerLookupTableSA::CountIndex(eAudioLookupIndex index)
{
    if (index < 0 || index > 8)
        return 0;

    return m_Entries[index].size();
}

SAudioLookupEntrySA* CAudioContainerLookupTableSA::GetEntry(eAudioLookupIndex lookupIndex, uint8 bankIndex)
{
    if (lookupIndex < 0 || lookupIndex > 8)
        return nullptr;

    if (static_cast<unsigned int>(bankIndex + 1) > m_Entries[lookupIndex].size())
        return nullptr;

    return m_Entries[lookupIndex][bankIndex];
}

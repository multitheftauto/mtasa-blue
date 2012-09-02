/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CHandlingEntry.cpp
*  PURPOSE:     Vehicle handling data entry
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CHandlingEntry::CHandlingEntry ( tHandlingData* pOriginal )
{
    if ( pOriginal )
    {
        // Copy the data from our original
        m_Handling = *pOriginal;
    }
}

// Apply the handling data from another data
void CHandlingEntry::ApplyHandlingData ( const CHandlingEntry* pData )
{
    // Copy the data from our handling entry
    m_Handling = pData->m_Handling;
}

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
    // Store gta's pointer
    memcpy ( &m_Handling, pOriginal, sizeof ( tHandlingData ) );
}


// Apply the handlingdata from another data
void CHandlingEntry::ApplyHandlingData ( CHandlingEntry* pData )
{
    // Copy the data
    CHandlingEntry* pEntry = static_cast < CHandlingEntry* > ( pData );
    m_Handling = pEntry->m_Handling;
}

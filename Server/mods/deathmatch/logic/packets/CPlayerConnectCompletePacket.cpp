/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerConnectCompletePacket.cpp
*  PURPOSE:     Player connect complete packet class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define MAX_CONN_TEXT_LEN 128

bool CPlayerConnectCompletePacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Send the connection string
    SString strConnText ( "%s %s [%s]", MTA_DM_FULL_STRING, MTA_DM_VERSIONSTRING, MTA_OS_STRING );
    BitStream.WriteString ( strConnText.Left ( MAX_CONN_TEXT_LEN ) );

    // Send the full server version
    BitStream.WriteString ( CStaticFunctionDefinitions::GetVersionSortable () );

    return true;
}

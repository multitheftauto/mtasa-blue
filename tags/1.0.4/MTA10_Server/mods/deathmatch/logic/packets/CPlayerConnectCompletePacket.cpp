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

CPlayerConnectCompletePacket::CPlayerConnectCompletePacket()
{
    _snprintf(m_szConnText, MAX_CONN_TEXT_LEN, "%s %s [%s]", MTA_DM_FULL_STRING, MTA_DM_VERSIONSTRING, MTA_OS_STRING);
}

IO_DECLARE_FUNCTION(CPlayerConnectCompletePacket,
{
    if (!IO_VARIABLE_STRING_MAX(m_szConnText, MAX_CONN_TEXT_LEN)) 
    { 
        IO_RAISE_ERROR(1, "Could not perform IO on connection text buffer."); 
        return false; 
    } 
 
    return true;
})

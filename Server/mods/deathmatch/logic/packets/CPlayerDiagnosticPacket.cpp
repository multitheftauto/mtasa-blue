/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerDiagnosticPacket.cpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerDiagnosticPacket.h"

namespace
{
    constexpr unsigned short     MAX_PLAYER_DIAGNOSTIC_PACKET_LENGTH = 1024;
    constexpr unsigned long long MAX_PLAYER_DIAGNOSTIC_LEVEL_VALUE = 0xFFFFFFFFULL;

    bool ParsePlayerDiagnosticLevel(const std::string& strLevel, uint& uiLevel)
    {
        if (strLevel.empty() || strLevel.length() > 10)
            return false;

        unsigned long long ullValue = 0;
        for (char c : strLevel)
        {
            if (c < '0' || c > '9')
                return false;

            ullValue = (ullValue * 10) + static_cast<unsigned long long>(c - '0');
            if (ullValue > MAX_PLAYER_DIAGNOSTIC_LEVEL_VALUE)
                return false;
        }

        uiLevel = static_cast<uint>(ullValue);
        return true;
    }
}

bool CPlayerDiagnosticPacket::Read(NetBitStreamInterface& BitStream)
{
    unsigned short usLength = 0;
    if (!BitStream.Read(usLength) || !BitStream.CanReadNumberOfBytes(usLength) || usLength > MAX_PLAYER_DIAGNOSTIC_PACKET_LENGTH)
        return false;

    m_uiLevel = 0;
    m_strMessage.clear();

    if (usLength == 0)
        return true;

    std::string strPayload(usLength, '\0');
    if (!BitStream.Read(&strPayload[0], usLength) || strPayload.find('\0') != std::string::npos)
        return false;

    const size_t uiCommaPos = strPayload.find(',');
    if (uiCommaPos == std::string::npos)
    {
        // No level prefix in the payload. Keep level at 0 and store the message as-is.
        m_strMessage = strPayload;
        return true;
    }

    if (uiCommaPos > 0)
        ParsePlayerDiagnosticLevel(strPayload.substr(0, uiCommaPos), m_uiLevel);

    if (uiCommaPos + 1 < strPayload.length())
        m_strMessage = strPayload.substr(uiCommaPos + 1);

    return true;
}

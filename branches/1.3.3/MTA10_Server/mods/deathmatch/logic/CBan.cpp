/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBan.cpp
*  PURPOSE:     Ban descriptor class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBan::CBan ( void )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::BAN );
    m_tTimeOfBan = 0;
    m_tTimeOfUnban = 0;
}

CBan::~CBan ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::BAN, m_uiScriptID );
}

SString CBan::GetDurationDesc( void )
{
    time_t Start = GetTimeOfBan ();
    time_t End = GetTimeOfUnban ();
    if ( End > Start )
    {
        time_t Duration = End - Start;
        int iDays = static_cast < int > ( Duration / 86400 );
        Duration = Duration % 86400;
        int iHours = static_cast < int > ( Duration / 3600 );
        Duration = Duration % 3600;
        int iMins = static_cast < int > ( Duration / 60 );

        if ( iDays )
            return SString ( "%d day%s", iDays, iDays > 1 ? "s" : "" );
        if ( iHours )
            return SString ( "%d hour%s", iHours, iHours > 1 ? "s" : "" );
        if ( iMins )
            return SString ( "%d min%s", iMins, iMins > 1 ? "s" : "" );
    }
    return "";
}


SString CBan::GetReasonText( void ) const
{
    // Remove after first '(' because of extra info added by admin panel
    size_t pos = m_strReason.find_first_of ( " (" );
    if ( pos != std::string::npos )
        return m_strReason.substr ( 0, pos );
    return m_strReason;
}

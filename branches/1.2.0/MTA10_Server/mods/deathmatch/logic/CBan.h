/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBan.h
*  PURPOSE:     Ban descriptor class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBAN_H
#define __CBAN_H

#include "CCommon.h"
#include <string>

class CBan
{
public:
                                CBan                    ( void ) ;

    inline const std::string&   GetIP                   ( void )                        { return m_strIP; };
    inline void                 SetIP                   ( const std::string& strIP )    { m_strIP = strIP; };

    inline const std::string&   GetNick                 ( void )                        { return m_strNick; };
    inline void                 SetNick                 ( const std::string& strNick )  { m_strNick = strNick; };

    inline const std::string&   GetBanner               ( void )                        { return m_strBanner; };
    void                        SetBanner               ( const std::string& strBanner ){ m_strBanner = strBanner; };

    inline const std::string&   GetReason               ( void )                        { return m_strReason; };
    inline void                 SetReason               ( const std::string& strReason ){ m_strReason = strReason; };

    inline const time_t         GetTimeOfBan            ( void )                        { return m_tTimeOfBan; };
    inline void                 SetTimeOfBan            ( time_t tTimeOfBan )           { m_tTimeOfBan = tTimeOfBan; };

    inline time_t               GetTimeOfUnban          ( void )                        { return m_tTimeOfUnban; };
    inline void                 SetTimeOfUnban          ( time_t tTimeOfUnban )         { m_tTimeOfUnban = tTimeOfUnban; };

    inline const std::string&   GetSerial               ( void )                        { return m_strSerial; };
    void                        SetSerial               ( const std::string& strSerial ){ m_strSerial = strSerial; };

    inline const std::string&   GetAccount              ( void )                        { return m_strAccount; };
    void                        SetAccount              ( const std::string& strAccount ){ m_strAccount = strAccount; };

    SString                     GetDurationDesc         ( void );
    SString                     GetReasonText           ( void ) const;

private:
    std::string                 m_strIP;
    std::string                 m_strNick;
    std::string                 m_strBanner;
    std::string                 m_strReason;
    std::string                 m_strSerial;
    std::string                 m_strAccount;
    time_t                      m_tTimeOfBan;
    time_t                      m_tTimeOfUnban;
};

#endif

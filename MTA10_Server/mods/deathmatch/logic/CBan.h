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

    inline  const std::string&  GetIP                   ( void )                        { return m_strIP; };
    inline void                 SetIP                   ( const std::string& strIP )    { m_strIP = strIP; };

    inline  const std::string&  GetNick                 ( void )                        { return m_strNick; };
    inline void                 SetNick                 ( const std::string& strNick )  { m_strNick = strNick; };

    inline const std::string&   GetBanner               ( void )                        { return m_strBanner; };
    void                        SetBanner               ( const std::string& strBanner ){ m_strBanner = strBanner; };

    inline const std::string&   GetReason               ( void )                        { return m_strReason; };
    inline void                 SetReason               ( const std::string& strReason ){ m_strReason = strReason; };

    inline const std::string&   GetDateOfBan            ( void )                        { return m_strDateOfBan; };
    inline void                 SetDateOfBan            ( const std::string& strDate )  { m_strDateOfBan = strDate; };

    inline const std::string&   GetTimeOfBan            ( void )                        { return m_strTimeOfBan; };
    inline void                 SetTimeOfBan            ( const std::string& strTime )  { m_strTimeOfBan = strTime; };

    inline time_t               GetTimeOfUnban          ( void )                        { return m_tTimeOfUnban; };
    inline void                 SetTimeOfUnban          ( time_t tTimeOfUnban )         { m_tTimeOfUnban = tTimeOfUnban; };

    inline const std::string&   GetSerial               ( void )                        { return m_strSerial; };
    void                        SetSerial               ( const std::string& strSerial ){ m_strSerial = strSerial; };

private:
    std::string                 m_strIP;
    std::string                 m_strNick;
    std::string                 m_strBanner;
    std::string                 m_strReason;
    std::string                 m_strDateOfBan;
    std::string                 m_strTimeOfBan;
    std::string                 m_strSerial;
    time_t                      m_tTimeOfUnban;

};

#endif

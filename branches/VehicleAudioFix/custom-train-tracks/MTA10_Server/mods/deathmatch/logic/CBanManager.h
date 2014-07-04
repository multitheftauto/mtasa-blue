/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBanManager.h
*  PURPOSE:     Ban manager class
*  DEVELOPERS:  Oliver Brown <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBANMANAGER_H
#define __CBANMANAGER_H

#include "CBan.h"
#include "CClient.h"
#include "CPlayerManager.h"

class CBanManager
{
public:
                        CBanManager             ( void );
    virtual             ~CBanManager            ( void );

    void                DoPulse                 ( void );

    CBan*               AddBan                  ( CPlayer* pPlayer, const SString& strBanner = "Console", const SString& strReason = "", time_t tTimeOfUnban = 0 );
    CBan*               AddBan                  ( const SString& strIP, const SString& strBanner = "Console", const SString& strReason = "", time_t tTimeOfUnban = 0 );

    CBan*               AddSerialBan            ( CPlayer* pPlayer, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0 );
    CBan*               AddSerialBan            ( const SString& strSerial, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0 );

    CBan*               AddAccountBan           ( CPlayer* pPlayer, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0 );
    CBan*               AddAccountBan           ( const SString& szAccount, CClient* pBanner = NULL, const SString& strReason = "", time_t tTimeOfUnban = 0 );

    CBan*               AddBan                  ( const SString& strBanner = "Console", const SString& strReason = "", time_t tTimeOfUnban = 0 );

    CBan*               GetBanFromScriptID      ( uint uiScriptID );

    bool                IsSpecificallyBanned    ( const char* szIP );
    bool                IsSerialBanned          ( const char* szSerial );
    bool                IsAccountBanned         ( const char* szAccount );
    CBan*               GetBanFromAccount       ( const char* szAccount );
    void                RemoveBan               ( CBan* pBan );
    void                RemoveAllBans           ( bool bPermanentDelete = false );

    CBan*               GetBan                  ( const char* szIP );
    CBan*               GetBan                  ( const char* szNick, unsigned int uiOccurrance );
    CBan*               GetBanFromSerial        ( const char* szSerial );
    CBan*               GetBanFromIP            ( const char* szIP );

    unsigned int        GetBansWithNick         ( const char* szNick );
    unsigned int        GetBansWithBanner       ( const char* szBanner );

    bool                LoadBanList             ( void );
    bool                ReloadBanList           ( void );
    void                SaveBanList             ( void );
    void                SafeSetValue            ( CXMLNode* pNode, const char* szKey, const std::string& strValue );
    void                SafeSetValue            ( CXMLNode* pNode, const char* szKey, unsigned int );
    std::string         SafeGetValue            ( CXMLNode* pNode, const char* szKey );
    bool                IsValidIP               ( const char* szIP );

    inline list < CBan* > ::const_iterator  IterBegin   ( void )                    { return m_BanManager.begin (); };
    inline list < CBan* > ::const_iterator  IterEnd     ( void )                    { return m_BanManager.end (); };

private:
    SString             m_strPath;

    CMappedList < CBan* >   m_BanManager;

    time_t              m_tUpdate;

    bool                IsValidIPPart               ( const char* szIP );
    bool                m_bAllowSave;
};

#endif

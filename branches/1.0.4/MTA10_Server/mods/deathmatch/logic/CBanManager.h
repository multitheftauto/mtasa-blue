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

    CBan*               AddBan                  ( CPlayer* pPlayer, CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );
    CBan*               AddBan                  ( const char* szIP, CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );

    CBan*               AddSerialBan            ( CPlayer* pPlayer, CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );
    CBan*               AddSerialBan            ( const char* szSerial, CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );

    CBan*               AddAccountBan           ( CPlayer* pPlayer, CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );
    CBan*               AddAccountBan           ( const char* szAccount, CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );

    CBan*               AddBan                  ( CClient* pBanner = NULL, const char* szReason = NULL, time_t tTimeOfUnban = 0 );

    bool                Exists                  ( CBan* pBan );

    bool                IsBanned                ( const char* szIP );
    bool                IsSpecificallyBanned    ( const char* szIP );
    bool                IsSerialBanned          ( const char* szSerial );
    bool                IsAccountBanned         ( const char* szAccount );
    void                RemoveBan               ( CBan* pBan );
    void                RemoveAllBans           ( bool bPermanentDelete = false );

    CBan*               GetBan                  ( const char* szIP );
    CBan*               GetBan                  ( const char* szNick, unsigned int uiOccurrance );
    CBan*               GetBanFromSerial        ( const char* szSerial );

    unsigned int        GetBansWithNick         ( const char* szNick );
    unsigned int        GetBansWithBanner       ( const char* szBanner );

    bool                LoadBanList             ( void );
    void                SaveBanList             ( void );
    void                SafeSetValue            ( CXMLNode* pNode, const char* szKey, const std::string& strValue );
    void                SafeSetValue            ( CXMLNode* pNode, const char* szKey, unsigned int );
    std::string         SafeGetValue            ( CXMLNode* pNode, const char* szKey );
    bool                IsValidIP               ( const char* szIP );

    inline list < CBan* > ::const_iterator  IterBegin   ( void )                    { return m_BanManager.begin (); };
    inline list < CBan* > ::const_iterator  IterEnd     ( void )                    { return m_BanManager.end (); };

private:
    SString             m_strPath;

    list < CBan* >      m_BanManager;

    time_t              m_tUpdate;

    bool                IsValidIPPart               ( const char* szIP );
    bool                m_bAllowSave;
};

#endif

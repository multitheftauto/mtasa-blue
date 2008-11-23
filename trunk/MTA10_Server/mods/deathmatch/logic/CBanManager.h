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

    CBan*               AddBan                  ( const char* szIP, bool bSaveList = true );
    CBan*               AddBan                  ( CPlayer* pPlayer, CClient* pBanner, const char* szReason = NULL, time_t tTimeOfUnban = 0 );
    CBan*               AddBan                  ( const char* szIP, CClient* pBanner, const char* szReason = NULL, time_t tTimeOfUnban = 0 );
	CBan*				AddBan					( const char* szIP, CPlayer* pPlayer );
	CBan*				AddSerialBan			( const char* szSerial );
	CBan*				AddSerialBan			( const char* szSerial, const char* szReason, CClient* pBanner );
	CBan*				AddSerialBan			( const char* szSerial, const char* szReason );
    bool                IsBanned                ( const char* szIP );
    bool                IsSpecificallyBanned    ( const char* szIP );
	bool				IsSerialBanned			( const char* szSerial );
    void                RemoveBan               ( CBan* pBan );
    void                RemoveBan               ( const char* szIP );
    void                RemoveAllBans           ( bool bPermanentDelete = false );

    CBan*               GetBan                  ( const char* szIP );
    CBan*               GetBan                  ( const char* szNick, unsigned int uiOccurrance );
	CBan*				GetSerialBan			( const char* szSerial );

    unsigned int        GetBansWithNick         ( const char* szNick );
    unsigned int        GetBansWithBanner       ( const char* szBanner );
    unsigned int        GetBansWithDate         ( const char* szDateOfBan );

    bool                LoadBanList             ( void );
    void                SaveBanList             ( void );
    bool                IsValidIP               ( const char* szIP );

	char*				GetFilePath				( void )			{ return reinterpret_cast <char*> (m_szPath); }

private:
    char                m_szFileName            [20];
    char                m_szPath                [MAX_PATH];

    list < CBan* >      m_BanManager;

};

#endif

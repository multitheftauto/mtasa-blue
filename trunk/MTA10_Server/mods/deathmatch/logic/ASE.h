/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/ASE.h
*  PURPOSE:     All-Seeing Eye server query protocol handler class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class ASE;

#ifndef __ASE_H__
#define __ASE_H__

#ifdef WIN32
	#include <conio.h>
	#include <winsock.h>
    #define sockclose closesocket
#else
	#include <sys/socket.h>
	#include <sys/stat.h>
	#include <netinet/in.h>
	#define sockclose close
#endif

#include <string.h>
#include <stdio.h>

#include "CMainConfig.h"
#include "CPlayerManager.h"
#include "CPlayer.h"

#include <list>

class CASERule;

class ASE
{
public:
	                        ASE                 ( CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, unsigned short usPort, const char* szServerIP = NULL, bool bLan = false );
	                        ~ASE                ( void );

	void                    DoPulse             ( void );

    static ASE*             GetInstance         ( void )                { return _instance; }

    std::string             QueryFull           ( void );
    std::string             QueryLight          ( void );

    CLanBroadcast*          InitLan             ( void );

    const char*             GetGameType         ( void )                { return m_strGameType.c_str(); }
    void                    SetGameType         ( const char * szGameType );
    const char*             GetMapName          ( void )                { return m_strMapName.c_str(); }
    void                    SetMapName          ( const char * szMapName );

    CMainConfig*            GetMainConfig       ( void )                { return m_pMainConfig; };
    CPlayerManager*         GetPlayerManager    ( void )                { return m_pPlayerManager; };

    char*                   GetRuleValue        ( char* szKey );
    void                    SetRuleValue        ( char* szKey, char* szValue );
    bool                    RemoveRuleValue     ( char* szKey );
    void                    ClearRules          ( void );

    list < CASERule* > ::iterator IterBegin     ( void )                { return m_Rules.begin (); }
    list < CASERule* > ::iterator IterEnd       ( void )                { return m_Rules.end (); }

private:

    CMainConfig*            m_pMainConfig;
    CPlayerManager*         m_pPlayerManager;

    std::string             m_strGameType;
    std::string             m_strMapName;
    std::string             m_strIP;
    std::string             m_strPort;

    static ASE*             _instance;

    list < CASERule* >      m_Rules;

    unsigned int			m_Socket;
    sockaddr_in             m_SockAddr;

    bool                    m_bLan;
    unsigned short          m_usPort;

protected:
	void                    GetStatusVals();

};

class CASERule
{
public:
    inline              CASERule        ( char* szKey, char* szValue )
    {
        m_szKey = NULL;
        m_szValue = NULL;
        SetKey ( szKey );
        SetValue ( szValue );
    }
    inline              ~CASERule       ( void )
    {
        delete [] m_szKey;
        delete [] m_szValue;
    }
    inline char*        GetKey          ( void )            { return m_szKey; }
    inline void         SetKey          ( char* szKey )
    {
        if ( m_szKey )
        {
            delete [] m_szKey;
            m_szKey = NULL;
        }
        if ( szKey )
        {
            m_szKey = new char [ strlen ( szKey ) + 1 ];
            strcpy ( m_szKey, szKey );
        }
    }
    inline char*        GetValue        ( void )            { return m_szValue; }
    inline void         SetValue          ( char* szValue )
    {
        if ( m_szValue )
        {
            delete [] m_szValue;
            m_szValue = NULL;
        }
        if ( szValue )
        {
            m_szValue = new char [ strlen ( szValue ) + 1 ];
            strcpy ( m_szValue, szValue );
        }
    }
private:
    char*               m_szKey;
    char*               m_szValue;
};

#endif

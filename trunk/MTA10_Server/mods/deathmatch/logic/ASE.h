/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/ASE.h
*  PURPOSE:     All-Seeing Eye server query protocol handler class
*  DEVELOPERS:  Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class ASE;

#ifndef __ASE_H__
#define __ASE_H__

#ifdef WIN32
#include <winsock.h>
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
	                        ASE                 ( CMainConfig* pMainConfig, CPlayerManager* pPlayerManager, int iPort, const char* lpszServerIP = NULL );
	                        ~ASE                ( void );

	void                    DoPulse             ( void );

    inline static ASE*      GetInstance         ( void )                { return _instance; }
    
    inline char*            GetGameType         ( void )                { return m_szGameType; }
    inline void             SetGameType         ( const char * szGameType );
    inline char*            GetMapName          ( void )                { return m_szMapName; }
    inline void             SetMapName          ( const char * szMapName );

    inline CMainConfig*     GetMainConfig       ( void )                { return m_pMainConfig; };
    inline CPlayerManager*  GetPlayerManager    ( void )                { return m_pPlayerManager; };

    char*                   GetRuleValue        ( char* szKey );
    void                    SetRuleValue        ( char* szKey, char* szValue );
    bool                    RemoveRuleValue     ( char* szKey );
    void                    ClearRules          ( void );

    list < CASERule* > ::iterator IterBegin     ( void )                { return m_Rules.begin (); }
    list < CASERule* > ::iterator IterEnd       ( void )                { return m_Rules.end (); }
private:
    CMainConfig*            m_pMainConfig;
    CPlayerManager*         m_pPlayerManager;
    char                    m_szGameType [64];
    char                    m_szMapName [64];

    static ASE*             _instance;

    list < CASERule* >      m_Rules;

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

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccountManager.h
*  PURPOSE:     User account manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CAccountManager;

#ifndef __CACCOUNTMANAGER_H
#define __CACCOUNTMANAGER_H

#include "CAccount.h"
#include "CXMLConfig.h"

class CAccountManager: public CXMLConfig
{
    friend class CAccount;
public:
                                CAccountManager             ( char* szFileName );
                                ~CAccountManager            ( void );

    void                        DoPulse                     ( void );

    bool                        Load                        ( const char* szFileName = NULL );
    bool                        Load                        ( CXMLNode* pParent );
    bool                        LoadSetting                 ( CXMLNode* pNode );
    bool                        Save                        ( const char* szFileName = NULL );
    bool                        Save                        ( CXMLNode* pParent );
    bool                        SaveSettings                ( CXMLNode* pParent );

    CAccount*                   Get                         ( const char* szName, bool bRegistered = true );
    CAccount*                   Get                         ( const char* szName, const char* szIP );
    bool                        Exists                      ( CAccount* pAccount );
    bool                        LogIn                       ( CClient* pClient, CClient* pEchoClient, const char* szNick, const char* szPassword );
    bool                        LogIn                       ( CClient* pClient, CClient* pEchoClient, CAccount* pAccount, bool bAutoLogin = false );
    bool                        LogOut                      ( CClient* pClient, CClient* pEchoClient );

    inline bool                 IsAutoLoginEnabled          ( void )                    { return m_bAutoLogin; }
    inline bool                 SetAutoLoginEnabled         ( bool bEnabled )           { m_bAutoLogin = bEnabled; return bEnabled; }

protected:
    inline void                 AddToList                   ( CAccount* pAccount )      { m_List.push_back ( pAccount ); }
    void                        RemoveFromList              ( CAccount* pAccount );

    void                        MarkAsChanged               ( CAccount* pAccount );

public:
    void                        RemoveAll                   ( void );

    inline list < CAccount* > ::const_iterator  IterBegin   ( void )                    { return m_List.begin (); };
    inline list < CAccount* > ::const_iterator  IterEnd     ( void )                    { return m_List.end (); };

protected:
    list < CAccount* >          m_List;
    bool                        m_bRemoveFromList;

    bool                        m_bAutoLogin;

    bool                        m_bChangedSinceSaved;
    long long                   m_llLastTimeSaved;
    CConnectHistory             m_AccountProtect;
};

#endif

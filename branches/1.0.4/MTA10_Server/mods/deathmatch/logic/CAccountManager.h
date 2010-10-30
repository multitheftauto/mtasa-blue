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

//
// CAccountList
//
// Combination list and map - For fast list Contains() method
//
class CAccountList
{
    list < CAccount* >          m_List;
    std::map < CAccount*, int > m_Map;
public:

    // list only
    list < CAccount* > ::iterator  begin ( void )   { return m_List.begin (); }
    list < CAccount* > ::iterator  end ( void )     { return m_List.end (); }

    // map only
    bool Contains ( CAccount* pAccount ) const      { return MapContains ( m_Map, pAccount ); }

    // list and map
    void push_back ( CAccount* pAccount )
    {
        m_List.push_back ( pAccount );
        MapSet ( m_Map, pAccount, 1 );
    }

    void remove ( CAccount* pAccount )
    {
        m_List.remove ( pAccount );
        MapRemove ( m_Map, pAccount );
    }

    void clear ( void )
    {
        m_List.clear ();
        m_Map.clear ();
    }
};


//
// CAccountManager
//
class CAccountManager: public CXMLConfig
{
    friend class CAccount;
public:
                                CAccountManager             ( char* szFileName, SString strBuffer );
                                ~CAccountManager            ( void );

    void                        DoPulse                     ( void );

    bool                        Load                        ( const char* szFileName = NULL );
    bool                        Load                        ( CXMLNode* pParent );
    bool                        LoadSetting                 ( CXMLNode* pNode );
    bool                        Save                        ( const char* szFileName = NULL );
    bool                        Save                        ( CXMLNode* pParent );
    void                        Save                        ( CAccount* pParent );
    bool                        SaveSettings                ( void );

    CAccount*                   Get                         ( const char* szName, bool bRegistered = true );
    CAccount*                   Get                         ( const char* szName, const char* szIP );
    bool                        Exists                      ( CAccount* pAccount );
    bool                        LogIn                       ( CClient* pClient, CClient* pEchoClient, const char* szNick, const char* szPassword );
    bool                        LogIn                       ( CClient* pClient, CClient* pEchoClient, CAccount* pAccount, bool bAutoLogin = false );
    bool                        LogOut                      ( CClient* pClient, CClient* pEchoClient );

    inline bool                 IsAutoLoginEnabled          ( void )                    { return m_bAutoLogin; }
    inline bool                 SetAutoLoginEnabled         ( bool bEnabled )           { m_bAutoLogin = bEnabled; SaveSettings(); return bEnabled; }

    CLuaArgument*               GetAccountData              ( CAccount* pAccount, char* szKey );
    bool                        SetAccountData              ( CAccount* pAccount, char* szKey, SString strValue, int iType );
    bool                        CopyAccountData             ( CAccount* pFromAccount, CAccount* pToAccount );

    bool                        ConvertXMLToSQL             ( const char* szFileName );
    bool                        LoadXML                     ( CXMLNode* pParent );
    void                        SmartLoad                   ( void );
    void                        Register                    ( CAccount* pAccount );
    void                        RemoveAccount               ( CAccount* pAccount );
protected:
    inline void                 AddToList                   ( CAccount* pAccount )      { m_List.push_back ( pAccount ); }
    void                        RemoveFromList              ( CAccount* pAccount );

    void                        MarkAsChanged               ( CAccount* pAccount );
    void                        ClearSQLDatabase            ( void );
public:
    void                        RemoveAll                   ( void );

    inline list < CAccount* > ::const_iterator  IterBegin   ( void )                    { return m_List.begin (); };
    inline list < CAccount* > ::const_iterator  IterEnd     ( void )                    { return m_List.end (); };

protected:
    CAccountList                m_List;
    bool                        m_bRemoveFromList;

    bool                        m_bAutoLogin;

    bool                        m_bChangedSinceSaved;
    long long                   m_llLastTimeSaved;
    CConnectHistory             m_AccountProtect;
    CRegistry*                  m_pSaveFile;
    bool                        m_bLoadXML;
    int                         m_iAccounts;
};

#endif

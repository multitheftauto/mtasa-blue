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
typedef uint SDbConnectionId;

#define GUEST_ACCOUNT_NAME          "guest"
#define HTTP_GUEST_ACCOUNT_NAME     "http_guest"
#define CONSOLE_ACCOUNT_NAME        "Console"
#define CONSOLE_NICK_NAME           "Console"

//
// CFastList with additional name->account mapping
//
class CMappedAccountList : protected CFastList < CAccount* >
{
public:
    typedef CFastList < CAccount* > Super;
    typedef CFastList < CAccount* >::iterator        iterator;
    typedef CFastList < CAccount* >::const_iterator  const_iterator;

    // CMappedList functions
    bool        contains ( CAccount* item ) const   { return Super::contains ( item ); }
    iterator    begin ( void )                      { return Super::begin (); }
    iterator    end ( void )                        { return Super::end (); }

    void push_back ( CAccount* item )
    {
        assert ( !MapContainsPair( m_NameAccountMap, item->GetName (), item ) );
        MapInsert( m_NameAccountMap, item->GetName (), item );
        Super::push_back ( item );
        assert ( m_NameAccountMap.size () == size () );
    }

    void remove ( CAccount* item )
    {
        MapRemovePair( m_NameAccountMap, item->GetName (), item );
        Super::remove ( item );
        assert ( m_NameAccountMap.size () == size () );
    }

    void clear ( void )
    {
        assert ( m_NameAccountMap.size () == size () );
        m_NameAccountMap.clear ();
        Super::clear ();
    }

    size_t size ( void ) const
    {
        return Super::size ();
    }

    // Account functions
    void FindAccountMatches ( std::vector < CAccount* >* pOutResults, const SString& strName )
    {
        MultiFind ( m_NameAccountMap, strName, pOutResults );
    }

    void ChangingName ( CAccount* pAccount, const SString& strOldName, const SString& strNewName )
    {
        if ( MapContainsPair ( m_NameAccountMap, strOldName, pAccount ) )
        {
            MapRemovePair ( m_NameAccountMap, strOldName, pAccount );
            assert ( !MapContainsPair ( m_NameAccountMap, strNewName, pAccount ) );
            MapInsert( m_NameAccountMap, strNewName, pAccount );
        }
    }

protected:
    std::multimap < SString, CAccount* > m_NameAccountMap;
};

// Returns true if the item is in the itemList
template < class T >
bool ListContains ( const CMappedAccountList& itemList, const T& item )
{
    return itemList.contains ( item );
}


//
// CAccountManager
//
class CAccountManager: public CXMLConfig
{
    friend class CAccount;
public:
                                CAccountManager             ( const char* szFileName, SString strBuffer );
                                ~CAccountManager            ( void );

    void                        DoPulse                     ( void );

    bool                        Load                        ( void );
    bool                        Load                        ( CXMLNode* pParent );
    bool                        LoadSetting                 ( CXMLNode* pNode );
    bool                        Save                        ( void );
    bool                        Save                        ( CXMLNode* pParent );
    void                        Save                        ( CAccount* pParent, bool bCheckForErrors = true );

    bool                        SaveSettings                ( void );
    bool                        IntegrityCheck              ( void );

    CAccount*                   Get                         ( const char* szName, bool bRegistered = true );
    CAccount*                   Get                         ( const char* szName, const char* szIP );
    CAccount*                   GetAccountFromScriptID      ( uint uiScriptID );

    bool                        LogIn                       ( CClient* pClient, CClient* pEchoClient, const char* szNick, const char* szPassword );
    bool                        LogIn                       ( CClient* pClient, CClient* pEchoClient, CAccount* pAccount, bool bAutoLogin = false );
    bool                        LogOut                      ( CClient* pClient, CClient* pEchoClient );

    inline bool                 IsAutoLoginEnabled          ( void )                    { return m_bAutoLogin; }
    inline void                 SetAutoLoginEnabled         ( bool bEnabled )           { m_bAutoLogin = bEnabled; }

    CLuaArgument*               GetAccountData              ( CAccount* pAccount, const char* szKey );
    bool                        SetAccountData              ( CAccount* pAccount, const char* szKey, const SString& strValue, int iType );
    bool                        CopyAccountData             ( CAccount* pFromAccount, CAccount* pToAccount );
    bool                        GetAllAccountData           ( CAccount* pAccount, lua_State* pLua );

    bool                        ConvertXMLToSQL             ( const char* szFileName );
    bool                        LoadXML                     ( CXMLNode* pParent );
    void                        SmartLoad                   ( void );
    void                        Register                    ( CAccount* pAccount );
    void                        RemoveAccount               ( CAccount* pAccount );
protected:
    void                        AddToList                   ( CAccount* pAccount )      { m_List.push_back ( pAccount ); }
    void                        RemoveFromList              ( CAccount* pAccount );

    void                        MarkAsChanged               ( CAccount* pAccount );
    void                        ChangingName                ( CAccount* pAccount, const SString& strOldName, const SString& strNewName );
    void                        ClearSQLDatabase            ( void );
public:
    void                        RemoveAll                   ( void );
    static void                 StaticDbCallback            ( CDbJobData* pJobData, void* pContext );
    void                        DbCallback                  ( CDbJobData* pJobData );

    CMappedAccountList::const_iterator  IterBegin           ( void )                    { return m_List.begin (); };
    CMappedAccountList::const_iterator  IterEnd             ( void )                    { return m_List.end (); };

protected:
    CMappedAccountList          m_List;

    bool                        m_bAutoLogin;

    bool                        m_bChangedSinceSaved;
    long long                   m_llLastTimeSaved;
    CConnectHistory             m_AccountProtect;
    SDbConnectionId             m_hDbConnection;
    CDatabaseManager*           m_pDatabaseManager;
    bool                        m_bLoadXML;
    int                         m_iAccounts;
};

#endif

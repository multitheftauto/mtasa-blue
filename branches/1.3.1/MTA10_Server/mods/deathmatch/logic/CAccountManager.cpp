/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAccountManager.cpp
*  PURPOSE:     User account manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CAccountManager::CAccountManager ( const char* szFileName, SString strBuffer ): CXMLConfig ( szFileName )
    , m_AccountProtect( 6, 30000, 60000 * 1 )     // Max of 6 attempts per 30 seconds, then 1 minute ignore
{
    m_bAutoLogin = false;
    m_llLastTimeSaved = GetTickCount64_ ();
    m_bChangedSinceSaved = false;
    //set loadXML to false
    m_bLoadXML = false;
    m_iAccounts = 1;

    //Load internal.db
    m_pDatabaseManager = g_pGame->GetDatabaseManager ();
    m_hDbConnection = m_pDatabaseManager->Connect ( "sqlite", PathConform ( strBuffer ) );

    // Check if new installation
    CRegistryResult result;
	m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT name FROM sqlite_master WHERE type='table' AND name='accounts'" );
    bool bNewInstallation = ( result.nRows == 0 );

    //Create all our tables (Don't echo the results)
    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, name TEXT, password TEXT, ip TEXT, serial TEXT)" );
    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE TABLE IF NOT EXISTS userdata (id INTEGER PRIMARY KEY, userid INTEGER, key TEXT, value TEXT, type INTEGER)" );
    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE TABLE IF NOT EXISTS settings (id INTEGER PRIMARY KEY, key TEXT, value INTEGER)" );

    // Check if unique index on accounts exists
	m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT name FROM sqlite_master WHERE type='index' AND name='IDX_ACCOUNTS_NAME_U'" );
    if ( result.nRows == 0 )
    {
        // Need to add unique index on accounts
        if ( !bNewInstallation )
            CLogger::LogPrintNoStamp ( "Updating accounts table...\n" );

        // Make sure we have a non-unique index to speed up the duplication removal
	    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE INDEX IF NOT EXISTS IDX_ACCOUNTS_NAME on accounts(name)" );
        // Remove any duplicate name entries
	    m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM accounts WHERE rowid in "
						                                " (SELECT A.rowid"
						                                " FROM accounts A, accounts B"
						                                " WHERE A.rowid > B.rowid AND A.name = B.name)" );
        // Remove non-unique index
        m_pDatabaseManager->Execf ( m_hDbConnection, "DROP INDEX IF EXISTS IDX_ACCOUNTS_NAME" );
        // Add unique index
	    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE UNIQUE INDEX IF NOT EXISTS IDX_ACCOUNTS_NAME_U on accounts(name)" );
    }

    // Check if unique index on userdata exists
	m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT name FROM sqlite_master WHERE type='index' AND name='IDX_USERDATA_USERID_KEY_U'" );
    if ( result.nRows == 0 )
    {
        // Need to add unique index on userdata
        if ( !bNewInstallation )
            CLogger::LogPrintNoStamp ( "Updating userdata table...\n" );

        // Make sure we have a non-unique index to speed up the duplication removal
	    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE INDEX IF NOT EXISTS IDX_USERDATA_USERID_KEY on userdata(userid,key)" );
        // Remove any duplicate userid+key entries
	    m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM userdata WHERE rowid in "
						                                " (SELECT A.rowid"
						                                " FROM userdata A, userdata B"
						                                " WHERE A.rowid > B.rowid AND A.userid = B.userid AND A.key = B.key)" );
        // Remove non-unique index
        m_pDatabaseManager->Execf ( m_hDbConnection, "DROP INDEX IF EXISTS IDX_USERDATA_USERID_KEY" );
        // Add unique index
	    m_pDatabaseManager->Execf ( m_hDbConnection, "CREATE UNIQUE INDEX IF NOT EXISTS IDX_USERDATA_USERID_KEY_U on userdata(userid,key)" );
    }
    
    // Ensure old indexes are removed
    m_pDatabaseManager->Execf ( m_hDbConnection, "DROP INDEX IF EXISTS IDX_ACCOUNTS_NAME" );
    m_pDatabaseManager->Execf ( m_hDbConnection, "DROP INDEX IF EXISTS IDX_USERDATA_USERID" );
    m_pDatabaseManager->Execf ( m_hDbConnection, "DROP INDEX IF EXISTS IDX_USERDATA_USERID_KEY" );

    //Pull our settings
    m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT key, value from settings" );

    //Did we get any results
    if ( result.nRows == 0 )
    {
        //Set our settings and clear the accounts/userdata tables just in case
        m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO settings (key, value) VALUES(?,?)", SQLITE_TEXT, "XMLParsed", SQLITE_INTEGER, 0 );
        //Tell the Server to load the xml file rather than the SQL
        m_bLoadXML = true;
    }
    else
    {
        bool bLoadXMLMissing = true;
        for (int i = 0;i < result.nRows;i++) 
        {
            SString strSetting = (const char *)result.Data[i][0].pVal;

            //Do we have a result for XMLParsed
            if ( strSetting == "XMLParsed" ) 
            {
                //Is XMLParsed zero
                if ( result.Data[i][1].nVal == 0 ) 
                {
                    //Tell the Server to load the xml file rather than the SQL
                    m_bLoadXML = true;
                }
                bLoadXMLMissing = false;
            }
        }
        //if we didn't load the XMLParsed variable
        if ( bLoadXMLMissing )
        {
            //Insert it
            m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO settings (key, value) VALUES(?,?)", SQLITE_TEXT, "XMLParsed", SQLITE_INTEGER, 0 );
            //Tell the Server to load the xml file rather than the SQL
            m_bLoadXML = true;
        }
    }

    //Check whether autologin was enabled in the main config
    m_bAutoLogin = g_pGame->GetConfig()->IsAutoLoginEnabled();
}
void CAccountManager::ClearSQLDatabase ( void )
{    
    //No settings file or server owner wants to reload from the accounts file
    //Clear the accounts and userdata tables
    m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE from accounts" );
    m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE from userdata");
}

CAccountManager::~CAccountManager ( void )
{
    //Save everything
    Save ();
    //Delete our save file
    m_pDatabaseManager->Disconnect ( m_hDbConnection );
    RemoveAll ();
}


void CAccountManager::DoPulse ( void )
{
    // Save it only once in a while whenever something has changed
    if ( m_bChangedSinceSaved &&
         GetTickCount64_ () > m_llLastTimeSaved + 15000 )
    {
        // Save it
        Save ();
    }
}

bool CAccountManager::ConvertXMLToSQL ( const char* szFileName )
{
    //##Keep for backwards compatability with accounts.xml##
    if ( szFileName == NULL )
        szFileName = m_strFileName.c_str ();

    if ( szFileName && szFileName [ 0 ] )
    {
        // Delete existing XML
        if ( m_pFile )
        {
            delete m_pFile;
        }

        // Create new one
        m_pFile = g_pServerInterface->GetXML ()->CreateXML ( szFileName );
        if ( m_pFile )
        {
            if ( m_pFile->Parse () )
            {
                CXMLNode* pRootNode = m_pFile->GetRootNode ();
                if ( pRootNode )
                {
                    ClearSQLDatabase();
                    return LoadXML ( pRootNode );
                }
            }
            else
            {
                //Save the settings to SQL
                SaveSettings();
                if ( FileExists ( szFileName ) )
                    CLogger::LogPrint ( "Conversion Failed: 'accounts.xml' failed to load.\n" );
                //Add Console to the SQL Database (You don't need to create an account since the server takes care of that (Have to do this here or Console may be created after other accounts if the owner uses addaccount too early)
                m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO accounts (name, password) VALUES(?,?)", SQLITE_TEXT, "Console", SQLITE_TEXT, "" );
                ++m_iAccounts;
            }
        }
    }
    return false;
}


bool CAccountManager::LoadXML ( CXMLNode* pParent )
{
    CLogger::LogPrint ( "Converting Accounts.xml into internal.db\n" );

    //##Keep for backwards compatability with accounts.xml##
    #define ACCOUNT_VALUE_LENGTH 128

    std::string strBuffer, strName, strPassword, strLevel, strIP, strDataKey, strDataValue;

    if ( pParent )
    {
        CXMLNode* pAccountNode = NULL;
        unsigned int uiAccountNodesCount = pParent->GetSubNodeCount ();
        for ( unsigned int i = 0 ; i < uiAccountNodesCount ; i++ )
        {
            pAccountNode = pParent->GetSubNode ( i );
            if ( pAccountNode == NULL )
                continue;

            strBuffer = pAccountNode->GetTagName ();
            if ( strBuffer.compare ( "account" ) == 0 )
            {
                CXMLAttribute* pAttribute = pAccountNode->GetAttributes ().Find ( "name" );
                if ( pAttribute )
                {
                    strName = pAttribute->GetValue ();

                    pAttribute = pAccountNode->GetAttributes ().Find ( "password" );
                    if ( pAttribute )
                    {
                        strPassword = pAttribute->GetValue ();
                        if ( !strName.empty () && !strPassword.empty () )
                        {
                            pAttribute = pAccountNode->GetAttributes ().Find ( "ip" );
                            if ( pAttribute )
                            {
                                strIP = pAttribute->GetValue ();
                                CAccount* pAccount = NULL;
                                pAttribute = pAccountNode->GetAttributes ().Find ( "serial" );
                                if ( pAttribute )
                                {
                                    //Insert the entry into the accounts database
                                    m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO accounts (name, ip, serial, password) VALUES(?,?,?,?)", SQLITE_TEXT, strName.c_str(), SQLITE_TEXT, strIP.c_str(), SQLITE_TEXT, pAttribute->GetValue ().c_str(), SQLITE_TEXT, strPassword.c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, strIP, m_iAccounts++, pAttribute->GetValue () );
                                
                                }
                                else
                                {
                                    //Insert the entry into the accounts database
                                    m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO accounts (name, ip, password) VALUES(?,?,?)", SQLITE_TEXT, strName.c_str(), SQLITE_TEXT, strIP.c_str(), SQLITE_TEXT, strPassword.c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, strIP, m_iAccounts++ );
                                }

                                // Grab the data on this account
                                CXMLNode* pDataNode = NULL;
                                int iType = LUA_TNIL;
                                unsigned int uiDataNodesCount = pAccountNode->GetSubNodeCount ();
                                for ( unsigned int j = 0 ; j < uiDataNodesCount ; j++ )
                                {
                                    pDataNode = pAccountNode->GetSubNode ( j );
                                    if ( pDataNode == NULL )
                                        continue;
                                    strBuffer = pDataNode->GetTagName ();
                                    if ( strBuffer == "nil_data" )
                                        iType = LUA_TNIL;
                                    else if ( strBuffer == "boolean_data" )
                                        iType = LUA_TBOOLEAN;
                                    else if ( strBuffer == "string_data" )
                                        iType = LUA_TSTRING;
                                    else if ( strBuffer == "number_data" )
                                        iType = LUA_TNUMBER;

                                    CXMLAttributes* pAttributes = &(pDataNode->GetAttributes ());
                                    CXMLAttribute* pAttribute = NULL;
                                    unsigned int uiDataValuesCount = pAttributes->Count ();
                                    for ( unsigned int a = 0 ; a < uiDataValuesCount ; a++ )
                                    {
                                        pAttribute = pAttributes->Get ( a );
                                        strDataKey = pAttribute->GetName ();
                                        strDataValue = pAttribute->GetValue ();
                                        char szKey[128];
                                        STRNCPY( szKey, strDataKey.c_str(), 128 );
                                        SetAccountData( pAccount, szKey, strDataValue, iType );
                                    }
                                }
                            }
                            else
                            {
                                CAccount* pAccount = NULL;
                                pAttribute = pAccountNode->GetAttributes ().Find ( "serial" );
                                if ( pAttribute )
                                {
                                    //Insert the entry into the accounts database
                                    m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO accounts (name, password, serial) VALUES(?,?,?)", SQLITE_TEXT, strName.c_str(), SQLITE_TEXT, strPassword.c_str(), SQLITE_TEXT, pAttribute->GetValue().c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, "", m_iAccounts++, pAttribute->GetValue () );
                                }
                                else
                                {
                                    //Insert the entry into the accounts database
                                    m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO accounts (name, password) VALUES(?,?)", SQLITE_TEXT, strName.c_str(), SQLITE_TEXT, strPassword.c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, "", m_iAccounts++, "" );
                                }
                            }
                        }
                        else
                        {
                            if ( strName == CONSOLE_ACCOUNT_NAME )
                            {
                                //Add Console to the SQL Database (You don't need to create an account since the server takes care of that
                                m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO accounts (name, password) VALUES(?,?)", SQLITE_TEXT, "Console", SQLITE_TEXT, "" );
                                ++m_iAccounts;
                            }
                        }
                    }
                }
            }
            else
            {
                //Load the settings from XML
                LoadSetting ( pAccountNode );
            }
        }
        //Save the settings to SQL
        SaveSettings();
        CLogger::LogPrint ( "Conversion Complete.\n" );
        m_bChangedSinceSaved = false;
        return true;
    }

    return false;
}


bool CAccountManager::Load( void )
{
    //Create a registry result
    CRegistryResult result;
    //Select all our required information from the accounts database
    m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT id,name,password,ip,serial from accounts" );

    //Work out how many rows we have
    int iResults = result.nRows;
    //Initialize all our variables
    SString strName, strPassword, strSerial, strIP;
    int iUserID = 0;
    m_iAccounts = 0;
    bool bNeedsVacuum = false;
    CAccount* pAccount = NULL;
    for ( int i = 0 ; i < iResults ; i++ )
    {
        //Fill User ID, Name & Password (Required data)
        iUserID = static_cast < int > ( result.Data[i][0].nVal );
        strName = (const char *)result.Data[i][1].pVal;

        // Check for overlong names and incorrect escapement
        bool bChanged = false;
        if ( strName.length () > 64 )
        {
            // Try to repair name
            if ( strName.length () <= 256 )
            {
                strName = strName.Replace ( "\"\"", "\"", true ).substr ( 0, 64 );
                bChanged = true;
            }

            // If name gone doolally or account with this name already exists, remove account
            if ( strName.length () > 256 || Get ( strName, true ) )
            {
                m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM accounts WHERE id=?", SQLITE_INTEGER, iUserID );
                m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM userdata WHERE userid=?", SQLITE_INTEGER, iUserID );
                bNeedsVacuum = true;
                CLogger::LogPrintf ( "Removed duplicate or damaged account for %s\n", strName.substr ( 0, 64 ).c_str() );
                continue;
            }
        }

        strPassword = "";
        // If we have an password
        if ( result.Data[i][2].pVal )
            strPassword = (const char *)result.Data[i][2].pVal;

        //if we have an IP
        if ( result.Data[i][3].pVal ) {
            //Fill the IP variable
            strIP = (const char *)result.Data[i][3].pVal;
            //If we have a Serial
            if ( result.Data[i][4].pVal ) {
                //Fill the serial variable
                strSerial = (const char *)result.Data[i][4].pVal;
                //Create a new account with the specified information
                pAccount = new CAccount ( this, true, strName, strPassword, strIP, iUserID, strSerial );
            }
            else
                //Create a new account with the specified information
                pAccount = new CAccount ( this, true, strName, strPassword, strIP, iUserID );
        }
        else {
            //Create a new account with the specified information
            pAccount = new CAccount ( this, true, strName, strPassword, "", iUserID );
        }
        if ( bChanged )
            pAccount->SetChanged ( bChanged );
        m_iAccounts = Max ( m_iAccounts, iUserID );
    }
    if ( bNeedsVacuum )
        m_pDatabaseManager->Execf ( m_hDbConnection, "VACUUM" );
    return true;
}


bool CAccountManager::LoadSetting ( CXMLNode* pNode )
{
    //##Keep for backwards compatability with accounts.xml##
    if ( pNode->GetTagName ().compare ( "autologin" ) == 0 )
    {
        bool bTemp;
        if ( pNode->GetTagContent ( bTemp ) )
        {
            m_bAutoLogin = bTemp;
        }
    }
    else
        return false;

    return true;
}


void CAccountManager::Save ( CAccount* pAccount )
{
    SString strName = pAccount->GetName();
    SString strPassword = pAccount->GetPasswordHash();
    SString strIP = pAccount->GetIP();
    SString strSerial = pAccount->GetSerial();

    m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT OR IGNORE INTO accounts (name, ip, serial, password) VALUES(?,?,?,?)", SQLITE_TEXT, strName.c_str (), SQLITE_TEXT, strIP.c_str (), SQLITE_TEXT, strSerial.c_str (), SQLITE_TEXT, strPassword.c_str () );

    if ( strSerial != "" )
        m_pDatabaseManager->QueryWithCallbackf ( m_hDbConnection, StaticDbCallback, this, "UPDATE accounts SET ip=?, serial=?, password=? WHERE name=?", SQLITE_TEXT, strIP.c_str (), SQLITE_TEXT, strSerial.c_str (), SQLITE_TEXT, strPassword.c_str (), SQLITE_TEXT, strName.c_str () );
    else
        //If we don't have a serial then IP and password will suffice
        m_pDatabaseManager->QueryWithCallbackf ( m_hDbConnection, StaticDbCallback, this, "UPDATE accounts SET ip=?, password=? WHERE name=?", SQLITE_TEXT, strIP.c_str (), SQLITE_TEXT, strPassword.c_str (), SQLITE_TEXT, strName.c_str () );

    //Set changed since saved to false
    pAccount->SetChanged( false );
}


bool CAccountManager::Save ( void )
{
    // Attempted save now
    m_bChangedSinceSaved = false;

    CMappedAccountList::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
        if ( (*iter)->IsRegistered () && (*iter)->HasChanged() )
            Save ( *iter );

    return true;
}


bool CAccountManager::SaveSettings ()
{
    //Update our XML Load SQL entry
    m_pDatabaseManager->Execf ( m_hDbConnection, "UPDATE settings SET value=? WHERE key=?", SQLITE_INTEGER, 1, SQLITE_TEXT, "XMLParsed" );
    return true;
}


bool CAccountManager::IntegrityCheck ()
{
    CRegistryResult result;
    //Select all our required information from the accounts database
    bool bOk = m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "PRAGMA integrity_check" );

    // Get result as a string
    SString strResult;
    if ( result.nRows && result.nColumns )
    {
        CRegistryResultCell& cell = result.Data[0][0];
        if ( cell.nType == SQLITE_TEXT )
            strResult = std::string ( (const char *)cell.pVal, cell.nLength - 1 );
    }

    // Process result
    if ( !bOk || !strResult.BeginsWithI ( "ok" ) )
    {
        CLogger::ErrorPrintf ( "%s", *strResult );
        CLogger::ErrorPrintf ( "%s\n", *m_pDatabaseManager->GetLastErrorMessage () );
        CLogger::ErrorPrintf ( "Errors were encountered loading '%s' database\n", *ExtractFilename ( PathConform ( "internal.db" ) ) );
        CLogger::ErrorPrintf ( "Maybe now is the perfect time to panic.\n" );
        CLogger::ErrorPrintf ( "See - http://wiki.multitheftauto.com/wiki/fixdb\n" );
        CLogger::ErrorPrintf ( "************************\n" );
        return false;
    }
    return true;
}


CAccount* CAccountManager::Get ( const char* szName, bool bRegistered )
{
    if ( szName && szName [ 0 ] )
    {
        std::vector < CAccount* > results;
        m_List.FindAccountMatches ( &results, szName );
        for ( uint i = 0 ; i < results.size () ; i++ )
        {
            CAccount* pAccount = results[i];
            if ( pAccount->IsRegistered () == bRegistered )
            {
                return pAccount;
            }
        }
    }
    return NULL;
}


CAccount* CAccountManager::Get ( const char* szName, const char* szIP )
{
    if ( szName && szName [ 0 ] && szIP && szIP [ 0 ] )
    {
        std::vector < CAccount* > results;
        m_List.FindAccountMatches ( &results, szName );
        for ( uint i = 0 ; i < results.size () ; i++ )
        {
            CAccount* pAccount = results[i];
            if ( pAccount->IsRegistered () )
            {
                if ( pAccount->GetIP ().compare ( szIP ) == 0 )
                {
                    return pAccount;
                }
            }
        }
    }
    return NULL;
}


CAccount* CAccountManager::GetAccountFromScriptID ( uint uiScriptID )
{
    CAccount* pAccount = (CAccount*) CIdArray::FindEntry ( uiScriptID, EIdClass::ACCOUNT );
    dassert ( !pAccount || ListContains ( m_List, pAccount ) );
    return pAccount;
}


void CAccountManager::RemoveFromList ( CAccount* pAccount )
{
    m_List.remove ( pAccount );
}

void CAccountManager::ChangingName ( CAccount* pAccount, const SString& strOldName, const SString& strNewName )
{
    m_List.ChangingName ( pAccount, strOldName, strNewName );
}

void CAccountManager::MarkAsChanged ( CAccount* pAccount )
{
    if ( pAccount->IsRegistered () ) 
    {
        m_bChangedSinceSaved = true;
        pAccount->SetChanged ( true );
    }
}

void CAccountManager::RemoveAll ( void )
{
    DeletePointersAndClearList ( m_List );
}

bool CAccountManager::LogIn ( CClient* pClient, CClient* pEchoClient, const char* szNick, const char* szPassword )
{
    // Is he already logged in?
    if ( pClient->IsRegistered () )
    {
        if ( pEchoClient ) pEchoClient->SendEcho ( "login: You are already logged in" );
        return false;
    }

    // Get the players details if relevant
    string strPlayerName, strPlayerIP, strPlayerSerial;
    if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );
        strPlayerIP = pPlayer->GetSourceIP ();
        strPlayerName = pPlayer->GetNick ();
        strPlayerSerial = pPlayer->GetSerial ();
    }

    if ( m_AccountProtect.IsFlooding ( strPlayerIP.c_str () ) )
    {
        if ( pEchoClient ) pEchoClient->SendEcho ( SString( "login: Account locked", szNick ).c_str() );
        CLogger::AuthPrintf ( "LOGIN: Ignoring %s trying to log in as '%s' (IP: %s  Serial: %s)\n", strPlayerName.c_str (), szNick, strPlayerIP.c_str (), strPlayerSerial.c_str () );
        return false;
    }

    // Grab the account on his nick if any
    CAccount* pAccount = g_pGame->GetAccountManager ()->Get ( szNick );
    if ( !pAccount )
    {
        if ( pEchoClient ) pEchoClient->SendEcho( SString( "login: No known account for '%s'", szNick ).c_str() );
        CLogger::AuthPrintf ( "LOGIN: %s tried to log in as '%s' (Unknown account) (IP: %s  Serial: %s)\n", strPlayerName.c_str (), szNick, strPlayerIP.c_str (), strPlayerSerial.c_str () );
        return false;
    }

    if ( pAccount->GetClient () )
    {
        if ( pEchoClient ) pEchoClient->SendEcho ( SString( "login: Account for '%s' is already in use", szNick ).c_str() );
        return false;
    }
    if ( strlen ( szPassword ) <= MIN_PASSWORD_LENGTH || strlen ( szPassword ) > MAX_PASSWORD_LENGTH || !pAccount->IsPassword ( szPassword ) )
    {
        if ( pEchoClient ) pEchoClient->SendEcho ( SString( "login: Invalid password for account '%s'", szNick ).c_str() );
        CLogger::AuthPrintf ( "LOGIN: %s tried to log in as '%s' with an invalid password (IP: %s  Serial: %s)\n", strPlayerName.c_str (), szNick, strPlayerIP.c_str (), strPlayerSerial.c_str () );
        m_AccountProtect.AddConnect ( strPlayerIP.c_str () );
        return false;
    }

    // Try to log him in
    return LogIn ( pClient, pEchoClient, pAccount );
}

bool CAccountManager::LogIn ( CClient* pClient, CClient* pEchoClient, CAccount* pAccount, bool bAutoLogin )
{
    // Log him in
    CAccount* pCurrentAccount = pClient->GetAccount ();
    pClient->SetAccount ( pAccount );
    pAccount->SetClient ( pClient );

    string strPlayerIP, strPlayerSerial;
    if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
    {
        CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );

        // Get the players details
        strPlayerIP = pPlayer->GetSourceIP () ;
        strPlayerSerial = pPlayer->GetSerial ();
        // Set in account
        pAccount->SetIP ( strPlayerIP );
        pAccount->SetSerial ( strPlayerSerial );
    }

    // Call the onClientLogin script event
    CElement* pClientElement = NULL;
    switch ( pClient->GetClientType () )
    {
        case CClient::CLIENT_PLAYER:
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );
            pClientElement = static_cast < CElement* > ( pPlayer );
            break;
        }
        case CClient::CLIENT_CONSOLE:
        {
            CConsoleClient* pConsoleClient = static_cast < CConsoleClient* > ( pClient );
            pClientElement = static_cast < CElement* > ( pConsoleClient );
            break;
        }
    }
    if ( pClientElement )
    {
        CLuaArguments Arguments;
        Arguments.PushAccount ( pCurrentAccount );
        Arguments.PushAccount ( pAccount );
        Arguments.PushBoolean ( bAutoLogin );
        if ( !pClientElement->CallEvent ( "onPlayerLogin", Arguments ) )
        {
            // DENIED!
            pClient->SetAccount ( pCurrentAccount );
            pAccount->SetClient ( NULL );
            return false;
        }
    }

    // Get the names of the groups the client belongs to - I did it like this for a larf
    string strGroupList;
    for ( list <CAccessControlListGroup* > ::const_iterator iterg = g_pGame->GetACLManager ()->Groups_Begin () ; iterg != g_pGame->GetACLManager ()->Groups_End (); iterg++ )
        for ( list <CAccessControlListGroupObject* > ::iterator itero = (*iterg)->IterBeginObjects () ; itero != (*iterg)->IterEndObjects (); itero++ )
            if ( (*itero)->GetObjectType () == CAccessControlListGroupObject::OBJECT_TYPE_USER )
                if ( (*itero)->GetObjectName () == pAccount->GetName () || strcmp ( (*itero)->GetObjectName (), "*" ) == 0 )
                    strGroupList = string( (*iterg)->GetGroupName () ) + ( strGroupList.length() ? ", " : "" ) + strGroupList;

    CLogger::AuthPrintf ( "LOGIN: (%s) %s successfully logged in as '%s' (IP: %s  Serial: %s)\n", strGroupList.c_str (), pClient->GetNick (), pAccount->GetName ().c_str (), strPlayerIP.c_str (), strPlayerSerial.c_str () );

    // Tell the player
    if ( pEchoClient )
    {
        if ( bAutoLogin )
            pEchoClient->SendEcho ( "auto-login: You successfully logged in" );
        else
            pEchoClient->SendEcho ( "login: You successfully logged in" );
    }

    // Update who was info
    if ( pClient->GetClientType () == CClient::CLIENT_PLAYER )
        g_pGame->GetConsole ()->GetWhoWas ()->OnPlayerLogin ( static_cast < CPlayer* > ( pClient ) );

    // Delete the old account if it was a guest account
    if ( !pCurrentAccount->IsRegistered () )
        delete pCurrentAccount;

    return true;
}

bool CAccountManager::LogOut ( CClient* pClient, CClient* pEchoClient )
{
    // Is he logged in?
    if ( !pClient->IsRegistered () )
    {
        if ( pEchoClient )
            pEchoClient->SendEcho ( "logout: You were not logged in" );
        return false;
    }

    CAccount* pCurrentAccount = pClient->GetAccount ();
    pCurrentAccount->SetClient ( NULL );

    CAccount* pAccount = new CAccount ( g_pGame->GetAccountManager (), false, GUEST_ACCOUNT_NAME );
    pClient->SetAccount ( pAccount );

    // Call the onClientLogout event
    CElement* pClientElement = NULL;
    switch ( pClient->GetClientType () )
    {
        case CClient::CLIENT_PLAYER:
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( pClient );
            pClientElement = static_cast < CElement* > ( pPlayer );
            break;
        }
        case CClient::CLIENT_CONSOLE:
        {
            CConsoleClient* pConsoleClient = static_cast < CConsoleClient* > ( pClient );
            pClientElement = static_cast < CElement* > ( pConsoleClient );
            break;
        }
    }
    if ( pClientElement )
    {
        // Call our script event
        CLuaArguments Arguments;
        Arguments.PushAccount ( pCurrentAccount );
        Arguments.PushAccount ( pAccount );
        if ( !pClientElement->CallEvent ( "onPlayerLogout", Arguments ) )
        {
            // DENIED!
            pClient->SetAccount ( pCurrentAccount );
            pCurrentAccount->SetClient ( pClient );
            delete pAccount;
            return false;
        }
    }

    // Tell the console
    CLogger::AuthPrintf ( "LOGOUT: %s logged out as '%s'\n", pClient->GetNick (), pCurrentAccount->GetName ().c_str () );

    // Tell the player
    if ( pEchoClient )
        pEchoClient->SendEcho ( "logout: You logged out" );

    return true;
}


CLuaArgument* CAccountManager::GetAccountData( CAccount* pAccount, const char* szKey )
{
    //Get the user ID
    int iUserID = pAccount->GetID();
    //create a new registry result for the query return
    CRegistryResult result;

    //Select the value and type from the database where the user is our user and the key is the required key
    m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT value,type from userdata where userid=? and key=? LIMIT 1", SQLITE_INTEGER, iUserID, SQLITE_TEXT, szKey );

    //Store the returned amount of rows
    int iResults = result.nRows;

    // Default result is nil
    CLuaArgument* pResult = new CLuaArgument ();

    //Do we have any results?
    if ( iResults > 0 )
    {
        int iType = static_cast < int > ( result.Data[0][1].nVal );
        //Account data is stored as text so we don't need to check what type it is just return it
        if ( iType == LUA_TBOOLEAN )
        {
            SString strResult = (const char *)result.Data[0][0].pVal;
            pResult->ReadBool ( strResult == "true" );
        }
        else
        if ( iType == LUA_TNUMBER )
            pResult->ReadNumber ( strtod ( (const char *)result.Data[0][0].pVal, NULL ) );
        else
            pResult->ReadString ( (const char *)result.Data[0][0].pVal );
    }
    else
    {
        //No results
        pResult->ReadBool ( false );
    }

    return pResult;
}

bool CAccountManager::SetAccountData( CAccount* pAccount, const char* szKey, const SString& strValue, int iType )
{
    if ( iType != LUA_TSTRING && iType != LUA_TNUMBER && iType != LUA_TBOOLEAN && iType != LUA_TNIL )
        return false;

    //Get the user ID
    int iUserID = pAccount->GetID();
    SString strKey = szKey;

    //Does the user want to delete the data?
    if ( strValue == "false" && iType == LUA_TBOOLEAN )
    {
        m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM userdata WHERE userid=? AND key=?", SQLITE_INTEGER, iUserID, SQLITE_TEXT, strKey.c_str () );
        return true;
    }

    m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT OR IGNORE INTO userdata (userid, key, value, type) VALUES(?,?,?,?)", SQLITE_INTEGER, pAccount->GetID (), SQLITE_TEXT, strKey.c_str (), SQLITE_TEXT, strValue.c_str (), SQLITE_INTEGER, iType );
    m_pDatabaseManager->QueryWithCallbackf ( m_hDbConnection, StaticDbCallback, this, "UPDATE userdata SET value=?, type=? WHERE userid=? AND key=?", SQLITE_TEXT, strValue.c_str (), SQLITE_INTEGER, iType, SQLITE_INTEGER, iUserID, SQLITE_TEXT, strKey.c_str () );
    return true;
}

bool CAccountManager::CopyAccountData( CAccount* pFromAccount, CAccount* pToAccount )
{
    //Get the user ID of the from account
    int iUserID = pFromAccount->GetID();
    //create a new registry result for the from account query return value
    CRegistryResult result;
    //create a new registry result for the to account query return value
    //initialize key and value strings
    SString strKey;
    SString strValue;

    //Select the key and value from the database where the user is our from account
    m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT key,value,type from userdata where userid=? LIMIT 1", SQLITE_INTEGER, iUserID );

    //Store the returned amount of rows
    int iResults = result.nRows;

    //Do we have any results?
    if ( iResults > 0 ) {
        //Loop through until i is the same as the number of rows
        for ( int i = 0;i < iResults;i++ ) 
        {
            //Get our key
            strKey = (const char *)result.Data[i][0].pVal;
            //Get our value
            strValue = (const char *)result.Data[i][1].pVal;
            int iType = static_cast < int > ( result.Data[i][2].nVal );
            //Select the id and userid where the user is the to account and the key is strKey
            CRegistryResult subResult;
            m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &subResult, "SELECT id,userid from userdata where userid=? and key=? LIMIT 1", SQLITE_INTEGER, iUserID, SQLITE_TEXT, strKey.c_str () );
            //If there is a key with this value update it otherwise insert it and store the return value in bRetVal
            if ( subResult.nRows > 0 )
                m_pDatabaseManager->Execf ( m_hDbConnection, "UPDATE userdata SET value=?, type=? WHERE userid=? AND key=?", SQLITE_TEXT, strValue.c_str (), SQLITE_INTEGER, iType, SQLITE_INTEGER, pToAccount->GetID (), SQLITE_TEXT, strKey.c_str () );
            else
                m_pDatabaseManager->Execf ( m_hDbConnection, "INSERT INTO userdata (userid, key, value, type) VALUES(?,?,?,?)", SQLITE_INTEGER, pToAccount->GetID (), SQLITE_TEXT, strKey.c_str (), SQLITE_TEXT, strValue.c_str (), SQLITE_INTEGER, iType );

        }
    }
    else
        //We had no results so return false (Nothing has changed)
        return false;
    
    return true;
}


bool CAccountManager::GetAllAccountData( CAccount* pAccount, lua_State* pLua )
{
    //Get the user ID
    int iUserID = pAccount->GetID();
    //create a new registry result for the query return
    CRegistryResult result;
    SString strKey;

    //Select the value and type from the database where the user is our user and the key is the required key
    m_pDatabaseManager->QueryWithResultf ( m_hDbConnection, &result, "SELECT key,value,type from userdata where userid=?", SQLITE_INTEGER, iUserID );

    //Store the returned amount of rows
    int iResults = result.nRows;

    //Do we have any results?
    if ( iResults > 0 ) 
    {
        //Loop through until i is the same as the number of rows
        for ( int i = 0;i < iResults;i++ ) 
        {
            //Get our key
            strKey = (const char *)result.Data[i][0].pVal;
            //Get our type
            int iType = static_cast < int > ( result.Data[i][2].nVal );
            //Account data is stored as text so we don't need to check what type it is just return it
            if ( iType == LUA_TNIL )
            {
                lua_pushstring ( pLua, strKey );
                lua_pushnil ( pLua );
                lua_settable ( pLua, -3 );
            }
            if ( iType == LUA_TBOOLEAN )
            {
                SString strResult = (const char *)result.Data[i][1].pVal;
                lua_pushstring ( pLua, strKey );
                lua_pushboolean ( pLua, strResult == "true" ? true : false );
                lua_settable ( pLua, -3 );
            }
            if ( iType == LUA_TNUMBER )
            {
                lua_pushstring ( pLua, strKey );
                lua_pushnumber ( pLua, strtod ( (const char*)result.Data[i][1].pVal, NULL ) );
                lua_settable ( pLua, -3 );
            }
            else
            {
                lua_pushstring ( pLua, strKey );
                lua_pushstring ( pLua, ( (const char*)result.Data[i][1].pVal ) );
                lua_settable ( pLua, -3 );
            }
        }
        return true;
    }
    return false;
}


void CAccountManager::SmartLoad ()
{
    //##Function to work out if we need to reload the accounts.xml file into internal.db##
    //If we need to reload the XML file do it
    if ( m_bLoadXML ) {
        //Convert XML to SQL with our filename
        ConvertXMLToSQL( m_strFileName.c_str () );
        //Set loadXML to false so when we save internal.db it won't reload our XML file next run
        m_bLoadXML = false;
    }
    else
        //else load our internal.db
        Load ();

}

void CAccountManager::Register( CAccount* pAccount )
{
    if ( pAccount->IsRegistered ()  )
    {
        //Give the Account an ID
        pAccount->SetID( ++m_iAccounts );
        //Force a save for this account
        Save ( pAccount );
    }
}

void CAccountManager::RemoveAccount ( CAccount* pAccount )
{
    int iUserID = pAccount->GetID();
    m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM accounts WHERE id=?", SQLITE_INTEGER, iUserID );
    m_pDatabaseManager->Execf ( m_hDbConnection, "DELETE FROM userdata WHERE userid=?", SQLITE_INTEGER, iUserID );
}


//
// Callback for some database queries.
//    This is only done to check for (and report) errors
//
void CAccountManager::StaticDbCallback ( CDbJobData* pJobData, void* pContext )
{
    if ( pJobData->stage == EJobStage::RESULT )
        ((CAccountManager*)pContext)->DbCallback ( pJobData );
#ifdef MTA_DEBUG
    else
        CLogger::LogPrintf ( "DEBUGINFO: StaticDbCallback stage was %d for '%s'\n", pJobData->stage, *pJobData->command.strData );
#endif
}

void CAccountManager::DbCallback ( CDbJobData* pJobData )
{
    if ( m_pDatabaseManager->QueryPoll ( pJobData, 0 ) )
    {
        if ( pJobData->result.status == EJobResult::FAIL ) 
            CLogger::LogPrintf ( "ERROR: While updating account with '%s': %s.\n", *pJobData->command.strData, *pJobData->result.strReason );
    }
    else
    {
        CLogger::LogPrintf ( "ERROR: Something worrying happened in DbCallback '%s': %s.\n", *pJobData->command.strData, *pJobData->result.strReason );
    }
}

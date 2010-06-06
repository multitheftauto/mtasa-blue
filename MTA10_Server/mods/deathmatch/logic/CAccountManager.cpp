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

CAccountManager::CAccountManager ( char* szFileName, SString strBuffer ): CXMLConfig ( szFileName )
    , m_AccountProtect( 6, 30000, 60000 * 1 )     // Max of 6 attempts per 30 seconds, then 1 minute ignore
{
    m_bRemoveFromList = true;
    m_bAutoLogin = false;
    m_llLastTimeSaved = GetTickCount64_ ();
    m_bChangedSinceSaved = false;
    //set loadXML to false
    m_bLoadXML = false;
    m_iAccounts = 1;

    //Create our database
    m_pSaveFile = new CRegistry ( "" );

    //Load internal.db
    m_pSaveFile->Load ( strBuffer );

    //Create all our tables (Don't echo the results)
    m_pSaveFile->CreateTable ( "accounts", "id INTEGER PRIMARY KEY, name TEXT, password TEXT, ip TEXT, serial TEXT", true );
    m_pSaveFile->CreateTable ( "userdata", "id INTEGER PRIMARY KEY, userid INTEGER, key TEXT, value TEXT, type INTEGER", true );
    m_pSaveFile->CreateTable ( "settings", "id INTEGER PRIMARY KEY, key TEXT, value INTEGER", true );
    
    //Create a new RegistryResult
    CRegistryResult result;

    //Pull our settings
    m_pSaveFile->Query ( &result, "SELECT key, value from settings" );

    //Did we get any results
    if ( result.nRows == 0 )
    {
        //Set our settings and clear the accounts/userdata tables just in case
        m_pSaveFile->Query ( "INSERT INTO settings (key, value) VALUES(?,?)", "autologin", SQLITE_INTEGER, 0 );
        m_pSaveFile->Query ( "INSERT INTO settings (key, value) VALUES(?,?)", "XMLParsed", SQLITE_INTEGER, 0 );
        //Tell the Server to load the xml file rather than the SQL
        m_bLoadXML = true;
    }
    else
    {
        bool bLoadXMLMissing = true;
        for (int i = 0;i < result.nRows;i++) 
        {
            SString strSetting = (char *)result.Data[i][0].pVal;
            //Do we have a result for autologin
            if ( strSetting == "autologin" )
                //Set the Auto login variable
                m_bAutoLogin = result.Data[i][1].nVal == 1 ? true : false;

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
            m_pSaveFile->Query ( "INSERT INTO settings (key, value) VALUES(?,?)", "XMLParsed", SQLITE_INTEGER, 0 );
            //Tell the Server to load the xml file rather than the SQL
            m_bLoadXML = true;
        }
        else if (result.nRows == 1) 
        {
            //if the results is one and we didn't trigger the other if statement then we are missing autologin so insert it
            m_pSaveFile->Query ( "INSERT INTO settings (key, value) VALUES(?,?)", "autologin", SQLITE_INTEGER, 0 );
        }
    }
}
void CAccountManager::ClearSQLDatabase ( void )
{    
    //No settings file or server owner wants to reload from the accounts file
    //Clear the accounts and userdata tables
    m_pSaveFile->Query ( "DELETE from accounts" );
    m_pSaveFile->Query ( "DELETE from userdata");
}

CAccountManager::~CAccountManager ( void )
{
    //Save everything
    Save ();
    //Delete our save file
    delete m_pSaveFile;
    RemoveAll ();
}


void CAccountManager::DoPulse ( void )
{
    m_pSaveFile->EndTransaction ( true );   // Flush any SetAccountData() calls

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
                CLogger::LogPrint ( "Conversion Failed: Accounts.xml failed to load.\n" );
                //Add Console to the SQL Database (You don't need to create an account since the server takes care of that (Have to do this here or Console may be created after other accounts if the owner uses addaccount too early)
                m_pSaveFile->Query ( "INSERT INTO accounts (name, password) VALUES(?,?)", "Console", "" );
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
        m_pSaveFile->BeginTransaction ();

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
                                    m_pSaveFile->Query ( "INSERT INTO accounts (name, ip, serial, password) VALUES(?,?,?,?)", strName.c_str(), strIP.c_str(), pAttribute->GetValue ().c_str(), strPassword.c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, strIP, m_iAccounts++, pAttribute->GetValue () );
                                
                                }
                                else
                                {
                                    //Insert the entry into the accounts database
                                    m_pSaveFile->Query ( "INSERT INTO accounts (name, ip, password) VALUES(?,?,?)", strName.c_str(), strIP.c_str(), strPassword.c_str() );
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
                                    m_pSaveFile->Query ( "INSERT INTO accounts (name, password, serial) VALUES(?,?,?)", strName.c_str(), strPassword.c_str(), pAttribute->GetValue().c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, "", m_iAccounts++, pAttribute->GetValue () );
                                }
                                else
                                {
                                    //Insert the entry into the accounts database
                                    m_pSaveFile->Query ( "INSERT INTO accounts (name, password) VALUES(?,?)", strName.c_str(), strPassword.c_str() );
                                    pAccount = new CAccount ( this, true, strName, strPassword, "", m_iAccounts++, "" );
                                }
                            }
                        }
                        else
                        {
                            if ( strName == "Console" )
                            {
                                //Add Console to the SQL Database (You don't need to create an account since the server takes care of that
                                m_pSaveFile->Query ( "INSERT INTO accounts (name, password) VALUES(?,?)", "Console", "" );
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
        m_pSaveFile->EndTransaction ();
        return true;
    }

    return false;
}


bool CAccountManager::Load( const char* szFileName )
{
    //Create a registry result
    CRegistryResult result;
    //Select all our required information from the accounts database
    m_pSaveFile->Query( &result, "SELECT id,name,password,ip,serial from accounts" );

    //Work out how many rows we have
    int iResults = result.nRows;
    //Initialize all our variables
    SString strName, strPassword, strSerial, strIP;
    int iUserID = 0;
    CAccount* pAccount = NULL;
    for ( int i = 0 ; i < iResults ; i++ )
    {
        //Fill User ID, Name & Password (Required data)
        iUserID = result.Data[i][0].nVal;
        strName = (char *)result.Data[i][1].pVal;
        strPassword = "";
        // If we have an password
        if ( result.Data[i][2].pVal )
            strPassword = (char *)result.Data[i][2].pVal;

        //if we have an IP
        if ( result.Data[i][3].pVal ) {
            //Fill the IP variable
            strIP = (char *)result.Data[i][3].pVal;
            //If we have a Serial
            if ( result.Data[i][4].pVal ) {
                //Fill the serial variable
                strSerial = (char *)result.Data[i][4].pVal;
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
    }
    m_iAccounts = iUserID;
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
    SString strPassword = pAccount->GetPassword();
    SString strIP = pAccount->GetIP();
    SString strSerial = pAccount->GetSerial();

    //Create a registry result
    CRegistryResult result;
    //Select ID From Accounts Where Name=strName
    m_pSaveFile->Query ( &result, "SELECT id FROM accounts WHERE name=?", strName.c_str() );

    //Check for results
    if ( result.nRows > 0 ) {
        //If we have a serial update that as well
        if ( strSerial != "" )
            m_pSaveFile->Query ( "UPDATE accounts SET ip=?, serial=?, password=? WHERE name=?", strIP.c_str (), strSerial.c_str (), strPassword.c_str (), strName.c_str () );
        else
            //If we don't have a serial then IP and password will suffice
            m_pSaveFile->Query ( "UPDATE accounts SET ip=?, password=? WHERE name=?", strIP.c_str (), strPassword.c_str (), strName.c_str () );
    }
    else
        //No entries so it isn't in the database therefore Insert it
        m_pSaveFile->Query ( "INSERT INTO accounts (name, ip, serial, password) VALUES(?,?,?,?)", strName.c_str (), strIP.c_str (), strSerial.c_str (), strPassword.c_str () );
    //Set changed since saved to false
    pAccount->SetChanged( false );
}

bool CAccountManager::Save ( const char* szFileName )
{
    m_pSaveFile->EndTransaction ( true );   // Flush any SetAccountData() calls

    // Attempted save now
    m_bChangedSinceSaved = false;
    m_llLastTimeSaved = GetTickCount64_ ();

    m_pSaveFile->BeginTransaction ();

    list < CAccount* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( (*iter)->IsRegistered () && (*iter)->HasChanged() )
        {
            CAccount * pAccount = (*iter);
            Save ( pAccount );
        }
    }

    m_pSaveFile->EndTransaction ();
    
    //Get the time took to save
    long long llDeltaTime = GetTickCount64_ () - m_llLastTimeSaved;
    //Greater than five seconds?
    if ( llDeltaTime > 5000 )
        //Echo the time taken
        CLogger::LogPrintf ( "INFO: Took %lld seconds to save accounts Database.\n", llDeltaTime / 1000 );

    return true;
}



bool CAccountManager::SaveSettings ()
{
    //Update our autologin and XML Load SQL entries
    m_pSaveFile->Query ( "UPDATE settings SET value=? WHERE key=?", SQLITE_INTEGER, m_bAutoLogin ? 1 : 0, "autologin" );
    m_pSaveFile->Query ( "UPDATE settings SET value=? WHERE key=?", SQLITE_INTEGER, 1, "XMLParsed" );

    return true;
}


CAccount* CAccountManager::Get ( const char* szName, bool bRegistered )
{
    if ( szName && szName [ 0 ] )
    {
        unsigned int uiHash = HashString ( szName );
        list < CAccount* > ::iterator iter = m_List.begin ();
        for ( ; iter != m_List.end () ; iter++ )
        {
            CAccount* pAccount = *iter;
            if ( pAccount->IsRegistered () == bRegistered )
            {
                if ( pAccount->GetNameHash() == uiHash && pAccount->GetName () == szName )
                {
                    return pAccount;
                }
            }
        }
    }
    return NULL;
}


CAccount* CAccountManager::Get ( const char* szName, const char* szIP )
{
    if ( szName && szName [ 0 ] && szIP && szIP [ 0 ] )
    {
        unsigned int uiHash = HashString ( szName );
        list < CAccount* > ::iterator iter = m_List.begin ();
        for ( ; iter != m_List.end () ; iter++ )
        {
            CAccount* pAccount = *iter;
            if ( pAccount->IsRegistered () )
            {
                if ( pAccount->GetNameHash() == uiHash && pAccount->GetName () == szName )
                {
                    if ( pAccount->GetIP ().compare ( szIP ) == 0 )
                    {
                        return pAccount;
                    }
                }
            }
        }
    }
    return NULL;
}


bool CAccountManager::Exists ( CAccount* pAccount )
{
    list < CAccount* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( *iter == pAccount )
        {
            return true;
        }
    }
    return false;
}


void CAccountManager::RemoveFromList ( CAccount* pAccount )
{
    if ( m_bRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pAccount );
    }
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
    m_bRemoveFromList = false;
    list < CAccount* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        delete *iter;
    }
    m_List.clear ();
    m_bRemoveFromList = true;
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
        char szIP [32] = { "\0" };
        strPlayerIP = pPlayer->GetSourceIP ( szIP );
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

        char szIP [ 25 ];
        pPlayer->GetSourceIP ( szIP );
        // Set IP in account
        pAccount->SetIP ( szIP );
        // Get the players details
        strPlayerIP = szIP;
        strPlayerSerial = pPlayer->GetSerial ();
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

    CAccount* pAccount = new CAccount ( g_pGame->GetAccountManager (), false, "guest" );
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


CLuaArgument* CAccountManager::GetAccountData( CAccount* pAccount, char* szKey )
{
    m_pSaveFile->EndTransaction ( true );   // Flush any SetAccountData() calls

    //Get the user ID
    int iUserID = pAccount->GetID();
    //create a new registry result for the query return
    CRegistryResult result;

    //Select the value and type from the database where the user is our user and the key is the required key
    m_pSaveFile->Query ( &result, "SELECT value,type from userdata where userid=? and key=?", SQLITE_INTEGER, iUserID, szKey );

    //Store the returned amount of rows
    int iResults = result.nRows;

    //Do we have any results?
    if ( iResults > 0 ) {
        int iType = result.Data[0][1].nVal;
        //Account data is stored as text so we don't need to check what type it is just return it
        if ( iType == LUA_TNIL )
            return new CLuaArgument;
        if ( iType == LUA_TBOOLEAN )
        {
            SString strResult = (char *)result.Data[0][0].pVal;
            return new CLuaArgument ( strResult == "true" ? true : false );
        }
        if ( iType == LUA_TNUMBER )
            return new CLuaArgument ( strtod ( (char *)result.Data[0][0].pVal, NULL ) );
        else
            return new CLuaArgument ( (char *)result.Data[0][0].pVal );
    }

    //No results
    return new CLuaArgument ( false );
}

bool CAccountManager::SetAccountData( CAccount* pAccount, char* szKey, SString strValue, int iType )
{
    if ( iType != LUA_TSTRING && iType != LUA_TNUMBER && iType != LUA_TBOOLEAN && iType != LUA_TNIL )
        return false;

    m_pSaveFile->BeginTransaction ();   // Batch successive SetAccountData() calls

    //Get the user ID
    int iUserID = pAccount->GetID();
    SString strKey = szKey;

    //Does the user want to delete the data?
    if ( strValue == "false" && iType == LUA_TBOOLEAN )
    {
        m_pSaveFile->Query ( "DELETE FROM userdata WHERE key=? AND userid=?", strKey.c_str (), SQLITE_INTEGER, iUserID );
        return true;
    }

    //create a new registry result for the query return
    CRegistryResult result;

    //Select the key and value from the database where the user is our user and the key is the required key
    m_pSaveFile->Query ( &result, "SELECT id,userid from userdata where userid=? and key=?", SQLITE_INTEGER, iUserID, strKey.c_str () );

    //If there is a key with this value update it otherwise insert it
    if ( result.nRows > 0 )
        return m_pSaveFile->Query ( "UPDATE userdata SET value=?, type=? WHERE userid=? AND key=?", strValue.c_str (), SQLITE_INTEGER, iType, SQLITE_INTEGER, iUserID, strKey.c_str () );
    else
        return m_pSaveFile->Query ( "INSERT INTO userdata (userid, key, value, type) VALUES(?,?,?,?)", SQLITE_INTEGER, pAccount->GetID (), strKey.c_str (), strValue.c_str (), SQLITE_INTEGER, iType );
   
    //Return false as nothing has changed
    return false;
}

bool CAccountManager::CopyAccountData( CAccount* pFromAccount, CAccount* pToAccount )
{
    m_pSaveFile->EndTransaction ( true );   // Flush any SetAccountData() calls

    //Get the user ID of the from account
    int iUserID = pFromAccount->GetID();
    //create a new registry result for the from account query return value
    CRegistryResult result;
    //create a new registry result for the to account query return value
    CRegistryResult subResult;
    //initialize key and value strings
    SString strKey;
    SString strValue;

    //Select the key and value from the database where the user is our from account
    m_pSaveFile->Query ( &result, "SELECT key,value,type from userdata where userid=?", SQLITE_INTEGER, iUserID );

    //Store the returned amount of rows
    int iResults = result.nRows;

    //Do we have any results?
    if ( iResults > 0 ) {
        //Loop through until i is the same as the number of rows
        for ( int i = 0;i < iResults;i++ ) 
        {
            //Get our key
            strKey = (char *)result.Data[i][0].pVal;
            //Get our value
            strValue = (char *)result.Data[i][1].pVal;
            int iType = result.Data[i][2].nVal;
            //Select the id and userid where the user is the to account and the key is strKey
            m_pSaveFile->Query ( &subResult, "SELECT id,userid from userdata where userid=? and key=?", SQLITE_INTEGER, iUserID, strKey.c_str () );
            //If there is a key with this value update it otherwise insert it and store the return value in bRetVal
            if ( subResult.nRows > 0 )
                m_pSaveFile->Query ( "UPDATE userdata SET value=?, type=? WHERE userid=? AND key=?", strValue.c_str (), SQLITE_INTEGER, iType, SQLITE_INTEGER, pToAccount->GetID (), strKey.c_str () );
            else
                m_pSaveFile->Query ( "INSERT INTO userdata (userid, key, value, type) VALUES(?,?,?,?)", SQLITE_INTEGER, pToAccount->GetID (), strKey.c_str (), strValue.c_str (), SQLITE_INTEGER, iType );

        }
    }
    else
        //We had no results so return false (Nothing has changed)
        return false;
    
    return true;
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
        Load( "" );

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
    m_pSaveFile->Query ( "DELETE FROM accounts WHERE id=?", SQLITE_INTEGER, iUserID );
    m_pSaveFile->Query ( "DELETE FROM userdata WHERE userid=?", SQLITE_INTEGER, iUserID );
}

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

CAccountManager::CAccountManager ( char* szFileName ): CXMLConfig ( szFileName )
    , m_AccountProtect( 6, 30000, 60000 * 1 )     // Max of 6 attempts per 30 seconds, then 1 minute ignore
{
    m_bRemoveFromList = true;
    m_bAutoLogin = false;
    m_llLastTimeSaved = GetTickCount64_ ();
    m_bChangedSinceSaved = false;
}


CAccountManager::~CAccountManager ( void )
{
    Save ();
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


bool CAccountManager::Load ( const char* szFileName )
{
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
                    return Load ( pRootNode );
                }
            }
        }
    }
    return false;
}


bool CAccountManager::Load ( CXMLNode* pParent )
{
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
									pAccount = new CAccount ( this, true, strName, strPassword, strIP, pAttribute->GetValue () );
								}
								else
								{
	                                pAccount = new CAccount ( this, true, strName, strPassword, strIP );
								}

                                // Grab the data on this account
                                CXMLNode* pDataNode = NULL;
                                unsigned int uiDataNodesCount = pAccountNode->GetSubNodeCount ();
                                for ( unsigned int j = 0 ; j < uiDataNodesCount ; j++ )
                                {
                                    pDataNode = pAccountNode->GetSubNode ( j );
                                    if ( pDataNode == NULL )
                                        continue;

                                    strBuffer = pDataNode->GetTagName ();
                                    if ( strBuffer.compare ( "nil_data") == 0 )
                                    {
                                        CXMLAttributes* pAttributes = &(pDataNode->GetAttributes ());
                                        CXMLAttribute* pAttribute = NULL;
                                        unsigned int uiDataValuesCount = pAttributes->Count ();
                                        for ( unsigned int a = 0 ; a < uiDataValuesCount ; a++ )
                                        {
                                            pAttribute = pAttributes->Get ( a );
                                            strDataKey = pAttribute->GetName ();
                                            strDataValue = pAttribute->GetValue ();
                                            if ( strcmp ( strDataValue.c_str (), "nil" ) == 0 )
                                            {
                                                CLuaArgument Argument;
                                                pAccount->SetData ( strDataKey.c_str (), &Argument );
                                            }
                                        }
                                    }
                                    else if ( strBuffer.compare ( "boolean_data" ) == 0 )
                                    {
                                        CXMLAttributes* pAttributes = &(pDataNode->GetAttributes ());
                                        CXMLAttribute* pAttribute = NULL;
                                        unsigned int uiDataValuesCount = pAttributes->Count ();
                                        for ( unsigned int a = 0 ; a < uiDataValuesCount ; a++ )
                                        {
                                            pAttribute = pAttributes->Get ( a );
                                            strDataKey = pAttribute->GetName ();
                                            strDataValue = pAttribute->GetValue ();
                                            if ( strcmp ( strDataValue.c_str (), "true" ) == 0 )
                                            {
                                                CLuaArgument Argument ( true );
                                                pAccount->SetData ( strDataKey.c_str (), &Argument );
                                            }
                                            /* Don't bother saving/load false booleans
                                            else if ( strcmp ( strDataValue.c_str (), "false" ) == 0 )
                                            {
                                                CLuaArgument Argument ( false );
                                                pAccount->SetData ( strDataKey.c_str (), &Argument );
                                            }*/
                                        }
                                    }
                                    else if ( strBuffer.compare ( "string_data" ) == 0 )
                                    {
                                        CXMLAttributes* pAttributes = &(pDataNode->GetAttributes ());
                                        CXMLAttribute* pAttribute = NULL;
                                        unsigned int uiDataValuesCount = pAttributes->Count ();
                                        for ( unsigned int a = 0 ; a < uiDataValuesCount ; a++ )
                                        {
                                            pAttribute = pAttributes->Get ( a );
                                            strDataKey = pAttribute->GetName ();
                                            strDataValue = pAttribute->GetValue ();

                                            CLuaArgument Argument ( strDataValue.c_str () );
                                            pAccount->SetData ( strDataKey.c_str (), &Argument );
                                        }
                                    }
                                    else if ( strBuffer.compare ( "number_data" ) == 0 )
                                    {
                                        CXMLAttributes* pAttributes = &(pDataNode->GetAttributes ());
                                        CXMLAttribute* pAttribute = NULL;
                                        unsigned int uiDataValuesCount = pAttributes->Count ();
                                        for ( unsigned int a = 0 ; a < uiDataValuesCount ; a++ )
                                        {
                                            pAttribute = pAttributes->Get ( a );
                                            strDataKey = pAttribute->GetName ();
                                            strDataValue = pAttribute->GetValue ();

                                            CLuaArgument Argument ( strtod ( strDataValue.c_str (), NULL ) );
                                            pAccount->SetData ( strDataKey.c_str (), &Argument );
                                        }
                                    }
                                }
                            }
							else
							{
								CAccount* pAccount = NULL;
								pAttribute = pAccountNode->GetAttributes ().Find ( "serial" );
								if ( pAttribute )
								{
									pAccount = new CAccount ( this, true, strName, strPassword, NULL, pAttribute->GetValue () );
								}
								else
								{
									pAccount = new CAccount ( this, true, strName, strPassword );
								}
							}
                        }
                    }
                }
            }
            else
            {
                LoadSetting ( pAccountNode );
            }
        }

        m_bChangedSinceSaved = false;
        return true;
    }

    return false;
}


bool CAccountManager::LoadSetting ( CXMLNode* pNode )
{
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


bool CAccountManager::Save ( const char* szFileName )
{
    // Attempted save now
    m_bChangedSinceSaved = false;
    m_llLastTimeSaved = GetTickCount64_ ();

    if ( szFileName == NULL )
        szFileName = m_strFileName.c_str ();

    if ( szFileName == NULL || szFileName [ 0 ] == 0 )
        return false;

    FILE* pFile = fopen ( szFileName, "w+" );
    if ( pFile == NULL )
        return false;
    fclose ( pFile );

    // Delete existing XML
    if ( m_pFile )
    {
        delete m_pFile;
    }

    // Create the XML
    m_pFile = g_pServerInterface->GetXML ()->CreateXML ( szFileName );
    if ( !m_pFile )
        return false;

    // Get the root node. Eventually make one called accounts
    CXMLNode* pRootNode = m_pFile->GetRootNode ();
    if ( !pRootNode )
        pRootNode = m_pFile->CreateRootNode ( "accounts" );

    // Save everything into it
    if ( !Save ( pRootNode ) )
    {
        delete m_pFile;
        m_pFile = NULL;
        return false;
    }

    m_pFile->Write ();
    delete m_pFile;
    m_pFile = NULL;

    long long llDeltaTime = GetTickCount64_ () - m_llLastTimeSaved;
    if ( llDeltaTime > 5000 )
        CLogger::LogPrintf ( "INFO: Took %lld seconds to save accounts XML file.\n", llDeltaTime / 1000 );

    return true;
}


bool CAccountManager::Save ( CXMLNode* pParent )
{
    if ( pParent )
    {
        SaveSettings ( pParent );

        CXMLNode* pNode = NULL;

        list < CAccount* > ::iterator iter = m_List.begin ();
        for ( ; iter != m_List.end () ; iter++ )
        {
            if ( (*iter)->IsRegistered () )
            {
                pNode = pParent->CreateSubNode ( "account" );
                if ( pNode )
                {
                    CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( "name" );
                    if ( pAttribute )
                        pAttribute->SetValue ( (*iter)->GetName ().c_str () );

                    pAttribute = pNode->GetAttributes ().Create ( "password" );
                    if ( pAttribute )
                        pAttribute->SetValue ( (*iter)->GetPassword ().c_str () );

                    const char* szIP = (*iter)->GetIP ().c_str ();
                    if ( szIP && szIP [ 0 ] )
                    {
                        pAttribute = pNode->GetAttributes ().Create ( "ip" );
                        if ( pAttribute )
                            pAttribute->SetValue ( szIP );
                    }

					const char* szSerial = (*iter)->GetSerial ().c_str ();
                    if ( szSerial && szSerial [ 0 ] )
                    {
                        pAttribute = pNode->GetAttributes ().Create ( "serial" );
                        if ( pAttribute )
                            pAttribute->SetValue ( szSerial );
                    }

                    // Do we have any data to save?
                    if ( (*iter)->DataCount () > 0 )
                    {
                        // Sort our data into separate lists of each type
                        list < CAccountData * > nilList, boolList, stringList, numberList;
                        list < CAccountData * > * pDataList = NULL;
                        list < CAccountData * > ::iterator iterData = (*iter)->DataBegin ();
                        for ( ; iterData != (*iter)->DataEnd () ; iterData++ )
                        {
                            switch ( (*iterData)->GetValue ()->GetType () )
                            {
                                case LUA_TNIL:
                                    nilList.push_back ( *iterData );
                                    break;
                                case LUA_TBOOLEAN:
                                    boolList.push_back ( *iterData );
                                    break;
                                case LUA_TSTRING:
                                    stringList.push_back ( *iterData );
                                    break;
                                case LUA_TNUMBER:
                                    numberList.push_back ( *iterData );
                                    break;
                            }
                        }
                        CXMLAttribute * pAttribute = NULL;
                        CXMLNode * pDataNode = NULL;
                        CAccountData * pData = NULL;
                        char szArgumentAsString [ 128 ];
                        // Do we have any nil data?
                        pDataList = &nilList;
                        if ( !pDataList->empty () )
                        {
                            // Create a node for this type of data
                            pDataNode = pNode->CreateSubNode ( "nil_data" );
                            if ( pDataNode )
                            {
                                iterData = pDataList->begin ();
                                for ( ; iterData != pDataList->end (); iterData++ )
                                {
                                    pData = *iterData;
                                    pAttribute = pDataNode->GetAttributes ().Create ( pData->GetKey ().c_str () );
                                    if ( pAttribute )
                                    {
                                        pData->GetValue ()->GetAsString ( szArgumentAsString, 128 );
                                        pAttribute->SetValue ( szArgumentAsString );
                                    }
                                }
                            }
                        }
                        // Do we have any boolean data?
                        pDataList = &boolList;
                        if ( !pDataList->empty () )
                        {
                            // Create a node for this type of data
                            pDataNode = pNode->CreateSubNode ( "boolean_data" );
                            if ( pDataNode )
                            {
                                pDataList = &boolList;
                                iterData = pDataList->begin ();
                                for ( ; iterData != pDataList->end (); iterData++ )
                                {
                                    pData = *iterData;
                                    pAttribute = pDataNode->GetAttributes ().Create ( pData->GetKey ().c_str () );
                                    if ( pAttribute )
                                    {
                                        pData->GetValue ()->GetAsString ( szArgumentAsString, 128 );
                                        pAttribute->SetValue ( szArgumentAsString );
                                    }
                                }
                            }
                        }
                        // Do we have any string data?
                        pDataList = &stringList;
                        if ( !pDataList->empty () )
                        {
                            // Create a node for this type of data
                            pDataNode = pNode->CreateSubNode ( "string_data" );
                            if ( pDataNode )
                            {
                                iterData = pDataList->begin ();
                                for ( ; iterData != pDataList->end (); iterData++ )
                                {
                                    pData = *iterData;
                                    pAttribute = pDataNode->GetAttributes ().Create ( pData->GetKey ().c_str () );
                                    if ( pAttribute )
                                    {
                                        pData->GetValue ()->GetAsString ( szArgumentAsString, 128 );
                                        pAttribute->SetValue ( szArgumentAsString );
                                    }
                                }
                            }
                        }
                        // Do we have any number data?
                        pDataList = &numberList;
                        if ( !pDataList->empty () )
                        {
                            // Create a node for this type of data
                            pDataNode = pNode->CreateSubNode ( "number_data" );
                            if ( pDataNode )
                            {
                                iterData = pDataList->begin ();
                                for ( ; iterData != pDataList->end (); iterData++ )
                                {
                                    pData = *iterData;
                                    pAttribute = pDataNode->GetAttributes ().Create ( pData->GetKey ().c_str () );
                                    if ( pAttribute )
                                    {
                                        pData->GetValue ()->GetAsString ( szArgumentAsString, 128 );
                                        pAttribute->SetValue ( szArgumentAsString );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }
    return false;
}


bool CAccountManager::SaveSettings ( CXMLNode* pParent )
{
    SetBoolean ( pParent, "autologin", m_bAutoLogin );
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
                if ( pAccount->GetNameHash() == uiHash )
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
                if ( pAccount->GetNameHash() == uiHash )
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
        m_bChangedSinceSaved = true;
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

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBanManager.cpp
*  PURPOSE:     Ban manager class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBanManager::CBanManager ( void )
{
	m_szFileName[0] = '\0';
    strcpy ( m_szFileName, "banlist.xml" );

    g_pServerInterface->GetModManager ()->GetAbsolutePath ( m_szFileName, m_szPath, MAX_PATH );

    m_tUpdate = 0;
}


CBanManager::~CBanManager ( void )
{
    SaveBanList ();
    RemoveAllBans ();
}


void CBanManager::DoPulse ( void )
{
    time_t tTime = time ( NULL );

    if ( tTime > m_tUpdate )
    {
        list < CBan* > ::const_iterator iter = m_BanManager.begin ();
        while ( iter != m_BanManager.end () )
        {
            if ( (*iter)->GetTimeOfUnban () > 0 )
            {
                if ( tTime >= (*iter)->GetTimeOfUnban () )
                {
                    // Trigger the event
                    CLuaArguments Arguments;
                    Arguments.PushUserData ( *iter );
                    g_pGame->GetMapManager()->GetRootElement()->CallEvent ( "onUnban", Arguments );

                    RemoveBan ( *iter );
                    iter = m_BanManager.begin ();
                    continue;
                }
            }
            iter++;
        }
        m_tUpdate = tTime + 1;
    }
}


CBan* CBanManager::AddBan ( CPlayer* pPlayer, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        char szIP[256] = { '\0' };
        pPlayer->GetSourceIP ( szIP );

        if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) )
        {
            CBan* pBan = AddBan ( pBanner, szReason, tTimeOfUnban );
            pBan->SetNick ( pPlayer->GetNick() );
            pBan->SetIP ( szIP );

            g_pNetServer->AddBan ( szIP );

            // Save the list
            SaveBanList ();
            return pBan;
        }
    }

    return NULL;
}


CBan* CBanManager::AddBan ( const char* szIP, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) )
    {
        CBan* pBan = AddBan ( pBanner, szReason, tTimeOfUnban );
        pBan->SetIP ( szIP );

        g_pNetServer->AddBan ( szIP );

        // Save the list
        SaveBanList ();
        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( CPlayer* pPlayer, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        if ( !pPlayer->GetSerial ().empty() && !IsSerialBanned ( pPlayer->GetSerial ().c_str () ) )
	    {
		    CBan* pBan = AddBan ( pBanner, szReason, tTimeOfUnban );
            pBan->SetNick ( pPlayer->GetNick() );
            pBan->SetSerial ( pPlayer->GetSerial () );
            SaveBanList ();

            return pBan;
        }
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( const char* szSerial, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( /*IsValidSerial ( szSerial ) &&*/ !IsSerialBanned ( szSerial ) )
	{
		CBan* pBan = AddBan ( pBanner, szReason, tTimeOfUnban );
        pBan->SetSerial ( szSerial );
        SaveBanList ();

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddAccountBan ( CPlayer* pPlayer, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        if ( !pPlayer->GetSerialUser ().empty() && !IsAccountBanned ( pPlayer->GetSerialUser ().c_str () ) )
	    {
		    CBan* pBan = AddBan ( pBanner, szReason, tTimeOfUnban );
            pBan->SetNick ( pPlayer->GetNick() );
            pBan->SetAccount ( pPlayer->GetSerialUser () );
            SaveBanList ();

            return pBan;
        }
    }

    return NULL;
}


CBan* CBanManager::AddAccountBan ( const char* szAccount, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( !IsAccountBanned ( szAccount ) )
	{
		CBan* pBan = AddBan ( pBanner, szReason, tTimeOfUnban );
        pBan->SetSerial ( szAccount );
        SaveBanList ();

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddBan ( CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    // Create the ban and assign its values
    CBan* pBan = new CBan;
    pBan->SetTimeOfBan ( time ( NULL ) );
    pBan->SetTimeOfUnban ( tTimeOfUnban );

    if ( szReason )
        pBan->SetReason ( szReason );

    if ( pBanner )
        pBan->SetBanner ( pBanner->GetNick () );

    // Add it to the back of our banned list, add it to net server's ban list
    m_BanManager.push_back ( pBan );

    return pBan;
}


bool CBanManager::Exists ( CBan* pBan )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( *iter == pBan )
        {
            return true;
        }
    }

    return false;
}


bool CBanManager::IsBanned ( const char* szIP )
{
    return g_pNetServer->IsBanned ( szIP );
}


bool CBanManager::IsSpecificallyBanned ( const char* szIP )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetIP () == szIP )
        {
            return true;
        }
    }

    return false;
}


bool CBanManager::IsSerialBanned ( const char* szSerial )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetSerial () == szSerial )
        {
            return true;
        }
    }

    return false;
}


bool CBanManager::IsAccountBanned ( const char* szAccount )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetAccount () == szAccount )
        {
            return true;
        }
    }

    return false;
}


void CBanManager::RemoveBan ( CBan* pBan )
{
    if ( pBan )
    {
        if ( !pBan->GetIP ().empty() )
            g_pNetServer->RemoveBan ( pBan->GetIP ().c_str () );
        if ( !m_BanManager.empty() ) m_BanManager.remove ( pBan );
        delete pBan;
    }
	SaveBanList ();
}


void CBanManager::RemoveAllBans ( bool bPermanentDelete )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        delete *iter;
    }

    m_BanManager.clear ();

    if ( bPermanentDelete )
    {
        SaveBanList ();
    }
}


CBan* CBanManager::GetBan ( const char* szIP )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetIP () == szIP )
        {
            return *iter;
        }
    }
    
    return NULL;
}


CBan* CBanManager::GetBan ( const char* szNick, unsigned int uiOccurrance )
{
    unsigned int uiOccurrances = 0;
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetNick () == szNick )
        {
            uiOccurrances++;

            if ( uiOccurrance == uiOccurrances )
            {
                return *iter;
            }
        }
    }

    return NULL;
}


unsigned int CBanManager::GetBansWithNick ( const char* szNick )
{
    unsigned int uiOccurrances = 0;
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetNick () == szNick )
        {
            uiOccurrances++;
        }
    }

    return uiOccurrances;
}


unsigned int CBanManager::GetBansWithBanner ( const char* szBanner )
{
    unsigned int uiOccurrances = 0;
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetBanner (), szBanner )
        {
            uiOccurrances++;
        }
    }

    return uiOccurrances;
}


bool CBanManager::LoadBanList ( void )
{
    // Create the XML
    CXMLFile* pFile = g_pServerInterface->GetXML ()->CreateXML ( m_szPath );
    if ( !pFile )
    {
        return false;
    }

    // Parse it
    if ( !pFile->Parse () )
    {
        delete pFile;
        CLogger::ErrorPrintf ( "Error parsing banlist\n" );
        return false;
    }

    // Grab the XML root node
    CXMLNode* pRootNode = pFile->GetRootNode ();
    if ( !pRootNode )
    {
        pRootNode = pFile->CreateRootNode ( "banlist" );
    }

    // Is the rootnode's name <banlist>?
    if ( pRootNode->GetTagName ().compare ( "banlist" ) != 0 )
    {
        CLogger::ErrorPrintf ( "Wrong root node ('banlist')\n" );
        return false;
    }

    // Iterate the nodes
    CXMLNode* pNode = NULL;
    unsigned int uiCount = pRootNode->GetSubNodeCount ();

    for ( unsigned int i = 0; i < uiCount; i++ )
    {
        // Grab the node
        pNode = pRootNode->GetSubNode ( i );

        if ( pNode )
        {
            if ( pNode->GetTagName ().compare ( "ban" ) == 0 )
            {
                std::string strIP = SafeGetValue ( pNode, "ip" ),
                            strSerial = SafeGetValue ( pNode, "serial" ),
                            strAccount = SafeGetValue ( pNode, "account" );
                if ( !strIP.empty() || !strSerial.empty() || !strAccount.empty() )
                {
                    CBan* pBan = AddBan ();
                    if ( IsValidIP ( strIP.c_str() ) )
                    {
                        pBan->SetIP ( strIP );
                        g_pNetServer->AddBan ( strIP.c_str() );
                    }
                    pBan->SetAccount ( strAccount );
                    pBan->SetSerial ( strSerial );
                    pBan->SetBanner ( SafeGetValue ( pNode, "banner" ) );
                    pBan->SetNick ( SafeGetValue ( pNode, "nick" ) );
                    pBan->SetReason ( SafeGetValue ( pNode, "reason" ) );

                    std::string strTime = SafeGetValue ( pNode, "time" );
                    if ( !strTime.empty() ) pBan->SetTimeOfBan ( ( time_t ) atoi ( strTime.c_str() ) );

                    strTime = SafeGetValue ( pNode, "unban" );
                    if ( !strTime.empty() ) pBan->SetTimeOfUnban ( ( time_t ) atoi ( strTime.c_str() ) );
                }
            }
        }
    }

    delete pFile;
    return true;
}


void CBanManager::SaveBanList ( void )
{
    // Create the XML file
    CXMLFile* pFile = g_pServerInterface->GetXML ()->CreateXML ( m_szPath );
    if ( pFile )
    {
		// create the root node again as you are outputting all the bans again not just new ones
		CXMLNode* pRootNode = pFile->CreateRootNode ( "banlist" );	

        // Check it was created
        if ( pRootNode )
        {
            // Iterate the ban list adding it to the XML tree
            CXMLNode* pNode;
            list < CBan* >::const_iterator iter = m_BanManager.begin ();
            for ( ; iter != m_BanManager.end (); iter++ )
            {
                pNode = pRootNode->CreateSubNode ( "ban" );

                if ( pNode )
                {
                    SafeSetValue ( pNode, "nick", (*iter)->GetNick() );
                    SafeSetValue ( pNode, "ip", (*iter)->GetIP() );
                    SafeSetValue ( pNode, "serial", (*iter)->GetSerial() );
                    SafeSetValue ( pNode, "account", (*iter)->GetAccount() );
                    SafeSetValue ( pNode, "banner", (*iter)->GetBanner() );
                    SafeSetValue ( pNode, "reason", (*iter)->GetReason() );
                    SafeSetValue ( pNode, "time", ( unsigned int )(*iter)->GetTimeOfBan() );
                    if ( (*iter)->GetTimeOfUnban() > 0 )
                    {
                        SafeSetValue ( pNode, "unban", ( unsigned int )(*iter)->GetTimeOfUnban() );
                    }
                }
            }

            // Write the XML file
            pFile->Write ();
        }

        // Delete the file pointer
        delete pFile;
    }
}


void CBanManager::SafeSetValue ( CXMLNode* pNode, const char* szKey, std::string strValue )
{
    if ( !strValue.empty() )
	{
        CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( szKey );
        if ( pAttribute )
        {
            pAttribute->SetValue ( strValue.c_str () );
        }
	}
}


void CBanManager::SafeSetValue ( CXMLNode* pNode, const char* szKey, unsigned int uiValue )
{
    if ( uiValue )
	{
        CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( szKey );
        if ( pAttribute )
        {
            pAttribute->SetValue ( uiValue );
        }
	}
}


std::string CBanManager::SafeGetValue ( CXMLNode* pNode, const char* szKey )
{
    CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( szKey );

    if ( pAttribute )
    {
        return pAttribute->GetValue ();
    }
    return std::string();
}


bool CBanManager::IsValidIP ( const char* szIP )
{
    char strIP[256] = { '\0' };
    strncpy ( strIP, szIP, 255 );
	strIP[255] = '\0';

    char* szIP1 = strtok ( strIP, "." );
    char* szIP2 = strtok ( NULL, "." );
    char* szIP3 = strtok ( NULL, "." );
    char* szIP4 = strtok ( NULL, "\r" );

    if ( szIP1 && szIP2 && szIP3 && szIP4 )
    {
        if ( IsNumericString ( szIP1 ) &&
             IsNumericString ( szIP2 ) &&
             IsNumericString ( szIP3 ) &&
             IsNumericString ( szIP4 ) )
        {
            int iIP1 = atoi ( szIP1 );
            int iIP2 = atoi ( szIP2 );
            int iIP3 = atoi ( szIP3 );
            int iIP4 = atoi ( szIP4 );

            if ( iIP1 >= 0 && iIP1 < 256 )
            {
                if ( iIP2 >= 0 && iIP2 < 256 )
                {
                    if ( iIP3 >= 0 && iIP3 < 256 )
                    {
                        if ( iIP4 >= 0 && iIP4 < 256 )
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

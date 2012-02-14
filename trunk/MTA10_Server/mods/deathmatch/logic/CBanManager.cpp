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
    m_strPath = g_pServerInterface->GetModManager ()->GetAbsolutePath ( FILENAME_BANLIST );
    m_tUpdate = 0;
    m_bAllowSave = false;
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


CBan* CBanManager::AddBan ( CPlayer* pPlayer, const SString& strBanner, const SString& strReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        char szIP[256] = { '\0' };
        pPlayer->GetSourceIP ( szIP );

        if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) )
        {
            CBan* pBan = AddBan ( strBanner, strReason, tTimeOfUnban );
            pBan->SetNick ( pPlayer->GetNick() );
            pBan->SetIP ( szIP );
            return pBan;
        }
    }

    return NULL;
}


CBan* CBanManager::AddBan ( const SString& strIP, const SString& strBanner, const SString& strReason, time_t tTimeOfUnban )
{
    if ( IsValidIP ( strIP.c_str() ) && !IsSpecificallyBanned ( strIP.c_str() ) )
    {
        CBan* pBan = AddBan ( strBanner, strReason, tTimeOfUnban );
        pBan->SetIP ( strIP );
        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( CPlayer* pPlayer, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        if ( !pPlayer->GetSerial ().empty() && !IsSerialBanned ( pPlayer->GetSerial ().c_str () ) )
        {
            CBan* pBan = AddBan ( pBanner->GetNick(), strReason, tTimeOfUnban );
            pBan->SetNick ( pPlayer->GetNick() );
            pBan->SetSerial ( pPlayer->GetSerial () );
            SaveBanList ();

            return pBan;
        }
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( const SString& strSerial, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban )
{
    if ( /*IsValidSerial ( szSerial ) &&*/ !IsSerialBanned ( strSerial.c_str() ) )
    {
        CBan* pBan = AddBan ( pBanner->GetNick(), strReason, tTimeOfUnban );
        pBan->SetSerial ( strSerial );
        SaveBanList ();

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddAccountBan ( CPlayer* pPlayer, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        if ( !pPlayer->GetSerialUser ().empty() && !IsAccountBanned ( pPlayer->GetSerialUser ().c_str () ) )
        {
            CBan* pBan = AddBan ( pBanner->GetNick(), strReason, tTimeOfUnban );
            pBan->SetNick ( pPlayer->GetNick() );
            pBan->SetAccount ( pPlayer->GetSerialUser () );
            SaveBanList ();

            return pBan;
        }
    }

    return NULL;
}


CBan* CBanManager::AddAccountBan ( const SString& strAccount, CClient* pBanner, const SString& strReason, time_t tTimeOfUnban )
{
    if ( !IsAccountBanned ( strAccount.c_str() ) )
    {
        CBan* pBan = AddBan ( pBanner->GetNick(), strReason, tTimeOfUnban );
        pBan->SetSerial ( strAccount );
        SaveBanList ();

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddBan ( const SString& strBanner, const SString& strReason, time_t tTimeOfUnban )
{
    // Create the ban and assign its values
    CBan* pBan = new CBan;
    pBan->SetTimeOfBan ( time ( NULL ) );
    pBan->SetTimeOfUnban ( tTimeOfUnban );

    if ( strReason.length() > 0 )
        pBan->SetReason ( strReason.c_str() );

    if ( strBanner.length() > 0 )
        pBan->SetBanner ( strBanner );

    // Add it to the back of our banned list, add it to net server's ban list
    m_BanManager.push_back ( pBan );

    return pBan;
}


bool CBanManager::Exists ( CBan* pBan )
{
    return m_BanManager.Contains ( pBan );
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
        m_BanManager.remove ( pBan );
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


// Include wildcard checks
CBan* CBanManager::GetBanFromIP ( const char* szIP )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        bool bMatch = false, bFindNextOctet = false;
        int iBan = 0, iOctetCount = 1;
        const SString& strIP = (*iter)->GetIP ().c_str ();

        if ( strIP.Contains ( "*" ) )
        {
            for ( uint i = 0 ; i < 17 ; i++ )
            {
                char a = szIP[i];
                char b = strIP[iBan];
                if ( b == '.' )
                {
                    // End of octet so increment
                    iOctetCount++;
                }
                if ( bFindNextOctet )
                {
                    if ( a == '.' )
                    {
                        bFindNextOctet = false;
                        // Increment iBan to get to next octet as well
                        iBan++;
                    }
                }
                else if ( a == b )
                {
                    // matched char
                    // iBan can only be incremented at specific times or it won't work properly
                    iBan++;
                }
                else
                {
                    // iBan can only be incremented at specific times or it won't work properly
                    iBan++;
                    if ( !a )
                    {
                        // end of source IP range
                        if ( !b )
                        {
                            // end of banned IP range also, end check
                            break;
                        }
                        // banned range not ended so make false and end this check
                        bMatch = false;
                        break;
                    }
                    else if ( b == '*' )
                    {
                        // wildcard char
                        if ( iOctetCount == 4 )
                        {
                            // wildcard found at last octet, nothing further to check
                            break;
                        }
                        if ( a != '.' )
                        {
                            // need to find the next octet as we are still looking at an IP
                            bFindNextOctet = true;
                        }
                    }
                    else
                    {
                        // doesn't match so make false and end this check
                        bMatch = false;
                        break;
                    }
                }
            }
            if ( bMatch )
            {
                // ban found, return it
                return *iter;
            }
        }
        else
        {
            if ( strIP == szIP )
            {
                return *iter;   // Full match
            }
        }
    }
    // return NULL as no match found
    return NULL;
}


CBan* CBanManager::GetBanFromSerial ( const char* szSerial )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetSerial () == szSerial )
            return *iter;
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
    m_bAllowSave = true;

    // Create the XML
    CXMLFile* pFile = g_pServerInterface->GetXML ()->CreateXML ( m_strPath );
    if ( !pFile )
    {
        return false;
    }

    // Parse it
    if ( !pFile->Parse () )
    {
        delete pFile;
        if ( FileExists ( m_strPath ) )
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
    // Only allow save after a load was attempted
    if ( !m_bAllowSave )
        return;

    // Create the XML file
    CXMLFile* pFile = g_pServerInterface->GetXML ()->CreateXML ( m_strPath );
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
            if ( !pFile->Write () )
                CLogger::ErrorPrintf ( "Error saving '%s'\n", FILENAME_BANLIST );
        }

        // Delete the file pointer
        delete pFile;
    }
}


void CBanManager::SafeSetValue ( CXMLNode* pNode, const char* szKey, const std::string& strValue )
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
        if ( IsValidIPPart ( szIP1 ) &&
             IsValidIPPart ( szIP2 ) &&
             IsValidIPPart ( szIP3 ) &&
             IsValidIPPart ( szIP4 ) )
             return true;
        
    }

    return false;
}

bool CBanManager::IsValidIPPart ( const char* szIP )
{
    if ( IsNumericString ( szIP ) )
    {
        int iIP = atoi ( szIP );
        if ( iIP >= 0 && iIP < 256 )
            return true;
    }
    else if ( strcmp ( szIP, "*" ) == 0 )
        return true;

    return false;

}

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
}


CBanManager::~CBanManager ( void )
{
    SaveBanList ();
    RemoveAllBans ();
}


void CBanManager::DoPulse ( void )
{
    list < CBan* > ::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        time_t tTime = time ( NULL );

        if ( (*iter)->GetTimeOfUnban () )
        {
            if ( tTime >= (*iter)->GetTimeOfUnban () )
            {
                RemoveBan ( *iter );
                return;
            }
        }
    }
}


CBan* CBanManager::AddBan ( const char* szIP, bool bSaveList )
{
    if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) )
    {
        CBan* pBan = new CBan;
        pBan->SetIP ( szIP );

        m_BanManager.push_back ( pBan );
        g_pNetServer->AddBan ( szIP );

        if ( bSaveList )
        {
            SaveBanList ();
        }

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddBan ( CPlayer* pPlayer, CClient* pBanner, const char* szReason, time_t tTimeOfUnban )
{
    if ( pPlayer )
    {
        char szIP[256] = { '\0' };
        pPlayer->GetSourceIP ( szIP );

        if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) )
        {
            time_t rawtime = time(NULL);
            tm* time = localtime(&rawtime);

            char szDate[256] = { '\0' };
            _snprintf ( szDate, 256, "%d/%d/%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900 );
			szDate[255] = '\0';

            char szTime[256] = { '\0' };
            _snprintf ( szTime, 256, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec );
			szTime[255] = '\0';

            // Create the ban and assign its values
            CBan* pBan = new CBan;
            pBan->SetIP ( szIP );
            pBan->SetNick ( pPlayer->GetNick () );
            pBan->SetReason ( szReason );
            pBan->SetDateOfBan ( szDate );
            pBan->SetTimeOfBan ( szTime );
            pBan->SetTimeOfUnban ( tTimeOfUnban );

            // Eventually set the banner
            if ( pBanner )
            {
                pBan->SetBanner ( pBanner->GetNick () );
            }

            // Add it to the back of our banned list, add it to net server's ban list
            m_BanManager.push_back ( pBan );
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
    if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) && pBanner )
    {
        time_t rawtime = time(NULL);
        tm* time = localtime(&rawtime);

        char szDate[256] = { '\0' };
        _snprintf ( szDate, 256, "%d/%d/%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900 );
		szDate[255] = '\0';

        char szTime[256] = { '\0' };
        _snprintf ( szTime, 256, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec );
		szTime[255] = '\0';

        // Create the ban and assign its values
        CBan* pBan = new CBan;
        pBan->SetIP ( szIP );
        pBan->SetReason ( szReason );
        pBan->SetDateOfBan ( szDate );
        pBan->SetTimeOfBan ( szTime );
        pBan->SetTimeOfUnban ( tTimeOfUnban );

        // Eventually set the banner
        if ( pBanner )
        {
            pBan->SetBanner ( pBanner->GetNick () );
        }

        // Add it to the back of our banned list, add it to net server's ban list
        m_BanManager.push_back ( pBan );
        g_pNetServer->AddBan ( szIP );

        // Save the list
        SaveBanList ();
        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddBan ( const char* szIP, CPlayer* pBanner )
{
    if ( IsValidIP ( szIP ) && !IsSpecificallyBanned ( szIP ) && pBanner )
	{
		time_t rawtime = time(NULL);
        tm* time = localtime(&rawtime);

        char szDate[256] = { '\0' };
        _snprintf ( szDate, 256, "%d/%d/%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900 );
		szDate[255] = '\0';

        char szTime[256] = { '\0' };
        _snprintf ( szTime, 256, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec );
		szTime[255] = '\0';

        // Create the ban and assign its values
        CBan* pBan = new CBan;
        pBan->SetIP ( szIP );
        pBan->SetDateOfBan ( szDate );
        pBan->SetTimeOfBan ( szTime );

        // Eventually set the banner
        if ( pBanner )
        {
            pBan->SetBanner ( pBanner->GetNick () );
        }

        // Add it to the back of our banned list, add it to net server's ban list
        m_BanManager.push_back ( pBan );
        g_pNetServer->AddBan ( szIP );

        // Save the list
        SaveBanList ();
        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( const char* szSerial )
{
    if ( /*IsValidSerial ( szSerial ) &&*/ !IsSerialBanned ( szSerial ) )
	{
		time_t rawtime = time(NULL);
        tm* time = localtime(&rawtime);

        char szDate[256] = { '\0' };
        _snprintf ( szDate, 256, "%d/%d/%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900 );
		szDate[255] = '\0';

        char szTime[256] = { '\0' };
        _snprintf ( szTime, 256, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec );
		szTime[255] = '\0';

        // Create the ban and assign its values
        CBan* pBan = new CBan;
        pBan->SetSerial ( szSerial );
        pBan->SetDateOfBan ( szDate );
        pBan->SetTimeOfBan ( szTime );

        m_BanManager.push_back ( pBan );
        SaveBanList ();

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( const char* szSerial, const char* szReason, CClient* pBanner )
{
    if ( /*IsValidSerial ( szSerial ) &&*/ !IsSerialBanned ( szSerial ) )
	{
		time_t rawtime = time(NULL);
        tm* time = localtime(&rawtime);

        char szDate[256] = { '\0' };
        _snprintf ( szDate, 256, "%d/%d/%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900 );
		szDate[255] = '\0';

        char szTime[256] = { '\0' };
        _snprintf ( szTime, 256, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec );
		szTime[255] = '\0';

        // Create the ban and assign its values
        CBan* pBan = new CBan;
		if ( szReason )
			pBan->SetReason ( szReason );
        pBan->SetSerial ( szSerial );
        pBan->SetDateOfBan ( szDate );
        pBan->SetTimeOfBan ( szTime );

        // Eventually set the banner
        if ( pBanner )
        {
            pBan->SetBanner ( pBanner->GetNick () );
        }

        m_BanManager.push_back ( pBan );
        SaveBanList ();

        return pBan;
    }

    return NULL;
}


CBan* CBanManager::AddSerialBan ( const char* szSerial, const char* szReason )
{
    if ( /*IsValidSerial ( szSerial ) &&*/ !IsSerialBanned ( szSerial ) )
	{
		time_t rawtime = time(NULL);
        tm* time = localtime(&rawtime);

        char szDate[256] = { '\0' };
        _snprintf ( szDate, 256, "%d/%d/%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900 );
		szDate[255] = '\0';

        char szTime[256] = { '\0' };
        _snprintf ( szTime, 256, "%02d:%02d:%02d", time->tm_hour, time->tm_min, time->tm_sec );
		szTime[255] = '\0';

        // Create the ban and assign its values
        CBan* pBan = new CBan;
		if ( szReason )
			pBan->SetReason ( szReason );
        pBan->SetSerial ( szSerial );
        pBan->SetDateOfBan ( szDate );
        pBan->SetTimeOfBan ( szTime );

        m_BanManager.push_back ( pBan );
        SaveBanList ();

        return pBan;
    }

    return NULL;
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


void CBanManager::RemoveBan ( CBan* pBan )
{
    if ( pBan )
    {
        g_pNetServer->RemoveBan ( pBan->GetIP ().c_str () );
        if ( !m_BanManager.empty() ) m_BanManager.remove ( pBan );
        delete pBan;
    }
	SaveBanList ();
}


void CBanManager::RemoveBan ( const char* szIP )
{
    CBan* pBan = GetBan ( szIP );

    if ( pBan )
    {
        g_pNetServer->RemoveBan ( szIP );
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


CBan* CBanManager::GetSerialBan ( const char* szSerial )
{
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetSerial () == szSerial )
        {
            return *iter;
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


unsigned int CBanManager::GetBansWithDate ( const char* szDateOfBan )
{
    unsigned int uiOccurrances = 0;
    list < CBan* >::const_iterator iter = m_BanManager.begin ();
    for ( ; iter != m_BanManager.end (); iter++ )
    {
        if ( (*iter)->GetDateOfBan (), szDateOfBan )
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

    // Is the rootnode's name <map>?
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
            if ( pNode->GetTagName ().compare ( "ip" ) == 0 )
            {
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( "address" );

                if ( pAttribute )
                {
                    if ( !pAttribute->GetValue ().empty () )
                    {
                        CBan* pBan = AddBan ( pAttribute->GetValue ().c_str (), false );
                        CXMLNode* pSubNode = NULL;

                        if ( pBan )
                        {
                            unsigned int uiSubCount = pNode->GetSubNodeCount ();

                            for ( unsigned int ui = 0; ui < uiSubCount; ui++ )
                            {
                                pSubNode = pNode->GetSubNode ( ui );

                                if ( pSubNode )
                                {
                                    if ( pSubNode->GetTagName ().compare ( "nick" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetNick ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
									if ( pSubNode->GetTagName ().compare ( "serial" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetSerial ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "banner" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetBanner ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "reason" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetReason ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "date" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetDateOfBan ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "time" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetTimeOfBan ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "unban" ) == 0 )
                                    {
                                        int iTime = 0;
                                        pSubNode->GetTagContent ( iTime );

                                        pBan->SetTimeOfUnban ( iTime );
                                    }
                                }
                            }
                        }
                    }
                }
			}

            else if ( pNode->GetTagName ().compare ( "serial" ) == 0 )
            {
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( "value" );

                if ( pAttribute )
                {
                    if ( !pAttribute->GetValue ().empty () )
                    {
                        CBan* pBan = AddSerialBan ( pAttribute->GetValue ().c_str () );
                        CXMLNode* pSubNode = NULL;

                        if ( pBan )
                        {
                            unsigned int uiSubCount = pNode->GetSubNodeCount ();

                            for ( unsigned int ui = 0; ui < uiSubCount; ui++ )
                            {
                                pSubNode = pNode->GetSubNode ( ui );

                                if ( pSubNode )
                                {
                                    if ( pSubNode->GetTagName ().compare ( "nick" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetNick ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "banner" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetBanner ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "reason" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetReason ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "date" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetDateOfBan ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "time" ) == 0 )
                                    {
                                        if ( !pSubNode->GetTagContent ().empty () )
                                        {
                                            pBan->SetTimeOfBan ( pSubNode->GetTagContent ().c_str () );
                                        }
                                    }
                                    else if ( pSubNode->GetTagName ().compare ( "unban" ) == 0 )
                                    {
                                        int iTime = 0;
                                        pSubNode->GetTagContent ( iTime );

                                        pBan->SetTimeOfUnban ( iTime );
                                    }
                                }
                            }
                        }
                    }
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
                pNode = NULL;

                // We got an IP to ban? Create an IP node
                const std::string& strIP = (*iter)->GetIP ();
				if ( strIP.length () > 0 )
				{
					// Create the new 'ip' node
					pNode = pRootNode->CreateSubNode ( "ip" );
					if ( pNode )
					{
						// Create the address attribute
						CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( "address" );
						if ( pAttribute )
						{
							// Add it to the attribute
							pAttribute->SetValue ( strIP.c_str () );
                        }
                    }
                }
                else
                {
                    // Got a serial ban to write?
                    const std::string& strSerial = (*iter)->GetSerial ();
                    if ( strSerial.length () > 0 )
                    {
                        // Create the new 'ip' node
                        pNode = pRootNode->CreateSubNode ( "serial" );
                        if ( pNode )
                        {
                            // Create the address attribute
                            CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( "value" );
                            if ( pAttribute )
                            {
                                // Add it to the attribute
                                pAttribute->SetValue ( strSerial.c_str () );
                            }
                        }
                    }

                    // Did we create a node successfully?
                    if ( pNode )
                    {
						// Create subnode pointer
						CXMLNode* pSubNode = NULL;

						// Create subnodes here
                        const std::string& strNick = (*iter)->GetNick ();
						if ( strNick.length () > 0 )
						{
							pSubNode = pNode->CreateSubNode ( "nick" );
							if ( pSubNode )
							{
								pSubNode->SetTagContent ( strNick.c_str () );
							}
						}

                        const std::string& strSerial = (*iter)->GetSerial ();
						if ( strSerial.length () > 0 )
						{
							pSubNode = pNode->CreateSubNode ( "serial" );
							if ( pSubNode )
							{
								pSubNode->SetTagContent ( strSerial.c_str () );
							}
						}

                        const std::string& strBanner = (*iter)->GetBanner ();
						if ( strBanner.length () > 0 )
						{
							pSubNode = pNode->CreateSubNode ( "banner" );
							if ( pSubNode )
							{
								pSubNode->SetTagContent ( strBanner.c_str () );
							}
						}

                        const std::string& strReason = (*iter)->GetReason ();
						if ( strReason.length () > 0 )
						{
							pSubNode = pNode->CreateSubNode ( "reason" );
							if ( pSubNode )
							{
								pSubNode->SetTagContent ( strReason.c_str () );
							}
						}

                        const std::string& strDate = (*iter)->GetDateOfBan ();
						if ( strDate.length () > 0 )
						{
							pSubNode = pNode->CreateSubNode ( "date" );
							if ( pSubNode )
							{
								pSubNode->SetTagContent ( strDate.c_str () );
							}
						}

                        const std::string& strTimeOfBan = (*iter)->GetTimeOfBan ();
						if ( strTimeOfBan.length () > 0 )
						{
							pSubNode = pNode->CreateSubNode ( "time" );
							if ( pSubNode )
							{
								pSubNode->SetTagContent ( strTimeOfBan.c_str () );
							}
						}

						if ( (*iter)->GetTimeOfUnban () )
						{
							pSubNode = pNode->CreateSubNode ( "unban" );

							if ( pSubNode )
							{
								pSubNode->SetTagContent ( (unsigned int) (*iter)->GetTimeOfUnban () );
							}
						}
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

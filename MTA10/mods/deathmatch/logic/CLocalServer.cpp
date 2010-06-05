/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLocalServer.cpp
*  PURPOSE:     Local server setup GUI
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

#ifndef snprintf
#define snprintf _snprintf
#endif

extern CCoreInterface* g_pCore;
extern CClientGame* g_pClientGame;

CLocalServer::CLocalServer ( const char* szConfig )
{
    m_strConfig = szConfig;
    m_pConfig = NULL;

    m_pGUI = g_pCore->GetGUI();

    m_pWindow = reinterpret_cast < CGUIWindow* > ( m_pGUI->CreateWnd ( NULL, "HOST GAME" ) );
    m_pWindow->SetMovable ( true );

    CVector2D resolution = m_pGUI->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 360.0f / 2, resolution.fY / 2 - 440.0f / 2 + yoff  ), false );
    m_pWindow->SetSize ( CVector2D ( 360.0f, 440.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetVisible ( true );

    m_pTabs = reinterpret_cast < CGUITabPanel* > ( m_pGUI->CreateTabPanel ( m_pWindow ) );
	m_pTabs->SetPosition ( CVector2D ( 0.0f, 0.06f ), true );
	m_pTabs->SetSize ( CVector2D ( 1.0f, 0.85f ), true );
	m_pTabGeneral = m_pTabs->CreateTab ( "General" );
    //m_pTabs->CreateTab ( "Gamemode" );
    m_pTabResources = m_pTabs->CreateTab ( "Resources" );

    m_pLabelName = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Server name:" ) );
    m_pLabelName->SetPosition ( CVector2D ( 0.03f, 0.07f ), true );
	m_pLabelName->AutoSize ( "Server name:" );

    m_pEditName = reinterpret_cast < CGUIEdit* > ( m_pGUI->CreateEdit ( m_pTabGeneral, "Default MTA Server" ) );
    m_pEditName->SetPosition ( CVector2D ( 0.4f, 0.06f ), true );
    m_pEditName->SetSize ( CVector2D ( 0.57f, 0.06f ), true );
    m_pEditName->SetMaxLength ( 64 );

    m_pLabelPass = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Password:" ) );
    m_pLabelPass->SetPosition ( CVector2D ( 0.03f, 0.14f ), true );
	m_pLabelPass->AutoSize ( "Password:" );

    m_pEditPass = reinterpret_cast < CGUIEdit* > ( m_pGUI->CreateEdit ( m_pTabGeneral ) );
    m_pEditPass->SetPosition ( CVector2D ( 0.4f, 0.13f ), true );
    m_pEditPass->SetSize ( CVector2D ( 0.57f, 0.06f ), true );
    m_pEditPass->SetMaxLength ( 8 );

    m_pLabelPlayers = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Max players:" ) );
    m_pLabelPlayers->SetPosition ( CVector2D ( 0.03f, 0.21f ), true );
	m_pLabelPlayers->AutoSize ( "Max players:" );

    m_pEditPlayers = reinterpret_cast < CGUIEdit* > ( m_pGUI->CreateEdit ( m_pTabGeneral ) );
    m_pEditPlayers->SetPosition ( CVector2D ( 0.4f, 0.20f ), true );
    m_pEditPlayers->SetSize ( CVector2D ( 0.17f, 0.06f ), true );
    m_pEditPlayers->SetMaxLength ( 3 );

    m_pLabelBroadcast = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Broadcast:" ) );
    m_pLabelBroadcast->SetPosition ( CVector2D ( 0.03f, 0.35f ), true );
	m_pLabelBroadcast->AutoSize ( "Broadcast:" );

    m_pBroadcastLan = reinterpret_cast < CGUICheckBox* > ( m_pGUI->CreateCheckBox ( m_pTabGeneral, "LAN", true ) );
    m_pBroadcastLan->SetPosition ( CVector2D ( 0.4f, 0.33f ), true );
	m_pBroadcastLan->SetSize ( CVector2D ( 0.45f, 0.08f ), true );

    m_pBroadcastInternet = reinterpret_cast < CGUICheckBox* > ( m_pGUI->CreateCheckBox ( m_pTabGeneral, "Internet", true ) );
    m_pBroadcastInternet->SetPosition ( CVector2D ( 0.4f, 0.38f ), true );
	m_pBroadcastInternet->SetSize ( CVector2D ( 0.45f, 0.08f ), true );

    m_pResourcesCur = reinterpret_cast < CGUIGridList* > ( m_pGUI->CreateGridList ( m_pTabResources, false ) );
    m_pResourcesCur->SetPosition ( CVector2D ( 0.03f, 0.06f ), true );
    m_pResourcesCur->SetSize ( CVector2D ( 0.45f, 0.5f ), true );
	m_pResourcesCur->SetSorting ( false );
	m_pResourcesCur->SetSelectionMode ( CGUIGridList::SelectionMode::CellSingle );
    m_hResourcesCur = m_pResourcesCur->AddColumn ( "Selected", 0.80f );

    m_pResourceDel = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pTabResources, ">" ) );
    m_pResourceDel->SetPosition ( CVector2D ( 0.03f, 0.65f ), true );
    m_pResourceDel->SetSize ( CVector2D ( 0.45f, 0.05f ), true );

    m_pResourceAdd = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pTabResources, "<" ) );
    m_pResourceAdd->SetPosition ( CVector2D ( 0.03f, 0.58f ), true );
    m_pResourceAdd->SetSize ( CVector2D ( 0.45f, 0.05f ), true );

    m_pResourcesAll = reinterpret_cast < CGUIGridList* > ( m_pGUI->CreateGridList ( m_pTabResources, false ) );
    m_pResourcesAll->SetPosition ( CVector2D ( 0.52f, 0.06f ), true );
    m_pResourcesAll->SetSize ( CVector2D ( 0.45f, 0.9f ), true );
	m_pResourcesAll->SetSorting ( false );
	m_pResourcesAll->SetSelectionMode ( CGUIGridList::SelectionMode::CellSingle );
    m_hResourcesAll = m_pResourcesAll->AddColumn ( "All", 0.80f );

    m_pButtonStart = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pWindow, "Start" ) );
    m_pButtonStart->SetPosition ( CVector2D ( 0.33f, 0.93f ), true );
    m_pButtonStart->SetSize ( CVector2D ( 0.3f, 0.05f ), true );

    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 0.65f, 0.93f ), true );
    m_pButtonCancel->SetSize ( CVector2D ( 0.3f, 0.05f ), true );

    m_pResourceAdd->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnAddButtonClick, this ) );
    m_pResourceDel->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnDelButtonClick, this ) );
    m_pButtonStart->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnStartButtonClick, this ) );
    m_pButtonCancel->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnCancelButtonClick, this ) );

    Load();
}


CLocalServer::~CLocalServer ( void )
{
    if ( m_pConfig )
        delete m_pConfig;
    delete m_pWindow;
}


bool CLocalServer::OnAddButtonClick ( CGUIElement *pElement )
{
	CGUIListItem *pItem = m_pResourcesAll->GetSelectedItem ();
    if ( pItem )
    {
        m_pResourcesCur->SetItemText ( m_pResourcesCur->AddRow (), m_hResourcesCur, &pItem->GetText().c_str()[3] );
        m_pResourcesAll->RemoveRow ( m_pResourcesAll->GetItemRowIndex ( pItem ) );
    }
    return true;
}


bool CLocalServer::OnDelButtonClick ( CGUIElement *pElement )
{
	CGUIListItem *pItem = m_pResourcesCur->GetSelectedItem ();
    if ( pItem )
    {
        m_pResourcesAll->SetItemText ( m_pResourcesAll->AddRow (), m_hResourcesAll, &pItem->GetText().c_str()[3] );
        m_pResourcesCur->RemoveRow ( m_pResourcesCur->GetItemRowIndex ( pItem ) );
    }
    return true;
}


bool CLocalServer::OnStartButtonClick ( CGUIElement *pElement )
{
    Save();
	// Connect

    g_pClientGame->StartLocalGame ( m_strConfig.c_str(), m_pEditPass->GetText().c_str() );

    return true;
}


bool CLocalServer::OnCancelButtonClick ( CGUIElement *pElement )
{
    g_pCore->GetModManager ()->RequestUnload ();
    return true;
}

bool CLocalServer::Load ( void )
{
    // Get server module root
    SString strServerPath = g_pCore->GetInstallRoot ();
    strServerPath += "/server/mods/deathmatch";

    SString strConfigPath ( "%s/%s", strServerPath.c_str (), m_strConfig.c_str () );
    m_pConfig = g_pCore->GetXML ()->CreateXML ( strConfigPath );
    if ( m_pConfig && m_pConfig->Parse() )
    {
        CXMLNode* pRoot = m_pConfig->GetRootNode();
        CXMLNode* pServerName = pRoot->FindSubNode ( "servername", 0 );
        if ( pServerName ) m_pEditName->SetText ( pServerName->GetTagContent().c_str() );
        CXMLNode* pServerPass = pRoot->FindSubNode ( "password", 0 );
        if ( pServerPass ) m_pEditPass->SetText ( pServerPass->GetTagContent().c_str() );
        CXMLNode* pServerPlayers = pRoot->FindSubNode ( "maxplayers", 0 );
        if ( pServerPlayers ) m_pEditPlayers->SetText ( pServerPlayers->GetTagContent().c_str() );

        // Read the startup resources
        list < CXMLNode* > ::const_iterator iter = pRoot->ChildrenBegin ();
        for ( ; iter != pRoot->ChildrenEnd (); iter++ )
        {
            CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( *iter );
            if ( pNode->GetTagName ().compare ( "resource" ) == 0 )
            {
                CXMLAttribute* src = pNode->GetAttributes().Find ( "src" );
                if ( src && src->GetValue()[1] )
                {
                    m_pResourcesCur->SetItemText ( m_pResourcesCur->AddRow (), m_hResourcesCur, src->GetValue().c_str() );
                }
            }
        }
    }
    //

    SString strResourceDirectoryPath ( "%s/resources/*", strServerPath.c_str () );

    unsigned int uiCount = 0;

    #ifdef WIN32

        // Find all .map files in the maps folder
        WIN32_FIND_DATA FindData;
        HANDLE hFind = FindFirstFile ( strResourceDirectoryPath, &FindData );
        if ( hFind != INVALID_HANDLE_VALUE )
        {
            // Remove the extension and store the time
            FindData.cFileName [ strlen ( FindData.cFileName ) - 4 ] = 0;
            // Add each file
            do
            {
                if ( strcmp ( FindData.cFileName, ".." ) != 0 && strcmp ( FindData.cFileName, "." ) != 0 )
                {
                    char * extn = NULL;
                    if ( ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY )
                    {
                        extn = &FindData.cFileName [ strlen ( FindData.cFileName ) - 3 ];
                        FindData.cFileName [ strlen ( FindData.cFileName ) - 4 ] = 0;
                    }

                    if ( extn == NULL || strcmp ( extn, "zip" ) == 0 )
                    {
                        // Add the resource
                        HandleResource ( FindData.cFileName );

                        // Increment the counter
                        uiCount++;
                    }
                }
            } while ( FindNextFile ( hFind, &FindData ) );

            // Close the search
            FindClose ( hFind );
        }
    #else
        DIR *Dir;
		struct dirent *DirEntry;
		time_t llHighestTime = 0;
		char szPath[MAX_PATH] = {0};

		if ( ( Dir = opendir ( strResourceDirectoryPath ) ) )
		{
			while ( ( DirEntry = readdir ( Dir ) ) != NULL )
			{
                // Skip . and .. entry
                if ( strcmp ( DirEntry->d_name, "." ) != 0 && 
                     strcmp ( DirEntry->d_name, ".." ) != 0 )
                {
				    struct stat Info;
				    bool bDir = false;

				    // Get the path
				    if ( strlen(szBuffer) + strlen(DirEntry->d_name) < MAX_PATH )
                    {
					    strcpy ( szPath, szBuffer );
					    unsigned long ulPathLength = strlen ( szPath );

					    if ( szPath [ ulPathLength-1 ] != '/') strcat ( szPath, "/" );

					    strcat ( szPath, DirEntry->d_name );

					    // Determine the file stats
					    if ( lstat ( szPath, &Info ) != -1 )
						    bDir = S_ISDIR ( Info.st_mode );
					    else
						    CLogger::ErrorPrintf ( "Unable to stat %s\n", szPath );

				        // Chop off the extension if it's not a dir
				        char * extn = NULL;
				        if ( !bDir )
                        {
					        extn = &(DirEntry->d_name [ strlen ( DirEntry->d_name ) - 3 ]);
					        DirEntry->d_name [ strlen ( DirEntry->d_name ) - 4 ] = 0;
				        }
                        if ( extn == NULL || strcmp ( extn, "zip" ) == 0 )
                        {
                            // Add the resource
                            HandleResource ( DirEntry->d_name );

                            // Increment the counter
                            uiCount++;
                        }

				    }
                }


			}

			// Close the directory handle
			closedir ( Dir );
		}
    #endif
    return true;
}

bool CLocalServer::Save ( void )
{
    if ( m_pConfig && m_pConfig->GetRootNode() )
    {
        StoreConfigValue ( "servername", ( m_pEditName->GetText().length() > 0 ) ? m_pEditName->GetText().c_str() : "MTA Local Server" );
        StoreConfigValue ( "maxplayers", ( atoi ( m_pEditPlayers->GetText().c_str() ) ) ? m_pEditPlayers->GetText().c_str() : "32" );
        StoreConfigValue ( "donotbroadcastlan", ( m_pBroadcastLan->IsActive () ) ? "0" : "1" );
        StoreConfigValue ( "ase", ( m_pBroadcastInternet->IsActive () ) ? "1" : "0" );
        StoreConfigValue ( "password", m_pEditPass->GetText().c_str() );

        // Remove old resources from the config
        CXMLNode* pRoot = m_pConfig->GetRootNode();
        list < CXMLNode* > ::const_iterator iter = pRoot->ChildrenBegin ();
        for ( ; iter != pRoot->ChildrenEnd (); iter++ )
        {
            CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( *iter );
            if ( pNode->GetTagName().compare ( "resource" ) == 0 )
            {
                pRoot->DeleteSubNode ( pNode );
                iter = pRoot->ChildrenBegin ();
            }
        }

        // Add new resources to the config
        for ( int i = 0; i < m_pResourcesCur->GetRowCount(); i++ )
        {
            CXMLNode* pResourceNode = pRoot->CreateSubNode ( "resource" );
            pResourceNode->GetAttributes().Create ( "src" )->SetValue ( m_pResourcesCur->GetItemText ( i, 1 ) );
            pResourceNode->GetAttributes().Create ( "startup" )->SetValue ( "1" );
            pResourceNode->GetAttributes().Create ( "protected" )->SetValue ( "0" );
        }
        m_pConfig->Write ();
    }
    return true;
}


void CLocalServer::StoreConfigValue ( const char* szNode, const char* szValue )
{
    CXMLNode* pRoot = m_pConfig->GetRootNode();
    CXMLNode* pNode = pRoot->FindSubNode ( szNode, 0 );
    if ( pNode )
    {
        pNode->SetTagContent ( szValue );
    }
    else
    {
        pNode = pRoot->CreateSubNode ( szNode );
        pNode->SetTagContent ( szValue );
    }
}

void CLocalServer::HandleResource ( char* szResource )
{
    for ( int i = 0; i < m_pResourcesCur->GetRowCount(); i++ )
    {
        if ( strcmp ( szResource, m_pResourcesCur->GetItemText ( i, 1 ) ) == 0 )
            return;
    }
	m_pResourcesAll->SetItemText ( m_pResourcesAll->AddRow (), m_hResourcesAll, szResource );
}
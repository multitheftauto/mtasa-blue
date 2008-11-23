/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainConfig.cpp
*  PURPOSE:     Configuration parsing and querying
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/* (IJs) This class should be redesigned and rewritten from scratch by using
   the well known cvars approach and one set of global query functions instead
   of one set of query functions per variable. Unnecessary and redundant. */

#include "StdInc.h"

#ifndef snprintf
#define snprintf _snprintf
#endif

extern CCore* g_pCore;

CMainConfig::CMainConfig ( char* szFileName ): CXMLConfig ( szFileName )
{
	m_bLoaded = false;

    // Default settings
	iMenuOptions = CMainMenu::GetMenuOptionsDefault ();

	strCommunityUsername = "";
	strCommunityPassword = "";

    strNick = "Player";
    strHost = "127.0.0.1";
    usPort = 22003;
    strPassword = "";
    qc_strHost = "127.0.0.1";
    qc_usPort = 22003;
    qc_strPassword = "";
    strDebugFileName = "";
    vecConsolePosition = CVector2D ( 0.20f, 0.20f );
    
	usFPSLimit = 100;

    ucChatFont = 1;
    ucChatLines = 7;
    SetChatColor ( 0, 0, 128, 100 );
    SetChatInputColor ( 0, 0, 191, 110 );
    SetChatInputPrefixColor ( 172, 213, 254, 255 );
    SetChatInputTextColor ( 172, 213, 254, 255 );
    vecChatScale = CVector2D ( 1.0f, 1.0f );
    fChatWidth = 1.0f;
    bChatCssStyleText = false;
    bChatCssStyleBackground = false;
    ulChatLineLife = 12000;
    ulChatLineFadeOut = 3000;
    bChatUseCEGUI = false;
    fTextScale = 1.0f;
    bInvertMouse = false;
    bSteerWithMouse = false;
    bFlyWithMouse = false;
}


CMainConfig::~CMainConfig ( void )
{
    Save ();
}


bool CMainConfig::Load ( char* szFileName )
{
    if ( !szFileName ) szFileName = m_szFileName;

    bool bXMLSuccess = false;

    CXMLNode* pKeyBindsNode = NULL;

    if ( szFileName && szFileName [ 0 ] )
    {
        m_pFile = g_pCore->GetXML ()->CreateXML ( szFileName );
        if ( m_pFile )
        {
            if ( m_pFile->Parse () )
            {
                CXMLNode* pRootNode = m_pFile->GetRootNode ();
                if ( pRootNode )
                {
                    bXMLSuccess = true;

                    LoadSettings ( pRootNode->FindSubNode ( "settings" ) );
                    pKeyBindsNode = pRootNode->FindSubNode ( "binds" );

                    CServerBrowser *pBrowser = CServerBrowser::GetSingletonPtr ();
                    LoadServerList ( pRootNode->FindSubNode ( "favourite_servers" ),        CONFIG_FAVOURITE_LIST_TAG,  pBrowser->GetFavouritesList () );
                    LoadServerList ( pRootNode->FindSubNode ( "recently_played_servers" ),  CONFIG_RECENT_LIST_TAG,     pBrowser->GetRecentList () );
                }
            }
        }
    }

    // Load the sub-configs, use defaults with a NULL node
    g_pCore->GetKeyBinds ()->LoadFromXML ( pKeyBindsNode );
	m_bLoaded = true;

    return bXMLSuccess;
}


bool CMainConfig::LoadServerList ( CXMLNode* pNode, std::string strTagName, CServerList *pList )
{
    CXMLNode* pSubNode = NULL;
    in_addr Address;
    int iPort;

    if ( !pNode ) return false;

    // Loop through all subnodes looking for relevant nodes
    unsigned int uiCount = pNode->GetSubNodeCount ();
    for ( unsigned int i = 0; i < uiCount; i++ )
    {
        pSubNode = pNode->GetSubNode ( i );
        if ( pSubNode && pSubNode->GetTagName ().compare ( strTagName ) == 0 )
        {
            // This node is relevant, so get the attributes we need and add it to the list
            CXMLAttribute* pHostAttribute = pSubNode->GetAttributes ().Find ( "host" );
            CXMLAttribute* pPortAttribute = pSubNode->GetAttributes ().Find ( "port" );
            if ( pHostAttribute && pPortAttribute ) {
                CServerListItem::Parse ( pHostAttribute->GetValue (), Address );
                iPort = atoi ( pPortAttribute->GetValue ().c_str () ) + SERVER_LIST_QUERY_PORT_OFFSET;
                if ( iPort > 0 )
                    pList->Add ( CServerListItem ( Address, iPort ) );
            }
        }
    }
    return true;
}


bool CMainConfig::SaveServerList ( CXMLNode* pNode, std::string strTagName, CServerList *pList )
{
    if ( !pNode ) return false;

    // Start by clearing out all previous nodes
    pNode->DeleteAllSubNodes ();

    // Iterate through the list, adding any items to our node
    CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
    int j = 0;
    int k = pList->GetServerCount ();
    if ( k > 0 ) {
        for ( CServerListIterator i = i_b; i != i_e; i++ ) {
            CServerListItem * pServer = *i;

            // Add the (scanned) item to the node
            if ( pServer->bScanned ) {
                CXMLNode * pSubNode = pNode->CreateSubNode ( strTagName.c_str () );
                if ( pSubNode )
                {
                    CXMLAttribute* pHostAttribute = pSubNode->GetAttributes ().Create ( "host" );
					pHostAttribute->SetValue ( pServer->strHost.c_str () );
                    
                    CXMLAttribute* pPortAttribute = pSubNode->GetAttributes ().Create ( "port" );
					pPortAttribute->SetValue ( pServer->usGamePort );
                }
            }
            j++;
        }
    }
    return true;
}

bool CMainConfig::Save ( char* szFileName )
{
    if ( !szFileName ) szFileName = m_szFileName;
    if ( !szFileName || !szFileName [ 0 ] ) return false;
    if ( !m_pFile )
    {
        m_pFile = g_pCore->GetXML ()->CreateXML ( szFileName );
        if ( !m_pFile ) return false;
    }

    CXMLNode* pRootNode = m_pFile->GetRootNode ();
    if ( !pRootNode )
    {
        pRootNode = m_pFile->CreateRootNode ( "mainconfig" );
        if ( !pRootNode ) return false;
    }

    CXMLNode* pSettingsNode = GetNodeForSave ( pRootNode, "settings" ); 
    if ( pSettingsNode ) SaveSettings ( pSettingsNode );

    CXMLNode* pBindsNode = GetNodeForSave ( pRootNode, "binds" );
    if ( pBindsNode ) g_pCore->GetKeyBinds ()->SaveToXML ( pBindsNode );

    // Server lists
    CServerBrowser *pBrowser = CServerBrowser::GetSingletonPtr ();
    SaveServerList ( GetNodeForSave ( pRootNode, "favourite_servers" ),         CONFIG_FAVOURITE_LIST_TAG,  pBrowser->GetFavouritesList () );
    SaveServerList ( GetNodeForSave ( pRootNode, "recently_played_servers" ),   CONFIG_RECENT_LIST_TAG,     pBrowser->GetRecentList () );

    m_pFile->Write ();

    return true;
}

bool CMainConfig::LoadSettings ( CXMLNode* pNode )
{
    if ( !pNode ) return false;

    std::string strValue;

    char * szKey = "nick";
#ifdef MTA_DEBUG
    szKey = "nick_d";
#endif

    if ( GetString ( pNode, szKey, strValue ) == IS_SUCCESS )
    {
        // Swap char 28s with 39s for So'lide
        /*
        for ( unsigned int i = 0 ; i < strValue.length (); i++ )
        {
            if ( szTemp [ i ] == 28 )
                szTemp [ i ] = 39;
        }
        */
        strNick = strValue;
    }
    if ( GetString ( pNode, "host", strValue ) == IS_SUCCESS )
        strHost = strValue;
    int iTemp;
    if ( GetInteger ( pNode, "port", iTemp ) == IS_SUCCESS )
        usPort = ( unsigned short ) iTemp;
    if ( GetString ( pNode, "password", strValue ) == IS_SUCCESS )
        strPassword = strValue;

    if ( GetString ( pNode, "community_username", strValue ) == IS_SUCCESS )
        strCommunityUsername = strValue;
    if ( GetString ( pNode, "community_password", strValue ) == IS_SUCCESS )
        strCommunityPassword = strValue;

	if ( GetString ( pNode, "qc_host", strValue ) == IS_SUCCESS )
        qc_strHost = strValue;        
    if ( GetInteger ( pNode, "qc_port", iTemp ) == IS_SUCCESS )
        qc_usPort = ( unsigned short ) iTemp;
    if ( GetString ( pNode, "qc_password", strValue ) == IS_SUCCESS )
        qc_strPassword = strValue;

    if ( GetString ( pNode, "debugfile", strValue) == IS_SUCCESS )
	{
		if ( stricmp ( strValue.c_str(), "true" ) == 0 || stricmp ( strValue.c_str(), "yes" ) == 0 )
		{
			char szBuffer [MAX_PATH + 1];
            char szFileName [MAX_PATH + 1];
			g_pCore->GetModInstallRoot ( "deathmatch", szBuffer, MAX_PATH );
			snprintf ( szFileName, MAX_PATH, "%s\\debugscript.log", szBuffer );
			strDebugFileName = szBuffer;
		}
	}

    szKey = "console_pos";
#ifdef MTA_DEBUG
    szKey = "console_pos_d";
#endif
    if ( GetString ( pNode, szKey, strValue ) == IS_SUCCESS )
    {
        CVector2D vec;
        std::istringstream iss ( strValue );

        try {
            iss >> vec.fX >> vec.fY;
            vecConsolePosition = vec;
        } catch(std::ios::failure e) {}
    }
    szKey = "console_size";
#ifdef MTA_DEBUG
    szKey = "console_size_d";
#endif
    if ( GetString ( pNode, szKey, strValue ) == IS_SUCCESS )
    {
        CVector2D vec;
        std::istringstream iss ( strValue );

        try {
            iss >> vec.fX >> vec.fY;
            vecConsoleSize = vec;
        } catch(std::ios::failure e) {}
    }
    else
    {
        CVector2D ScreenSize = g_pCore->GetGUI ()->GetResolution ();
        vecConsoleSize = CVector2D ( 550.0f * ScreenSize.fX / 1024.0f /*NATIVE_RES_X*/, 360.0f * ScreenSize.fX / 768.0f /*NATIVE_RES_Y*/ );
    }
    if ( GetInteger ( pNode, "chat_font", iTemp ) == IS_SUCCESS )
        ucChatFont = ( unsigned char ) iTemp;
    if ( GetInteger ( pNode, "chat_lines", iTemp ) == IS_SUCCESS )
        ucChatLines = ( unsigned char ) iTemp;
    unsigned char R, G, B, A;
    if ( GetRGBA ( pNode, "chat_color", R, G, B, A ) == IS_SUCCESS )
        SetChatColor ( R, G, B, A );
    if ( GetRGBA ( pNode, "chat_input_color", R, G, B, A ) == IS_SUCCESS )
        SetChatInputColor ( R, G, B, A );
    if ( GetString ( pNode, "chat_scale", strValue ) == IS_SUCCESS )
    {
        CVector2D vec;
        std::istringstream iss ( strValue );

        try {
            iss >> vec.fX >> vec.fY;
            vecChatScale = vec;
        } catch(std::ios::failure e) {}
    }
    if ( GetRGBA ( pNode, "chat_input_prefix_color", R, G, B, A ) == IS_SUCCESS )
        SetChatInputPrefixColor ( R, G, B, A );
    if ( GetRGBA ( pNode, "chat_input_text_color", R, G, B, A ) == IS_SUCCESS )
        SetChatInputTextColor ( R, G, B, A );
    if ( GetString ( pNode, "chat_width", strValue ) == IS_SUCCESS )
        fChatWidth = ( float ) atof ( strValue.c_str() );
    bool bTemp;
    if ( GetBoolean ( pNode, "chat_css_style_text", bTemp ) )
        bChatCssStyleText = bTemp;
    if ( GetBoolean ( pNode, "chat_css_style_background", bTemp ) )
        bChatCssStyleBackground = bTemp;
    if ( GetInteger ( pNode, "chat_line_life", iTemp ) )
        ulChatLineLife = ( unsigned long ) iTemp;
    if ( GetInteger ( pNode, "chat_line_fade_out", iTemp ) )
        ulChatLineFadeOut = ( unsigned long ) iTemp;
    if ( GetBoolean ( pNode, "chat_use_cegui", bTemp ) )
        bChatUseCEGUI = bTemp;
	if ( GetInteger ( pNode, "mtu_size", iTemp ) )
		usMTUSize = ( unsigned short ) iTemp;
	if ( GetInteger ( pNode, "fps_limit", iTemp ) == IS_SUCCESS )
		if ( iTemp >= 20 && iTemp <= 100 ) usFPSLimit = ( unsigned short ) iTemp;
	if ( GetInteger ( pNode, "menu_options", iTemp ) )
		iMenuOptions = iTemp;
    if ( GetString ( pNode, "text_scale", strValue ) == IS_SUCCESS )
        fTextScale = ( float ) atof ( strValue.c_str() );
    if ( GetBoolean ( pNode, "invert_mouse", bTemp ) )
        bInvertMouse = bTemp;
    if ( GetBoolean ( pNode, "steer_with_mouse", bTemp ) )
        bSteerWithMouse = bTemp;
    if ( GetBoolean ( pNode, "fly_with_mouse", bTemp ) )
        bFlyWithMouse = bTemp;

    return true;
}


bool CMainConfig::SaveSettings ( CXMLNode* pNode )
{
    // Swap char 28s with 5s for So'lide
    char szNick [ 64 ];
    strcpy ( szNick, strNick.c_str () );
    for ( unsigned int i = 0 ; i < strlen ( szNick ) ; i++ )
    {
        if ( szNick [ i ] == 39 )
            szNick [ i ] = 28;
    } 
    char * szKey = "nick";
#ifdef MTA_DEBUG
    szKey = "nick_d";
#endif
    SetString ( pNode, szKey, szNick );
    SetString ( pNode, "host", strHost.c_str () );
    SetInteger ( pNode, "port", ( int ) usPort );
    SetString ( pNode, "password", strPassword.c_str () );

    SetString ( pNode, "community_username", strCommunityUsername.c_str () );
    SetString ( pNode, "community_password", strCommunityPassword.c_str () );

	SetString ( pNode, "qc_host", qc_strHost.c_str () );
    SetInteger ( pNode, "qc_port", ( int ) qc_usPort );
    SetString ( pNode, "qc_password", qc_strPassword.c_str () );

    SetString ( pNode, "debugfile", strDebugFileName.c_str () );
    char szTemp [ 256 ];
    sprintf ( szTemp, "%f %f", vecConsolePosition.fX, vecConsolePosition.fY );

szKey = "console_pos";
#ifdef MTA_DEBUG
    szKey = "console_pos_d";
#endif
    SetString ( pNode, szKey, szTemp );
    sprintf ( szTemp, "%f %f", vecConsoleSize.fX, vecConsoleSize.fY );

    szKey = "console_size";
#ifdef MTA_DEBUG
    szKey = "console_size_d";
#endif
    SetString ( pNode, szKey, szTemp );
    SetInteger ( pNode, "chat_font", ( int ) ucChatFont );
    SetInteger ( pNode, "chat_lines", ( int ) ucChatLines );
    SetRGBA ( pNode, "chat_color", ucChatColorR, ucChatColorG, ucChatColorB, ucChatColorA );
    SetRGBA ( pNode, "chat_input_color", ucChatInputColorR, ucChatInputColorG, ucChatInputColorB, ucChatInputColorA );
    sprintf ( szTemp, "%f %f", vecChatScale.fX, vecChatScale.fY );
    SetString ( pNode, "chat_scale", szTemp );
    SetRGBA ( pNode, "chat_input_prefix_color", ucChatInputPrefixColorR, ucChatInputPrefixColorG, ucChatInputPrefixColorB, ucChatInputPrefixColorA );
    SetRGBA ( pNode, "chat_input_text_color", ucChatInputTextColorR, ucChatInputTextColorG, ucChatInputTextColorB, ucChatInputTextColorA );
    sprintf ( szTemp, "%f", fChatWidth );
    SetString ( pNode, "chat_width", szTemp );
    SetBoolean ( pNode, "chat_css_style_text", bChatCssStyleText );
    SetBoolean ( pNode, "chat_css_style_background", bChatCssStyleBackground );
    SetInteger ( pNode, "chat_line_life", ( int ) ulChatLineLife );
    SetInteger ( pNode, "chat_line_fade_out", ( int ) ulChatLineFadeOut );
    SetBoolean ( pNode, "chat_use_cegui", bChatUseCEGUI );
	SetInteger ( pNode, "mtu_size", ( int ) usMTUSize );
	SetInteger ( pNode, "fps_limit", ( int ) usMTUSize );
	SetInteger ( pNode, "menu_options", iMenuOptions );
    sprintf ( szTemp, "%f", fTextScale );
    SetString ( pNode, "text_scale", szTemp );
    SetBoolean ( pNode, "invert_mouse", bInvertMouse );
    SetBoolean ( pNode, "steer_with_mouse", bSteerWithMouse );
    SetBoolean ( pNode, "fly_with_mouse", bFlyWithMouse );

    return true;
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.cpp
*  PURPOSE:     Resource object
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Chris McArthur <>
*               Ed Lyons <eai@opencoding.net>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

extern CClientGame* g_pClientGame;

int CResource::m_iShowingCursor = 0;

CResource::CResource ( unsigned short usID, char* szResourceName, CClientEntity* pResourceEntity, CClientEntity* pResourceDynamicEntity )
{
    m_usID = usID;
    m_bActive = false;
    m_bInDownloadQueue = false;
    m_bShowingCursor = false;

    if ( szResourceName )
    {
        strncpy ( m_szResourceName, szResourceName, MAX_RESOURCE_NAME_LENGTH );
        if ( MAX_RESOURCE_NAME_LENGTH )
        {
            m_szResourceName [ MAX_RESOURCE_NAME_LENGTH-1 ] = 0;
        }
    }
    m_pLuaManager = g_pClientGame->GetLuaManager();
    m_pRootEntity = g_pClientGame->GetRootEntity ();
    m_pDefaultElementGroup = new CElementGroup ( this );
    m_elementGroups.push_back ( m_pDefaultElementGroup ); // for use by scripts
    m_pResourceEntity = pResourceEntity;
    m_pResourceDynamicEntity = pResourceDynamicEntity;

    // Create our GUI root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceGUIEntity = new CClientDummy ( g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "guiroot" );
    m_pResourceGUIEntity->MakeSystemEntity ();

    // Create our COL root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceCOLRoot = new CClientDummy ( g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "colmodelroot" );
    m_pResourceCOLRoot->MakeSystemEntity ();

    // Create our DFF root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceDFFEntity = new CClientDummy ( g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "dffroot" );
    m_pResourceDFFEntity->MakeSystemEntity ();

    // Create our TXD root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceTXDRoot = new CClientDummy ( g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "txdroot" );
    m_pResourceTXDRoot->MakeSystemEntity ();

    // Create our IFP root element. We set its parent when we're loaded.
    // Make it a system entity so nothing but us can delete it.
    m_pResourceIFPRoot = new CClientDummy ( g_pClientGame->GetManager(), INVALID_ELEMENT_ID, "ifproot" );
    m_pResourceIFPRoot->MakeSystemEntity ();

    m_strResourceDirectoryPath = SString ( "%s/resources/%s", g_pClientGame->GetModRoot (), m_szResourceName );

    m_pLuaVM = m_pLuaManager->CreateVirtualMachine ( this );
    if ( m_pLuaVM )
    {
        m_pLuaVM->SetScriptName ( szResourceName );
    }
}


CResource::~CResource ( void )
{
    // Make sure we don't force the cursor on
    ShowCursor ( false );

    // Do this before we delete our elements.
    m_pRootEntity->CleanUpForVM ( m_pLuaVM, true );
    m_pLuaManager->RemoveVirtualMachine ( m_pLuaVM );

    // Remove all keybinds on this VM
    g_pClientGame->GetScriptKeyBinds ()->RemoveAllKeys ( m_pLuaVM );
    g_pCore->GetKeyBinds()->SetAllCommandsActive ( m_szResourceName, false );

    // Destroy the txd root so all dff elements are deleted except those moved out
    g_pClientGame->GetElementDeleter ()->DeleteRecursive ( m_pResourceTXDRoot );
    m_pResourceTXDRoot = NULL;

    // Destroy the ifp root so all ifp elements are deleted except those moved out
    g_pClientGame->GetElementDeleter ()->DeleteRecursive ( m_pResourceIFPRoot );
    m_pResourceIFPRoot = NULL;

    // Destroy the ddf root so all dff elements are deleted except those moved out
    g_pClientGame->GetElementDeleter ()->DeleteRecursive ( m_pResourceDFFEntity );
    m_pResourceDFFEntity = NULL;

    // Destroy the colmodel root so all colmodel elements are deleted except those moved out
    g_pClientGame->GetElementDeleter ()->DeleteRecursive ( m_pResourceCOLRoot );
    m_pResourceCOLRoot = NULL;

    // Destroy script fonts created by the resourece
    g_pClientGame->GetScriptFontLoader()->UnloadFonts ( this );

    // Destroy the gui root so all gui elements are deleted except those moved out
    g_pClientGame->GetElementDeleter ()->DeleteRecursive ( m_pResourceGUIEntity );
    m_pResourceGUIEntity = NULL;

    // Undo all changes to water
    g_pGame->GetWaterManager ()->UndoChanges ( this );

    // TODO: remove them from the core too!!
    // Destroy all the element groups attached directly to this resource
    list < CElementGroup* > ::iterator itere = m_elementGroups.begin ();
    for ( ; itere != m_elementGroups.end (); itere++ )
    {
        delete (*itere);
    }
    m_elementGroups.clear();
    m_pDefaultElementGroup = NULL;

    m_pRootEntity = NULL;
    m_pResourceEntity = NULL;

    list < CResourceFile* >::iterator iter = m_ResourceFiles.begin ();
    for ( ; iter != m_ResourceFiles.end (); iter++ )
    {
        delete ( *iter );
    }
    m_ResourceFiles.empty ();

    list < CResourceConfigItem* >::iterator iterc = m_ConfigFiles.begin ();
    for ( ; iterc != m_ConfigFiles.end (); iterc++ )
    {
        delete ( *iterc );
    }
    m_ConfigFiles.empty ();

    // Delete the exported functions
    list < CExportedFunction* >::iterator iterExportedFunction = m_exportedFunctions.begin();
    for ( ; iterExportedFunction != m_exportedFunctions.end(); iterExportedFunction++ )
    {
        delete ( *iterExportedFunction );
    }
    m_exportedFunctions.empty();
}

CDownloadableResource* CResource::QueueFile ( CDownloadableResource::eResourceType resourceType, const char *szFileName, CChecksum serverChecksum )
{
    // Create the resource file and add it to the list
    SString strBuffer ( "%s\\resources\\%s\\%s", g_pClientGame->GetModRoot (), m_szResourceName, szFileName );

    CResourceFile* pResourceFile = new CResourceFile ( resourceType, szFileName, strBuffer, serverChecksum );
    if ( pResourceFile )
    {
        m_ResourceFiles.push_back ( pResourceFile );
    }

    return pResourceFile;
}


CDownloadableResource* CResource::AddConfigFile ( char *szFileName, CChecksum serverChecksum )
{
    // Create the config file and add it to the list
    SString strBuffer ( "%s\\resources\\%s\\%s", g_pClientGame->GetModRoot (), m_szResourceName, szFileName );
    
    CResourceConfigItem* pConfig = new CResourceConfigItem ( this, szFileName, strBuffer, serverChecksum );
    if ( pConfig )
    {
        m_ConfigFiles.push_back ( pConfig );
    }

    return pConfig;
}

void CResource::AddExportedFunction ( char *szFunctionName )
{
    m_exportedFunctions.push_back(new CExportedFunction ( szFunctionName ) );
}

bool CResource::CallExportedFunction ( const char * szFunctionName, CLuaArguments& args, CLuaArguments& returns, CResource& caller )
{
    list < CExportedFunction* > ::iterator iter =  m_exportedFunctions.begin ();
    for ( ; iter != m_exportedFunctions.end (); iter++ )
    {
        if ( strcmp ( (*iter)->GetFunctionName(), szFunctionName ) == 0 )
        {
            if ( args.CallGlobal ( m_pLuaVM, szFunctionName, &returns ) )
            {
                return true;
            }
        }
    }
    return false;
}


//
// Quick integrity check of png, dff and txd files
//
static bool CheckFileForCorruption( string strPath )
{
    const char* szExt   = strPath.c_str () + max<long>( 0, strPath.length () - 4 );
    bool bIsBad         = false;

    if ( stricmp ( szExt, ".PNG" ) == 0 )
    {
        // Open the file
        if ( FILE* pFile = fopen ( strPath.c_str (), "rb" ) )
        {
            // This is what the png header should look like
            unsigned char pGoodHeader [8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

             // Load the header
            unsigned char pBuffer [8] = { 0,0,0,0,0,0,0,0 };
            fread ( pBuffer, 1, 8, pFile );

            // Check header integrity
            if ( memcmp ( pBuffer, pGoodHeader, 8 ) )
                bIsBad = true;

            // Close the file
            fclose ( pFile );
        }
    }
    else
    if ( stricmp ( szExt, ".TXD" ) == 0 || stricmp ( szExt, ".DFF" ) == 0 )
    {
        // Open the file
        if ( FILE* pFile = fopen ( strPath.c_str (), "rb" ) )
        {
            struct {
                long id;
                long size;
                long ver;
            } header = {0,0,0};

            // Load the first header
            fread ( &header, 1, sizeof(header), pFile );
            long pos = sizeof(header);
            long validSize = header.size + pos;

            // Step through the sections
            while ( pos < validSize )
            {
                if ( fread ( &header, 1, sizeof(header), pFile ) != sizeof(header) )
                    break;
                fseek ( pFile, header.size, SEEK_CUR );
                pos += header.size + sizeof(header);
            }

            // Check integrity
            if ( pos != validSize )
                bIsBad = true;
               
            // Close the file
            fclose ( pFile );
        }        
    }

    return bIsBad;
}


void CResource::Load ( CClientEntity *pRootEntity )
{
    m_pRootEntity = pRootEntity;
    if ( m_pRootEntity )
    {
        // Set the GUI parent to the resource root entity
        m_pResourceCOLRoot->SetParent ( m_pResourceEntity );
        m_pResourceDFFEntity->SetParent ( m_pResourceEntity );
        m_pResourceGUIEntity->SetParent ( m_pResourceEntity );
        m_pResourceTXDRoot->SetParent ( m_pResourceEntity );
        m_pResourceIFPRoot->SetParent ( m_pResourceEntity );
    }

    CLogger::LogPrintf ( "> Starting resource '%s'", m_szResourceName );

    char szBuffer [ MAX_PATH ] = { 0 };
    list < CResourceConfigItem* >::iterator iterc = m_ConfigFiles.begin ();
    for ( ; iterc != m_ConfigFiles.end (); iterc++ )
    {
        if ( !(*iterc)->Start() )
        {
            CLogger::LogPrintf ( "Failed to start resource item %s in %s\n", (*iterc)->GetName(), m_szResourceName );
        }
    }

    // Load the files that are queued in the list "to be loaded"
    list < CResourceFile* > ::iterator iter = m_ResourceFiles.begin ();
    for ( ; iter != m_ResourceFiles.end (); iter++ )
    {
        CResourceFile* pResourceFile = *iter;
        // Only load the resource file if it is a client script
        if ( pResourceFile->GetResourceType () == CDownloadableResource::RESOURCE_FILE_TYPE_CLIENT_SCRIPT )
        {
            // Load the file
            std::vector < char > buffer;
            FileLoad ( pResourceFile->GetName (), buffer );
            unsigned int iSize = buffer.size();

            // Check the contents
            if ( iSize > 0 && CChecksum::GenerateChecksumFromBuffer ( &buffer.at ( 0 ), iSize ).CompareWithLegacy ( pResourceFile->GetServerChecksum () ) )
            {
                //UTF-8 BOM?  Compare by checking the standard UTF-8 BOM of 3 characters (in signed format, hence negative)
                if ( buffer[0] != -0x11 || iSize < 1 || buffer[1] != -0x45 || iSize < 2 ||  buffer[2] != -0x41 ) //Not UTF-8
                    // Load the resource text
                    m_pLuaVM->LoadScriptFromBuffer ( &buffer.at ( 0 ), iSize, pResourceFile->GetName (), false );
                else // Load ignoring the first 3 bytes
                    m_pLuaVM->LoadScriptFromBuffer ( &buffer.at ( 3 ), iSize-3, pResourceFile->GetName (), true );
            }
            else
            {
                SString strBuffer ( "ERROR: File '%s' in resource '%s' - CRC mismatch.", pResourceFile->GetShortName (), m_szResourceName );
                g_pCore->ChatEchoColor ( strBuffer, 255, 0, 0 );
            }
        }
        else
        if ( CheckFileForCorruption ( pResourceFile->GetName () ) )
        {
            SString strBuffer ( "WARNING: File '%s' in resource '%s' is invalid.", pResourceFile->GetShortName (), m_szResourceName );
            g_pCore->DebugEchoColor ( strBuffer, 255, 0, 0 );
        }
    }

    // Set active flag
    m_bActive = true;

    // Did we get a resource root entity?
    if ( m_pResourceEntity )
    {
        // Call the Lua "onClientResourceStart" event
        CLuaArguments Arguments;
        Arguments.PushUserData ( this );
        m_pResourceEntity->CallEvent ( "onClientResourceStart", Arguments, true );
    }
    else
        assert ( 0 );
}


void CResource::DeleteClientChildren ( void )
{
    // Run this on our resource entity if we have one
    if ( m_pResourceEntity )
        m_pResourceEntity->DeleteClientChildren ();
}


void CResource::ShowCursor ( bool bShow, bool bToggleControls )
{
    // Different cursor showing state than earlier?
    if ( bShow != m_bShowingCursor )
    {
        // Going to show the cursor?
        if ( bShow )
        {
            // Increase the cursor ref count
            m_iShowingCursor += 1;
        }
        else
        {

            // Decrease the cursor ref count
            m_iShowingCursor -= 1;
        }

        // Update our showing cursor state
        m_bShowingCursor = bShow;

        // Show cursor if more than 0 resources wanting the cursor on
        g_pCore->ForceCursorVisible ( m_iShowingCursor > 0, bToggleControls );
        g_pClientGame->SetCursorEventsEnabled ( m_iShowingCursor > 0 );
    }
}

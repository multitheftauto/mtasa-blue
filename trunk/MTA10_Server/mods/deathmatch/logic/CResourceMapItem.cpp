/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceMapItem.cpp
*  PURPOSE:     Resource server-side map item class
*  DEVELOPERS:  Ed Lyons <>
*               Kevin Whiteside <>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource map item, being a .map file
// It's task is to load the .map file into elements and store them
// as a reference in a CElementGroup. This allows easy removal on the deletion
// of the resource.

#include "StdInc.h"

extern CGame* g_pGame;

CResourceMapItem::CResourceMapItem ( CResource * resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes * xmlAttributes, int iDimension ) : CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_pGroups = g_pGame->GetGroups();  // isn't in CGame
    m_pMarkerManager = g_pGame->GetMarkerManager();
    m_pBlipManager = g_pGame->GetBlipManager();
    m_pObjectManager = g_pGame->GetObjectManager();
    m_pPickupManager = g_pGame->GetPickupManager();
    m_pPlayerManager = g_pGame->GetPlayerManager();
    m_pRadarAreaManager = g_pGame->GetRadarAreaManager();
    m_pVehicleManager = g_pGame->GetVehicleManager();
    m_pTeamManager = g_pGame->GetTeamManager();
    m_pPedManager = g_pGame->GetPedManager();
    m_pLuaManager = g_pGame->GetLuaManager();
    m_pEvents = g_pGame->GetEvents();
    m_pScriptDebugging = g_pGame->GetScriptDebugging();
	m_pElementGroup = NULL;

    m_iDimension = iDimension;

    m_type = RESOURCE_FILE_TYPE_MAP;

    m_pMapElement = NULL;
    m_pXMLFile = NULL;
    m_pXMLRootNode = NULL;
    m_pRootElement = NULL;
    m_pVM = NULL;
}

CResourceMapItem::~CResourceMapItem ( void )
{
	Stop ();
}


bool CResourceMapItem::Start ( void )
{
    // Not already been run?
    if ( !m_pElementGroup )
    {
        m_pVM = m_resource->GetVirtualMachine ();
        m_pElementGroup = new CElementGroup ( m_resource );

        LoadMap ( m_szResourceFileName );
        return true;
    }

    return false;
}

bool CResourceMapItem::Stop ( void )
{
	// Delete our element group, including all the elements in it
	if ( m_pElementGroup )
    {
		delete m_pElementGroup;
		m_pElementGroup = NULL;
	}

    // Destroy our XML too
    if ( m_pXMLFile )
    {
        delete m_pXMLFile;
        m_pXMLFile = NULL;
        m_pXMLRootNode = NULL;
    }

    // Map element is deleted by element group
    m_pMapElement = NULL;
    return true;
}

bool CResourceMapItem::LoadMap ( char * szMapFilename )
{ 
    assert ( szMapFilename );
    assert ( strlen ( szMapFilename ) > 0 );

    // Don't load twice
    if ( !m_pMapElement && !m_pXMLRootNode )
    {
        // Grab the root element
        m_pRootElement = g_pGame->GetMapManager ()->GetRootElement();

        // Create Map Element
        m_pMapElement = new CDummy ( g_pGame->GetGroups(), m_resource->GetResourceRootElement () );
        m_pMapElement->SetTypeName ( "map" );
        m_pMapElement->SetName ( m_szShortName );

        // Load and parse it
        m_pXMLFile = g_pServerInterface->GetXML ()->CreateXML ( szMapFilename );
        if ( m_pXMLFile )
        {
            // Try to parse it
            if ( m_pXMLFile->Parse () )
            {
                // Grab the root item
                m_pXMLRootNode = m_pXMLFile->GetRootNode ();
                if ( m_pXMLRootNode )
                {
                    // Is the rootnode's name <map>?
                    std::string strBuffer;
                    strBuffer = m_pXMLRootNode->GetTagName ();
                    if ( strBuffer.compare ( "map" ) == 0 )
                    {
                        // Load the data
                        m_bIsLoaded = LoadSubNodes ( *m_pXMLRootNode, m_pMapElement, NULL, true );
                        if ( m_bIsLoaded )
                        {
                            LinkupElements ();

                            // Add map element to element group
                            m_pElementGroup->Add ( m_pMapElement );

                            // Success
                            return true;
                        }
                    }
                }
            }

            // Failed, destroy the XML
            delete m_pXMLRootNode;
            m_pXMLRootNode = NULL;
        }

        // Delete map element
        delete m_pMapElement;
    }

    // Failed
    return false;
}


bool CResourceMapItem::LoadSubNodes ( CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart )
{
    // Iterate the nodes
    CXMLNode* pNode = NULL;
    list < CXMLNode * > ::iterator iter = Node.ChildrenBegin ();
    for ( ; iter != Node.ChildrenEnd () ; iter++ )
    {
        // Grab the node
        pNode = *iter;
        if ( pNode )
        {
            // Handle it (if it can't be handled, just ignore it and continue to the next)
            HandleNode ( *pNode, pParent, pAdded, bIsDuringStart, NULL );
        }
    }

    // Success
    return true;
}

bool CResourceMapItem::HandleNode ( CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart, CElement** pCreated )
{
    // Grab the name
    std::string strBuffer;
    strBuffer = Node.GetTagName ();

    // Handle it based on the tag name
    CElement* pNode = NULL;
    if ( strBuffer.compare ( "vehicle" ) == 0 )
    {
        pNode = m_pVehicleManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
    }
    else if ( strBuffer.compare ( "object" ) == 0 )
    {
        pNode = m_pObjectManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
    }
    else if ( strBuffer.compare ( "blip" ) == 0 )
    {
        CBlip* pBlip = m_pBlipManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
        pNode = pBlip;
        /*
        if ( pBlip )
        {
            pBlip->SetIsSynced ( bIsDuringStart );
        }
        */
    }
    else if ( strBuffer.compare ( "pickup" ) == 0 )
    {
        pNode = m_pPickupManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
    }
    else if ( strBuffer.compare ( "marker" ) == 0 )
    {
        CMarker* pMarker = m_pMarkerManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
        pNode = pMarker;
        /*
        if ( pMarker )
        {
            pMarker->SetIsSynced ( bIsDuringStart );
        }
        */
    }
    else if ( strBuffer.compare ( "radararea" ) == 0 )
    {
        CRadarArea* pRadarArea = m_pRadarAreaManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
        pNode = pRadarArea;
        /*
        if ( pRadarArea )
        {
            pRadarArea->SetIsSynced ( bIsDuringStart );
        }
        */
    }
    else if ( strBuffer.compare ( "team" ) == 0 )
    {
        pNode = m_pTeamManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
    }
    else if ( strBuffer.compare ( "ped" ) == 0 )
    {
        pNode = m_pPedManager->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
    }
	else if ( strBuffer.empty () )
	{
		// Comment, return true
		return true;
	}
    else
    {
        pNode = m_pGroups->CreateFromXML ( pParent, Node, m_pVM, m_pEvents );
    }

    // Set the node we created in the pointer we were given
    if ( pCreated )
    {
        *pCreated = pNode;
    }

    // Got a node created?
    if ( pNode )
    {
        // Set its typename to the name of the tag
        pNode->SetTypeName ( strBuffer.c_str () );

        // Add it to our list over elements added
        if ( pAdded )
        {
            pAdded->push_back ( pNode );
        }

        // Add this element to this map's element group
        if ( m_pElementGroup )
            m_pElementGroup->Add ( pNode );

        // Load the elements below it
        return LoadSubNodes ( Node, pNode, pAdded, bIsDuringStart );
    }
    return false;
}

CElement* CResourceMapItem::LoadNode ( CXMLNode& Node, CElement* pParent, vector < CElement* >* pAdded, bool bIsDuringStart )
{
    // Load the given node and its children
    CElement* pLoadedRoot;
    HandleNode ( Node, pParent, pAdded, bIsDuringStart, &pLoadedRoot );
    return pLoadedRoot;
}


void CResourceMapItem::LinkupElements ( void )
{
    // Link up all the attaching elements
    list < CVehicle* > ::const_iterator iterVehicles = m_pVehicleManager->IterBegin ();
    for ( ; iterVehicles != m_pVehicleManager->IterEnd (); iterVehicles++ )
    {
        CVehicle* pVehicle = *iterVehicles;

        char* szAttachToID = pVehicle->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pVehicle->AttachTo ( pElement );
        }
    }

    list < CPlayer* > ::const_iterator iterPlayers = m_pPlayerManager->IterBegin ();
    for ( ; iterPlayers != m_pPlayerManager->IterEnd (); iterPlayers++ )
    {
        CPlayer* pPlayer = *iterPlayers;
        char* szAttachToID = pPlayer->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pPlayer->AttachTo ( pElement );
        }
    }

    list < CObject* > ::const_iterator iterObjects = m_pObjectManager->IterBegin ();
    for ( ; iterObjects != m_pObjectManager->IterEnd (); iterObjects++ )
    {
        CObject* pObject = *iterObjects;
        char* szAttachToID = pObject->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pObject->AttachTo ( pElement );
        }
    }

    list < CBlip* > ::const_iterator iterBlips = m_pBlipManager->IterBegin ();
    for ( ; iterBlips != m_pBlipManager->IterEnd (); iterBlips++ )
    {
        CBlip* pBlip = *iterBlips;
        char* szAttachToID = pBlip->GetAttachToID ();
        if ( szAttachToID [ 0 ] )
        {
            CElement* pElement = g_pGame->GetMapManager ()->GetRootElement ()->FindChild ( szAttachToID, 0, true );
            if ( pElement )
                pBlip->AttachTo ( pElement );
        }
    }
}
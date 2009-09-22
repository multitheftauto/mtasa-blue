/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElement.cpp
*  PURPOSE:     Base entity (element) class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*               Cecill Etheredge <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CElement::CElement ( CElement* pParent, CXMLNode* pNode )
{
    // Allocate us an unique ID
    m_ID = CElementIDs::PopUniqueID ( this );

    // Init
    m_pParent = pParent;
    m_pXMLNode = pNode;
    m_pElementGroup = NULL;

    m_iType = CElement::UNKNOWN;
    m_strName = "";
    m_strTypeName = "unknown";
    m_bIsBeingDeleted = false;
    m_usDimension = 0;
    m_ucSyncTimeContext = 1;
    m_ucInterior = 0;
    m_bMapCreated = false;

    // Store the line
    if ( m_pXMLNode )
    {
        m_uiLine = m_pXMLNode->GetLine ();
    }
    else
    {
        m_uiLine = 0;
    }

    // Add us to our parent's list
    if ( pParent )
    {
        pParent->m_Children.push_back ( this );
    }

    m_uiTypeHash = HashString ( m_strTypeName.c_str () );
    if ( m_pParent )
        CElement::AddEntityFromRoot ( m_uiTypeHash, this );

    // Make an event manager for us
    m_pEventManager = new CMapEventManager;
    m_pCustomData = new CCustomData;

    m_pAttachedTo = NULL;
    m_szAttachToID [ 0 ] = 0;
}


CElement::~CElement ( void )
{
    // Get rid of the children elements
    ClearChildren ();
    SetParentObject ( NULL );

    // Remove ourselves from our element group
    if ( m_pElementGroup )
        m_pElementGroup->Remove ( this );

    // Delete our event manager
    delete m_pCustomData;
    delete m_pEventManager;

    // Unreference us from what's referencing us
    list < CPerPlayerEntity* > ::const_iterator iter = m_ElementReferenced.begin ();
    for ( ; iter != m_ElementReferenced.end (); iter++ )
    {
        if ( !(*iter)->m_ElementReferences.empty() ) (*iter)->m_ElementReferences.remove ( this );
    }

    RemoveAllCollisions ( true );

    // Null all camera elements referencing us
    list < CPlayerCamera* > ::const_iterator iterFollowingCameras = m_FollowingCameras.begin ();
    for ( ; iterFollowingCameras != m_FollowingCameras.end (); iterFollowingCameras++ )
    {
        (*iterFollowingCameras)->m_pTarget = NULL;
    }

    if ( m_pAttachedTo )
        m_pAttachedTo->RemoveAttachedElement ( this );

    list < CElement* > ::iterator iterAttached = m_AttachedElements.begin ();
    for ( ; iterAttached != m_AttachedElements.end () ; iterAttached++ )
    {
        // Make sure our attached element stores it's current position
        (*iterAttached)->GetPosition ();
        // Unlink it
        (*iterAttached)->m_pAttachedTo = NULL;
    }

    list < CPed * > ::iterator iterUsers = m_OriginSourceUsers.begin ();
    for ( ; iterUsers != m_OriginSourceUsers.end () ; iterUsers++ )
    {
        CPed* pPed = *iterUsers;
        if ( pPed->m_pContactElement == this )
            pPed->m_pContactElement = NULL;
    }

    // Deallocate our unique ID
    CElementIDs::PushUniqueID ( this );

    // Remove our reference from the element deleter
    g_pGame->GetElementDeleter ()->Unreference ( this );

    // Ensure nothing has inadvertently set a parent
    assert ( m_pParent == NULL );
}


void CElement::SetTypeName ( std::string strTypeName )
{
    CElement::RemoveEntityFromRoot ( m_uiTypeHash, this );
    m_uiTypeHash = HashString ( strTypeName.c_str () );
    m_strTypeName = strTypeName;
    if ( m_pParent )
        CElement::AddEntityFromRoot ( m_uiTypeHash, this );
}


CElement* CElement::FindChild ( const char* szName, unsigned int uiIndex, bool bRecursive )
{
    assert ( szName );

    // Is it our name?
    unsigned int uiCurrentIndex = 0;
    if ( strcmp ( szName, m_strName.c_str () ) == 0 )
    {
        if ( uiIndex == 0 )
        {
            return this;
        }
        else
        {
            ++uiCurrentIndex;
        }
    }

    // Find it among our children
    return FindChildIndex ( szName, uiIndex, uiCurrentIndex, bRecursive );
}


CElement* CElement::FindChildByType ( const char* szType, unsigned int uiIndex, bool bRecursive )
{
    assert ( szType );

    // Is it our type?
    unsigned int uiCurrentIndex = 0;
    if ( strcmp ( szType, GetTypeName ().c_str () ) == 0 )
    {
        if ( uiIndex == 0 )
        {
            return this;
        }
        else
        {
            ++uiCurrentIndex;
        }
    }

    // Find it among our children
    unsigned int uiTypeHash = HashString ( szType );
    return FindChildByTypeIndex ( uiTypeHash, uiIndex, uiCurrentIndex, bRecursive );
}


void CElement::FindAllChildrenByType ( const char* szType, lua_State* pLua )
{
    assert ( szType );
    assert ( pLua );

    // Add all children of the given type to the table
    unsigned int uiIndex = 0;
    unsigned int uiTypeHash = HashString ( szType );

    if ( this == g_pGame->GetMapManager ()->GetRootElement () )
    {
        GetEntitiesFromRoot ( uiTypeHash, pLua );
    }
    else
    {
        FindAllChildrenByTypeIndex ( uiTypeHash, pLua, uiIndex );
    }
}


void CElement::GetChildren ( lua_State* pLua )
{
    assert ( pLua );

    // Add all our children to the table on top of the given lua main's stack
    unsigned int uiIndex = 0;
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        // Add it to the table
        lua_pushnumber ( pLua, ++uiIndex );
        lua_pushelement ( pLua, *iter );
        lua_settable ( pLua, -3 );
    }
}


bool CElement::IsMyChild ( CElement* pElement, bool bRecursive )
{
	// Since VERIFY_ELEMENT is calling us, the pEntity argument could be NULL
	if ( pElement == NULL ) return false;

    // Is he us?
    if ( pElement == this )
        return true;

    // Is he our child directly?
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        // Return true if this is our child. If not check if he's one of our children's children if we were asked to do a recursive search.
        if ( *iter == pElement )
        {
            return true;
        }
        else if ( bRecursive && (*iter)->IsMyChild ( pElement, true ) )
        {
            return true;
        }
    }

    // He's not under us
    return false;
}


void CElement::ClearChildren ( void )
{
    // Sanity check
    assert ( m_pParent != this );

    // Process our children - Move up to our parent
	list < CElement* > cloneList = m_Children;
    list < CElement* > ::const_iterator iter = cloneList.begin ();
    for ( ; iter != cloneList.end () ; ++iter )
        (*iter)->SetParentObject ( m_pParent );

    // This list should now be empty
    assert ( m_Children.size () == 0 );
}


CElement* CElement::SetParentObject ( CElement* pParent )
{
    // Different parent?
    if ( pParent != m_pParent )
    {
        // Already got a parent?
        if ( m_pParent )
        {
            // Call the on children remove on our current parent
            m_pParent->OnSubtreeRemove ( this );

            // Eventually unreference us from the previous parent entity
            m_pParent->m_Children.remove ( this );
        }

        // Get into/out-of FromRoot info
        bool bOldFromRoot = CElement::IsFromRoot ( m_pParent );
        bool bNewFromRoot = CElement::IsFromRoot ( pParent );

        // Moving out of FromRoot?
        if ( bOldFromRoot && !bNewFromRoot )
            CElement::RemoveEntityFromRoot ( m_uiTypeHash, this );

        // Grab the root element now
        CElement* pRoot;
        if ( pParent )
        {
            pRoot = pParent->GetRootElement ();
        }
        else
        {
            assert ( m_pParent );
            pRoot = m_pParent->GetRootElement ();
        }

        // Set the new parent
        m_pParent = pParent;

        // New parent?
        if ( pParent )
        {
            // Add us to the new parent's child list
            pParent->m_Children.push_back ( this );

            // Moving into FromRoot?
            if ( !bOldFromRoot && bNewFromRoot )
                CElement::AddEntityFromRoot ( m_uiTypeHash, this );

            // Call the on children add event on the new parent
            pParent->OnSubtreeAdd ( this );
        }

        // Update all per-player references from the root and down
        pRoot->UpdatePerPlayerEntities ();
    }

    return pParent;
}


void CElement::SetXMLNode ( CXMLNode* pNode )
{
    // Set the node
    m_pXMLNode = pNode;

    // If the node exists, set our line
    if ( pNode )
    {
        m_uiLine = pNode->GetLine ();
    }
    else
    {
        m_uiLine = 0;
    }
};


bool CElement::AddEvent ( CLuaMain* pLuaMain, const char* szName, int iLuaFunction, bool bPropagated )
{
    return m_pEventManager->Add ( pLuaMain, szName, iLuaFunction, bPropagated );
}


bool CElement::CallEvent ( const char* szName, const CLuaArguments& Arguments, CPlayer* pCaller )
{
    CEvents* pEvents = g_pGame->GetEvents();

    // Make sure our event-manager knows we're about to call an event
    pEvents->PreEventPulse ();

    // Call the event on our parents/us first
    CallParentEvent ( szName, Arguments, this, pCaller );

    // Call it on all our children
    CallEventNoParent ( szName, Arguments, this, pCaller );

    // Tell the event manager that we're done calling the event
    pEvents->PostEventPulse ();

    // Return whether our event was cancelled or not
    return ( !pEvents->WasEventCancelled () );
}


bool CElement::DeleteEvent ( CLuaMain* pLuaMain, const char* szName, int iLuaFunction )
{
    return m_pEventManager->Delete ( pLuaMain, szName, iLuaFunction );
}


void CElement::DeleteEvents ( CLuaMain* pLuaMain, bool bRecursive )
{
    // Delete it from our events
    m_pEventManager->Delete ( pLuaMain );

    // Delete it from all our children's events
    if ( bRecursive )
    {
        list < CElement* > ::const_iterator iter = m_Children.begin ();
        for ( ; iter != m_Children.end (); iter++ )
        {
            (*iter)->DeleteEvents ( pLuaMain, true );
        }
    }
}


void CElement::DeleteAllEvents ( void )
{
    m_pEventManager->DeleteAll ();
}


void CElement::ReadCustomData ( CLuaMain* pLuaMain, CEvents* pEvents )
{
    assert ( pLuaMain );
    assert ( pEvents );

    // Got an XML node?
    if ( m_pXMLNode )
    {
        // Iterate the attributes of our XML node
        CXMLAttributes* pAttributes = &(m_pXMLNode->GetAttributes ());
        unsigned int uiAttributeCount = pAttributes->Count ();
        for ( unsigned int uiIndex = 0; uiIndex < uiAttributeCount; uiIndex++ )
        {
            // Grab the node (we can assume it exists here)
            CXMLAttribute* pAttribute = pAttributes->Get ( uiIndex );

            // Make a lua argument from it and set the content
            CLuaArguments args;
            if ( !args.ReadFromJSONString ( pAttribute->GetValue ().c_str() ) )
                args.PushString ( pAttribute->GetValue ().c_str () );
            SetCustomData ( pAttribute->GetName ().c_str (), *args[0], pLuaMain );
        }
    }
}


CLuaArgument* CElement::GetCustomData ( const char* szName, bool bInheritData, bool* pbIsSynced )
{
    assert ( szName );

    // Grab it and return a pointer to the variable
    const SCustomData* pData = m_pCustomData->Get ( szName );
    if ( pData )
    {
        if ( pbIsSynced )
            *pbIsSynced = pData->bSynchronized;
        return (CLuaArgument *)&pData->Variable;
    }

    // If none, try returning parent's custom data
    if ( bInheritData && m_pParent )
    {
        return m_pParent->GetCustomData ( szName, true, pbIsSynced );
    }

    // None available
    return NULL;
}

CLuaArguments* CElement::GetAllCustomData ( CLuaArguments * table )
{
    assert ( table );

    // Grab it and return a pointer to the variable
    map < string, SCustomData > :: const_iterator iter = m_pCustomData->IterBegin();
    int i = 1;
    for ( ; iter != m_pCustomData->IterEnd(); iter++ )
    {
        table->PushString( iter->first.c_str () );       // key
        table->PushArgument ( iter->second.Variable );   // value
        i++;
    }

    // None available
    return table;
}


bool CElement::GetCustomDataString ( const char* szName, char* pOut, size_t sizeBuffer, bool bInheritData )
{
    // Grab the custom data variable
    CLuaArgument* pData = GetCustomData ( szName, bInheritData );
    if ( pData )
    {
        // Make sure it gets 0 terminated
        sizeBuffer -= 1;
        pOut [sizeBuffer] = 0;

        // Write the content depending on what type it is
        int iType = pData->GetType ();
        if ( iType == LUA_TSTRING )
        {
            strncpy ( pOut, pData->GetString ().c_str (), sizeBuffer );
        }
        else if ( iType == LUA_TNUMBER )
        {
            _snprintf ( pOut, sizeBuffer, "%f", pData->GetNumber () );
        }
        else if ( iType == LUA_TBOOLEAN )
        {
            _snprintf ( pOut, sizeBuffer, "%u", pData->GetBoolean () );
        }
        else if ( iType == LUA_TNIL )
        {
            pOut [0] = 0;
        }
        else
        {
            return false;
        }

        return true;
    }

    return false;
}


bool CElement::GetCustomDataInt ( const char* szName, int& iOut, bool bInheritData )
{
    // Grab the custom data variable
    CLuaArgument* pData = GetCustomData ( szName, bInheritData );
    if ( pData )
    {
        // Write the content depending on what type it is
        int iType = pData->GetType ();
        if ( iType == LUA_TSTRING )
        {
            iOut = atoi ( pData->GetString ().c_str () );
        }
        else if ( iType == LUA_TNUMBER )
        {
            iOut = static_cast < int > ( pData->GetNumber () );
        }
        else if ( iType == LUA_TBOOLEAN )
        {
            if ( pData->GetBoolean () )
            {
                iOut = 1;
            }
            else
            {
                iOut = 0;
            }
        }
        else
        {
            return false;
        }

        return true;
    }

    return false;
}


bool CElement::GetCustomDataFloat ( const char* szName, float& fOut, bool bInheritData )
{
    // Grab the custom data variable
    CLuaArgument* pData = GetCustomData ( szName, bInheritData );
    if ( pData )
    {
        // Write the content depending on what type it is
        int iType = pData->GetType ();
        if ( iType == LUA_TSTRING )
        {
            fOut = static_cast < float > ( atof ( pData->GetString ().c_str () ) );
        }
        else if ( iType == LUA_TNUMBER )
        {
            fOut = static_cast < float > ( pData->GetNumber () );
        }
        else
        {
            return false;
        }

        return true;
    }

    return false;
}


bool CElement::GetCustomDataBool ( const char* szName, bool& bOut, bool bInheritData )
{
    // Grab the custom data variable
    CLuaArgument* pData = GetCustomData ( szName, bInheritData );
    if ( pData )
    {
        // Write the content depending on what type it is
        int iType = pData->GetType ();
        if ( iType == LUA_TSTRING )
        {
            const char* szString = pData->GetString ().c_str ();
            if ( strcmp ( szString, "true" ) == 0 || strcmp ( szString, "1" ) == 0 )
            {
                bOut = true;
            }
            else if ( strcmp ( szString, "false" ) == 0 || strcmp ( szString, "0" ) == 0 )
            {
                bOut = false;
            }
            else
            {
                return false;
            }
        }
        else if ( iType == LUA_TNUMBER )
        {
            int iNumber = static_cast < int > ( pData->GetNumber () );
            if ( iNumber == 1 )
            {
                bOut = true;
            }
            else if ( iNumber == 0 )
            {
                bOut = false;
            }
            else
            {
                return false;
            }
        }
        else if ( iType == LUA_TBOOLEAN )
        {
            bOut = pData->GetBoolean ();
        }
        else
        {
            return false;
        }

        return true;
    }

    return false;
}


void CElement::SetCustomData ( const char* szName, const CLuaArgument& Variable, CLuaMain* pLuaMain, bool bSynchronized )
{
    assert ( szName );

    // Grab the old variable
    CLuaArgument oldVariable;
    const SCustomData * pData = m_pCustomData->Get ( szName );
    if ( pData )
    {
        oldVariable = pData->Variable;
    }

    // Set the new data
    m_pCustomData->Set ( szName, Variable, pLuaMain, bSynchronized );

    // Trigger the onElementDataChange event on us
    CLuaArguments Arguments;
    Arguments.PushString ( szName );
    Arguments.PushArgument ( oldVariable  );
    CallEvent ( "onElementDataChange", Arguments );
}


bool CElement::DeleteCustomData ( const char* szName, bool bRecursive )
{
    // Delete the custom data
    bool bReturn = m_pCustomData->Delete ( szName );

    // If recursive, delete our children's data
    if ( bRecursive )
    {
        list < CElement* > ::const_iterator iter = m_Children.begin ();
        for ( ; iter != m_Children.end (); iter++ )
        {
            // Delete it. If we deleted any, remember that we've done that so we can return true.
            if ( (*iter)->DeleteCustomData ( szName, true ) )
                bReturn = true;
        }
    }

    // Return whether we deleted some
    return bReturn;
}


void CElement::DeleteAllCustomData ( CLuaMain* pLuaMain, bool bRecursive )
{
    // Delete our data
    m_pCustomData->DeleteAll ( pLuaMain );

    // If recursive, delete our children's data
    if ( bRecursive )
    {
        list < CElement* > ::const_iterator iter = m_Children.begin ();
        for ( ; iter != m_Children.end (); iter++ )
        {
            (*iter)->DeleteAllCustomData ( pLuaMain, true );
        }
    }
}

CXMLNode* CElement::OutputToXML ( CXMLNode* pNodeParent )
{
    // Create a new node for this element
    CXMLNode * pNode = pNodeParent->CreateSubNode ( GetTypeName ().c_str () );

    // Output the custom data values to it as arguments
    m_pCustomData->OutputToXML ( pNode );

    // Go through each child element and call this function on it
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->OutputToXML ( pNode );
    }

    return pNode;
}

void CElement::CleanUpForVM ( CLuaMain* pLuaMain, bool bRecursive )
{
    // Delete all our events and custom datas attached to that VM
    DeleteEvents ( pLuaMain, false );
    //DeleteCustomData ( pLuaMain, false ); * Removed to keep custom data global

    // If recursive, do it on our children too
    if ( bRecursive )
    {
        list < CElement* > ::const_iterator iter = m_Children.begin ();
        for ( ; iter != m_Children.end (); iter++ )
        {
            (*iter)->CleanUpForVM ( pLuaMain, true );
        }
    }
}


bool CElement::LoadFromCustomData ( CLuaMain* pLuaMain, CEvents* pEvents )
{
    assert ( pLuaMain );
    assert ( pEvents );

    // Mark this as an MAP created element
    m_bMapCreated = true;

    // Read out all the attributes into our custom data records
    ReadCustomData ( pLuaMain, pEvents );

    // Grab the "id" custom data into our m_strName member
    char szBuf[MAX_ELEMENT_NAME_LENGTH + 1] = {0};
    GetCustomDataString ( "id", szBuf, MAX_ELEMENT_NAME_LENGTH, false );
    m_strName = szBuf;

    // Grab the attaching custom data
    GetCustomDataString ( "attachTo", m_szAttachToID, MAX_ELEMENT_NAME_LENGTH, true );
    GetCustomDataFloat ( "attachX", m_vecAttachedPosition.fX, true );
    GetCustomDataFloat ( "attachY", m_vecAttachedPosition.fY, true );
    GetCustomDataFloat ( "attachZ", m_vecAttachedPosition.fZ, true );

    // Load the special attributes from our custom data
    return ReadSpecialData ();
}


void CElement::OnSubtreeAdd ( CElement* pElement )
{
    // Call the event on the elements that references us
    list < CPerPlayerEntity* > ::const_iterator iter = m_ElementReferenced.begin ();
    for ( ; iter != m_ElementReferenced.end (); iter++ )
    {
        (*iter)->OnReferencedSubtreeAdd ( pElement );
    }

    // Call the event on our parent
    if ( m_pParent )
    {
        m_pParent->OnSubtreeAdd ( pElement );
    }
}


void CElement::OnSubtreeRemove ( CElement* pElement )
{
    // Call the event on the elements that references us
    list < CPerPlayerEntity* > ::const_iterator iter = m_ElementReferenced.begin ();
    for ( ; iter != m_ElementReferenced.end (); iter++ )
    {
        (*iter)->OnReferencedSubtreeRemove ( pElement );
    }

    // Call the event on our parent
    if ( m_pParent )
    {
        m_pParent->OnSubtreeRemove ( pElement );
    }
}


void CElement::UpdatePerPlayerEntities ( void )
{
    // Call a virtual method that will end up CPerPlayerEntity or some other class using references
    UpdatePerPlayer ();

    // Call ourselves on our children
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->UpdatePerPlayerEntities ();
    }
}


unsigned int CElement::GetTypeID ( const char* szTypeName )
{
    if ( strcmp ( szTypeName, "dummy" ) == 0 )
        return CElement::DUMMY;
    else if ( strcmp ( szTypeName, "player" ) == 0 )
        return CElement::PLAYER;
    else if ( strcmp ( szTypeName, "vehicle" ) == 0 )
        return CElement::VEHICLE;
    else if ( strcmp ( szTypeName, "object" ) == 0 )
        return CElement::OBJECT;
    else if ( strcmp ( szTypeName, "marker" ) == 0 )
        return CElement::MARKER;
    else if ( strcmp ( szTypeName, "blip" ) == 0 )
        return CElement::BLIP;
    else if ( strcmp ( szTypeName, "pickup" ) == 0 )
        return CElement::PICKUP;
    else if ( strcmp ( szTypeName, "radararea" ) == 0 )
        return CElement::RADAR_AREA;
    else if ( strcmp ( szTypeName, "console" ) == 0 )
        return CElement::CONSOLE;
    else
        return CElement::UNKNOWN;
}


CElement* CElement::GetRootElement ( void )
{
    if ( m_pParent )
    {
        return m_pParent->GetRootElement ();
    }

    return this;
}


CElement* CElement::FindChildIndex ( const char* szName, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive )
{
    assert ( szName );

    // Look among our children
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        // Name matches?
        if ( strcmp ( (*iter)->GetName ().c_str (), szName ) == 0 )
        {
            // Does the index match? If it doesn't, increment it and keep searching
            if ( uiIndex == uiCurrentIndex )
            {
                return *iter;
            }
            else
            {
                ++uiCurrentIndex;
            }
        }

        // Tell this child to search too if recursive
        if ( bRecursive )
        {
            CElement* pElement = (*iter)->FindChildIndex ( szName, uiIndex, uiCurrentIndex, true );
            if ( pElement )
            {
                return pElement;
            }
        }
    }

    // Doesn't exist within us
    return NULL;
}


CElement* CElement::FindChildByTypeIndex ( unsigned int uiTypeHash, unsigned int uiIndex, unsigned int& uiCurrentIndex, bool bRecursive )
{
    // Look among our children
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        // Name matches?
        if ( (*iter)->GetTypeHash() == uiTypeHash )
        {
            // Does the index match? If it doesn't, increment it and keep searching
            if ( uiIndex == uiCurrentIndex )
            {
                return *iter;
            }
            else
            {
                ++uiCurrentIndex;
            }
        }

        // Tell this child to search too if recursive
        if ( bRecursive )
        {
            CElement* pElement = (*iter)->FindChildByTypeIndex ( uiTypeHash, uiIndex, uiCurrentIndex, true );
            if ( pElement )
            {
                return pElement;
            }
        }
    }

    // Doesn't exist within us
    return NULL;
}


void CElement::FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, lua_State* pLua, unsigned int& uiIndex )
{
    assert ( pLua );

    // Our type matches?
    if ( uiTypeHash == m_uiTypeHash )
    {
        // Add it to the table
        lua_pushnumber ( pLua, ++uiIndex );
        lua_pushelement ( pLua, this );
        lua_settable ( pLua, -3 );
    }

    // Call us on the children
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->FindAllChildrenByTypeIndex ( uiTypeHash, pLua, uiIndex );
    }
}


void CElement::CallEventNoParent ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller )
{
    // Call it on us if this isn't the same class it was raised on
    if ( pSource != this )
    {
        m_pEventManager->Call ( szName, Arguments, pSource, this, pCaller );
    }

    // Call it on all our children
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->CallEventNoParent ( szName, Arguments, pSource, pCaller );
    }
}


void CElement::CallParentEvent ( const char* szName, const CLuaArguments& Arguments, CElement* pSource, CPlayer* pCaller )
{
    // Call the event on us
    m_pEventManager->Call ( szName, Arguments, pSource, this, pCaller );

    // Call parent's handler
    if ( m_pParent )
    {
        m_pParent->CallParentEvent ( szName, Arguments, pSource, pCaller );
    }
}


bool CElement::CollisionExists ( CColShape* pShape )
{
    list < CColShape* > ::iterator iter = m_Collisions.begin ();
    for ( ; iter != m_Collisions.end () ; iter++ )
    {
        if ( *iter == pShape )
        {
            return true;
        }
    }
    return false;
}


void CElement::RemoveAllCollisions ( bool bNotify )
{
    if ( bNotify )
    {
        list < CColShape* > ::iterator iter = m_Collisions.begin ();
        for ( ; iter != m_Collisions.end () ; iter++ )
        {
            (*iter)->RemoveCollider ( this );
        }
    }
    m_Collisions.clear ();
}


CClient* CElement::GetClient ( void )
{
    CClient* pClient = NULL;
    switch ( GetType () )
    {
        case CElement::PLAYER:
        {
            CPlayer* pPlayer = static_cast < CPlayer* > ( this );
            pClient = static_cast < CClient* > ( pPlayer );
            break;
        }
        case CElement::CONSOLE:
        {
            CConsoleClient* pConsoleClient = static_cast < CConsoleClient* > ( this );
            pClient = static_cast < CClient* > ( pConsoleClient );   
            break;
        }
    }
    return pClient;
}


void CElement::AttachTo ( CElement* pElement )
{
    if ( m_pAttachedTo )
        m_pAttachedTo->RemoveAttachedElement ( this );

    m_pAttachedTo = pElement;

    if ( m_pAttachedTo )
        m_pAttachedTo->AddAttachedElement ( this );
}


void CElement::GetAttachedOffsets ( CVector & vecPosition, CVector & vecRotation )
{
    vecPosition = m_vecAttachedPosition;
    vecRotation = m_vecAttachedRotation;
}


void CElement::SetAttachedOffsets ( CVector & vecPosition, CVector & vecRotation )
{
    m_vecAttachedPosition = vecPosition;
    m_vecAttachedRotation = vecRotation;
}


bool CElement::IsElementAttached ( CElement* pElement )
{
    list < CElement* > ::iterator iter = m_AttachedElements.begin ();
    for ( ; iter != m_AttachedElements.end (); iter++ )
    {
        if ( *iter == pElement )
            return true;
    }

    return false;
}


bool CElement::IsAttachable ( void )
{
    switch ( GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        case CElement::BLIP:
        case CElement::VEHICLE:
        case CElement::OBJECT:
        case CElement::MARKER:
        case CElement::PICKUP:
        {
            return true;
            break;
        }
        default: break;
    }
	return false;
}


bool CElement::IsAttachToable ( void )
{
    switch ( GetType () )
    {
        case CElement::PED:
        case CElement::PLAYER:
        case CElement::BLIP:
        case CElement::VEHICLE:
        case CElement::OBJECT:
        case CElement::MARKER:
        case CElement::PICKUP:
        {
            return true;
            break;
        }
        default: break;
    }
	return false;
}


unsigned char CElement::GenerateSyncTimeContext ( void )
{
    // Increment the sync time index
    ++m_ucSyncTimeContext;

    // It can't be 0 because that will make it not work when wraps around
    if ( m_ucSyncTimeContext == 0 )
        ++m_ucSyncTimeContext;

    // Return the new value
    return m_ucSyncTimeContext;
}


bool CElement::CanUpdateSync ( unsigned char ucRemote )
{
    // We can update this element's sync only if the sync time
    // matches or either of them are 0 (ignore).
    return ( m_ucSyncTimeContext == ucRemote ||
             ucRemote == 0 ||
             m_ucSyncTimeContext == 0 );
}


// Entities from root optimization for getElementsByType
CElement::t_mapEntitiesFromRoot CElement::ms_mapEntitiesFromRoot;
bool CElement::ms_bEntitiesFromRootInitialized = false;

void CElement::StartupEntitiesFromRoot ()
{
    if ( !ms_bEntitiesFromRootInitialized )
    {
        ms_mapEntitiesFromRoot.set_deleted_key ( (unsigned int)0x00000000 );
        ms_mapEntitiesFromRoot.set_empty_key ( (unsigned int)0xFFFFFFFF );
        ms_bEntitiesFromRootInitialized = true;
    }
}

// Returns true if top parent is root
bool CElement::IsFromRoot ( CElement* pEntity )
{
    if ( !pEntity )
        return false;
    if ( g_pGame && g_pGame->GetMapManager () )
    {
        if ( pEntity == g_pGame->GetMapManager ()->GetRootElement () )
            return true;
    }
    else
    {
        if ( pEntity->GetTypeName () == "root" )
            return true;
    }
    return CElement::IsFromRoot ( pEntity->GetParentEntity () );
}

void CElement::AddEntityFromRoot ( unsigned int uiTypeHash, CElement* pEntity, bool bDebugCheck )
{
    // Check
    assert ( CElement::IsFromRoot ( pEntity ) );

    // Insert into list
    std::list < CElement* >& listEntities = ms_mapEntitiesFromRoot [ uiTypeHash ];
    listEntities.remove ( pEntity );
    listEntities.push_front ( pEntity );

    // Apply to child elements as well
    list < CElement* > ::const_iterator iter = pEntity->IterBegin ();
    for ( ; iter != pEntity->IterEnd (); iter++ )
        CElement::AddEntityFromRoot ( (*iter)->GetTypeHash (), *iter, false );

#if MTA_DEBUG
    if ( bDebugCheck )
        _CheckEntitiesFromRoot ( uiTypeHash );
#endif
}

void CElement::RemoveEntityFromRoot ( unsigned int uiTypeHash, CElement* pEntity )
{
    // Remove from list
    t_mapEntitiesFromRoot::iterator find = ms_mapEntitiesFromRoot.find ( uiTypeHash );
    if ( find != ms_mapEntitiesFromRoot.end () )
    {
        std::list < CElement* >& listEntities = find->second;
        listEntities.remove ( pEntity );
        if ( listEntities.size () == 0 )
            ms_mapEntitiesFromRoot.erase ( find );
    }

    // Apply to child elements as well
    list < CElement* > ::const_iterator iter = pEntity->IterBegin ();
    for ( ; iter != pEntity->IterEnd (); iter++ )
        CElement::RemoveEntityFromRoot ( (*iter)->GetTypeHash (), *iter );
}

void CElement::GetEntitiesFromRoot ( unsigned int uiTypeHash, lua_State* pLua )
{
#if MTA_DEBUG
    _CheckEntitiesFromRoot ( uiTypeHash );
#endif

    t_mapEntitiesFromRoot::iterator find = ms_mapEntitiesFromRoot.find ( uiTypeHash );
    if ( find != ms_mapEntitiesFromRoot.end () )
    {
        std::list < CElement* >& listEntities = find->second;
        CElement* pEntity;
        unsigned int uiIndex = 0;

        for ( std::list < CElement* >::const_reverse_iterator i = listEntities.rbegin ();
              i != listEntities.rend ();
              ++i )
        {
            pEntity = *i;

            // Add it to the table
            lua_pushnumber ( pLua, ++uiIndex );
            lua_pushelement ( pLua, pEntity );
            lua_settable ( pLua, -3 );
        }
    }    
}



#if MTA_DEBUG

//
// Check that GetEntitiesFromRoot produces the same results as FindAllChildrenByTypeIndex on the root element
//
void CElement::_CheckEntitiesFromRoot ( unsigned int uiTypeHash )
{
    std::map < CElement*, int > mapResults1;
    g_pGame->GetMapManager ()->GetRootElement ()->_FindAllChildrenByTypeIndex ( uiTypeHash, mapResults1 );

    std::map < CElement*, int > mapResults2;
    _GetEntitiesFromRoot ( uiTypeHash, mapResults2 );

    std::map < CElement*, int > :: const_iterator iter1 = mapResults1.begin ();
    std::map < CElement*, int > :: const_iterator iter2 = mapResults2.begin ();

    for ( ; iter1 != mapResults1.end (); ++iter1 )
    {
        CElement* pElement1 = iter1->first;

        if ( mapResults2.find ( pElement1 ) == mapResults2.end () )
        {
            OutputDebugString ( SString ( "Server: 0x%08x  %s is missing from GetEntitiesFromRoot list\n", pElement1, pElement1->GetTypeName ().c_str () ) );
        }
    }

    for ( ; iter2 != mapResults2.end (); ++iter2 )
    {
        CElement* pElement2 = iter2->first;

        if ( mapResults1.find ( pElement2 ) == mapResults1.end () )
        {
            OutputDebugString ( SString ( "Server: 0x%08x  %s is missing from FindAllChildrenByTypeIndex list\n", pElement2, pElement2->GetTypeName ().c_str () ) );
        }
    }

    assert ( mapResults1 == mapResults2 );
}


void CElement::_FindAllChildrenByTypeIndex ( unsigned int uiTypeHash, std::map < CElement*, int >& mapResults )
{
    // Our type matches?
    if ( uiTypeHash == m_uiTypeHash )
    {
        // Add it to the table
        assert ( mapResults.find ( this ) == mapResults.end () );
        mapResults [ this ] = 1;

        ElementID ID = this->GetID ();
        assert ( ID != INVALID_ELEMENT_ID );
        assert ( this == CElementIDs::GetElement ( ID ) );
        if ( this->IsBeingDeleted () )
            OutputDebugString ( SString ( "Server: 0x%08x  %s is flagged as IsBeingDeleted() but is still in FindAllChildrenByTypeIndex\n", this, this->GetTypeName ().c_str () ) );
    }

    // Call us on the children
    list < CElement* > ::const_iterator iter = m_Children.begin ();
    for ( ; iter != m_Children.end (); iter++ )
    {
        (*iter)->_FindAllChildrenByTypeIndex ( uiTypeHash, mapResults );
    }
}


void CElement::_GetEntitiesFromRoot ( unsigned int uiTypeHash, std::map < CElement*, int >& mapResults )
{
    t_mapEntitiesFromRoot::iterator find = ms_mapEntitiesFromRoot.find ( uiTypeHash );
    if ( find != ms_mapEntitiesFromRoot.end () )
    {
        const std::list < CElement* >& listEntities = find->second;
        CElement* pEntity;
        unsigned int uiIndex = 0;

        for ( std::list < CElement* >::const_reverse_iterator i = listEntities.rbegin ();
              i != listEntities.rend ();
              ++i )
        {
            pEntity = *i;

            assert ( pEntity );
            ElementID ID = pEntity->GetID ();
            assert ( ID != INVALID_ELEMENT_ID );
            assert ( pEntity == CElementIDs::GetElement ( ID ) );
            if ( pEntity->IsBeingDeleted () )
                OutputDebugString ( SString ( "Server: 0x%08x  %s is flagged as IsBeingDeleted() but is still in GetEntitiesFromRoot\n", pEntity, pEntity->GetTypeName ().c_str () ) );

            assert ( mapResults.find ( pEntity ) == mapResults.end () );
            mapResults [ pEntity ] = 1;
        }
    }    
}


#endif

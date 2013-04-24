/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaACLDefs.cpp
*  PURPOSE:     Lua ACL function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// Helper function
static const char* GetResourceName ( lua_State* luaVM )
{
    CLuaMain * luaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
    return luaMain ? luaMain->GetScriptName () : "";
}


void CLuaACLDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "aclReload", CLuaACLDefs::aclReload );
    CLuaCFunctions::AddFunction ( "aclSave", CLuaACLDefs::aclSave );

    CLuaCFunctions::AddFunction ( "aclCreate", CLuaACLDefs::aclCreate );
    CLuaCFunctions::AddFunction ( "aclDestroy", CLuaACLDefs::aclDestroy );

    CLuaCFunctions::AddFunction ( "aclGet", CLuaACLDefs::aclGet );

    CLuaCFunctions::AddFunction ( "aclList", CLuaACLDefs::aclList );
    CLuaCFunctions::AddFunction ( "aclGetName", CLuaACLDefs::aclGetName );

    CLuaCFunctions::AddFunction ( "aclGetRight", CLuaACLDefs::aclGetRight );
    CLuaCFunctions::AddFunction ( "aclSetRight", CLuaACLDefs::aclSetRight );
    CLuaCFunctions::AddFunction ( "aclListRights", CLuaACLDefs::aclListRights );
    CLuaCFunctions::AddFunction ( "aclRemoveRight", CLuaACLDefs::aclRemoveRight );

    CLuaCFunctions::AddFunction ( "aclCreateGroup", CLuaACLDefs::aclCreateGroup );
    CLuaCFunctions::AddFunction ( "aclDestroyGroup", CLuaACLDefs::aclDestroyGroup );

    CLuaCFunctions::AddFunction ( "aclGetGroup", CLuaACLDefs::aclGetGroup );
    CLuaCFunctions::AddFunction ( "aclGroupList", CLuaACLDefs::aclGroupList );

    CLuaCFunctions::AddFunction ( "aclGroupGetName", CLuaACLDefs::aclGroupGetName );
    CLuaCFunctions::AddFunction ( "aclGroupAddACL", CLuaACLDefs::aclGroupAddACL );
    CLuaCFunctions::AddFunction ( "aclGroupListACL", CLuaACLDefs::aclGroupListACL );
    CLuaCFunctions::AddFunction ( "aclGroupRemoveACL", CLuaACLDefs::aclGroupRemoveACL );
    CLuaCFunctions::AddFunction ( "aclGroupAddObject", CLuaACLDefs::aclGroupAddObject );
    CLuaCFunctions::AddFunction ( "aclGroupListObjects", CLuaACLDefs::aclGroupListObjects );
    CLuaCFunctions::AddFunction ( "aclGroupRemoveObject", CLuaACLDefs::aclGroupRemoveObject );

    CLuaCFunctions::AddFunction ( "isObjectInACLGroup", CLuaACLDefs::isObjectInACLGroup );
    CLuaCFunctions::AddFunction ( "hasObjectPermissionTo", CLuaACLDefs::hasObjectPermissionTo );
}


int CLuaACLDefs::aclReload ( lua_State* luaVM )
{
    // Just reload the XML file
    bool bSuccess = m_pACLManager->Load ();

    // Return whether we succeeded or not
    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}


int CLuaACLDefs::aclSave ( lua_State* luaVM )
{
    // Just save the XML file
    bool bSuccess = m_pACLManager->Save ();

    // Return whether we succeeded or not
    lua_pushboolean ( luaVM, bSuccess );
    return 1;
}


int CLuaACLDefs::aclCreate ( lua_State* luaVM )
{
//  acl aclCreate ( string aclName )
    SString strACLName;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strACLName );
    
    if ( !argStream.HasErrors () )
    {
         // See that the name doesn't exist already
        CAccessControlList* pACL = m_pACLManager->GetACL ( strACLName );
        
        if ( !pACL )
        {
            // Create a new ACL with that name
            pACL = m_pACLManager->AddACL ( strACLName );
            CLogger::LogPrintf ( "ACL: %s: ACL '%s' created\n", GetResourceName ( luaVM ), pACL->GetName () );
            // Return the created ACL
            lua_pushacl ( luaVM, pACL );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclDestroy ( lua_State* luaVM )
{
//  bool aclDestroy ( acl theACL )
    CAccessControlList* pACL;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pACL );
    
    if ( !argStream.HasErrors () )
    {
        // Delete it
        CLogger::LogPrintf ( "ACL: %s: ACL '%s' deleted\n", GetResourceName ( luaVM ), pACL->GetName () );
        m_pACLManager->DeleteACL ( pACL );
        // Return true
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGet ( lua_State* luaVM )
{
//  acl aclGet ( string aclName )
    SString strACLName;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strACLName );
    
    if ( !argStream.HasErrors () )
    {
        // See that the name doesn't exist already
        CAccessControlList* pACL = m_pACLManager->GetACL ( strACLName );
        if ( pACL )
        {
            // Return the created ACL
            lua_pushacl ( luaVM, pACL );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclList ( lua_State* luaVM )
{
//  table aclList ()

    // Create a table to return into
    lua_newtable ( luaVM );

    // Loop through ACL stuff
    unsigned int uiIndex = 0;
    list <CAccessControlList* > ::const_iterator iter = m_pACLManager->ACL_Begin ();
    for ( ; iter != m_pACLManager->ACL_End (); iter++ )
    {
        // Push onto the table
        lua_pushnumber ( luaVM, ++uiIndex );
        lua_pushacl ( luaVM, *iter );
        lua_settable ( luaVM, -3 );
    }

    // Return true
    return 1;
}


int CLuaACLDefs::aclGetName ( lua_State* luaVM )
{
//  string aclGetName ( acl theAcl )
    CAccessControlList* pACL;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pACL );
    
    if ( !argStream.HasErrors () )
    {
        // Return its name
        lua_pushstring ( luaVM, pACL->GetName () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGetRight ( lua_State* luaVM )
{
//  bool aclGetRight ( acl theAcl, string rightName )
    CAccessControlList* pACL; SString strRight;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pACL );
    argStream.ReadString ( strRight );
    
    if ( !argStream.HasErrors () )
    {
        // Grab the type from the name passed
        const char* szRightAftedDot = strRight;
        CAccessControlListRight::ERightType eType;
        if ( StringBeginsWith ( strRight, "command." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
            szRightAftedDot += 8;
        }
        else if ( StringBeginsWith ( strRight, "function." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
            szRightAftedDot += 9;
        }
        else if ( StringBeginsWith ( strRight, "resource." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
            szRightAftedDot += 9;
        }
        else if ( StringBeginsWith ( strRight, "general." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_GENERAL;
            szRightAftedDot += 8;
        }
        else
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Grab the right from the name and type
        CAccessControlListRight* pACLRight = pACL->GetRight ( szRightAftedDot, eType );
        if ( pACLRight )
        {
            lua_pushboolean ( luaVM, pACLRight->GetRightAccess () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclSetRight ( lua_State* luaVM )
{
//  bool aclSetRight ( acl theAcl, string rightName, bool hasAccess )
    CAccessControlList* pACL; SString strRight; bool bAccess;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pACL );
    argStream.ReadString ( strRight );
    argStream.ReadBool ( bAccess );
    
    if ( !argStream.HasErrors () )
    {
        // Grab the type from the name passed
        const char* szRightAftedDot = strRight;
        CAccessControlListRight::ERightType eType;
        if ( StringBeginsWith ( strRight, "command." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
            szRightAftedDot += 8;
        }
        else if ( StringBeginsWith ( strRight, "function." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
            szRightAftedDot += 9;
        }
        else if ( StringBeginsWith ( strRight, "resource." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
            szRightAftedDot += 9;
        }
        else if ( StringBeginsWith ( strRight, "general." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_GENERAL;
            szRightAftedDot += 8;
        }
        else
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Grab the right from the name and type
        CAccessControlListRight* pACLRight = pACL->GetRight ( szRightAftedDot, eType );
        if ( pACLRight )
        {
            // Set the new access right
            if ( pACLRight->GetRightAccess () != bAccess )
                CLogger::LogPrintf ( "ACL: %s: Right '%s' changed to %s in ACL '%s'\n", GetResourceName ( luaVM ), strRight.c_str (), bAccess ? "ALLOW" : "DISALLOW", pACL->GetName () );
            pACLRight->SetRightAccess ( bAccess );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        // Try to add it
        pACLRight = pACL->AddRight ( szRightAftedDot, eType, bAccess );
        if ( pACLRight )
        {
            // LOGLEVEL_LOW to stop spam from admin resource at new server startup
            CLogger::LogPrintf ( LOGLEVEL_LOW, "ACL: %s: Right '%s' %s added in ACL '%s'\n", GetResourceName ( luaVM ), strRight.c_str (), bAccess ? "ALLOW" : "DISALLOW", pACL->GetName () );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclListRights ( lua_State* luaVM )
{
//  table aclListRights ( acl theACL )
    CAccessControlList* pACL; SString strType; bool bAll = true; CAccessControlListRight::ERightType eAllowed = (CAccessControlListRight::ERightType)-1;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pACL );
    if ( argStream.NextIsString () )
    {
        argStream.ReadString ( strType );

        bAll = false;
        if ( strType == "command" )
            eAllowed = CAccessControlListRight::RIGHT_TYPE_COMMAND;
        else if ( strType == "function" )
            eAllowed = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
        else if ( strType == "resource" )
            eAllowed = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
        else if ( strType == "general" )
            eAllowed = CAccessControlListRight::RIGHT_TYPE_GENERAL;
        else
            bAll = true;
    }
    
    if ( !argStream.HasErrors () )
    {
        // Create a table to return into
        lua_newtable ( luaVM );

        // Loop through ACL
        char szRightName [128];
        CAccessControlListRight::ERightType eType;
        unsigned int uiIndex = 0;
        list <CAccessControlListRight* > ::const_iterator iter = pACL->IterBegin ();
        for ( ; iter != pACL->IterEnd (); iter++ )
        {
            // Type
            eType = (*iter)->GetRightType ();
        	if ( !bAll && eType != eAllowed )
        		continue;

            switch ( eType )
            {
                case CAccessControlListRight::RIGHT_TYPE_COMMAND:
                strcpy ( szRightName, "command." );
                break;

                case CAccessControlListRight::RIGHT_TYPE_FUNCTION:
                strcpy ( szRightName, "function." );
                break;

                case CAccessControlListRight::RIGHT_TYPE_RESOURCE:
                strcpy ( szRightName, "resource." );
                break;

                case CAccessControlListRight::RIGHT_TYPE_GENERAL:
                strcpy ( szRightName, "general." );
                break;

                default:
                strcpy ( szRightName, "unknown." );
                break;
            }

            // Append right name
            strncat ( szRightName, (*iter)->GetRightName (), 128 );

            // Push its name onto the table
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushstring ( luaVM, szRightName );
            lua_settable ( luaVM, -3 );
        }

        // Return the table
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclRemoveRight ( lua_State* luaVM )
{
//  bool aclRemoveRight ( acl theAcl, string rightName )
    CAccessControlList* pACL; SString strRight;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pACL );
    argStream.ReadString ( strRight );
    
    if ( !argStream.HasErrors () )
    {
        // Grab the type from the name passed
        const char* szRightAftedDot = strRight;
        CAccessControlListRight::ERightType eType;
        if ( StringBeginsWith ( strRight, "command." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
            szRightAftedDot += 8;
        }
        else if ( StringBeginsWith ( strRight, "function." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
            szRightAftedDot += 9;
        }
        else if ( StringBeginsWith ( strRight, "resource." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
            szRightAftedDot += 9;
        }
        else if ( StringBeginsWith ( strRight, "general." ) )
        {
            eType = CAccessControlListRight::RIGHT_TYPE_GENERAL;
            szRightAftedDot += 8;
        }
        else
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Try removing the right
        CAccessControlListRight* pACLRight = pACL->GetRight ( szRightAftedDot, eType );
        bool bAccess = pACLRight && pACLRight->GetRightAccess ();
        if ( pACL->RemoveRight ( szRightAftedDot, eType ) )
        {
            CLogger::LogPrintf ( "ACL: %s: Right '%s' %s removed from ACL '%s'\n", GetResourceName ( luaVM ), strRight.c_str (), bAccess ? "ALLOW" : "DISALLOW", pACL->GetName () );
            // Return success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclCreateGroup ( lua_State* luaVM )
{
//  aclgroup aclCreateGroup ( string groupName )
    SString strGroup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strGroup );
    
    if ( !argStream.HasErrors () )
    {
        // Find the ACL specified
        CAccessControlListGroup* pGroup = m_pACLManager->GetGroup ( strGroup );
        if ( !pGroup )
        {
            // Create the group
            pGroup = m_pACLManager->AddGroup ( strGroup );
            CLogger::LogPrintf ( "ACL: %s: Group '%s' created\n", GetResourceName ( luaVM ), pGroup->GetGroupName () );
            // And return it
            lua_pushaclgroup ( luaVM, pGroup );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclDestroyGroup ( lua_State* luaVM )
{
//  bool aclDestroyGroup ( aclgroup aclGroup )
    CAccessControlListGroup* pGroup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    
    if ( !argStream.HasErrors () )
    {
        // Delete it
        CLogger::LogPrintf ( "ACL: %s: Group '%s' deleted\n", GetResourceName ( luaVM ), pGroup->GetGroupName () );
        m_pACLManager->DeleteGroup ( pGroup );
        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGetGroup ( lua_State* luaVM )
{
    // aclgroup aclGetGroup ( string groupName )
    SString strGroup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strGroup );
    
    if ( !argStream.HasErrors () )
    {
         // Find the ACL specified
        CAccessControlListGroup* pGroup = m_pACLManager->GetGroup ( strGroup );
        if ( pGroup )
        {
            // Return the group
            lua_pushaclgroup ( luaVM, pGroup );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupList ( lua_State* luaVM )
{
//  table aclGroupList ()

    // Create a table to return into
    lua_newtable ( luaVM );

    // Loop through ACL stuff
    unsigned int uiIndex = 0;
    list <CAccessControlListGroup* > ::const_iterator iter = m_pACLManager->Groups_Begin ();
    for ( ; iter != m_pACLManager->Groups_End (); iter++ )
    {
        // Push onto the table
        lua_pushnumber ( luaVM, ++uiIndex );
        lua_pushaclgroup ( luaVM, *iter );
        lua_settable ( luaVM, -3 );
    }

    // Return table
    return 1;
}

int CLuaACLDefs::aclGroupGetName ( lua_State* luaVM )
{
//  string aclGroupGetName ( aclGroup )
    CAccessControlListGroup* pGroup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    
    if ( !argStream.HasErrors () )
    {
        // Return its name
        lua_pushstring ( luaVM, pGroup->GetGroupName () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupAddACL ( lua_State* luaVM )
{
//  bool aclGroupAddACL ( aclgroup theGroup, acl theACL )
    CAccessControlListGroup* pGroup; CAccessControlList* pACL;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    argStream.ReadUserData ( pACL );
    
    if ( !argStream.HasErrors () )
    {
        // Add the ACL to the group
        pGroup->AddACL ( pACL );
        CLogger::LogPrintf ( "ACL: %s: ACL '%s' added to group '%s'\n", GetResourceName ( luaVM ), pACL->GetName (), pGroup->GetGroupName () );
        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupListACL ( lua_State* luaVM )
{
//  table aclGroupListACL ( aclgroup theGroup )
    CAccessControlListGroup* pGroup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    
    if ( !argStream.HasErrors () )
    {
        // Create a table to return into
        lua_newtable ( luaVM );

        // Loop through ACL stuff
        unsigned int uiIndex = 0;
        list <CAccessControlList* > ::const_iterator iter = pGroup->IterBeginACL ();
        for ( ; iter != pGroup->IterEndACL (); iter++ )
        {
            // Push onto the table
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushacl ( luaVM, *iter );
            lua_settable ( luaVM, -3 );
        }
        // Return the table
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Return true
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupRemoveACL ( lua_State* luaVM )
{
//  bool aclGroupRemoveACL ( aclgroup theGroup, acl theACL )
    CAccessControlListGroup* pGroup; CAccessControlList* pACL;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    argStream.ReadUserData ( pACL );
    
    if ( !argStream.HasErrors () )
    {
        // Add the ACL to the group
        pGroup->RemoveACL ( pACL );
        CLogger::LogPrintf ( "ACL: %s: ACL '%s' removed from group '%s'\n", GetResourceName ( luaVM ), pACL->GetName (), pGroup->GetGroupName () );
        // Return success
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupAddObject ( lua_State* luaVM )
{
//  bool aclGroupAddObject ( aclgroup theGroup, string theObjectName )
    CAccessControlListGroup* pGroup; SString strObject;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    argStream.ReadString ( strObject );
    
    if ( !argStream.HasErrors () )
    {
        // Figure out what type of object this is
        const char* szObjectAfterDot = strObject;
        CAccessControlListGroupObject::EObjectType eType;
        if ( StringBeginsWith ( strObject, "resource." ) )
        {
            eType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
            szObjectAfterDot += 9;
        }
        else if ( StringBeginsWith ( strObject, "user." ) )
        {
            eType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
            szObjectAfterDot += 5;
        }
        else
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Make sure it doesn't already exist
        if ( !pGroup->FindObjectMatch ( szObjectAfterDot, eType ) )
        {
            // Set it
            pGroup->AddObject ( szObjectAfterDot, eType );
            CLogger::LogPrintf ( "ACL: %s: Object '%s' added to group '%s'\n", GetResourceName ( luaVM ), strObject.c_str (), pGroup->GetGroupName () );
            // Return success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupListObjects ( lua_State* luaVM )
{
    // table aclGroupListObjects ( aclgroup theGroup )
    CAccessControlListGroup* pGroup;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    
    if ( !argStream.HasErrors () )
    {
        // Create a table to return into
        lua_newtable ( luaVM );

        // Loop through ACL stuff
        char szBuffer [255];
        unsigned int uiIndex = 0;
        list <CAccessControlListGroupObject* > ::const_iterator iter = pGroup->IterBeginObjects ();
        for ( ; iter != pGroup->IterEndObjects (); iter++ )
        {
            // Put the base type depending on the type
            switch ( (*iter)->GetObjectType () )
            {
                case CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE:
                    strcpy ( szBuffer, "resource." );
                    break;

                case CAccessControlListGroupObject::OBJECT_TYPE_USER:
                    strcpy ( szBuffer, "user." );
                    break;
            };

            // Append the object name
            strncat ( szBuffer, (*iter)->GetObjectName (), 254 );

            // Push its name onto the table
            lua_pushnumber ( luaVM, ++uiIndex );
            lua_pushstring ( luaVM, szBuffer );
            lua_settable ( luaVM, -3 );
        }
        // Return the table
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupRemoveObject ( lua_State* luaVM )
{
//  bool aclGroupRemoveObject ( aclgroup theGroup, string theObjectString )
    CAccessControlListGroup* pGroup; SString strObject;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pGroup );
    argStream.ReadString ( strObject );
    
    if ( !argStream.HasErrors () )
    {
        // Figure out what type of object this is
        const char* szObjectAfterDot = strObject;
        CAccessControlListGroupObject::EObjectType eType;

        if ( StringBeginsWith ( strObject, "resource." ) )
        {
            eType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
            szObjectAfterDot += 9;
        }
        else if ( StringBeginsWith ( strObject, "user." ) )
        {
            eType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
            szObjectAfterDot += 5;
        }
        else
        {
            lua_pushboolean ( luaVM, false );
            return 1;
        }

        // Remove it
        if ( pGroup->RemoveObject ( szObjectAfterDot, eType ) )
        {
            // Return success
            CLogger::LogPrintf ( "ACL: %s: Object '%s' removed from group '%s'\n", GetResourceName ( luaVM ), strObject.c_str (), pGroup->GetGroupName () );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::hasObjectPermissionTo ( lua_State* luaVM )
{
//  bool hasObjectPermissionTo ( string / element theObject, string theAction [, bool defaultPermission = true ] )
    CResource* pResource = NULL; CElement* pElement = NULL; SString strObject; SString strRightName; bool bDefault; CAccessControlListGroupObject::EObjectType eObjectType;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserDataOfType < CResource > () )
        argStream.ReadUserData ( pResource );
    else
    if ( argStream.NextIsUserDataOfType < CElement > () )
        argStream.ReadUserData ( pElement );
    else
        argStream.ReadString ( strObject );

    argStream.ReadString ( strRightName );
    argStream.ReadBool ( bDefault, true );

    if ( !argStream.HasErrors () )
    {
        if ( pResource )
        {
            // Grab the resource's name
            strObject = pResource->GetName ();
            eObjectType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
        }
        else if ( pElement )
        {
            // Grab the client this player/console/whatever is
            CClient* pClient = pElement->GetClient ();
            if ( pClient )
            {
                // Get his account
                CAccount* pAccount = pClient->GetAccount ();
                if ( pAccount )
                {
                    // Grab the username
                    strObject = pAccount->GetName ();
                    eObjectType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
                }
            }
        }
        else
        {
            // Extract the object name itself including the type
            const char * szName = CAccessControlListManager::ExtractObjectName ( strObject.c_str (), eObjectType );
            strObject = szName ? szName : "";
        }

        // Got a string?
        if ( !strObject.empty () )
        {
            // Extract the right name itself including the type
            CAccessControlListRight::ERightType eRightType;
            strRightName = CAccessControlListManager::ExtractRightName ( strRightName, eRightType );

            // Did we get a right name without the prefix?
            if ( strRightName )
            {
                bool bHasPermission = m_pACLManager->CanObjectUseRight ( strObject, eObjectType, strRightName, eRightType, bDefault );

                // Return whether we had access or not
                lua_pushboolean ( luaVM, bHasPermission );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}

int CLuaACLDefs::isObjectInACLGroup ( lua_State* luaVM )
{
//  bool isObjectInACLGroup ( string theObject, aclgroup theGroup )
    SString strObject; CAccessControlListGroup* pGroup; CAccessControlListGroupObject::EObjectType GroupObjectType;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strObject );
    argStream.ReadUserData ( pGroup );
    
    if ( !argStream.HasErrors () )
    {
        // Figure out what type of object this is
        const char* szObjectAfterDot = strObject;
        if ( StringBeginsWith ( strObject, "resource." ) ) {
            szObjectAfterDot += 9;
            GroupObjectType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
        }
        else if ( StringBeginsWith ( strObject, "user." ) )
        {
            szObjectAfterDot += 5;
            GroupObjectType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
        }
        else
        {
            // Invalid group type
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        if ( pGroup->FindObjectMatch ( szObjectAfterDot, GroupObjectType ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

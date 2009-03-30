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
    bool bSuccess = m_pACLManager->Load ( m_pMainConfig->GetAccessControlListFile ().c_str () );

    // Return whether we succeeded or not
	lua_pushboolean ( luaVM, bSuccess );
	return 1;
}


int CLuaACLDefs::aclSave ( lua_State* luaVM )
{
    // Just save the XML file
    bool bSuccess = m_pACLManager->Save ( m_pMainConfig->GetAccessControlListFile ().c_str () );

    // Return whether we succeeded or not
	lua_pushboolean ( luaVM, bSuccess );
	return 1;
}


int CLuaACLDefs::aclCreate ( lua_State* luaVM )
{
    // Verify the argument types
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
	{
        // Grab the arguments
		const char* szACLName = lua_tostring ( luaVM, 1 );

        // See that the name doesn't exist already
	    CAccessControlList* pACL = m_pACLManager->GetACL ( szACLName );
	    if ( !pACL )
	    {
            // Create a new ACL with that name
            pACL = m_pACLManager->AddACL ( szACLName );

            // Return the created ACL
            lua_pushacl ( luaVM, pACL );
            return 1;
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclCreate" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclDestroy ( lua_State* luaVM )
{
    // Verify the argument types
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab the arguments
        CAccessControlList* pACL = lua_toacl ( luaVM, 1 );
        if ( pACL )
        {
            // Delete it
            m_pACLManager->DeleteACL ( pACL );

            // Return true
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclDestroy" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclGet ( lua_State* luaVM )
{
    // Verify the argument types
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
	{
        // Grab the arguments
		const char* szACLName = lua_tostring ( luaVM, 1 );

        // See that the name doesn't exist already
	    CAccessControlList* pACL = m_pACLManager->GetACL ( szACLName );
	    if ( pACL )
	    {
            // Return the created ACL
            lua_pushacl ( luaVM, pACL );
            return 1;
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGet" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclList ( lua_State* luaVM )
{
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
    // Verify the argument types
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab and verify the ACL pointer
        CAccessControlList* pACL = lua_toacl ( luaVM, 1 );
        if ( pACL )
        {
            // Return its name
            lua_pushstring ( luaVM, pACL->GetName () );
            return 1;
        }
    }

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclGetRight ( lua_State* luaVM )
{
    // Verify the argument types
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
		 lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        // Grab the arguments
        CAccessControlList* pACL = lua_toacl ( luaVM, 1 );
	    char* szRight = (char*) lua_tostring ( luaVM, 2 );
		bool bAccess = lua_toboolean ( luaVM, 3 ) ?true:false;

        // Verify the ACL pointer
        if ( pACL )
        {
            // Grab the type from the name passed 
            char* szRightAftedDot = szRight;
			CAccessControlListRight::ERightType eType;
			if ( StringBeginsWith ( szRight, "command." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
                szRightAftedDot += 8;
            }
			else if ( StringBeginsWith ( szRight, "function." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
                szRightAftedDot += 9;
            }
			else if ( StringBeginsWith ( szRight, "resource." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
                szRightAftedDot += 9;
            }
			else if ( StringBeginsWith ( szRight, "general." ) )
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
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGetRight" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclSetRight ( lua_State* luaVM )
{
    // Verify the argument types
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
		 lua_type ( luaVM, 2 ) == LUA_TSTRING &&
	     lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
	{
        // Grab the arguments
		CAccessControlList* pACL = lua_toacl ( luaVM, 1 );
	    char* szRight = (char*) lua_tostring ( luaVM, 2 );
		bool bAccess = lua_toboolean ( luaVM, 3 ) ?true:false;

        // Verify the ACL pointer
        if ( pACL )
        {
            // Grab the type from the name passed
            char* szRightAftedDot = szRight;
			CAccessControlListRight::ERightType eType;
			if ( StringBeginsWith ( szRight, "command." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
                szRightAftedDot += 8;
            }
			else if ( StringBeginsWith ( szRight, "function." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
                szRightAftedDot += 9;
            }
			else if ( StringBeginsWith ( szRight, "resource." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
                szRightAftedDot += 9;
            }
			else if ( StringBeginsWith ( szRight, "general." ) )
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
                pACLRight->SetRightAccess ( bAccess );
                lua_pushboolean ( luaVM, true );
			    return 1;
			}

            // Try to add it
			pACLRight = pACL->AddRight ( szRightAftedDot, eType, bAccess );
			if ( pACLRight )
			{
                // Return success
				lua_pushboolean ( luaVM, true );
				return 1;
			}
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclSetRight" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclListRights ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab and verify the ACL
        CAccessControlList* pACL = lua_toacl ( luaVM, 1 );
        if ( pACL )
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
                switch ( eType )
                {
                    case CAccessControlListRight::RIGHT_TYPE_COMMAND:
                        strcpy ( szRightName, "command." );
                        break;

                    case CAccessControlListRight::RIGHT_TYPE_FUNCTION:
                        strcpy ( szRightName, "function." );
                        break;

                    case  CAccessControlListRight::RIGHT_TYPE_RESOURCE:
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
    }

    // Return true
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclRemoveRight ( lua_State* luaVM )
{
    // Verify the arguents
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
		 lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        // Grab the argument strings
		CAccessControlList* pACL = lua_toacl ( luaVM, 1 );
		char* szRight = (char*) lua_tostring ( luaVM, 2 );

        // Verify the ACL pointer
        if ( pACL )
        {
            // Grab the type from the name passed
            char* szRightAftedDot = szRight;
			CAccessControlListRight::ERightType eType;
			if ( StringBeginsWith ( szRight, "command." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_COMMAND;
                szRightAftedDot += 8;
            }
			else if ( StringBeginsWith ( szRight, "function." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_FUNCTION;
                szRightAftedDot += 9;
            }
			else if ( StringBeginsWith ( szRight, "resource." ) )
            {
				eType = CAccessControlListRight::RIGHT_TYPE_RESOURCE;
                szRightAftedDot += 9;
            }
			else if ( StringBeginsWith ( szRight, "general." ) )
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
			if ( pACL->RemoveRight ( szRightAftedDot, eType ) )
			{
                // Return success
				lua_pushboolean ( luaVM, true );
				return 1;
			}
		}
	}
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclRemoveRight" );

	lua_pushboolean ( luaVM, false );
	return 1;
}


int CLuaACLDefs::aclCreateGroup ( lua_State* luaVM )
{
    // Verify the arguents
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
	{
        // Grab the argument strings
		char* szGroup = (char*) lua_tostring ( luaVM, 1 );

        // Find the ACL specified
		CAccessControlListGroup* pGroup = m_pACLManager->GetGroup ( szGroup );
		if ( !pGroup )
		{
            // Create the group
            pGroup = m_pACLManager->AddGroup ( szGroup );

            // And return it
            lua_pushaclgroup ( luaVM, pGroup );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclCreateGroup" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclDestroyGroup ( lua_State* luaVM )
{
    // Verify the arguents
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
         // Grab the arguments
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );

        // Verify the group
        if ( pGroup )
        {
            // Delete it
            m_pACLManager->DeleteGroup ( pGroup );

            // Return success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclDestroyGroup" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGetGroup ( lua_State* luaVM )
{
    // Verify the arguents
	if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
	{
        // Grab the argument strings
		char* szGroup = (char*) lua_tostring ( luaVM, 1 );

        // Find the ACL specified
		CAccessControlListGroup* pGroup = m_pACLManager->GetGroup ( szGroup );
		if ( pGroup )
		{
            // Return the group
            lua_pushaclgroup ( luaVM, pGroup );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGetGroup" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupList ( lua_State* luaVM )
{
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
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab the arguments
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );

        // Verify the group
        if ( pGroup )
        {
            // Return its name
            lua_pushstring ( luaVM, pGroup->GetGroupName () );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGroupGetName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupAddACL ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab the arguments
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );
        CAccessControlList* pACL = lua_toacl ( luaVM, 2 );

        // Verify the group and ACL
        if ( pGroup && pACL )
        {
            // Add the ACL to the group
            pGroup->AddACL ( pACL );

            // Return success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGroupAddACL" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupListACL ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab and verify the group
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );
        if ( pGroup )
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
    }

    // Return true
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupRemoveACL ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab the arguments
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );
        CAccessControlList* pACL = lua_toacl ( luaVM, 2 );

        // Verify the group and ACL
        if ( pGroup && pACL )
        {
            // Add the ACL to the group
            pGroup->RemoveACL ( pACL );

            // Return success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGroupRemoveACL" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupAddObject ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        // Grab the arguments
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );
        const char* szObject = lua_tostring ( luaVM, 2 );

        // Verify the group and ACL
        if ( pGroup )
        {
            // Figure out what type of object this is
            const char* szObjectAfterDot = szObject;
            CAccessControlListGroupObject::EObjectType eType;
			if ( StringBeginsWith ( szObject, "resource." ) )
            {
                eType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
                szObjectAfterDot += 9;
            }
			else if ( StringBeginsWith ( szObject, "user." ) )
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

                // Return success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGroupAddObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupListObjects ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
	{
        // Grab and verify the group
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );
        if ( pGroup )
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
    }

    // Return true
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::aclGroupRemoveObject ( lua_State* luaVM )
{
    // Verify the arguents  
	if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
	{
        // Grab the arguments
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 1 );
        const char* szObject = lua_tostring ( luaVM, 2 );

        // Verify the group and ACL
        if ( pGroup )
        {
            // Figure out what type of object this is
            const char* szObjectAfterDot = szObject;
            CAccessControlListGroupObject::EObjectType eType;
			if ( StringBeginsWith ( szObject, "resource." ) )
            {
                eType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
                szObjectAfterDot += 9;
            }
			else if ( StringBeginsWith ( szObject, "user." ) )
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
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
	else
		m_pScriptDebugging->LogBadType ( luaVM, "aclGroupRemoveObject" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaACLDefs::hasObjectPermissionTo ( lua_State* luaVM )
{
    // What object name we're going to check
    CAccessControlListGroupObject::EObjectType eObjectType;
    std::string strObject;

    // Got a pointer argument?
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab it
        CResource* pResource = lua_toresource ( luaVM, 1 );
        CElement* pElement = lua_toelement ( luaVM, 1 );

        // Is it a valid resource?
        if ( pResource )
        {
            // Grab the resource's name
            strObject = pResource->GetName ();
            eObjectType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
        }

        // Is this a valid element?
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
    }

    // Got a string argument?
    else if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Change the pointer to point to our argument
        strObject = lua_tostring ( luaVM, 1 );

        // Extract the object name itself including the type
        const char * szName = CAccessControlListManager::ExtractObjectName ( strObject.c_str (), eObjectType );
        strObject = szName ? szName : "";
    }

    // Got a string?
    if ( !strObject.empty () )
    {
        // Got a string with the action to check for permission?
        if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            // Grab the right name we should've gotten passed
            const char* szRightName = lua_tostring ( luaVM, 2 );

            // Extract the right name itself including the type
            CAccessControlListRight::ERightType eRightType;
            szRightName = CAccessControlListManager::ExtractRightName ( szRightName, eRightType );

            // Did we get a right name without the prefix?
            if ( szRightName )
            {
                // Third argument to specify what the return defaults to.
                // This is if no ACL could be found.
                bool bDefault = true;
                if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                {
                    bDefault = lua_toboolean ( luaVM, 3 ) ? true:false;
                }

                // Check whether he has permissions to do that
                bool bHasPermission = m_pACLManager->CanObjectUseRight ( strObject.c_str (),
                                                                         eObjectType,
                                                                         szRightName,
                                                                         eRightType,
                                                                         bDefault );

                // Return whether we had access or not
                lua_pushboolean ( luaVM, bHasPermission );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "hasObjectPermissionTo" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "hasObjectPermissionTo" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "hasObjectPermissionTo" );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}

int CLuaACLDefs::isObjectInACLGroup ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
	{
        const char* szObject = lua_tostring ( luaVM, 1 );
        CAccessControlListGroup* pGroup = lua_toaclgroup ( luaVM, 2 );
        CAccessControlListGroupObject::EObjectType GroupObjectType;

        if ( pGroup && m_pACLManager->VerifyGroup ( pGroup ) )
        {
            if ( StringBeginsWith ( szObject, "resource." ) ) {
                szObject += 9;
                GroupObjectType = CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE;
            }
            else if ( StringBeginsWith ( szObject, "user." ) )
            {
                szObject += 5;
                GroupObjectType = CAccessControlListGroupObject::OBJECT_TYPE_USER;
            }
            else
            {
                // Invalid group type
                lua_pushboolean ( luaVM, false );
                return 1;
            }
            if ( pGroup->FindObjectMatch ( szObject, GroupObjectType ) )
            { 
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}

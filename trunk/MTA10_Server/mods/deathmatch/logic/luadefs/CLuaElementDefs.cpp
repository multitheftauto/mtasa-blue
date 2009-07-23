/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaElementDefs.cpp
*  PURPOSE:     Lua element definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaElementDefs::LoadFunctions ( void )
{
    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "attachElementToElement", CLuaElementDefs::attachElements );
    CLuaCFunctions::AddFunction ( "detachElementFromElement", CLuaElementDefs::detachElements );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **



    // Create/destroy
    CLuaCFunctions::AddFunction ( "createElement", CLuaElementDefs::createElement );
    CLuaCFunctions::AddFunction ( "destroyElement", CLuaElementDefs::destroyElement );
    CLuaCFunctions::AddFunction ( "cloneElement", CLuaElementDefs::cloneElement );

    // Get
    CLuaCFunctions::AddFunction ( "isElement", CLuaElementDefs::isElement );
    CLuaCFunctions::AddFunction ( "isElementWithinColShape", CLuaElementDefs::isElementWithinColShape );
    CLuaCFunctions::AddFunction ( "isElementWithinMarker", CLuaElementDefs::isElementWithinMarker );
    CLuaCFunctions::AddFunction ( "isElementInWater", CLuaElementDefs::isElementInWater );

    CLuaCFunctions::AddFunction ( "getElementByID", CLuaElementDefs::getElementByID );
    CLuaCFunctions::AddFunction ( "getElementByIndex", CLuaElementDefs::getElementByIndex );
    CLuaCFunctions::AddFunction ( "getElementChildren", CLuaElementDefs::getElementChildren );
    CLuaCFunctions::AddFunction ( "getElementChild", CLuaElementDefs::getElementChild );
    CLuaCFunctions::AddFunction ( "getElementChildrenCount", CLuaElementDefs::getElementChildrenCount );
    CLuaCFunctions::AddFunction ( "getAllElementData", CLuaElementDefs::getAllElementData );
    CLuaCFunctions::AddFunction ( "getElementID", CLuaElementDefs::getElementID );
    CLuaCFunctions::AddFunction ( "getElementParent", CLuaElementDefs::getElementParent );
    CLuaCFunctions::AddFunction ( "getElementPosition", CLuaElementDefs::getElementPosition );
    CLuaCFunctions::AddFunction ( "getElementVelocity", CLuaElementDefs::getElementVelocity );
    CLuaCFunctions::AddFunction ( "getElementsByType", CLuaElementDefs::getElementsByType );
    CLuaCFunctions::AddFunction ( "getElementType", CLuaElementDefs::getElementType );
    CLuaCFunctions::AddFunction ( "getElementInterior", CLuaElementDefs::getElementInterior );
    CLuaCFunctions::AddFunction ( "getElementsWithinColShape", CLuaElementDefs::getElementsWithinColShape );
    CLuaCFunctions::AddFunction ( "getElementDimension", CLuaElementDefs::getElementDimension );
    CLuaCFunctions::AddFunction ( "getElementZoneName", CLuaElementDefs::getElementZoneName );
    CLuaCFunctions::AddFunction ( "getElementColShape", CLuaElementDefs::getElementColShape );
    CLuaCFunctions::AddFunction ( "getElementAlpha", CLuaElementDefs::getElementAlpha );
    CLuaCFunctions::AddFunction ( "getElementHealth", CLuaElementDefs::getElementHealth );
    CLuaCFunctions::AddFunction ( "getElementModel", CLuaElementDefs::getElementModel );
    CLuaCFunctions::AddFunction ( "getElementSyncer", CLuaElementDefs::getElementSyncer );

    // Attachement
    CLuaCFunctions::AddFunction ( "attachElements", CLuaElementDefs::attachElements );
    CLuaCFunctions::AddFunction ( "detachElements", CLuaElementDefs::detachElements );
    CLuaCFunctions::AddFunction ( "isElementAttached", CLuaElementDefs::isElementAttached );
    CLuaCFunctions::AddFunction ( "getAttachedElements", CLuaElementDefs::getAttachedElements );
    CLuaCFunctions::AddFunction ( "getElementAttachedTo", CLuaElementDefs::getElementAttachedTo );

    // Element data
    CLuaCFunctions::AddFunction ( "getElementData", CLuaElementDefs::getElementData );
    CLuaCFunctions::AddFunction ( "setElementData", CLuaElementDefs::setElementData );
    CLuaCFunctions::AddFunction ( "removeElementData", CLuaElementDefs::removeElementData );

    // Set
    CLuaCFunctions::AddFunction ( "setElementID", CLuaElementDefs::setElementID );
    CLuaCFunctions::AddFunction ( "setElementParent", CLuaElementDefs::setElementParent );
    CLuaCFunctions::AddFunction ( "setElementPosition", CLuaElementDefs::setElementPosition );
    CLuaCFunctions::AddFunction ( "setElementVelocity", CLuaElementDefs::setElementVelocity );
    CLuaCFunctions::AddFunction ( "setElementVisibleTo", CLuaElementDefs::setElementVisibleTo );
    CLuaCFunctions::AddFunction ( "clearElementVisibleTo", CLuaElementDefs::clearElementVisibleTo );
    CLuaCFunctions::AddFunction ( "isElementVisibleTo", CLuaElementDefs::isElementVisibleTo );
    CLuaCFunctions::AddFunction ( "setElementInterior", CLuaElementDefs::setElementInterior );
    CLuaCFunctions::AddFunction ( "setElementDimension", CLuaElementDefs::setElementDimension );
    CLuaCFunctions::AddFunction ( "setElementAlpha", CLuaElementDefs::setElementAlpha );
    CLuaCFunctions::AddFunction ( "setElementHealth", CLuaElementDefs::setElementHealth );
    CLuaCFunctions::AddFunction ( "setElementModel", CLuaElementDefs::setElementModel );
}


int CLuaElementDefs::createElement ( lua_State* luaVM )
{
    // Grab the optional name argument
    char szDefaultID [] = "";
    const char* szID = szDefaultID;
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument2 == LUA_TSTRING )
    {
        szID = lua_tostring ( luaVM, 2 );
    }
    else if ( iArgument2 != LUA_TNONE )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "createElement" );
        lua_pushboolean ( luaVM, false );
        return 1;
    }

    // Verify argument types
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TSTRING )
    {
        // Grab the string
        const char* szTypeName = lua_tostring ( luaVM, 1 );

		// Get the virtual machine
		CLuaMain * pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
			// Get the resource
			CResource* pResource = pLuaMain->GetResource();
			if ( pResource )
			{
				// Try to create
				CDummy* pDummy = CStaticFunctionDefinitions::CreateElement ( pResource, szTypeName, szID );
				if ( pDummy )
				{
					// Get the group
					CElementGroup * pGroup = pResource->GetElementGroup();
					if ( pGroup )
					{
						pGroup->Add ( pDummy );
					}
					lua_pushelement ( luaVM, pDummy );
					return 1;
				}
			}
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "createElement" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::destroyElement ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::DestroyElement ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "destroyElement", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "destroyElement" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaElementDefs::cloneElement ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CVector vecPosition = pElement->GetPosition ();
            bool bCloneChildren = false;
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
            {
                vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
                int iArgument3 = lua_type ( luaVM, 3 );
                if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
                {
                    vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
                    int iArgument4 = lua_type ( luaVM, 4 );
                    if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
                    {
                        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

                        if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
                        {
                            bCloneChildren = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
                        }
                    }
                }
            }
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
				CResource * pResource = pLuaMain->GetResource();
                if ( pResource )
                {
					CElement* pNewElement = CStaticFunctionDefinitions::CloneElement ( pResource, pElement, vecPosition, bCloneChildren );

					if ( pNewElement )
					{
						lua_pushelement ( luaVM, pNewElement );
						return 1;
					}
				}
			}
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "cloneElement", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "cloneElement" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::isElement ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementChildren ( lua_State* luaVM )
{
    // Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            // Grab the argument
            CElement* pElement = lua_toelement ( luaVM, 1 );
            if ( pElement )
            {
                // Create a new table
                lua_newtable ( luaVM );

                // Add all the elements with a matching type to it
                pElement->GetChildren ( luaVM );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getElementChildren", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getElementChildren" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementChild ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned int uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2  ) );
        if ( pElement )
        {
            CElement* pChild = CStaticFunctionDefinitions::GetElementChild ( pElement, uiIndex );
            if ( pChild )
            {
                lua_pushelement ( luaVM, pChild );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementChild", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementChild" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementChildrenCount ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned int uiCount;
            if ( CStaticFunctionDefinitions::GetElementChildrenCount ( pElement, uiCount ) )
            {
                lua_pushnumber ( luaVM, static_cast < lua_Number > ( uiCount ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementChildrenCount", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementChildrenCount" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementID ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            lua_pushstring ( luaVM, pElement->GetName ().c_str () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementID", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementByID ( lua_State* luaVM )
{
    // Eventually read out the index
    int iArgument2 = lua_type ( luaVM, 2 );
    unsigned int uiIndex = 0;
    if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
    {
        uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
    }

    // Check that we got the name in plac
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        const char* szID = lua_tostring ( luaVM, 1 );
        CElement* pElement = CStaticFunctionDefinitions::GetElementByID ( szID, uiIndex );
        if ( pElement )
        {
            lua_pushelement ( luaVM, pElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementByID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementByIndex ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        const char* szType = lua_tostring ( luaVM, 1 );
        unsigned int uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );

        CElement* pElement = CStaticFunctionDefinitions::GetElementByIndex ( szType, uiIndex );
        if ( pElement )
        {
            lua_pushelement ( luaVM, pElement );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementByIndex" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementData ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szName = lua_tostring ( luaVM, 2 );
        bool bInherit = true;

        if ( lua_type ( luaVM, 3) == LUA_TBOOLEAN )
            bInherit = ( lua_toboolean ( luaVM, 3 ) ) ? true:false;

        if ( pElement )
        {
            CLuaArgument* pVariable = CStaticFunctionDefinitions::GetElementData ( pElement, szName, bInherit );
            if ( pVariable )
            {
                pVariable->Push ( luaVM );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementData", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaElementDefs::getAllElementData ( lua_State* luaVM )
{
    // element
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
//          _asm int 3
//          CLuaArguments * pTable = new CLuaArguments();
			CLuaArguments Args;
            if ( CStaticFunctionDefinitions::GetAllElementData ( pElement, &Args ) )
            {
                Args.PushAsTable ( luaVM );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getAllElementData", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getAllElementData" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementParent ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CElement* pParent = CStaticFunctionDefinitions::GetElementParent ( pElement );
            if ( pParent )
            {
                lua_pushelement ( luaVM, pParent );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementParent", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementParent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementPosition ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element, verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Grab the position
            CVector vecPosition;
            if ( CStaticFunctionDefinitions::GetElementPosition ( pElement, vecPosition ) )
            {
                // Return it
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                lua_pushnumber ( luaVM, vecPosition.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementPosition", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementVelocity ( lua_State* luaVM )
{
    // Verify the argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element, verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Grab the velocity
            CVector vecVelocity;
            if ( CStaticFunctionDefinitions::GetElementVelocity ( pElement, vecVelocity ) )
            {
                // Return it
                lua_pushnumber ( luaVM, vecVelocity.fX );
                lua_pushnumber ( luaVM, vecVelocity.fY );
                lua_pushnumber ( luaVM, vecVelocity.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementVelocity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementType ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            lua_pushstring ( luaVM, pElement->GetTypeName ().c_str () );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementType", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementType" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementsByType ( lua_State* luaVM )
{
    // Find our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING &&
            ( iArgument2 == LUA_TNONE || iArgument2 == LUA_TLIGHTUSERDATA ) )
        {
            // see if a root argument has been specified
            CElement * startAt = m_pRootElement;
            if ( iArgument2 == LUA_TLIGHTUSERDATA )
            {
                // if its valid, use that, otherwise, produce an error
                CElement * pElement = lua_toelement ( luaVM, 2 );
                if ( pElement )
                    startAt = pElement;
                else
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "getElementsByType", "element", 2 );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
            }

            // Grab the argument
            const char* szType = lua_tostring ( luaVM, 1 );

            // Create a new table
            lua_newtable ( luaVM );

            // Add all the elements with a matching type to it
            startAt->FindAllChildrenByType ( szType, luaVM );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getElementsByType" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementInterior ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucInterior;
            if ( CStaticFunctionDefinitions::GetElementInterior ( pElement, ucInterior ) )
            {
                lua_pushnumber ( luaVM, ucInterior );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementInterior", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementInterior" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaElementDefs::isElementWithinMarker ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        CMarker* pMarker = lua_tomarker ( luaVM, 2 );
        if ( pElement )
        {
            if ( pMarker )
            {
                bool bWithin;
                if ( CStaticFunctionDefinitions::IsElementWithinMarker ( pElement, pMarker, bWithin ) )
                {
                    lua_pushboolean ( luaVM, bWithin );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinMarker", "marker", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinMarker", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementWithinMarker" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::isElementWithinColShape ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        CColShape* pColShape = lua_tocolshape ( luaVM, 2 );
        if ( pElement )
        {
            if ( pColShape )
            {
                bool bWithin;
                if ( CStaticFunctionDefinitions::IsElementWithinColShape ( pElement, pColShape, bWithin ) )
                {
                    lua_pushboolean ( luaVM, bWithin );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinColShape", "colshape", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementWithinColShape", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementWithinColShape" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementsWithinColShape ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            const char* szType = NULL;
            if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
            {
                szType = lua_tostring ( luaVM, 2 );
                if ( szType [ 0 ] == NULL )
                    szType = NULL;
            }

            CColShape* pColShape = lua_tocolshape ( luaVM, 1 );
            if ( pColShape )
            {
		        // Create a new table
		        lua_newtable ( luaVM );

                // Add all the elements within the shape to it
                unsigned int uiIndex = 0;
                list < CElement* > ::iterator iter = pColShape->CollidersBegin ();
                for ( ; iter != pColShape->CollidersEnd (); iter++ )
                {
                    if ( szType == NULL || strcmp ( (*iter)->GetTypeName ().c_str (), szType ) == 0 )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
			            lua_pushelement ( luaVM, *iter );
			            lua_settable ( luaVM, -3 );
                    }
                }
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getElementsWithinColShape", "colshape", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getElementsWithinColShape" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementDimension ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned short usDimension;
            if ( CStaticFunctionDefinitions::GetElementDimension ( pElement, usDimension ) )
            {
                lua_pushnumber ( luaVM, usDimension );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementDimension", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementDimension" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementZoneName ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        bool bCitiesOnly = false;
        if ( lua_type ( luaVM, 2 ) == LUA_TBOOLEAN )
            bCitiesOnly = ( lua_toboolean ( luaVM, 2 ) ) ? true:false;

        if ( pElement )
        {
            char szZoneName [ 128 ];
            if ( CStaticFunctionDefinitions::GetElementZoneName ( pElement, szZoneName, 128, bCitiesOnly ) )
            {
                lua_pushstring ( luaVM, szZoneName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementZoneName", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementZoneName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::isElementAttached ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::IsElementAttached ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementAttached", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementAttached" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementAttachedTo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );

        if ( pElement )
        {
            CElement* pElementAttachedTo = CStaticFunctionDefinitions::GetElementAttachedTo ( pElement );
            if ( pElementAttachedTo )
            {
                lua_pushelement ( luaVM, pElementAttachedTo );
                return 1;
            }
            else
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementAttachedTo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementAttached" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getAttachedElements ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
        {
            CElement* pElement = lua_toelement ( luaVM, 1 );
            if ( pElement )
            {
		        // Create a new table
		        lua_newtable ( luaVM );

                // Add All Attached Elements
                unsigned int uiIndex = 0;
                list < CElement* > ::const_iterator iter = pElement->AttachedElementsBegin ();
                for ( ; iter != pElement->AttachedElementsEnd () ; iter++ )
                {
                    if ( ( *iter )->GetAttachedToElement() == pElement )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
			            lua_pushelement ( luaVM, *iter );
			            lua_settable ( luaVM, -3 );
                    }
                }
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "getAttachedElements", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "getAttachedElements" );
    }
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementColShape ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CColShape* pColShape = CStaticFunctionDefinitions::GetElementColShape ( pElement );
            if ( pColShape )
            {
                lua_pushelement ( luaVM, pColShape );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementColShape", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementColShape" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementAlpha ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucAlpha;
            if ( CStaticFunctionDefinitions::GetElementAlpha ( pElement, ucAlpha ) )
            {
                lua_pushnumber ( luaVM, ucAlpha );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementAlpha", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementAlpha" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementHealth ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            float fHealth;
            if ( CStaticFunctionDefinitions::GetElementHealth ( pElement, fHealth ) )
            {
                lua_pushnumber ( luaVM, fHealth );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementHealth", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementHealth" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementModel ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned short usModel;
            if ( CStaticFunctionDefinitions::GetElementModel ( pElement, usModel ) )
            {
                lua_pushnumber ( luaVM, usModel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementModel", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementModel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::getElementSyncer ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CElement* pSyncer = NULL;
            if ( pSyncer = CStaticFunctionDefinitions::GetElementSyncer ( pElement ) )
            {
                lua_pushelement ( luaVM, pSyncer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getElementSyncer", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getElementSyncer" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::clearElementVisibleTo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::ClearElementVisibleTo ( pElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "clearElementVisibleTo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "clearElementVisibleTo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::isElementVisibleTo ( lua_State* luaVM )
{
    // Verify types
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab/verify the elements
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CElement* pVisibleTo = lua_toelement ( luaVM, 2 );
            if ( pVisibleTo )
            {
                // Is it a per-player entity? If not return true as everything else is visible for everything.
                if ( IS_PERPLAYER_ENTITY ( pElement ) )
                {
                    // Return true if we're visible for the given element
                    if ( static_cast < CPerPlayerEntity* > ( pElement )->IsVisibleToReferenced ( pVisibleTo ) )
                    {
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                }
                else
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "isElementVisibleTo", "element", 2 );
            }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "isElementVisibleTo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementVisibleTo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::isElementInWater ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            bool bInWater;
            if ( CStaticFunctionDefinitions::IsElementInWater ( pElement, bInWater ) )
            {
                lua_pushboolean ( luaVM, bInWater );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isElementInWater" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementID ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        const char* szID = lua_tostring ( luaVM, 2 );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetElementID ( pElement, szID ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementID", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementID" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementData ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TSTRING &&
             lua_type ( luaVM, 3 ) != LUA_TNONE )
        {
            CElement* pElement = lua_toelement ( luaVM, 1 );
            const char* szName = lua_tostring ( luaVM, 2 );
            bool bSynchronize = true;
            CLuaArgument Variable;
            Variable.Read ( luaVM, 3 );

            if ( lua_type ( luaVM, 4 ) == LUA_TBOOLEAN )
                bSynchronize = ( lua_toboolean ( luaVM, 4 ) ) ? true:false;

            if ( pElement )
            {
                if ( CStaticFunctionDefinitions::SetElementData ( pElement, const_cast < char* > ( szName ), Variable, pLuaMain, bSynchronize ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setElementData", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "setElementData" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::removeElementData ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
             lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            CElement* pElement = lua_toelement ( luaVM, 1 );
            const char* szName = lua_tostring ( luaVM, 2 );

            if ( pElement )
            {
                if ( CStaticFunctionDefinitions::RemoveElementData ( pElement, szName ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "removeElementData", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeElementData" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementParent ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CElement* pParent = lua_toelement ( luaVM, 2 );
            if ( pParent )
            {
                if ( CStaticFunctionDefinitions::SetElementParent ( pElement, pParent ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setElementParent", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementParent", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementParent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementPosition ( lua_State* luaVM )
{
    // Verify the first argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            // Verify the coordinate arguments (we don't need the third argument if it's a radar area)
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING || pElement->GetType () == CElement::RADAR_AREA ) )
            {
                // Grab the position
                CVector vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );

                bool bWarp = true;
                if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
                    bWarp = lua_toboolean ( luaVM, 5 ) ? true : false;

                // Set the position
                if ( CStaticFunctionDefinitions::SetElementPosition ( pElement, vecPosition, bWarp ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setElementPosition" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementPosition", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementPosition" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementVelocity ( lua_State* luaVM )
{
    // Verify the first argument
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            int iArgument2 = lua_type ( luaVM, 2 );
            int iArgument3 = lua_type ( luaVM, 3 );
            int iArgument4 = lua_type ( luaVM, 4 );
            if ( ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
                 ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                 ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING || pElement->GetType () == CElement::RADAR_AREA ) )
            {
                // Grab the velocity
                CVector vecVelocity = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                                static_cast < float > ( lua_tonumber ( luaVM, 4 ) ) );
                // Set the velocity
                if ( CStaticFunctionDefinitions::SetElementVelocity ( pElement, vecVelocity ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadType ( luaVM, "setElementVelocity" );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementVelocity", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementVelocity" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementVisibleTo ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA &&
		 lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            CElement* pReference = lua_toelement ( luaVM, 2 );
            if ( pReference )
            {
				if ( CStaticFunctionDefinitions::SetElementVisibleTo ( pElement, pReference, lua_toboolean ( luaVM, 3 ) ? true:false ) )
				{
					lua_pushboolean ( luaVM, true );
					return 1;
				}
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "setElementVisibleTo", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementVisibleTo", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementVisibleTo" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementInterior ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucInterior = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
        bool bSetPosition = false;
        CVector vecPosition;

        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
             ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
             ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
        {
            vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 3 ) ),
                                    static_cast < float > ( lua_tonumber ( luaVM, 4 ) ),
                                    static_cast < float > ( lua_tonumber ( luaVM, 5 ) ) );
            bSetPosition = true;
        }
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetElementInterior ( pElement, ucInterior, bSetPosition, vecPosition ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementInterior", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementInterior" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementDimension ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned short usDimension = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetElementDimension ( pElement, usDimension ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementDimension", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementDimension" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::attachElements ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        CElement* pAttachedToElement = lua_toelement ( luaVM, 2 );
        CVector vecPosition, vecRotation;

        int iArgument3 = lua_type ( luaVM, 3 );
        if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
        {
            vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );

            int iArgument4 = lua_type ( luaVM, 4 );
            if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
            {
                vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );

                int iArgument5 = lua_type ( luaVM, 5 );
                if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                {
                    vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );

                    int iArgument6 = lua_type ( luaVM, 6 );
                    if ( iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNUMBER )
                    {
                        vecRotation.fX = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );

                        int iArgument7 = lua_type ( luaVM, 7 );
                        if ( iArgument7 == LUA_TSTRING || iArgument7 == LUA_TNUMBER )
                        {
                            vecRotation.fY = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );

                            int iArgument8 = lua_type ( luaVM, 8 );
                            if ( iArgument8 == LUA_TSTRING || iArgument8 == LUA_TNUMBER )
                            {
                                vecRotation.fZ = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
                            }
                        }
                    }
                }
            }
        }

        if ( pElement )
        {
            if ( pAttachedToElement )
            {
                if ( CStaticFunctionDefinitions::AttachElements ( pElement, pAttachedToElement, vecPosition, vecRotation ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "attachElements", "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "attachElements", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "attachElements" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::detachElements ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        CElement* pAttachedToElement = NULL;

        if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        {
            pAttachedToElement = lua_toelement ( luaVM, 2 );
            if ( !pAttachedToElement )
                m_pScriptDebugging->LogBadPointer ( luaVM, "detachElements", "element", 2 );
        }

        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::DetachElements ( pElement, pAttachedToElement ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "detachElements", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "detachElements" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementAlpha ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned char ucAlpha = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetElementAlpha ( pElement, ucAlpha ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementAlpha", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementAlpha" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementHealth ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            float fHealth = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetElementHealth ( pElement, fHealth ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementHealth", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementHealth" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaElementDefs::setElementModel ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        if ( pElement )
        {
            unsigned short usModel = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );
            if ( CStaticFunctionDefinitions::SetElementModel ( pElement, usModel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setElementModel", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setElementModel" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Element.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

int CLuaFunctionDefs::GetRootElement ( lua_State* luaVM )
{
    CClientEntity* pRoot = CStaticFunctionDefinitions::GetRootElement ();

    // Return the root element
    lua_pushelement ( luaVM, pRoot );
    return 1;
}


int CLuaFunctionDefs::IsElement ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementID ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Get its ID and return it
            const char* szName = pEntity->GetName ();
            if ( szName )
            {
                lua_pushstring ( luaVM, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementByID ( lua_State* luaVM )
{
    SString strID = "";
    unsigned int uiIndex = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strID );
    argStream.ReadNumber ( uiIndex, 0 );

    if ( !argStream.HasErrors ( ) )
    {
        // Try to find the element with that ID. Return it
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByID ( strID.c_str ( ), uiIndex );
        if ( pEntity )
        {
            lua_pushelement ( luaVM, pEntity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementByIndex ( lua_State* luaVM )
{
    SString strType = "";
    unsigned int uiIndex = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strType );
    argStream.ReadNumber ( uiIndex, 0 );

    if ( !argStream.HasErrors ( ) )
    {
        // Try to find it. Return it if we did.
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByIndex ( strType.c_str ( ), uiIndex );
        if ( pEntity )
        {
            lua_pushelement ( luaVM, pEntity );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementData ( lua_State* luaVM )
{
//  var getElementData ( element theElement, string key [, inherit = true] )
    CClientEntity* pEntity; SString strKey; bool bInherit;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strKey );
    argStream.ReadBool ( bInherit, true );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( strKey.length () > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom ( luaVM, SString ( "Truncated argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *SString ( "string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH ) ) );
                strKey = strKey.Left ( MAX_CUSTOMDATA_NAME_LENGTH );
            }

            CLuaArgument* pVariable = pEntity->GetCustomData ( strKey, bInherit );
            if ( pVariable )
            {
                pVariable->Push ( luaVM );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetElementMatrix ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    bool bBadSyntax;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bBadSyntax, true );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        // Grab the position
        CMatrix matrix;
        if ( CStaticFunctionDefinitions::GetElementMatrix ( *pEntity, matrix ) )
        {
            // Apparently some scripts like the dirty syntax... should be 0.0f but was 1.0f post 1.3.2
            float fData = bBadSyntax == true ? 1.0f : 0.0f;

            // Return it
            lua_createtable ( luaVM, 4, 0 );

            // First row
            lua_createtable ( luaVM, 4, 0 );
            lua_pushnumber ( luaVM, matrix.vRight.fX );
            lua_rawseti ( luaVM, -2, 1 );
            lua_pushnumber ( luaVM, matrix.vRight.fY );
            lua_rawseti ( luaVM, -2, 2 );
            lua_pushnumber ( luaVM, matrix.vRight.fZ );
            lua_rawseti ( luaVM, -2, 3 );
            lua_pushnumber ( luaVM, fData );
            lua_rawseti ( luaVM, -2, 4 );
            lua_rawseti ( luaVM, -2, 1 );

            // Second row
            lua_createtable ( luaVM, 4, 0 );
            lua_pushnumber ( luaVM, matrix.vFront.fX );
            lua_rawseti ( luaVM, -2, 1 );
            lua_pushnumber ( luaVM, matrix.vFront.fY );
            lua_rawseti ( luaVM, -2, 2 );
            lua_pushnumber ( luaVM, matrix.vFront.fZ );
            lua_rawseti ( luaVM, -2, 3 );
            lua_pushnumber ( luaVM, fData );
            lua_rawseti ( luaVM, -2, 4 );
            lua_rawseti ( luaVM, -2, 2 );

            // Third row
            lua_createtable ( luaVM, 4, 0 );
            lua_pushnumber ( luaVM, matrix.vUp.fX );
            lua_rawseti ( luaVM, -2, 1 );
            lua_pushnumber ( luaVM, matrix.vUp.fY );
            lua_rawseti ( luaVM, -2, 2 );
            lua_pushnumber ( luaVM, matrix.vUp.fZ );
            lua_rawseti ( luaVM, -2, 3 );
            lua_pushnumber ( luaVM, fData );
            lua_rawseti ( luaVM, -2, 4 );
            lua_rawseti ( luaVM, -2, 3 );

            // Fourth row
            lua_createtable ( luaVM, 4, 0 );
            lua_pushnumber ( luaVM, matrix.vPos.fX );
            lua_rawseti ( luaVM, -2, 1 );
            lua_pushnumber ( luaVM, matrix.vPos.fY );
            lua_rawseti ( luaVM, -2, 2 );
            lua_pushnumber ( luaVM, matrix.vPos.fZ );
            lua_rawseti ( luaVM, -2, 3 );
            lua_pushnumber ( luaVM, 1.0f );
            lua_rawseti ( luaVM, -2, 4 );
            lua_rawseti ( luaVM, -2, 4 );

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementPosition ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the position
            CVector vecPosition;
            if ( CStaticFunctionDefinitions::GetElementPosition ( *pEntity, vecPosition ) )
            {
                // Return it
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                lua_pushnumber ( luaVM, vecPosition.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementRotation ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        SString strRotationOrder = "default";
        if ( argStream.NextIsString ( ) ) 
        {
            argStream.ReadString ( strRotationOrder );
        }

        if ( pEntity )
        {
            // Grab the rotation
            CVector vecRotation;
            if ( CStaticFunctionDefinitions::GetElementRotation ( *pEntity, vecRotation, strRotationOrder.c_str ( ) ) )
            {
                // Return it
                lua_pushnumber ( luaVM, vecRotation.fX );
                lua_pushnumber ( luaVM, vecRotation.fY );
                lua_pushnumber ( luaVM, vecRotation.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementVelocity ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        // Grab the element, verify it
        if ( pEntity )
        {
            // Grab the velocity
            CVector vecVelocity;
            if ( CStaticFunctionDefinitions::GetElementVelocity ( *pEntity, vecVelocity ) )
            {
                // Return it
                lua_pushnumber ( luaVM, vecVelocity.fX );
                lua_pushnumber ( luaVM, vecVelocity.fY );
                lua_pushnumber ( luaVM, vecVelocity.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementType ( lua_State* luaVM )
{
    // Check the arg type
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Get its typename
            const char* szTypeName = pEntity->GetTypeName ();
            if ( szTypeName )
            {
                lua_pushstring ( luaVM, szTypeName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementChildren ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        // Find our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( pEntity )
            {
                if ( !argStream.NextIsString ( ) )
                {
                    // Create a new table
                    lua_newtable ( luaVM );

                    // Add all the elements with a matching type to it
                    pEntity->GetChildren ( luaVM );
                    return 1;
                }
                else if ( argStream.NextIsString ( ) )
                {
                    SString strType = "";
                    argStream.ReadString ( strType );
                    // Create a new table
                    lua_newtable ( luaVM );
    
                    // Add all the elements with a matching type to it
                    pEntity->GetChildrenByType ( strType.c_str ( ), luaVM );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadType ( luaVM );
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementChild ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    unsigned int uiIndex = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( uiIndex );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the child
            CClientEntity* pChild = CStaticFunctionDefinitions::GetElementChild ( *pEntity, uiIndex );
            if ( pChild )
            {
                lua_pushelement ( luaVM, pChild );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementChildrenCount ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the count
            unsigned int uiCount = pEntity->CountChildren ();
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( uiCount ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementParent ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the parent and return it
            CClientEntity* pParent = pEntity->GetParent ();
            if ( pParent )
            {
                lua_pushelement ( luaVM, pParent );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementsByType ( lua_State* luaVM )
{
    // Verify the argument
    SString strType = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strType );

    if ( !argStream.HasErrors ( ) )
    {
        // Find our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // see if a root argument has been specified
            CClientEntity* pStartAt = m_pRootEntity;
            CClientEntity* pEntity = NULL;
            bool bStreamedIn = false;

            if ( argStream.NextIsUserData ( ) )
            {
                // if its valid, use that, otherwise, produce an error
                argStream.ReadUserData ( pEntity );
                if ( pEntity )
                    pStartAt = pEntity;
                else
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "element", 2 );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
                if ( argStream.NextIsBool ( ) )
                {
                    argStream.ReadBool ( bStreamedIn );
                }
            }

            // Create a new table
            lua_newtable ( luaVM );

            // Add all the elements with a matching type to it
            pStartAt->FindAllChildrenByType ( strType.c_str ( ), luaVM, bStreamedIn );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementInterior ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the interior and return it
            unsigned char ucInterior;
            if ( CStaticFunctionDefinitions::GetElementInterior ( *pEntity, ucInterior ) )
            {
                lua_pushnumber ( luaVM, ucInterior );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementWithinColShape ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CClientColShape* pColShape = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pColShape );

    if ( !argStream.HasErrors ( ) )
    {
        // Valid element?
        if ( pEntity )
        {
            // Valid colshape?
            if ( pColShape )
            {
                // Check if it's within
                bool bWithin = pEntity->CollisionExists ( pColShape );
                lua_pushboolean ( luaVM, bWithin );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "colshape", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsElementWithinMarker( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CClientMarker* pMarker = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        // Valid element?
        if ( pEntity )
        {
            // Valid colshape?
            if ( pMarker )
            {
                // Check if it's within
                bool bWithin = pEntity->CollisionExists ( pMarker->GetColShape() );
                lua_pushboolean ( luaVM, bWithin );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "marker", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetElementsWithinColShape ( lua_State* luaVM )
{
    // Verify the arguments
    CClientColShape* pColShape = NULL;
    SString strType = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pColShape );
    argStream.ReadString ( strType, "" );

    if ( !argStream.HasErrors ( ) )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab optional type arg
            const char* szType = strType == "" ? NULL : strType.c_str ( );

            if ( pColShape )
            {
                // Create a new table
                lua_newtable ( luaVM );

                // Add all the elements within the shape to it
                unsigned int uiIndex = 0;
                CFastList < CClientEntity* > ::iterator iter = pColShape->CollidersBegin ();
                for ( ; iter != pColShape->CollidersEnd (); iter++ )
                {
                    if ( szType == NULL || strcmp ( (*iter)->GetTypeName (), szType ) == 0 )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
                        lua_pushelement ( luaVM, *iter );
                        lua_settable ( luaVM, -3 );
                    }
                }

                // We're returning 1 table
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "colshape", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementDimension ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the dimension
            unsigned short usDimension = pEntity->GetDimension ();
            lua_pushnumber ( luaVM, usDimension );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementBoundingBox ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the bounding box and return it
            CVector vecMin, vecMax;
            if ( CStaticFunctionDefinitions::GetElementBoundingBox ( *pEntity, vecMin, vecMax ) )
            {
                lua_pushnumber ( luaVM, vecMin.fX );
                lua_pushnumber ( luaVM, vecMin.fY );
                lua_pushnumber ( luaVM, vecMin.fZ );
                lua_pushnumber ( luaVM, vecMax.fX );
                lua_pushnumber ( luaVM, vecMax.fY );
                lua_pushnumber ( luaVM, vecMax.fZ );
                return 6;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementRadius ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab its radius and return it
            float fRadius = 0.0f;
            if ( CStaticFunctionDefinitions::GetElementRadius ( *pEntity, fRadius ) )
            {
                lua_pushnumber ( luaVM, fRadius );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementAttached ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            CClientEntity* pEntityAttachedTo = pEntity->GetAttachedTo();
            if ( pEntityAttachedTo )
            {
                if ( pEntityAttachedTo->IsEntityAttached ( pEntity ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetElementAttachedTo ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        CClientEntity* pEntityAttachedTo = NULL;

        // Valid?
        if ( pEntity )
        {
            // Grab the enity attached to it
            CClientEntity* pEntityAttachedTo = CStaticFunctionDefinitions::GetElementAttachedTo ( *pEntity );
            if ( pEntityAttachedTo )
            {
                lua_pushelement ( luaVM, pEntityAttachedTo );
                return 1;
            }
            else
            {
                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetAttachedElements ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( pEntity )
            {
                // Create a new table
                lua_newtable ( luaVM );

                // Add All Attached Elements
                unsigned int uiIndex = 0;
                list < CClientEntity* > ::const_iterator iter = pEntity->AttachedEntitiesBegin ();
                for ( ; iter != pEntity->AttachedEntitiesEnd () ; iter++ )
                {
                    CClientEntity * pAttached = *iter;
                    if ( pAttached->GetAttachedTo () == pEntity )
                    {
                        lua_pushnumber ( luaVM, ++uiIndex );
                        lua_pushelement ( luaVM, *iter );
                        lua_settable ( luaVM, -3 );
                    }
                }
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementDistanceFromCentreOfMassToBaseOfModel ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the distance and return it
            float fDistance;
            if ( CStaticFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel ( *pEntity, fDistance ) )
            {
                lua_pushnumber ( luaVM, fDistance );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementLocal ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Return whether it's local or not
            bool bLocal = pEntity->IsLocalEntity ();;
            lua_pushboolean ( luaVM, bLocal );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::GetElementAttachedOffsets ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        CVector vecPosition, vecRotation;

        // Valid element?
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::GetElementAttachedOffsets ( *pEntity, vecPosition, vecRotation ) )
            {
                lua_pushnumber( luaVM, vecPosition.fX );
                lua_pushnumber( luaVM, vecPosition.fY );
                lua_pushnumber( luaVM, vecPosition.fZ );
                lua_pushnumber( luaVM, vecRotation.fX );
                lua_pushnumber( luaVM, vecRotation.fY );
                lua_pushnumber( luaVM, vecRotation.fZ );
                return 6;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementAlpha ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab its alpha level and return it
            unsigned char ucAlpha;
            if ( CStaticFunctionDefinitions::GetElementAlpha ( *pEntity, ucAlpha ) )
            {
                lua_pushnumber ( luaVM, ucAlpha );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementHealth ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the health and return it
            float fHealth;
            if ( CStaticFunctionDefinitions::GetElementHealth ( *pEntity, fHealth ) )
            {
                lua_pushnumber ( luaVM, fHealth );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementModel ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            unsigned short usModel;
            if ( CStaticFunctionDefinitions::GetElementModel ( *pEntity, usModel ) )
            {
                lua_pushnumber ( luaVM, usModel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetElementColShape ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            CClientEntity* pColShape = static_cast < CClientEntity* > ( CStaticFunctionDefinitions::GetElementColShape ( pEntity ) );
            if ( pColShape )
            {
                lua_pushelement ( luaVM, pColShape );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementInWater ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            bool bInWater = false;
            if ( CStaticFunctionDefinitions::IsElementInWater ( *pEntity, bInWater ) )
            {
                lua_pushboolean ( luaVM, bInWater );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementSyncer ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            bool bIsSyncer = false;
            if ( CStaticFunctionDefinitions::IsElementSyncer ( *pEntity, bIsSyncer ) )
            {
                lua_pushboolean ( luaVM, bIsSyncer );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::IsElementCollidableWith ( lua_State* luaVM )
{
    // Verify the arguments
    CClientEntity* pEntity = NULL;
    CClientEntity* pWithEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pWithEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity && pWithEntity )
        {
            bool bCanCollide;
            if ( CStaticFunctionDefinitions::IsElementCollidableWith ( *pEntity, *pWithEntity, bCanCollide ) )
            {
                lua_pushboolean ( luaVM, bCanCollide );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementDoubleSided ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            lua_pushboolean ( luaVM, pEntity->IsDoubleSided () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetElementCollisionsEnabled ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::GetElementCollisionsEnabled ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementFrozen ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            bool bFrozen;
            if ( CStaticFunctionDefinitions::IsElementFrozen ( *pEntity, bFrozen ) )
            {
                lua_pushboolean ( luaVM, bFrozen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementStreamedIn ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Is this a streaming compatible class?
            if ( pEntity->IsStreamingCompatibleClass () )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( pEntity );

                // Return whether or not this class is streamed in
                lua_pushboolean ( luaVM, pStreamElement->IsStreamedIn () );
                return 1;
            }
            else if ( pEntity->GetType() == CCLIENTSOUND )
            {
                CClientSound* pSound = static_cast < CClientSound* > ( pEntity );
                lua_pushboolean ( luaVM, pSound->IsSoundStopped() ? false : true );
                return 1;
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "%s; element is not streaming compatible\n", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsElementStreamable ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Is this a streaming compatible class?
            if ( pEntity->IsStreamingCompatibleClass () )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( pEntity );

                bool bStreamable = ( pStreamElement->GetTotalStreamReferences () == 0 );
                // Return whether or not this element is set to never be streamed out
                lua_pushboolean ( luaVM, bStreamable );
                return 1;
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "%s; element is not streaming compatible\n", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

            // Return false, we're not streamable
            lua_pushboolean ( luaVM, false );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // We failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::IsElementOnScreen ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Return whether we're on the screen or not
            bool bOnScreen;
            if ( CStaticFunctionDefinitions::IsElementOnScreen ( *pEntity, bOnScreen ) )
            {
                lua_pushboolean ( luaVM, bOnScreen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::CreateElement ( lua_State* luaVM )
{
    // Verify the argument
    SString strTypeName = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strTypeName );

    if ( !argStream.HasErrors ( ) )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab its resource
            CResource * pResource = pLuaMain->GetResource();
            if ( pResource )
            {
                // Grab the optional name argument
                SString strID = "";
                if ( argStream.NextIsString ( ) )
                {
                    argStream.ReadString ( strID );
                }
                else
                {
                    m_pScriptDebugging->LogBadType ( luaVM );
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }

                // Try to create
                CClientDummy* pDummy = CStaticFunctionDefinitions::CreateElement ( *pResource, strTypeName.c_str ( ), strID.c_str ( ) );
                if ( pDummy )
                {
                    // Add it to the element group
                    // TODO: Get rid of element groups
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pDummy );
                    }

                    // Return it
                    lua_pushelement ( luaVM, pDummy );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "createElement; unable to create more elements\n" );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::DestroyElement ( lua_State* luaVM )
{
    // Verify the argument
    CClientEntity* pEntity = NULL;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Destroy it
            if ( CStaticFunctionDefinitions::DestroyElement ( *pEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementID ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    SString strKey = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        if ( pEntity )
        {
            // It returns false if we tried to change ID of server-created element
            if ( CStaticFunctionDefinitions::SetElementID ( *pEntity, strKey.c_str ( ) ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementData ( lua_State* luaVM )
{
//  bool setElementData ( element theElement, string key, var value, [bool synchronize = true] )
    CClientEntity* pEntity; SString strKey; CLuaArgument value; bool bSynchronize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strKey );
    argStream.ReadLuaArgument ( value );
    argStream.ReadBool ( bSynchronize, true );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( strKey.length () > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom ( luaVM, SString ( "Truncated argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *SString ( "string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH ) ) );
                strKey = strKey.Left ( MAX_CUSTOMDATA_NAME_LENGTH );
            }

            if ( CStaticFunctionDefinitions::SetElementData ( *pEntity, strKey, value, *pLuaMain, bSynchronize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveElementData ( lua_State* luaVM )
{
//  bool removeElementData ( element theElement, string key )
    CClientEntity* pEntity; SString strKey;;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( strKey.length () > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom ( luaVM, SString ( "Truncated argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *SString ( "string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH ) ) );
                strKey = strKey.Left ( MAX_CUSTOMDATA_NAME_LENGTH );
            }

            if ( CStaticFunctionDefinitions::RemoveElementData ( *pEntity, strKey ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementMatrix ( lua_State* luaVM )
{
//  setElementMatrix ( element theElement, table matrix )
    CClientEntity* pEntity; CMatrix matrix;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !ReadMatrix ( luaVM, argStream.m_iIndex, matrix ) )
        argStream.SetCustomError ( "Matrix is not 4 x 4" );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetElementMatrix ( *pEntity, matrix ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementPosition ( lua_State* luaVM )
{
    CClientEntity* pEntity; 
    CVector vecPosition;
    bool bWarp = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadBool ( bWarp, true );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        // Valid?
        if ( pEntity )
        {
            // Try to set the position
            if ( CStaticFunctionDefinitions::SetElementPosition ( *pEntity, vecPosition, bWarp ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementRotation ( lua_State* luaVM )
{
//  bool setElementRotation ( element theElement, float rotX, float rotY, float rotZ [, string rotOrder = "default", bool fixPedRotation = false ] )
    CClientEntity* pEntity; CVector vecRotation; SString strRotationOrder; bool bNewWay;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( vecRotation.fX );
    argStream.ReadNumber ( vecRotation.fY );
    argStream.ReadNumber ( vecRotation.fZ );
    argStream.ReadString ( strRotationOrder, "default" );
    argStream.ReadBool ( bNewWay, false );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetElementRotation ( *pEntity, vecRotation, strRotationOrder, bNewWay ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementVelocity ( lua_State* luaVM )
{
    CClientEntity* pEntity; 
    CVector vecVelocity;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( vecVelocity.fX );
    argStream.ReadNumber ( vecVelocity.fY );
    argStream.ReadNumber ( vecVelocity.fZ );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Set the velocity
            if ( CStaticFunctionDefinitions::SetElementVelocity ( *pEntity, vecVelocity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementParent ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CClientEntity* pParent = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pParent );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( pParent )
            {
                CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

                // Change the parent
                if ( CStaticFunctionDefinitions::SetElementParent ( *pEntity, *pParent, pLuaMain ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementInterior ( lua_State* luaVM )
{
    CClientEntity* pEntity; 
    unsigned int uiInterior = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( uiInterior );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( uiInterior <= 0xFF )
        {
            // Grab position if supplied
            bool bSetPosition = false;
            CVector vecPosition;

            if ( argStream.NextIsNumber ( ) && 
                 argStream.NextIsNumber ( 1 ) && 
                 argStream.NextIsNumber ( 2 ) )
            {
                argStream.ReadNumber ( vecPosition.fX );
                argStream.ReadNumber ( vecPosition.fY );
                argStream.ReadNumber ( vecPosition.fZ );
                if ( !argStream.HasErrors ( ) )
                {
                    bSetPosition = true;
                }
                else
                {
                    m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
                    // Failed
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
            }

            // Valid element?
            if ( pEntity )
            {
                // Set the interior
                if ( CStaticFunctionDefinitions::SetElementInterior ( *pEntity, uiInterior, bSetPosition, vecPosition ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementDimension ( lua_State* luaVM )
{
    CClientEntity* pEntity; 
    unsigned short usDimension = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( usDimension );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        // Valid?
        if ( pEntity )
        {
            // Change the dimension
            if ( CStaticFunctionDefinitions::SetElementDimension ( *pEntity, usDimension ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::AttachElements ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CClientEntity* pAttachedToEntity = NULL;
    CVector vecPosition, vecRotation;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pAttachedToEntity );
    argStream.ReadNumber ( vecPosition.fX, 0.0f );
    argStream.ReadNumber ( vecPosition.fY, 0.0f );
    argStream.ReadNumber ( vecPosition.fZ, 0.0f );
    argStream.ReadNumber ( vecRotation.fX, 0.0f );
    argStream.ReadNumber ( vecRotation.fY, 0.0f );
    argStream.ReadNumber ( vecRotation.fZ, 0.0f );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        // Valid element?
        if ( pEntity )
        {
            // Valid host element?
            if ( pAttachedToEntity )
            {
                // Try to attach them
                if ( CStaticFunctionDefinitions::AttachElements ( *pEntity, *pAttachedToEntity, vecPosition, vecRotation ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "element", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::DetachElements ( lua_State* luaVM )
{
    CClientEntity* pEntity; 
    CClientEntity* pAttachedToEntity = NULL;
    unsigned short usDimension = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pAttachedToEntity, NULL );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        // Valid?
        if ( pEntity )
        {
            // Try to detach
            if ( CStaticFunctionDefinitions::DetachElements ( *pEntity, pAttachedToEntity ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementAttachedOffsets ( lua_State* luaVM )
{
    CClientEntity* pEntity; 
    CClientEntity* pAttachedToEntity = NULL;
    unsigned short usDimension = 0;
    CVector vecPosition, vecRotation;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( vecPosition.fX, 0.0f );
    argStream.ReadNumber ( vecPosition.fY, 0.0f );
    argStream.ReadNumber ( vecPosition.fZ, 0.0f );
    argStream.ReadNumber ( vecRotation.fX, 0.0f );
    argStream.ReadNumber ( vecRotation.fY, 0.0f );
    argStream.ReadNumber ( vecRotation.fZ, 0.0f );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        // Valid element?
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementAttachedOffsets ( *pEntity, vecPosition, vecRotation ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementCollisionsEnabled ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    bool bEnabled = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bEnabled );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementCollisionsEnabled ( *pEntity, bEnabled ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetElementCollidableWith ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    CClientEntity* pWithEntity = NULL;
    bool bCanCollide = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pWithEntity );
    argStream.ReadBool ( bCanCollide );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity && pWithEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementCollidableWith ( *pEntity, *pWithEntity, bCanCollide ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            } 
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementDoubleSided ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    bool bDoubleSided = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bDoubleSided );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Grab the chosen value and set it
            pEntity->SetDoubleSided ( bDoubleSided );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementFrozen ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    bool bFrozen = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bFrozen );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementFrozen ( *pEntity, bFrozen ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementAlpha ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    unsigned char ucAlpha = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucAlpha );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementAlpha ( *pEntity, ucAlpha ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failure
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementHealth ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    float fHealth = 0.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fHealth );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementHealth ( *pEntity, fHealth ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementModel ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    unsigned short usModel = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( usModel );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetElementModel ( *pEntity, usModel ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetElementStreamable ( lua_State* luaVM )
{
    CClientEntity* pEntity;
    bool bStreamable = true;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bStreamable );

    // Verify the arguments
    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            // Is this a streaming compatible class?
            if ( pEntity->IsStreamingCompatibleClass () )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( pEntity );

                // TODO: maybe use a better VM-based reference system (rather than a boolean one)
                if ( bStreamable && ( pStreamElement->GetStreamReferences ( true ) > 0 ) )
                {
                    pStreamElement->RemoveStreamReference ( true );
                }
                else if ( !bStreamable && ( pStreamElement->GetStreamReferences ( true ) == 0 ) )
                {
                    pStreamElement->AddStreamReference ( true );
                }
                else
                {
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }

                // Return success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // We failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetLowLodElement ( lua_State* luaVM )
{
//  element getLowLODElement ( element theElement )
    CClientEntity* pEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        CClientEntity* pLowLodEntity;
        if ( CStaticFunctionDefinitions::GetLowLodElement ( *pEntity, pLowLodEntity ) )
        {
            lua_pushelement ( luaVM, pLowLodEntity );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetLowLodElement ( lua_State* luaVM )
{
//  bool setLowLODElement ( element theElement )
    CClientEntity* pEntity; CClientEntity* pLowLodEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadUserData ( pLowLodEntity, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetLowLodElement ( *pEntity, pLowLodEntity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsElementLowLod ( lua_State* luaVM )
{
//  bool isElementLowLOD ( element theElement )
    CClientEntity* pEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        bool bLowLod;
        if ( CStaticFunctionDefinitions::IsElementLowLod ( *pEntity, bLowLod ) )
        {
            lua_pushboolean ( luaVM, bLowLod );
            return 1;
        }        
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

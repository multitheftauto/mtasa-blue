/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLuaVectorDefs.cpp
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaVector4Defs::Create ( lua_State* luaVM )
{
    CVector4D vector;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsTable () )
    {
        lua_pushvalue ( luaVM, 1 );

        lua_pushstring ( luaVM, "x" );
        lua_rawget ( luaVM, -2 );
        if ( lua_isnumber ( luaVM, -1 ) )
        {
            vector.fX = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }
        else
        {
            lua_pop ( luaVM, 1 );
            lua_rawgeti ( luaVM, -1, 1 );
            if ( lua_isnumber ( luaVM, -1 ) )
                vector.fX = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }

        lua_pushstring ( luaVM, "y" );
        lua_rawget ( luaVM, -2 );
        if ( lua_isnumber ( luaVM, -1 ) )
        {
            vector.fY = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }
        else
        {
            lua_pop ( luaVM, 1 );
            lua_rawgeti ( luaVM, -1, 2 );
            if ( lua_isnumber ( luaVM, -1 ) )
                vector.fY = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }

        lua_pushstring ( luaVM, "z" );
        lua_rawget ( luaVM, -2 );
        if ( lua_isnumber ( luaVM, -1 ) )
        {
            vector.fZ = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }
        else
        {
            lua_pop ( luaVM, 1 );
            lua_rawgeti ( luaVM, -1, 3 );
            if ( lua_isnumber ( luaVM, -1 ) )
                vector.fZ = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }

        lua_pushstring ( luaVM, "w" );
        lua_rawget ( luaVM, -2 );
        if ( lua_isnumber ( luaVM, -1 ) )
        {
            vector.fW = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }
        else
        {
            lua_pop ( luaVM, 1 );
            lua_rawgeti ( luaVM, -1, 3 );
            if ( lua_isnumber ( luaVM, -1 ) )
                vector.fW = ( float ) lua_tonumber ( luaVM, -1 );
            lua_pop ( luaVM, 1 );
        }
    }
    else if ( argStream.NextIsNumber () )
    {
        argStream.ReadNumber ( vector.fX );
        if ( argStream.NextIsNumber () )
        {
            argStream.ReadNumber ( vector.fY );
            if ( argStream.NextIsNumber () )
                argStream.ReadNumber ( vector.fZ );
            if ( argStream.NextIsNumber () )
                argStream.ReadNumber ( vector.fW );
        }
    }
    lua_pushvector ( luaVM, vector );
    return 1;
}

int CLuaVector4Defs::Destroy ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        delete pVector;

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::GetLength ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pVector->Length () );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1; 
}

int CLuaVector4Defs::GetLengthSquared ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pVector->LengthSquared () );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::GetNormalized ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        CVector4D vector ( *pVector );
        vector.Normalize ();

        lua_pushvector ( luaVM, vector );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;   
}

int CLuaVector4Defs::Normalize ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        pVector->Normalize ();
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;      
}

int CLuaVector4Defs::Dot ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );
    argStream.ReadUserData ( pVector2 );

    if ( !argStream.HasErrors () )
    {
        CVector4D vector ( *pVector1 );
        float fProduct = vector.DotProduct ( *pVector2 );
        
        lua_pushnumber ( luaVM, fProduct );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::ToString ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        SString string = SString ( "vector4: { x = %.3f, y = %.3f, z = %.3f, w = %.3f }", pVector->fX, pVector->fY, pVector->fZ, pVector->fW );
        lua_pushstring ( luaVM, string.c_str () );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::SetX ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;
    float fValue = 0.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );
    argStream.ReadNumber ( fValue );

    if ( !argStream.HasErrors () )
    {
        pVector->fX = fValue;

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::SetY ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;
    float fValue = 0.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );
    argStream.ReadNumber ( fValue );

    if ( !argStream.HasErrors () )
    {
        pVector->fY = fValue;

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::SetZ ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;
    float fValue = 0.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );
    argStream.ReadNumber ( fValue );

    if ( !argStream.HasErrors () )
    {
        pVector->fZ = fValue;

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::SetW ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;
    float fValue = 0.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );
    argStream.ReadNumber ( fValue );

    if ( !argStream.HasErrors () )
    {
        pVector->fW = fValue;

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::GetX ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pVector->fX );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;    
}

int CLuaVector4Defs::GetY ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pVector->fY );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::GetZ ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pVector->fZ );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::GetW ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pVector->fW );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Add ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );
    argStream.ReadUserData ( pVector2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, *pVector1 + *pVector2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Sub ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );
    argStream.ReadUserData ( pVector2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, *pVector1 - *pVector2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Mul ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );

    if ( argStream.NextIsNumber () )
    {
        float fValue = 0.0f;
        argStream.ReadNumber ( fValue );

        lua_pushvector ( luaVM, *pVector1 * fValue );
        return 1;
    }
    else
    {
        argStream.ReadUserData ( pVector2 );

        if ( !argStream.HasErrors () )
        {
            lua_pushvector ( luaVM, *pVector1 * *pVector2 );
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Div ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );

    if ( argStream.NextIsNumber () )
    {
        float fValue = 0.0f;
        argStream.ReadNumber ( fValue );

        lua_pushvector ( luaVM, *pVector1 / fValue );
        return 1;
    }
    else
    {
        argStream.ReadUserData ( pVector2 );

        if ( !argStream.HasErrors () )
        {
            lua_pushvector ( luaVM, *pVector1 / *pVector2 );
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Pow ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );

    if ( argStream.NextIsNumber () )
    {
        float fValue = 0.0f;
        argStream.ReadNumber ( fValue );

        CVector4D vector ( *pVector1 );
        vector.fX = pow ( vector.fX, fValue );
        vector.fY = pow ( vector.fY, fValue );
        vector.fZ = pow ( vector.fZ, fValue );
        vector.fW = pow ( vector.fW, fValue );

        lua_pushvector ( luaVM, vector );
        return 1;
    }
    else
    {
        argStream.ReadUserData ( pVector2 );

        if ( !argStream.HasErrors () )
        {
            CVector4D vector ( *pVector1 );
            vector.fX = pow ( vector.fX, pVector2->fX );
            vector.fY = pow ( vector.fY, pVector2->fY );
            vector.fZ = pow ( vector.fZ, pVector2->fZ );
            vector.fW = pow ( vector.fW, pVector2->fW );

            lua_pushvector ( luaVM, vector );
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Unm ( lua_State* luaVM )
{
    CLuaVector4D* pVector = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, CVector4D () - *pVector );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaVector4Defs::Eq ( lua_State* luaVM )
{
    CLuaVector4D* pVector1 = NULL;
    CLuaVector4D* pVector2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pVector1 );
    argStream.ReadUserData ( pVector2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushboolean ( luaVM, pVector1 == pVector2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}
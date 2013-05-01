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

int CLuaMatrixDefs::Create ( lua_State* luaVM )
{
    CMatrix matrix;

    lua_pushmatrix ( luaVM, matrix );
    return 1;
}

int CLuaMatrixDefs::Destroy ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        delete pMatrix;
    }
    return 0;
}

int CLuaMatrixDefs::ToString ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        SString string = "matrix";
        lua_pushstring ( luaVM, string.c_str () );
        return 1;
    }
    return 0;
}

int CLuaMatrixDefs::GetPosition ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pMatrix->vPos );
        return 1;
    }
    return 0;        
}

int CLuaMatrixDefs::GetRotation ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        CVector vecRotation;
        g_pMultiplayer->ConvertMatrixToEulerAngles ( *pMatrix, vecRotation.fX, vecRotation.fY, vecRotation.fZ );
        lua_pushvector ( luaVM, vecRotation );
        return 1;
    }
    return 0;        
}

int CLuaMatrixDefs::GetFront ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pMatrix->vFront );
        return 1;
    }
    return 0;        
}

int CLuaMatrixDefs::GetRight ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pMatrix->vRight );
        return 1;
    }
    return 0;        
}

int CLuaMatrixDefs::GetUp ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pMatrix->vUp );
        return 1;
    }
    return 0;        
}

int CLuaMatrixDefs::SetPosition ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors () )
        pMatrix->vPos = vecPosition;

    return !argStream.HasErrors ();
}

int CLuaMatrixDefs::SetFront ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecFront;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecFront );

    if ( !argStream.HasErrors () )
        pMatrix->vFront = vecFront;

    return !argStream.HasErrors ();
}

int CLuaMatrixDefs::SetRight ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecRight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecRight );

    if ( !argStream.HasErrors () )
        pMatrix->vRight = vecRight;

    return !argStream.HasErrors ();
}

int CLuaMatrixDefs::SetUp ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecUp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecUp );

    if ( !argStream.HasErrors () )
        pMatrix->vUp = vecUp;

    return !argStream.HasErrors ();
}

int CLuaMatrixDefs::Add ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
        lua_pushmatrix ( luaVM, *pMatrix1 + *pMatrix2 );

    return !argStream.HasErrors ();
}

int CLuaMatrixDefs::Sub ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
        lua_pushmatrix ( luaVM, *pMatrix1 - *pMatrix2 );

    return !argStream.HasErrors ();
}

int CLuaMatrixDefs::Mul ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );

    if ( argStream.NextIsUserDataOfType < CLuaMatrix > () )
    {
        CLuaMatrix* pMatrix2 = NULL;
        argStream.ReadUserData ( pMatrix2 );

        if ( !argStream.HasErrors () )
        {
            lua_pushmatrix ( luaVM, *pMatrix1 * *pMatrix2 );
            return 1;
        }
    }
    else
    {
        CVector vector;
        argStream.ReadVector3D ( vector );
        
        if ( !argStream.HasErrors () )
        {
            lua_pushvector ( luaVM, *pMatrix1 * vector );
            return 1;
        }
    }

    return 0;
}

int CLuaMatrixDefs::Div ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
        lua_pushmatrix ( luaVM, *pMatrix1 / *pMatrix2 );

    return !argStream.HasErrors ();
}
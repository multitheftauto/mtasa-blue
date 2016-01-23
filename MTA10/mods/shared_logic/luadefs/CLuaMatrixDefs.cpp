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

void CLuaMatrixDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classmetamethod ( luaVM, "__tostring", ToString );
    lua_classmetamethod ( luaVM, "__gc", Destroy );

    lua_classmetamethod ( luaVM, "__add", Add );
    lua_classmetamethod ( luaVM, "__sub", Sub );
    lua_classmetamethod ( luaVM, "__mul", Mul );
    lua_classmetamethod ( luaVM, "__div", Div );

    lua_classfunction ( luaVM, "create", Create );

    lua_classfunction ( luaVM, "transformPosition", TransformPosition );
    lua_classfunction ( luaVM, "transformDirection", TransformDirection );
    lua_classfunction ( luaVM, "inverse", Inverse );

    lua_classfunction ( luaVM, "getPosition", GetPosition );
    lua_classfunction ( luaVM, "getRotation", GetRotation );
    lua_classfunction ( luaVM, "getForward", GetForward );
    lua_classfunction ( luaVM, "getRight", GetRight );
    lua_classfunction ( luaVM, "getUp", GetUp );

    lua_classfunction ( luaVM, "setPosition", SetPosition );
    lua_classfunction ( luaVM, "setRotation", SetRotation );
    lua_classfunction ( luaVM, "setForward", SetForward );
    lua_classfunction ( luaVM, "setRight", SetRight );
    lua_classfunction ( luaVM, "setUp", SetUp );

    lua_classvariable ( luaVM, "position", SetPosition, GetPosition );
    lua_classvariable ( luaVM, "rotation", SetRotation, GetRotation );
    lua_classvariable ( luaVM, "forward", SetForward, GetForward );
    lua_classvariable ( luaVM, "right", SetRight, GetRight );
    lua_classvariable ( luaVM, "up", SetUp, GetUp );

    lua_registerclass ( luaVM, "Matrix" );
}


int CLuaMatrixDefs::Create ( lua_State* luaVM )
{
    CMatrix matrix;

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsVector3D () )
    {
        CVector vecPosition;
        argStream.ReadVector3D ( vecPosition );
        if ( argStream.NextIsVector3D () )
        {
            CVector vecRotation;
            argStream.ReadVector3D ( vecRotation );
            ConvertDegreesToRadiansNoWrap ( vecRotation );
            matrix = CMatrix ( vecPosition, vecRotation );
        }
        else
        {
            matrix = CMatrix ( vecPosition );
        }
    }
    else if ( argStream.NextIsUserDataOfType<CLuaMatrix> () )
    {
        argStream.ReadMatrix ( matrix );
        matrix = CMatrix ( matrix );
    }
    else if ( !argStream.NextIsNone () ) {
        argStream.SetCustomError ( "Expected vector3, matrix or nothing" );
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

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

int CLuaMatrixDefs::ToString ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        SString string = SString ( "Matrix: { %.3f, %.3f, %.3f } { %.3f, %.3f, %.3f } { %.3f, %.3f, %.3f } { %.3f, %.3f, %.3f }",
            pMatrix->vRight.fX, pMatrix->vRight.fY, pMatrix->vRight.fZ,
            pMatrix->vFront.fX, pMatrix->vFront.fY, pMatrix->vFront.fZ,
            pMatrix->vUp.fX, pMatrix->vUp.fY, pMatrix->vUp.fZ,
            pMatrix->vPos.fX, pMatrix->vPos.fY, pMatrix->vPos.fZ );

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

int CLuaMatrixDefs::TransformPosition ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CVector vector;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadVector3D ( vector );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector( luaVM, pMatrix1->TransformVector ( vector ) );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::TransformDirection ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CVector vector;

    CScriptArgReader argStream ( luaVM );

    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadVector3D ( vector );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, *pMatrix1 * vector );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::Inverse ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );

    if ( !argStream.HasErrors () )
    {
        lua_pushmatrix ( luaVM, pMatrix1->Inverse ( ) );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
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
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1; 
}

int CLuaMatrixDefs::GetRotation ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        CVector vecRotation;
        vecRotation = pMatrix->GetRotation ( );
        ConvertRadiansToDegrees ( vecRotation );

        lua_pushvector ( luaVM, vecRotation );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;    
}

int CLuaMatrixDefs::GetForward ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pMatrix->vFront );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
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
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;     
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
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean ( luaVM, false );
    return 1;  
}

int CLuaMatrixDefs::SetPosition ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecPosition );

    if ( !argStream.HasErrors () )
    {
        pMatrix->vPos = vecPosition;
        lua_pushboolean( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::SetRotation ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecRotation );

    if ( !argStream.HasErrors () )
    {
        ConvertRadiansToDegrees ( vecRotation );
        pMatrix->SetRotation ( vecRotation );

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

int CLuaMatrixDefs::SetForward ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecFront;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecFront );

    if ( !argStream.HasErrors () )
    {
        pMatrix->vFront = vecFront;
        lua_pushboolean( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::SetRight ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecRight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecRight );

    if ( !argStream.HasErrors () )
    {
        pMatrix->vRight = vecRight;
        lua_pushboolean( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::SetUp ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix = NULL;
    CVector vecUp;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix );
    argStream.ReadVector3D ( vecUp );

    if ( !argStream.HasErrors () )
    {
        pMatrix->vUp = vecUp;
        lua_pushboolean( luaVM, true );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::Add ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushmatrix ( luaVM, *pMatrix1 + *pMatrix2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::Sub ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushmatrix ( luaVM, *pMatrix1 - *pMatrix2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::Mul ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushmatrix ( luaVM, *pMatrix1 * *pMatrix2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}

int CLuaMatrixDefs::Div ( lua_State* luaVM )
{
    CLuaMatrix* pMatrix1 = NULL;
    CLuaMatrix* pMatrix2 = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMatrix1 );
    argStream.ReadUserData ( pMatrix2 );

    if ( !argStream.HasErrors () )
    {
        lua_pushmatrix ( luaVM, *pMatrix1 / *pMatrix2 );
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    }

    lua_pushboolean( luaVM, false );
    return 1;
}
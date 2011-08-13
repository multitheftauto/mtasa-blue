/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Object.cpp
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

int CLuaFunctionDefs::CreateObject ( lua_State* luaVM )
{
//  object createObject ( int modelid, float x, float y, float z, [float rx, float ry, float rz] )
    int iModelID; CVector vecPosition; CVector vecRotation;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iModelID );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecRotation.fX, 0 );
    argStream.ReadNumber ( vecRotation.fY, 0 );
    argStream.ReadNumber ( vecRotation.fZ, 0 );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                CClientObject* pObject = CStaticFunctionDefinitions::CreateObject ( *pResource, iModelID, vecPosition, vecRotation );
                if ( pObject )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( ( CClientEntity* ) pObject );
                    }

                    lua_pushelement ( luaVM, pObject );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "createObject", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::IsObjectStatic ( lua_State* luaVM )
{
//  bool isObjectStatic ( object theObject )
    CClientObject* pObject;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );

    if ( !argStream.HasErrors () )
    {
        bool bStatic;
        if ( CStaticFunctionDefinitions::IsObjectStatic ( *pObject, bStatic ) )
        {
            lua_pushboolean ( luaVM, bStatic );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "isObjectStatic" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetObjectScale ( lua_State* luaVM )
{
//  float getObjectScale ( object theObject )
    CClientObject* pObject;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pObject );

    if ( !argStream.HasErrors () )
    {
        float fScale;
        if ( CStaticFunctionDefinitions::GetObjectScale ( *pObject, fScale ) )
        {
            lua_pushnumber ( luaVM, fScale );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "getObjectScale", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::MoveObject ( lua_State* luaVM )
{
//  bool moveObject ( object theObject, int time, float targetx, float targety, float targetz,
//      [ float moverx, float movery, float moverz, string strEasingType, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
    CClientEntity* pEntity; int iTime; CVector vecTargetPosition; CVector vecTargetRotation;
    CEasingCurve::eType easingType; float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( iTime );
    argStream.ReadNumber ( vecTargetPosition.fX );
    argStream.ReadNumber ( vecTargetPosition.fY );
    argStream.ReadNumber ( vecTargetPosition.fZ );
    argStream.ReadNumber ( vecTargetRotation.fX, 0 );
    argStream.ReadNumber ( vecTargetRotation.fY, 0 );
    argStream.ReadNumber ( vecTargetRotation.fZ, 0 );
    argStream.ReadEnumString ( easingType, CEasingCurve::Linear );
    argStream.ReadNumber ( fEasingPeriod, 0.3f );
    argStream.ReadNumber ( fEasingAmplitude, 1.0f );
    argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::MoveObject ( *pEntity, iTime, vecTargetPosition, vecTargetRotation, easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "moveObject", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::StopObject ( lua_State* luaVM )
{
//  bool stopObject ( object theobject )
    CClientEntity* pEntity;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::StopObject ( *pEntity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "stopObject", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetObjectScale ( lua_State* luaVM )
{
//  bool setObjectScale ( object theObject, float scale )
    CClientEntity* pEntity; float fScale;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( fScale );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetObjectScale ( *pEntity, fScale ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setObjectScale", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetObjectStatic ( lua_State* luaVM )
{
//  bool setObjectStatic ( object theObject, bool toggle )
    CClientEntity* pEntity; bool bStatic;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadBool ( bStatic );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetObjectStatic ( *pEntity, bStatic ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "setObjectStatic", *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}

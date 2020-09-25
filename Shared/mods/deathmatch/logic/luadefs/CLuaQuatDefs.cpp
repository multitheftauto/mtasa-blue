/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CLuaVectorDefs.cpp
 *  PURPOSE:     Lua general class functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaQuatDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classmetamethod(luaVM, "__tostring", ToString);
    lua_classmetamethod(luaVM, "__gc", Destroy);

    lua_classmetamethod(luaVM, "__add", Add);
    lua_classmetamethod(luaVM, "__sub", Sub);
    lua_classmetamethod(luaVM, "__mul", Mul);
    lua_classmetamethod(luaVM, "__div", Div);
    lua_classmetamethod(luaVM, "__eq", Eq);
    lua_classmetamethod(luaVM, "__unm", Unm);
    lua_classmetamethod(luaVM, "__len", GetLengthSquared);

#ifdef MTA_CLIENT
    // Client
    lua_classfunction(luaVM, "create", Create);
    lua_classfunction(luaVM, "normalize", Normalize);
    lua_classfunction(luaVM, "fromRotationTo", FromRotationTo);
    lua_classfunction(luaVM, "fromAngleAxis", FromAngleAxis);
    lua_classfunction(luaVM, "transformDirection", TransformDirection);
    lua_classfunction(luaVM, "inverse", Inverse);
    lua_classfunction(luaVM, "conjugate", Conjugate);
    lua_classfunction(luaVM, "slerp", SphericalLerp);
    lua_classfunction(luaVM, "nlerp", NormalizedLerp);

    lua_classfunction(luaVM, "dot", DotProduct);

    lua_classfunction(luaVM, "getSquaredLength", GetLengthSquared);
    lua_classfunction(luaVM, "getNormalized", GetNormalized);

    lua_classfunction(luaVM, "getW", GetW);
    lua_classfunction(luaVM, "getX", GetX);
    lua_classfunction(luaVM, "getY", GetY);
    lua_classfunction(luaVM, "getZ", GetZ);

    lua_classfunction(luaVM, "setW", SetW);
    lua_classfunction(luaVM, "setX", SetX);
    lua_classfunction(luaVM, "setY", SetY);
    lua_classfunction(luaVM, "setZ", SetZ);

    lua_classvariable(luaVM, "w", SetW, GetW);
    lua_classvariable(luaVM, "x", SetX, GetX);
    lua_classvariable(luaVM, "y", SetY, GetY);
    lua_classvariable(luaVM, "z", SetZ, GetZ);

    lua_classvariable(luaVM, "squaredLength", NULL, GetLengthSquared);
    lua_classvariable(luaVM, "normalized", NULL, GetNormalized);
#else
    // Server
    lua_classfunction(luaVM, "create", "", Create);
    lua_classfunction(luaVM, "normalize", "", Normalize);
    lua_classfunction(luaVM, "fromRotationTo", "", FromRotationTo);
    lua_classfunction(luaVM, "fromAngleAxis", "", FromAngleAxis);
    lua_classfunction(luaVM, "transformDirection", "", TransformDirection);
    lua_classfunction(luaVM, "inverse", "", Inverse);
    lua_classfunction(luaVM, "conjugate", "", Conjugate);
    lua_classfunction(luaVM, "slerp", "", SphericalLerp);
    lua_classfunction(luaVM, "nlerp", "", NormalizedLerp);

    lua_classfunction(luaVM, "dot", "", DotProduct);

    lua_classfunction(luaVM, "getSquaredLength", "", GetLengthSquared);
    lua_classfunction(luaVM, "getNormalized", "", GetNormalized);

    lua_classfunction(luaVM, "getW", "", GetW);
    lua_classfunction(luaVM, "getX", "", GetX);
    lua_classfunction(luaVM, "getY", "", GetY);
    lua_classfunction(luaVM, "getZ", "", GetZ);

    lua_classfunction(luaVM, "setW", "", SetW);
    lua_classfunction(luaVM, "setX", "", SetX);
    lua_classfunction(luaVM, "setY", "", SetY);
    lua_classfunction(luaVM, "setZ", "", SetZ);

    lua_classvariable(luaVM, "w", "", "", SetW, GetW);
    lua_classvariable(luaVM, "x", "", "", SetX, GetX);
    lua_classvariable(luaVM, "y", "", "", SetY, GetY);
    lua_classvariable(luaVM, "z", "", "", SetZ, GetZ);

    lua_classvariable(luaVM, "squaredLength", "", "", NULL, GetLengthSquared);
    lua_classvariable(luaVM, "normalized", "", "", NULL, GetNormalized);
#endif

    lua_registerclass(luaVM, "Quaternion");
}

int CLuaQuatDefs::Create(lua_State* luaVM)
{
    CQuaternion quat;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsVector4D())
    {
        CVector4D vecComponents;
        argStream.ReadVector4D(vecComponents);

        quat = CQuaternion(vecComponents);
    }
    else if (argStream.NextIsVector3D())
    {
        CVector vecFrom;
        argStream.ReadVector3D(vecFrom);
        if (argStream.NextIsVector3D())
        {
            CVector vecTo;
            argStream.ReadVector3D(vecTo);

            quat.FromRotationTo(vecFrom, vecTo);
        }
        else
        {
            argStream.SetCustomError("Expected vector3");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else if (argStream.NextIsNumber())
    {
        float fAngle = 0.0f;
        argStream.ReadNumber(fAngle);

        if (argStream.NextIsVector3D())
        {
            CVector vecAxis;
            argStream.ReadVector3D(vecAxis);

            quat.FromAngleAxis(fAngle, vecAxis);
        }
        else
        {
            argStream.SetCustomError("Expected vector3");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else if (argStream.NextIsUserDataOfType<CLuaQuaternion>())
    {
        argStream.ReadQuaternion(quat);
    }
    else if (!argStream.NextIsNone())
    {
        argStream.SetCustomError("Expected vector4, quaternion or nothing");
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

        lua_pushboolean(luaVM, false);
        return 1;
    }

    lua_pushquaternion(luaVM, quat);
    return 1;
}

int CLuaQuatDefs::Destroy(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        delete pQuat;
        lua_addtotalbytes(luaVM, -LUA_GC_EXTRA_BYTES);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Normalize(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        pQuat->Normalize();
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::FromRotationTo(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    CVector vecFrom;
    CVector vecTo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);
    argStream.ReadVector3D(vecFrom);
    argStream.ReadVector3D(vecTo);

    if (!argStream.HasErrors())
    {
        pQuat->FromRotationTo(vecFrom, vecTo);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::FromAngleAxis(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    float fAngle;
    CVector vecAxis;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);
    argStream.ReadNumber(fAngle);
    argStream.ReadVector3D(vecAxis);

    if (!argStream.HasErrors())
    {
        pQuat->FromAngleAxis(fAngle, vecAxis);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::ToString(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        SString string = SString("Quaternion: { %.3f, %.3f, %.3f, %.3f }", pQuat->fW, pQuat->fX, pQuat->fY, pQuat->fZ);

        lua_pushstring(luaVM, string.c_str());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::TransformDirection(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    CVector vector;

    CScriptArgReader argStream(luaVM);

    argStream.ReadUserData(pQuat);
    argStream.ReadVector3D(vector);

    if (!argStream.HasErrors())
    {
        lua_pushvector(luaVM, *pQuat * vector);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Inverse(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, pQuat->Inverse());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Conjugate(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, pQuat->Conjugate());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::SphericalLerp(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = nullptr;
    CLuaQuaternion* pQuat2 = nullptr;
    float fTime = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);
    argStream.ReadNumber(fTime);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, pQuat1->Slerp(*pQuat2, fTime));
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::NormalizedLerp(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = nullptr;
    CLuaQuaternion* pQuat2 = nullptr;
    float fTime = 0.0f;
    bool bShortestPath = true;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);
    argStream.ReadNumber(fTime);
    argStream.ReadBool(bShortestPath);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, pQuat1->Nlerp(*pQuat2, fTime, bShortestPath));
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::GetLengthSquared(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pQuat->LengthSquared());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::GetNormalized(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, pQuat->GetNormalized());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::DotProduct(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = nullptr;
    CLuaQuaternion* pQuat2 = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pQuat1->DotProduct(*pQuat2));
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::GetW(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pQuat->fW);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::GetX(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pQuat->fX);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::GetY(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pQuat->fY);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::GetZ(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pQuat->fZ);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::SetW(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    float fValue = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        pQuat->fW = fValue;
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::SetX(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    float fValue = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        pQuat->fX = fValue;
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::SetY(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    float fValue = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        pQuat->fY = fValue;
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::SetZ(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;
    float fValue = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        pQuat->fZ = fValue;
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Add(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = NULL;
    CLuaQuaternion* pQuat2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, *pQuat1 + *pQuat2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Sub(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = NULL;
    CLuaQuaternion* pQuat2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, *pQuat1 - *pQuat2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Mul(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = NULL;
    CLuaQuaternion* pQuat2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, *pQuat1 * *pQuat2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Div(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = NULL;
    CLuaQuaternion* pQuat2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, *pQuat1 * pQuat2->Inverse());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Eq(lua_State* luaVM)
{
    CLuaQuaternion* pQuat1 = NULL;
    CLuaQuaternion* pQuat2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat1);
    argStream.ReadUserData(pQuat2);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pQuat1 == pQuat2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaQuatDefs::Unm(lua_State* luaVM)
{
    CLuaQuaternion* pQuat = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pQuat);

    if (!argStream.HasErrors())
    {
        lua_pushquaternion(luaVM, pQuat->Conjugate());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

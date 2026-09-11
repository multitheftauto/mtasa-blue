/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaVector2Defs.cpp
 *  PURPOSE:     Lua general class functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaVector2Defs.h"
#include "CStaticFunctionDefinitions.h"
#include "lua/LuaCommon.h"
#include "lua/CLuaShared.h"
#include "CScriptArgReader.h"

void CLuaVector2Defs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classmetamethod(luaVM, "__tostring", ToString);
    lua_classmetamethod(luaVM, "__gc", Destroy);

    lua_classmetamethod(luaVM, "__add", Add);
    lua_classmetamethod(luaVM, "__sub", Sub);
    lua_classmetamethod(luaVM, "__mul", Mul);
    lua_classmetamethod(luaVM, "__div", Div);
    lua_classmetamethod(luaVM, "__pow", Pow);
    lua_classmetamethod(luaVM, "__unm", Unm);
    lua_classmetamethod(luaVM, "__eq", Eq);
    lua_classmetamethod(luaVM, "__len", GetLength);

#ifdef MTA_CLIENT
    // Client
    lua_classfunction(luaVM, "create", Create);
    lua_classfunction(luaVM, "normalize", Normalize);
    lua_classfunction(luaVM, "dot", Dot);

    lua_classfunction(luaVM, "set", InPlaceSet);
    lua_classfunction(luaVM, "add", InPlaceAdd);
    lua_classfunction(luaVM, "sub", InPlaceSub);
    lua_classfunction(luaVM, "mul", InPlaceMul);
    lua_classfunction(luaVM, "div", InPlaceDiv);
    lua_classfunction(luaVM, "scale", InPlaceMul);
    lua_classfunction(luaVM, "addScaled", InPlaceAddScaled);

    lua_classfunction(luaVM, "getLength", GetLength);
    lua_classfunction(luaVM, "getSquaredLength", GetLengthSquared);
    lua_classfunction(luaVM, "getNormalized", GetNormalized);
    lua_classfunction(luaVM, "getX", GetX);
    lua_classfunction(luaVM, "getY", GetY);

    lua_classfunction(luaVM, "setX", SetX);
    lua_classfunction(luaVM, "setY", SetY);

    lua_classvariable(luaVM, "x", SetX, GetX);
    lua_classvariable(luaVM, "y", SetY, GetY);

    lua_classvariable(luaVM, "length", NULL, GetLength);
    lua_classvariable(luaVM, "squaredLength", NULL, GetLengthSquared);
    lua_classvariable(luaVM, "normalized", NULL, GetNormalized);
#else
    // Server
    lua_classfunction(luaVM, "create", "", Create);
    lua_classfunction(luaVM, "normalize", "", Normalize);
    lua_classfunction(luaVM, "dot", "", Dot);

    lua_classfunction(luaVM, "set", "", InPlaceSet);
    lua_classfunction(luaVM, "add", "", InPlaceAdd);
    lua_classfunction(luaVM, "sub", "", InPlaceSub);
    lua_classfunction(luaVM, "mul", "", InPlaceMul);
    lua_classfunction(luaVM, "div", "", InPlaceDiv);
    lua_classfunction(luaVM, "scale", "", InPlaceMul);
    lua_classfunction(luaVM, "addScaled", "", InPlaceAddScaled);

    lua_classfunction(luaVM, "getLength", "", GetLength);
    lua_classfunction(luaVM, "getSquaredLength", "", GetLengthSquared);
    lua_classfunction(luaVM, "getNormalized", "", GetNormalized);
    lua_classfunction(luaVM, "getX", "", GetX);
    lua_classfunction(luaVM, "getY", "", GetY);

    lua_classfunction(luaVM, "setX", "", SetX);
    lua_classfunction(luaVM, "setY", "", SetY);

    lua_classvariable(luaVM, "x", "", "", SetX, GetX);
    lua_classvariable(luaVM, "y", "", "", SetY, GetY);

    lua_classvariable(luaVM, "length", "", "", NULL, GetLength);
    lua_classvariable(luaVM, "squaredLength", "", "", NULL, GetLengthSquared);
    lua_classvariable(luaVM, "normalized", "", "", NULL, GetNormalized);
#endif

    lua_registerclass(luaVM, "Vector2");
}

int CLuaVector2Defs::Create(lua_State* luaVM)
{
    CVector2D vector;

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsTable())
    {
        lua_pushvalue(luaVM, 1);

        lua_pushstring(luaVM, "x");
        lua_rawget(luaVM, -2);
        if (lua_isnumber(luaVM, -1))
        {
            vector.fX = (float)lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);
        }
        else
        {
            lua_pop(luaVM, 1);
            lua_rawgeti(luaVM, -1, 1);
            if (lua_isnumber(luaVM, -1))
                vector.fX = (float)lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);
        }

        lua_pushstring(luaVM, "y");
        lua_rawget(luaVM, -2);
        if (lua_isnumber(luaVM, -1))
        {
            vector.fY = (float)lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);
        }
        else
        {
            lua_pop(luaVM, 1);
            lua_rawgeti(luaVM, -1, 2);
            if (lua_isnumber(luaVM, -1))
                vector.fY = (float)lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);
        }
    }
    else if (argStream.NextIsNumber())
    {
        argStream.ReadNumber(vector.fX);
        if (argStream.NextIsNumber())
        {
            argStream.ReadNumber(vector.fY);
        }
    }
    else if (argStream.NextIsVector2D())
    {
        argStream.ReadVector2D(vector);
    }

    lua_pushvector(luaVM, vector);
    return 1;
}

int CLuaVector2Defs::Destroy(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        delete pVector;
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

int CLuaVector2Defs::GetLength(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pVector->Length());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::GetLengthSquared(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pVector->LengthSquared());
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::GetNormalized(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        CVector2D vector(*pVector);
        vector.Normalize();

        lua_pushvector(luaVM, vector);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Normalize(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        pVector->Normalize();
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

int CLuaVector2Defs::Dot(lua_State* luaVM)
{
    CLuaVector2D* pVector1 = NULL;
    CLuaVector2D* pVector2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector1);
    argStream.ReadUserData(pVector2);

    if (!argStream.HasErrors())
    {
        CVector2D vector(*pVector1);
        float     fProduct = vector.DotProduct(*pVector2);

        lua_pushnumber(luaVM, fProduct);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::ToString(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        SString string = SString("vector2: { x = %.3f, y = %.3f }", pVector->fX, pVector->fY);
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

int CLuaVector2Defs::SetX(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;
    float         fValue = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        pVector->fX = fValue;

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

int CLuaVector2Defs::SetY(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;
    float         fValue = 0.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);
    argStream.ReadNumber(fValue);

    if (!argStream.HasErrors())
    {
        pVector->fY = fValue;

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

int CLuaVector2Defs::GetX(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pVector->fX);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::GetY(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pVector->fY);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Add(lua_State* luaVM)
{
    CLuaVector2D* pVector1 = NULL;
    CLuaVector2D* pVector2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector1);
    argStream.ReadUserData(pVector2);

    if (!argStream.HasErrors())
    {
        lua_pushvector(luaVM, *pVector1 + *pVector2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Sub(lua_State* luaVM)
{
    CLuaVector2D* pVector1 = NULL;
    CLuaVector2D* pVector2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector1);
    argStream.ReadUserData(pVector2);

    if (!argStream.HasErrors())
    {
        lua_pushvector(luaVM, *pVector1 - *pVector2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Mul(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    pVector1 = NULL;

    if (argStream.NextIsNumber())
    {
        // number * vector2
        float fValue = 0.0f;
        argStream.ReadNumber(fValue);
        argStream.ReadUserData(pVector1);

        if (!argStream.HasErrors())
        {
            lua_pushvector(luaVM, *pVector1 * fValue);
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    else
    {
        // vector2 * ?
        argStream.ReadUserData(pVector1);
        if (argStream.NextIsNumber())
        {
            float fValue = 0.0f;
            argStream.ReadNumber(fValue);
            if (!argStream.HasErrors())
            {
                lua_pushvector(luaVM, *pVector1 * fValue);
                return 1;
            }
            else
                m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
        else
        {
            CLuaVector2D* pVector2 = NULL;
            argStream.ReadUserData(pVector2);

            if (!argStream.HasErrors())
            {
                lua_pushvector(luaVM, *pVector1 * *pVector2);
                return 1;
            }
            else
                m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Div(lua_State* luaVM)
{
    CLuaVector2D* pVector1 = NULL;
    CLuaVector2D* pVector2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector1);

    if (argStream.NextIsNumber())
    {
        float fValue = 0.0f;
        argStream.ReadNumber(fValue);

        lua_pushvector(luaVM, *pVector1 / fValue);
        return 1;
    }
    else
    {
        argStream.ReadUserData(pVector2);

        if (!argStream.HasErrors())
        {
            lua_pushvector(luaVM, *pVector1 / *pVector2);
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Pow(lua_State* luaVM)
{
    CLuaVector2D* pVector1 = NULL;
    CLuaVector2D* pVector2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector1);

    if (argStream.NextIsNumber())
    {
        float fValue = 0.0f;
        argStream.ReadNumber(fValue);

        CVector2D vector(*pVector1);
        vector.fX = pow(vector.fX, fValue);
        vector.fY = pow(vector.fY, fValue);

        lua_pushvector(luaVM, vector);
        return 1;
    }
    else
    {
        argStream.ReadUserData(pVector2);

        if (!argStream.HasErrors())
        {
            CVector2D vector(*pVector1);
            vector.fX = pow(vector.fX, pVector2->fX);
            vector.fY = pow(vector.fY, pVector2->fY);

            lua_pushvector(luaVM, vector);
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        }
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Unm(lua_State* luaVM)
{
    CLuaVector2D* pVector = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector);

    if (!argStream.HasErrors())
    {
        lua_pushvector(luaVM, CVector2D() - *pVector);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::Eq(lua_State* luaVM)
{
    CLuaVector2D* pVector1 = NULL;
    CLuaVector2D* pVector2 = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pVector1);
    argStream.ReadUserData(pVector2);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pVector1 == pVector2);
        return 1;
    }
    else
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::InPlaceSet(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    targetVector = nullptr;
    argStream.ReadUserData(targetVector);

    if (argStream.NextIsUserData())
    {
        CLuaVector2D* sourceVector = nullptr;
        argStream.ReadUserData(sourceVector);
        if (!argStream.HasErrors() && targetVector && sourceVector)
        {
            targetVector->fX = sourceVector->fX;
            targetVector->fY = sourceVector->fY;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }
    else
    {
        float x = 0.0f;
        float y = 0.0f;
        argStream.ReadNumber(x);
        argStream.ReadNumber(y);
        if (!argStream.HasErrors() && targetVector)
        {
            targetVector->fX = x;
            targetVector->fY = y;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::InPlaceAdd(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    targetVector = nullptr;
    argStream.ReadUserData(targetVector);

    if (argStream.NextIsUserData())
    {
        CLuaVector2D* sourceVector = nullptr;
        argStream.ReadUserData(sourceVector);
        if (!argStream.HasErrors() && targetVector && sourceVector)
        {
            targetVector->fX += sourceVector->fX;
            targetVector->fY += sourceVector->fY;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }
    else
    {
        float x = 0.0f;
        float y = 0.0f;
        argStream.ReadNumber(x);
        argStream.ReadNumber(y);
        if (!argStream.HasErrors() && targetVector)
        {
            targetVector->fX += x;
            targetVector->fY += y;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::InPlaceSub(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    targetVector = nullptr;
    argStream.ReadUserData(targetVector);

    if (argStream.NextIsUserData())
    {
        CLuaVector2D* sourceVector = nullptr;
        argStream.ReadUserData(sourceVector);
        if (!argStream.HasErrors() && targetVector && sourceVector)
        {
            targetVector->fX -= sourceVector->fX;
            targetVector->fY -= sourceVector->fY;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }
    else
    {
        float x = 0.0f;
        float y = 0.0f;
        argStream.ReadNumber(x);
        argStream.ReadNumber(y);
        if (!argStream.HasErrors() && targetVector)
        {
            targetVector->fX -= x;
            targetVector->fY -= y;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::InPlaceMul(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    targetVector = nullptr;
    argStream.ReadUserData(targetVector);

    if (argStream.NextIsUserData())
    {
        CLuaVector2D* sourceVector = nullptr;
        argStream.ReadUserData(sourceVector);
        if (!argStream.HasErrors() && targetVector && sourceVector)
        {
            targetVector->fX *= sourceVector->fX;
            targetVector->fY *= sourceVector->fY;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }
    else if (argStream.NextIsNumber())
    {
        float factorX = 0.0f;
        argStream.ReadNumber(factorX);
        if (argStream.NextIsNumber())
        {
            float factorY = 0.0f;
            argStream.ReadNumber(factorY);
            if (!argStream.HasErrors() && targetVector)
            {
                targetVector->fX *= factorX;
                targetVector->fY *= factorY;
                lua_pushvalue(luaVM, 1);
                return 1;
            }
        }
        else
        {
            if (!argStream.HasErrors() && targetVector)
            {
                targetVector->fX *= factorX;
                targetVector->fY *= factorX;
                lua_pushvalue(luaVM, 1);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::InPlaceDiv(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    targetVector = nullptr;
    argStream.ReadUserData(targetVector);

    if (argStream.NextIsUserData())
    {
        CLuaVector2D* sourceVector = nullptr;
        argStream.ReadUserData(sourceVector);
        if (!argStream.HasErrors() && targetVector && sourceVector)
        {
            targetVector->fX /= sourceVector->fX;
            targetVector->fY /= sourceVector->fY;
            lua_pushvalue(luaVM, 1);
            return 1;
        }
    }
    else if (argStream.NextIsNumber())
    {
        float divisorX = 0.0f;
        argStream.ReadNumber(divisorX);
        if (argStream.NextIsNumber())
        {
            float divisorY = 0.0f;
            argStream.ReadNumber(divisorY);
            if (!argStream.HasErrors() && targetVector)
            {
                targetVector->fX /= divisorX;
                targetVector->fY /= divisorY;
                lua_pushvalue(luaVM, 1);
                return 1;
            }
        }
        else
        {
            if (!argStream.HasErrors() && targetVector)
            {
                targetVector->fX /= divisorX;
                targetVector->fY /= divisorX;
                lua_pushvalue(luaVM, 1);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaVector2Defs::InPlaceAddScaled(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);
    CLuaVector2D*    targetVector = nullptr;
    CLuaVector2D*    sourceVector = nullptr;
    float            scaleFactor = 1.0f;

    argStream.ReadUserData(targetVector);
    argStream.ReadUserData(sourceVector);
    argStream.ReadNumber(scaleFactor);

    if (!argStream.HasErrors() && targetVector && sourceVector)
    {
        targetVector->fX += sourceVector->fX * scaleFactor;
        targetVector->fY += sourceVector->fY * scaleFactor;
        lua_pushvalue(luaVM, 1);
        return 1;
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

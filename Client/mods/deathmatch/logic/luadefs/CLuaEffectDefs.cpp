/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaEffectDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaEffectDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"fxAddBlood", fxAddBlood},
        {"fxAddWood", fxAddWood},
        {"fxAddSparks", fxAddSparks},
        {"fxAddTyreBurst", fxAddTyreBurst},
        {"fxAddBulletImpact", fxAddBulletImpact},
        {"fxAddPunchImpact", fxAddPunchImpact},
        {"fxAddDebris", fxAddDebris},
        {"fxAddGlass", fxAddGlass},
        {"fxAddWaterHydrant", fxAddWaterHydrant},
        {"fxAddGunshot", fxAddGunshot},
        {"fxAddTankFire", fxAddTankFire},
        {"fxAddWaterSplash", fxAddWaterSplash},
        {"fxAddBulletSplash", fxAddBulletSplash},
        {"fxAddFootSplash", fxAddFootSplash},
        {"createEffect", CreateEffect},
        {"setEffectSpeed", SetEffectSpeed},
        {"getEffectSpeed", GetEffectSpeed},
        {"setEffectDensity", SetEffectDensity},
        {"getEffectDensity", GetEffectDensity},
        {"fxCreateParticle", ArgumentParser<FxCreateParticle>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaEffectDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "create", "createEffect");
    lua_classfunction(luaVM, "addBlood", "fxAddBlood");
    lua_classfunction(luaVM, "addBulletImpact", "fxAddBulletImpact");
    lua_classfunction(luaVM, "addBulletSplash", "fxAddBulletSplash");
    lua_classfunction(luaVM, "addDebris", "fxAddDebris");
    lua_classfunction(luaVM, "addFootSplash", "fxAddFootSplash");
    lua_classfunction(luaVM, "addGlass", "fxAddGlass");
    lua_classfunction(luaVM, "addGunshot", "fxAddGunshot");
    lua_classfunction(luaVM, "addPunchImpact", "fxAddPunchImpact");
    lua_classfunction(luaVM, "addSparks", "fxAddSparks");
    lua_classfunction(luaVM, "addTankFire", "fxAddTankFire");
    lua_classfunction(luaVM, "addTyreBurst", "fxAddTyreBurst");
    lua_classfunction(luaVM, "addWaterHydrant", "fxAddWaterHydrant");
    lua_classfunction(luaVM, "addWaterSplash", "fxAddWaterSplash");
    lua_classfunction(luaVM, "addWood", "fxAddWood");
    lua_classfunction(luaVM, "createParticle", "fxCreateParticle");

    lua_classfunction(luaVM, "setDensity", "setEffectDensity");
    lua_classfunction(luaVM, "setSpeed", "setEffectSpeed");

    lua_classfunction(luaVM, "getDensity", "getEffectDensity");
    lua_classfunction(luaVM, "getSpeed", "getEffectSpeed");

    lua_classvariable(luaVM, "density", "setEffectDensity", "getEffectDensity");
    lua_classvariable(luaVM, "speed", "setEffectSpeed", "getEffectSpeed");

    lua_registerclass(luaVM, "Effect", "Element");
}

int CLuaEffectDefs::fxAddBlood(lua_State* luaVM)
{
    // bool fxAddBlood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )

    CVector vecPosition, vecDirection;
    int     iCount = 1;
    float   fBrightness = 1.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);
    argStream.ReadNumber(iCount, 1);
    argStream.ReadNumber(fBrightness, 1.0f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddBlood(vecPosition, vecDirection, iCount, fBrightness))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddWood(lua_State* luaVM)
{
    // bool fxAddWood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )

    CVector vecPosition, vecDirection;
    int     iCount = 1;
    float   fBrightness = 1.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);
    argStream.ReadNumber(iCount, 1);
    argStream.ReadNumber(fBrightness, 1.0f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddWood(vecPosition, vecDirection, iCount, fBrightness))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddSparks(lua_State* luaVM)
{
    // bool fxAddSparks ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [float force=1, int count=1, float acrossLineX=0, float
    // acrossLineY=0, float acrossLineZ=0, bool blur=false, float spread=1, float life=1] )

    // Verify types
    CVector vecPosition, vecDirection;
    float   fForce = 1.0f;
    int     iCount = 1;
    CVector vecAcrossLine;
    bool    bBlur = false;
    float   fSpread = 1.0f;
    float   fLife = 1.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);
    argStream.ReadNumber(fForce, 1.0f);
    argStream.ReadNumber(iCount, 1);
    argStream.ReadVector3D(vecAcrossLine, vecAcrossLine);
    argStream.ReadBool(bBlur, false);
    argStream.ReadNumber(fSpread, 1.0f);
    argStream.ReadNumber(fLife, 1.0f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddSparks(vecPosition, vecDirection, fForce, iCount, vecAcrossLine, bBlur, fSpread, fLife))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddTyreBurst(lua_State* luaVM)
{
    // bool fxAddTyreBurst ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    CVector vecPosition, vecDirection;
    int     iCount = 1;
    float   fBrightness = 1.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddTyreBurst(vecPosition, vecDirection))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddBulletImpact(lua_State* luaVM)
{
    // bool fxAddBulletImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int smokeSize=1, int sparkCount=1, float
    // fSmokeIntensity=1.0] )

    // Verify types
    CVector vecPosition, vecDirection;
    int     iSmokeSize = 1;
    int     iSparkCount = 1;
    float   fSmokeIntensity = 1.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);
    argStream.ReadNumber(iSmokeSize, 1);
    argStream.ReadNumber(iSparkCount, 1);
    argStream.ReadNumber(fSmokeIntensity, 1.0f);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddBulletImpact(vecPosition, vecDirection, iSmokeSize, iSparkCount, fSmokeIntensity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddPunchImpact(lua_State* luaVM)
{
    // bool fxAddPunchImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    CVector vecPosition, vecDirection;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddPunchImpact(vecPosition, vecDirection))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddDebris(lua_State* luaVM)
{
    // bool fxAddDebris ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )

    // Verify types
    CVector vecPosition;
    RwColor rwColor;
    rwColor.r = 255;
    rwColor.g = 0;
    rwColor.b = 0;
    rwColor.a = 255;
    float fScale = 1.0f;
    int   iCount = 1;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(rwColor.r, 255);
    argStream.ReadNumber(rwColor.g, 0);
    argStream.ReadNumber(rwColor.b, 0);
    argStream.ReadNumber(rwColor.a, 255);
    argStream.ReadNumber(fScale, 1.0f);
    argStream.ReadNumber(iCount, 1);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddDebris(vecPosition, rwColor, fScale, iCount))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddGlass(lua_State* luaVM)
{
    // bool fxAddGlass ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )

    // Verify types
    CVector vecPosition;
    RwColor rwColor;
    rwColor.r = 255;
    rwColor.g = 0;
    rwColor.b = 0;
    rwColor.a = 255;
    float fScale = 1.0f;
    int   iCount = 1;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(rwColor.r, 255);
    argStream.ReadNumber(rwColor.g, 0);
    argStream.ReadNumber(rwColor.b, 0);
    argStream.ReadNumber(rwColor.a, 255);
    argStream.ReadNumber(fScale, 1.0f);
    argStream.ReadNumber(iCount, 1);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddGlass(vecPosition, rwColor, fScale, iCount))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddWaterHydrant(lua_State* luaVM)
{
    // bool fxAddWaterHydrant ( float posX, float posY, float posZ )

    // Verify types
    CVector vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddWaterHydrant(vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddGunshot(lua_State* luaVM)
{
    // bool fxAddGunshot ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [bool includeSparks=true] )

    // Verify types
    CVector vecPosition, vecDirection;
    bool    bIncludeSparks = true;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);
    argStream.ReadBool(bIncludeSparks, true);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddGunshot(vecPosition, vecDirection, bIncludeSparks))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddTankFire(lua_State* luaVM)
{
    // bool fxAddTankFire ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )

    // Verify types
    CVector vecPosition, vecDirection;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecDirection);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddTankFire(vecPosition, vecDirection))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddWaterSplash(lua_State* luaVM)
{
    // bool fxAddWaterSplash ( float posX, float posY, float posZ )

    // Verify types
    CVector          vecPosition;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddWaterSplash(vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddBulletSplash(lua_State* luaVM)
{
    // bool fxAddBulletSplash ( float posX, float posY, float posZ )

    CVector vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddBulletSplash(vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::fxAddFootSplash(lua_State* luaVM)
{
    // bool fxAddFootSplash ( float posX, float posY, float posZ )

    CVector vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FxAddFootSplash(vecPosition))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::CreateEffect(lua_State* luaVM)
{
    // bool createEffect ( string fxName, float posX, float posY, float posZ[, float rotX, float rotY, float rotZ, float drawDistance, bool soundEnable = false]
    // )

    CVector vecPosition;
    CVector vecRotation;
    SString strFxName;
    float   fDrawDistance;
    bool    bSoundEnable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strFxName);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));
    argStream.ReadNumber(fDrawDistance, 0);
    argStream.ReadBool(bSoundEnable, false);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientEffect* pFx = CStaticFunctionDefinitions::CreateEffect(*pResource, strFxName, vecPosition, bSoundEnable);
                if (pFx != NULL)
                {
                    pFx->SetRotationDegrees(vecRotation);
                    pFx->SetDrawDistance(fDrawDistance);
                    lua_pushelement(luaVM, pFx);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::GetEffectSpeed(lua_State* luaVM)
{
    // float getEffectSpeed ( effect theEffect )

    CClientEffect* pEffect;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEffect);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pEffect->GetEffectSpeed());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::SetEffectSpeed(lua_State* luaVM)
{
    // void setEffectSpeed ( effect theEffect, float fSpeed )

    CClientEffect* pEffect;
    float          fSpeed;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEffect);
    argStream.ReadNumber(fSpeed);

    if (!argStream.HasErrors())
    {
        pEffect->SetEffectSpeed(fSpeed);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::GetEffectDensity(lua_State* luaVM)
{
    // float getEffectDensity ( effect theEffect )

    CClientEffect* pEffect;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEffect);

    if (!argStream.HasErrors())
    {
        lua_pushnumber(luaVM, pEffect->GetEffectDensity());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaEffectDefs::SetEffectDensity(lua_State* luaVM)
{
    // void setEffectDensity ( effect theEffect, float fDensity )

    CClientEffect* pEffect;
    float          fDensity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEffect);
    argStream.ReadNumber(fDensity);

    if (!argStream.HasErrors())
    {
        if (pEffect->SetEffectDensity(fDensity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, "effect density beyond bounds");
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaEffectDefs::FxCreateParticle(eFxParticleSystems eParticleSystem, CVector vecPosition, CVector vecDirection, float fR, float fG, float fB, float fA, std::optional<bool> bRandomizeColors, std::optional<std::uint32_t> iCount, std::optional<float> fBrightness, std::optional<float> fSize, std::optional<bool> bRandomizeSizes, std::optional<float> fLife)
{
    return CStaticFunctionDefinitions::FxCreateParticle(eParticleSystem, vecPosition, vecDirection, fR/255, fG/255, fB/255, fA/255, bRandomizeColors.value_or(false), iCount.value_or(1), fBrightness.value_or(1.0f), fSize.value_or(0.3f), bRandomizeSizes.value_or(false), fLife.value_or(1.0f));
}

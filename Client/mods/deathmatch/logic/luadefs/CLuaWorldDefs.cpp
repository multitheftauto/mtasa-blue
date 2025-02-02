/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CSettings.h>
#include <game/CWeather.h>
#include <game/CColPoint.h>
#include <game/CCoronas.h>
#include <game/CClock.h>
#include "lua/CLuaFunctionParser.h"

void CLuaWorldDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{// World get functions
                                                                             {"getTime", GetTime},
                                                                             {"getColorFilter", ArgumentParser<GetColorFilter>},
                                                                             {"getRoofPosition", GetRoofPosition},
                                                                             {"getGroundPosition", GetGroundPosition},
                                                                             {"processLineAgainstMesh", ArgumentParser<ProcessLineAgainstMesh>},
                                                                             {"processLineOfSight", ProcessLineOfSight},
                                                                             {"getWorldFromScreenPosition", GetWorldFromScreenPosition},
                                                                             {"getScreenFromWorldPosition", GetScreenFromWorldPosition},
                                                                             {"getWeather", GetWeather},
                                                                             {"getZoneName", GetZoneName},
                                                                             {"getGravity", GetGravity},
                                                                             {"getGameSpeed", GetGameSpeed},
                                                                             {"getMinuteDuration", GetMinuteDuration},
                                                                             {"getWaveHeight", GetWaveHeight},
                                                                             {"getGaragePosition", GetGaragePosition},
                                                                             {"getGarageSize", GetGarageSize},
                                                                             {"getGarageBoundingBox", GetGarageBoundingBox},
                                                                             {"getBlurLevel", GetBlurLevel},
                                                                             {"getTrafficLightState", GetTrafficLightState},
                                                                             {"getSkyGradient", GetSkyGradient},
                                                                             {"getHeatHaze", GetHeatHaze},
                                                                             {"getJetpackMaxHeight", GetJetpackMaxHeight},
                                                                             {"getWindVelocity", GetWindVelocity},
                                                                             {"getInteriorSoundsEnabled", GetInteriorSoundsEnabled},
                                                                             {"getInteriorFurnitureEnabled", GetInteriorFurnitureEnabled},
                                                                             {"getFarClipDistance", GetFarClipDistance},
                                                                             {"getNearClipDistance", GetNearClipDistance},
                                                                             {"getVehiclesLODDistance", GetVehiclesLODDistance},
                                                                             {"getPedsLODDistance", GetPedsLODDistance},
                                                                             {"getFogDistance", GetFogDistance},
                                                                             {"getSunColor", GetSunColor},
                                                                             {"getSunSize", GetSunSize},
                                                                             {"getAircraftMaxHeight", GetAircraftMaxHeight},
                                                                             {"getAircraftMaxVelocity", GetAircraftMaxVelocity},
                                                                             {"getOcclusionsEnabled", GetOcclusionsEnabled},
                                                                             {"getCloudsEnabled", GetCloudsEnabled},
                                                                             {"getRainLevel", GetRainLevel},
                                                                             {"getMoonSize", GetMoonSize},
                                                                             {"getFPSLimit", GetFPSLimit},
                                                                             {"getBirdsEnabled", GetBirdsEnabled},
                                                                             {"getCoronaReflectionsEnabled", ArgumentParser<GetCoronaReflectionsEnabled>},
                                                                             {"getWorldProperty", ArgumentParser<GetWorldProperty>},

                                                                             // World set funcs
                                                                             {"setTime", SetTime},
                                                                             {"setColorFilter", ArgumentParser<SetColorFilter>},
                                                                             {"setGrainMultiplier", ArgumentParser<SetGrainMultiplier>},
                                                                             {"setGrainLevel", ArgumentParser<SetGrainLevel>},
                                                                             {"setSkyGradient", SetSkyGradient},
                                                                             {"setHeatHaze", SetHeatHaze},
                                                                             {"setWeather", SetWeather},
                                                                             {"setWeatherBlended", SetWeatherBlended},
                                                                             {"setGravity", SetGravity},
                                                                             {"setGameSpeed", SetGameSpeed},
                                                                             {"setWaveHeight", SetWaveHeight},
                                                                             {"setMinuteDuration", SetMinuteDuration},
                                                                             {"setGarageOpen", SetGarageOpen},
                                                                             {"setWorldSpecialPropertyEnabled", ArgumentParser<SetWorldSpecialPropertyEnabled>},
                                                                             {"setBlurLevel", SetBlurLevel},
                                                                             {"setJetpackMaxHeight", SetJetpackMaxHeight},
                                                                             {"setCloudsEnabled", SetCloudsEnabled},
                                                                             {"setTrafficLightState", SetTrafficLightState},
                                                                             {"setTrafficLightsLocked", SetTrafficLightsLocked},
                                                                             {"setWindVelocity", SetWindVelocity},
                                                                             {"setInteriorSoundsEnabled", SetInteriorSoundsEnabled},
                                                                             {"setInteriorFurnitureEnabled", SetInteriorFurnitureEnabled},
                                                                             {"setRainLevel", SetRainLevel},
                                                                             {"setFarClipDistance", SetFarClipDistance},
                                                                             {"setNearClipDistance", SetNearClipDistance},
                                                                             {"setVehiclesLODDistance", SetVehiclesLODDistance},
                                                                             {"setPedsLODDistance", SetPedsLODDistance},
                                                                             {"setFogDistance", SetFogDistance},
                                                                             {"setSunColor", SetSunColor},
                                                                             {"setSunSize", SetSunSize},
                                                                             {"setAircraftMaxHeight", SetAircraftMaxHeight},
                                                                             {"setAircraftMaxVelocity", SetAircraftMaxVelocity},
                                                                             {"setOcclusionsEnabled", SetOcclusionsEnabled},
                                                                             {"setBirdsEnabled", SetBirdsEnabled},
                                                                             {"setPedTargetingMarkerEnabled", SetPedTargetingMarkerEnabled},
                                                                             {"setMoonSize", SetMoonSize},
                                                                             {"setFPSLimit", SetFPSLimit},
                                                                             {"setCoronaReflectionsEnabled", ArgumentParser<SetCoronaReflectionsEnabled>},
                                                                             {"setWorldProperty", ArgumentParser<SetWorldProperty>},

                                                                             // World remove/restore functions 
                                                                             {"removeWorldModel", RemoveWorldBuilding},
                                                                             {"restoreAllWorldModels", RestoreWorldBuildings},
                                                                             {"restoreWorldModel", RestoreWorldBuilding},
                                                                             {"removeGameWorld", ArgumentParser<RemoveGameWorld>},
                                                                             {"restoreGameWorld", ArgumentParser<RestoreGameWorld>},

                                                                             {"setTimeFrozen", ArgumentParser<SetTimeFrozen>},
                                                                             {"setVolumetricShadowsEnabled", ArgumentParser<SetVolumetricShadowsEnabled>},
                                                                             {"setDynamicPedShadowsEnabled", ArgumentParser<SetDynamicPedShadowsEnabled>}, 


                                                                             // World create funcs
                                                                             {"createSWATRope", CreateSWATRope},
                                                                             {"createExplosion", CreateExplosion},

                                                                             // World reset funcs
                                                                             {"resetColorFilter", ArgumentParser<ResetColorFilter>},
                                                                             {"resetCoronaReflectionsEnabled", ArgumentParser<ResetCoronaReflectionsEnabled>},
                                                                             {"resetSkyGradient", ResetSkyGradient},
                                                                             {"resetHeatHaze", ResetHeatHaze},
                                                                             {"resetWindVelocity", ResetWindVelocity},
                                                                             {"resetRainLevel", ResetRainLevel},
                                                                             {"resetFarClipDistance", ResetFarClipDistance},
                                                                             {"resetNearClipDistance", ResetNearClipDistance},
                                                                             {"resetVehiclesLODDistance", ResetVehiclesLODDistance},
                                                                             {"resetPedsLODDistance", ResetPedsLODDistance},
                                                                             {"resetFogDistance", ResetFogDistance},
                                                                             {"resetSunColor", ResetSunColor},
                                                                             {"resetSunSize", ResetSunSize},
                                                                             {"resetMoonSize", ResetMoonSize},
                                                                             {"resetBlurLevel", ResetBlurLevel},
                                                                             {"resetWorldProperty", ArgumentParserWarn<false, ResetWorldProperty>},
                                                                             {"resetTimeFrozen", ArgumentParser<ResetTimeFrozen>},
                                                                             {"resetVolumetricShadows", ArgumentParser<ResetVolumetricShadows>},
                                                                             {"resetWorldProperties", ArgumentParser<ResetWorldProperties>},
                                                                             {"resetDynamicPedShadows", ArgumentParser<ResetDynamicPedShadows>},    
      
                                                                             // World check funcs
                                                                             {"areTrafficLightsLocked", AreTrafficLightsLocked},
                                                                             {"isPedTargetingMarkerEnabled", IsPedTargetingMarkerEnabled},
                                                                             {"isLineOfSightClear", IsLineOfSightClear},
                                                                             {"isWorldSpecialPropertyEnabled", ArgumentParserWarn<false, IsWorldSpecialPropertyEnabled>},
                                                                             {"isGarageOpen", IsGarageOpen},
                                                                             {"isTimeFrozen", ArgumentParser<IsTimeFrozen>},
                                                                             {"isVolumetricShadowsEnabled", ArgumentParser<IsVolumetricShadowsEnabled>},
                                                                             {"isDynamicPedShadowsEnabled", ArgumentParser<IsDynamicPedShadowsEnabled>},
                                                                             {"testSphereAgainstWorld", ArgumentParser<TestSphereAgainstWorld>}};

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

int CLuaWorldDefs::CreateExplosion(lua_State* luaVM)
{
    //  bool createExplosion ( float x, float y, float z, int type [, bool makeSound = true, float camShake = -1.0, bool damaging = true ] )
    CVector vecPosition;
    int     iType;
    bool    bMakeSound;
    float   fCamShake = -1.0;
    bool    bDamaging;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(iType);
    argStream.ReadBool(bMakeSound, true);
    argStream.ReadNumber(fCamShake, -1.0f);
    argStream.ReadBool(bDamaging, true);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::CreateExplosion(vecPosition, iType, bMakeSound, fCamShake, bDamaging))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetTime(lua_State* luaVM)
{
    // Get the time
    unsigned char ucHour, ucMinute;
    if (CStaticFunctionDefinitions::GetTime(ucHour, ucMinute))
    {
        // Return it
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucHour));
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucMinute));
        return 2;
    }

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetGroundPosition(lua_State* luaVM)
{
    //  float getGroundPosition ( float x, float y, float z )
    CVector vecStart;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecStart);

    if (!argStream.HasErrors())
    {
        // Get the ground position and return it
        float fGround = g_pGame->GetWorld()->FindGroundZFor3DPosition(&vecStart);
        lua_pushnumber(luaVM, fGround);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetRoofPosition(lua_State* luaVM)
{
    //  float getRoofPosition ( float x, float y, float z )
    CVector vecStart;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecStart);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    // Get the ground position and return it
    bool  bOutResult;
    float fRoof = g_pGame->GetWorld()->FindRoofZFor3DCoord(&vecStart, &bOutResult);
    if (bOutResult)
    {
        lua_pushnumber(luaVM, fRoof);
        return 1;
    }

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<bool, CLuaMultiReturn<bool, float, float, const char*, const char*, float, float, float>> CLuaWorldDefs::ProcessLineAgainstMesh(CClientEntity* e, CVector start, CVector end) {
    const auto ge = e->GetGameEntity();
    if (!ge) {
        // Element likely not streamed in, and such
        // Can't process it. This isn't an error per-se, thus we won't raise anything and treat this as a no-hit scenario
        return { false };
    }
    const SProcessLineOfSightMaterialInfoResult matInfo{g_pGame->GetWorld()->ProcessLineAgainstMesh(ge->GetInterface(), start, end)};
    if (!matInfo.valid) {
        return { false }; // No hit
    }
    return CLuaMultiReturn<bool, float, float, const char*, const char*, float, float, float>{
        true,

        matInfo.uv.fX,
        matInfo.uv.fY,

        matInfo.textureName,
        matInfo.frameName,

        matInfo.hitPos.fX,
        matInfo.hitPos.fY,
        matInfo.hitPos.fZ,
    };
}

int CLuaWorldDefs::ProcessLineOfSight(lua_State* L)
{
    //  bool float float float element float float float int int int processLineOfSight ( float startX, float startY, float startZ, float endX, float endY,
    //  float endZ,
    //      [ bool checkBuildings = true, bool checkVehicles = true, bool checkPlayers = true, bool checkObjects = true, bool checkDummies = true,
    //        bool seeThroughStuff = false, bool ignoreSomeObjectsForCamera = false, bool shootThroughStuff = false, element ignoredElement = nil, bool
    //        returnBuildingInfo = false, bCheckCarTires = false ] )
    CVector           vecStart;
    CVector           vecEnd;
    SLineOfSightFlags flags;
    CClientEntity*    pIgnoredElement;
    bool              bIncludeBuildingInfo;
    bool              bIncludeExtraMateriaInfo;

    CScriptArgReader argStream(L);
    argStream.ReadVector3D(vecStart);
    argStream.ReadVector3D(vecEnd);
    argStream.ReadBool(flags.bCheckBuildings, true);
    argStream.ReadBool(flags.bCheckVehicles, true);
    argStream.ReadBool(flags.bCheckPeds, true);
    argStream.ReadBool(flags.bCheckObjects, true);
    argStream.ReadBool(flags.bCheckDummies, true);
    argStream.ReadBool(flags.bSeeThroughStuff, false);
    argStream.ReadBool(flags.bIgnoreSomeObjectsForCamera, false);
    argStream.ReadBool(flags.bShootThroughStuff, false);
    argStream.ReadUserData(pIgnoredElement, NULL);
    argStream.ReadBool(bIncludeBuildingInfo, false);
    argStream.ReadBool(flags.bCheckCarTires, false);
    argStream.ReadBool(bIncludeExtraMateriaInfo, false);

    if (!argStream.HasErrors())
    {
        CEntity*                              pIgnoredEntity = pIgnoredElement ? pIgnoredElement->GetGameEntity() : NULL;
        CColPoint*                            pColPoint = NULL;
        CClientEntity*                        pColEntity = NULL;
        bool                                  bCollision;
        SLineOfSightBuildingResult            buildingResult;
        SProcessLineOfSightMaterialInfoResult matInfo;
        if (CStaticFunctionDefinitions::ProcessLineOfSight(vecStart, vecEnd, bCollision, &pColPoint, &pColEntity, flags, pIgnoredEntity,
                                                           bIncludeBuildingInfo ? &buildingResult : NULL, bIncludeExtraMateriaInfo ? &matInfo : nullptr))
        {
            // Got a collision?
            CVector vecColPosition;
            CVector vecColNormal;

            int   iMaterial = -1;
            float fLighting = -1;
            int   iPiece = -1;

            if (pColPoint)
            {
                // Get the collision position
                vecColPosition = pColPoint->GetPosition();
                vecColNormal = pColPoint->GetNormal();
                iMaterial = pColPoint->GetSurfaceTypeB();            // From test, only B function return relevant data
                fLighting = pColPoint->GetLightingForTimeOfDay();
                iPiece = pColPoint->GetPieceTypeB();

                // Delete the colpoint
                pColPoint->Destroy();
            }

            lua_pushboolean(L, bCollision);
            if (bCollision)
            {
                lua_pushnumber(L, vecColPosition.fX);
                lua_pushnumber(L, vecColPosition.fY);
                lua_pushnumber(L, vecColPosition.fZ);

                if (pColEntity)
                    lua_pushelement(L, pColEntity);
                else
                    lua_pushnil(L);

                lua_pushnumber(L, vecColNormal.fX);
                lua_pushnumber(L, vecColNormal.fY);
                lua_pushnumber(L, vecColNormal.fZ);

                lua_pushinteger(L, iMaterial);
                lua_pushnumber(L, fLighting);
                lua_pushinteger(L, iPiece);

                if (bIncludeBuildingInfo && buildingResult.bValid) // 8 args
                {
                    lua_pushnumber(L, buildingResult.usModelID);

                    lua_pushnumber(L, buildingResult.vecPosition.fX);
                    lua_pushnumber(L, buildingResult.vecPosition.fY);
                    lua_pushnumber(L, buildingResult.vecPosition.fZ);

                    lua_pushnumber(L, ConvertRadiansToDegrees(buildingResult.vecRotation.fX));
                    lua_pushnumber(L, ConvertRadiansToDegrees(buildingResult.vecRotation.fY));
                    lua_pushnumber(L, ConvertRadiansToDegrees(buildingResult.vecRotation.fZ));

                    lua_pushnumber(L, buildingResult.usLODModelID);
                } else {
                    for (auto i = 1 + 3 + 3 + 1; i-- > 0;) {
                        lua_pushnil(L);
                    }
                }

                if (bIncludeExtraMateriaInfo && matInfo.valid)  { // 7 args
                    lua::Push(L, matInfo.uv.fX);
                    lua::Push(L, matInfo.uv.fY);

                    lua::Push(L, matInfo.textureName);
                    lua::Push(L, matInfo.frameName);

                    lua::Push(L, matInfo.hitPos.fX);
                    lua::Push(L, matInfo.hitPos.fY);
                    lua::Push(L, matInfo.hitPos.fZ);
                } else {
                    for (auto i = 2 + 1 + 1 + 3; i-- > 0;) {
                        lua_pushnil(L);
                    }
                }

                return 11 + 8 + 7;
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(L, argStream.GetFullErrorMessage());

    lua_pushboolean(L, false);
    return 1;
}

int CLuaWorldDefs::IsLineOfSightClear(lua_State* luaVM)
{
    // bool isLineOfSightClear ( float startX,
    //    float startY,
    //    float startZ,
    //    float endX,
    //    float endY,
    //    float endZ,
    //    [ bool checkBuildings = true,
    //    bool checkVehicles = true,
    //    bool checkPeds = true,
    //    bool checkObjects = true,
    //    bool checkDummies = true,
    //    bool seeThroughStuff = false,
    //    bool ignoreSomeObjectsForCamera = false,
    //    element ignoredElement = nil ] )
    CVector           vecStart;
    CVector           vecEnd;
    SLineOfSightFlags flags;
    CClientEntity*    pIgnoredElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecStart);
    argStream.ReadVector3D(vecEnd);
    argStream.ReadBool(flags.bCheckBuildings, true);
    argStream.ReadBool(flags.bCheckVehicles, true);
    argStream.ReadBool(flags.bCheckPeds, true);
    argStream.ReadBool(flags.bCheckObjects, true);
    argStream.ReadBool(flags.bCheckDummies, true);
    argStream.ReadBool(flags.bSeeThroughStuff, false);
    argStream.ReadBool(flags.bIgnoreSomeObjectsForCamera, false);
    argStream.ReadUserData(pIgnoredElement, NULL);

    if (!argStream.HasErrors())
    {
        CEntity* pIgnoredEntity = pIgnoredElement ? pIgnoredElement->GetGameEntity() : NULL;
        bool     bIsClear;
        if (CStaticFunctionDefinitions::IsLineOfSightClear(vecStart, vecEnd, bIsClear, flags, pIgnoredEntity))
        {
            lua_pushboolean(luaVM, bIsClear);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetWorldFromScreenPosition(lua_State* luaVM)
{
    //  float, float, float getWorldFromScreenPosition ( float x, float y, float depth )
    CVector vecScreen;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecScreen);

    if (!argStream.HasErrors())
    {
        CVector vecWorld;
        if (CStaticFunctionDefinitions::GetWorldFromScreenPosition(vecScreen, vecWorld))
        {
            lua_pushnumber(luaVM, vecWorld.fX);
            lua_pushnumber(luaVM, vecWorld.fY);
            lua_pushnumber(luaVM, vecWorld.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetScreenFromWorldPosition(lua_State* luaVM)
{
    //  float float getScreenFromWorldPosition ( float x, float y, float z, [ float edgeTolerance=0, bool relative=true ] )
    CVector vecWorld;
    float   fEdgeTolerance;
    bool    bRelative;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecWorld);
    argStream.ReadNumber(fEdgeTolerance, 0);
    argStream.ReadBool(bRelative, true);

    if (!argStream.HasErrors())
    {
        CVector vecScreen;
        if (CStaticFunctionDefinitions::GetScreenFromWorldPosition(vecWorld, vecScreen, fEdgeTolerance, bRelative))
        {
            lua_pushnumber(luaVM, vecScreen.fX);
            lua_pushnumber(luaVM, vecScreen.fY);
            lua_pushnumber(luaVM, vecScreen.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetWeather(lua_State* luaVM)
{
    unsigned char ucWeather, ucWeatherBlendingTo;
    if (CStaticFunctionDefinitions::GetWeather(ucWeather, ucWeatherBlendingTo))
    {
        lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeather));

        if (ucWeatherBlendingTo != 0xFF)
            lua_pushnumber(luaVM, static_cast<lua_Number>(ucWeatherBlendingTo));
        else
            lua_pushnil(luaVM);

        return 2;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetZoneName(lua_State* luaVM)
{
    //  string getZoneName ( float x, float y, float z, [bool citiesonly=false] )
    CVector vecPosition;
    bool    bCitiesOnly;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadBool(bCitiesOnly, false);

    if (!argStream.HasErrors())
    {
        SString strZoneName;
        if (CStaticFunctionDefinitions::GetZoneName(vecPosition, strZoneName, bCitiesOnly))
        {
            lua_pushstring(luaVM, strZoneName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetGravity(lua_State* luaVM)
{
    float fGravity;
    if (CStaticFunctionDefinitions::GetGravity(fGravity))
    {
        lua_pushnumber(luaVM, fGravity);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetGameSpeed(lua_State* luaVM)
{
    float fSpeed;
    if (CStaticFunctionDefinitions::GetGameSpeed(fSpeed))
    {
        lua_pushnumber(luaVM, fSpeed);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetMinuteDuration(lua_State* luaVM)
{
    unsigned long ulDelay;
    if (CStaticFunctionDefinitions::GetMinuteDuration(ulDelay))
    {
        lua_pushnumber(luaVM, ulDelay);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetWaveHeight(lua_State* luaVM)
{
    float fHeight;
    if (CStaticFunctionDefinitions::GetWaveHeight(fHeight))
    {
        lua_pushnumber(luaVM, fHeight);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::IsGarageOpen(lua_State* luaVM)
{
    //  bool isGarageOpen ( int garageID )
    int iGarageID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGarageID);

    if (!argStream.HasErrors())
    {
        bool bIsOpen;

        if (CStaticFunctionDefinitions::IsGarageOpen(iGarageID, bIsOpen))
        {
            lua_pushboolean(luaVM, bIsOpen);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetGaragePosition(lua_State* luaVM)
{
    //  float, float, float getGaragePosition ( int garageID )
    int iGarageID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGarageID);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;

        if (CStaticFunctionDefinitions::GetGaragePosition(iGarageID, vecPosition))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetGarageSize(lua_State* luaVM)
{
    //  float, float, float getGarageSize ( int garageID )
    int iGarageID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGarageID);

    if (!argStream.HasErrors())
    {
        float fDepth;
        float fWidth;
        float fHeight;

        if (CStaticFunctionDefinitions::GetGarageSize(iGarageID, fHeight, fWidth, fDepth))
        {
            lua_pushnumber(luaVM, fHeight);
            lua_pushnumber(luaVM, fWidth);
            lua_pushnumber(luaVM, fDepth);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetGarageBoundingBox(lua_State* luaVM)
{
    //  float, float, float, float getGarageBoundingBox ( int garageID )
    int iGarageID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGarageID);

    if (!argStream.HasErrors())
    {
        float fLeft;
        float fRight;
        float fFront;
        float fBack;

        if (CStaticFunctionDefinitions::GetGarageBoundingBox(iGarageID, fLeft, fRight, fFront, fBack))
        {
            lua_pushnumber(luaVM, fLeft);
            lua_pushnumber(luaVM, fRight);
            lua_pushnumber(luaVM, fFront);
            lua_pushnumber(luaVM, fBack);
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetTrafficLightState(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pMultiplayer->GetTrafficLightState());
    return 1;
}

int CLuaWorldDefs::AreTrafficLightsLocked(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pMultiplayer->GetTrafficLightsLocked());
    return 1;
}

int CLuaWorldDefs::GetBlurLevel(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetBlurLevel());
    return 1;
}

int CLuaWorldDefs::SetBlurLevel(lua_State* luaVM)
{
    //  bool setBlurLevel ( int level )
    int iLevel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iLevel);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetBlurLevel(static_cast<unsigned char>(iLevel)))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetBlurLevel(lua_State* luaVM)
{
    g_pGame->GetSettings()->SetBlurControlledByScript(false);
    g_pGame->GetSettings()->ResetBlurEnabled();
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::SetTime(lua_State* luaVM)
{
    //  bool setTime ( int hour, int minute )
    int iHour;
    int iMinute;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iHour);
    argStream.ReadNumber(iMinute);

    if (!argStream.HasErrors())
    {
        // Set the new time
        if (CStaticFunctionDefinitions::SetTime(static_cast<unsigned char>(iHour), static_cast<unsigned char>(iMinute)))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetSkyGradient(lua_State* luaVM)
{
    unsigned char ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB;
    CStaticFunctionDefinitions::GetSkyGradient(ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB);

    lua_pushnumber(luaVM, ucTopR);
    lua_pushnumber(luaVM, ucTopG);
    lua_pushnumber(luaVM, ucTopB);
    lua_pushnumber(luaVM, ucBottomR);
    lua_pushnumber(luaVM, ucBottomG);
    lua_pushnumber(luaVM, ucBottomB);
    return 6;
}

int CLuaWorldDefs::SetSkyGradient(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    // Set the new sky gradient
    uchar ucTopRed, ucTopGreen, ucTopBlue;
    uchar ucBottomRed, ucBottomGreen, ucBottomBlue;

    argStream.ReadNumber(ucTopRed, 0);
    argStream.ReadNumber(ucTopGreen, 0);
    argStream.ReadNumber(ucTopBlue, 0);
    argStream.ReadNumber(ucBottomRed, 0);
    argStream.ReadNumber(ucBottomGreen, 0);
    argStream.ReadNumber(ucBottomBlue, 0);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetSkyGradient(ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetSkyGradient(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetSkyGradient())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetHeatHaze(lua_State* luaVM)
{
    SHeatHazeSettings settings;
    CStaticFunctionDefinitions::GetHeatHaze(settings);

    lua_pushnumber(luaVM, settings.ucIntensity);
    lua_pushnumber(luaVM, settings.ucRandomShift);
    lua_pushnumber(luaVM, settings.usSpeedMin);
    lua_pushnumber(luaVM, settings.usSpeedMax);
    lua_pushnumber(luaVM, settings.sScanSizeX);
    lua_pushnumber(luaVM, settings.sScanSizeY);
    lua_pushnumber(luaVM, settings.usRenderSizeX);
    lua_pushnumber(luaVM, settings.usRenderSizeY);
    lua_pushboolean(luaVM, settings.bInsideBuilding);
    return 9;
}

int CLuaWorldDefs::SetHeatHaze(lua_State* luaVM)
{
    CScriptArgReader argStream(luaVM);

    // Set the new heat haze settings
    SHeatHazeSettings heatHaze;
    argStream.ReadNumber(heatHaze.ucIntensity);
    argStream.ReadNumber(heatHaze.ucRandomShift, 0);
    argStream.ReadNumber(heatHaze.usSpeedMin, 12);
    argStream.ReadNumber(heatHaze.usSpeedMax, 18);
    argStream.ReadNumber(heatHaze.sScanSizeX, 75);
    argStream.ReadNumber(heatHaze.sScanSizeY, 80);
    argStream.ReadNumber(heatHaze.usRenderSizeX, 80);
    argStream.ReadNumber(heatHaze.usRenderSizeY, 85);
    argStream.ReadBool(heatHaze.bInsideBuilding, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetHeatHaze(heatHaze))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetHeatHaze(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::ResetHeatHaze())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetWeather(lua_State* luaVM)
{
    //  bool setWeather ( int weatherID )
    int iWeatherID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iWeatherID);

    if (!argStream.HasErrors())
    {
        // Set the new time
        if (CStaticFunctionDefinitions::SetWeather(static_cast<unsigned char>(iWeatherID)))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetWeatherBlended(lua_State* luaVM)
{
    //  bool setWeatherBlended ( int weatherID )
    int iWeatherID;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iWeatherID);

    if (!argStream.HasErrors())
    {
        // Set the new time
        if (CStaticFunctionDefinitions::SetWeatherBlended(static_cast<unsigned char>(iWeatherID)))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Return false
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetGravity(lua_State* luaVM)
{
    //  bool setGravity ( float level )
    float fGravity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fGravity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGravity(fGravity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetGameSpeed(lua_State* luaVM)
{
    //  bool setGameSpeed ( float value )
    float fSpeed;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fSpeed);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGameSpeed(fSpeed))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetMinuteDuration(lua_State* luaVM)
{
    //  bool setMinuteDuration ( int milliseconds )
    int iMilliseconds;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iMilliseconds);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMinuteDuration(iMilliseconds))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetWaveHeight(lua_State* luaVM)
{
    //  bool setWaveHeight ( float height )
    float fHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fHeight);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWaveHeight(fHeight))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetGarageOpen(lua_State* luaVM)
{
    //  bool setGarageOpen ( int garageID, bool open )
    int  iGarageID;
    bool bOpen;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iGarageID);
    argStream.ReadBool(bOpen);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetGarageOpen(iGarageID, bOpen))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetJetpackMaxHeight(lua_State* luaVM)
{
    //  bool setJetpackMaxHeight ( float Height )
    float fHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fHeight);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetJetpackMaxHeight(fHeight))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::RemoveWorldBuilding(lua_State* luaVM)
{
    int              iModelToRemove;
    CVector          vecPosition;
    float            fRadius = 0;
    char             cInterior = -1;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelToRemove);
    argStream.ReadNumber(fRadius);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(cInterior, -1);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                uint uiAmount;
                CStaticFunctionDefinitions::RemoveWorldBuilding(iModelToRemove, fRadius, vecPosition.fX, vecPosition.fY, vecPosition.fZ, cInterior, uiAmount);

                lua_pushboolean(luaVM, true);
                lua_pushnumber(luaVM, uiAmount);
                return 2;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::RestoreWorldBuildings(lua_State* luaVM)
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CResource* pResource = pLuaMain->GetResource();
        if (pResource)
        {
            uint uiAmount;
            CStaticFunctionDefinitions::RestoreWorldBuildings(uiAmount);

            lua_pushboolean(luaVM, true);
            lua_pushnumber(luaVM, uiAmount);
            return 2;
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::RestoreWorldBuilding(lua_State* luaVM)
{
    int              iModelToRestore;
    CVector          vecPosition;
    float            fRadius = 0;
    char             cInterior = -1;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iModelToRestore);
    argStream.ReadNumber(fRadius);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(cInterior, -1);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                uint uiAmount;
                CStaticFunctionDefinitions::RestoreWorldBuilding(iModelToRestore, fRadius, vecPosition.fX, vecPosition.fY, vecPosition.fZ, cInterior, uiAmount);

                lua_pushboolean(luaVM, true);
                lua_pushnumber(luaVM, uiAmount);
                return 2;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetAircraftMaxHeight(lua_State* luaVM)
{
    //  bool setAircraftMaxHeight ( float Height )
    float fHeight;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fHeight);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAircraftMaxHeight(fHeight))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetAircraftMaxVelocity(lua_State* luaVM)
{
    //  bool setAircraftMaxVelocity ( float fVelocity )
    float fVelocity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fVelocity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetAircraftMaxVelocity(fVelocity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetOcclusionsEnabled(lua_State* luaVM)
{
    //  bool setOcclusionsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetOcclusionsEnabled(bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaWorldDefs::IsWorldSpecialPropertyEnabled(const WorldSpecialProperty property) noexcept
{
    return m_pClientGame->IsWorldSpecialProperty(property);
}

bool CLuaWorldDefs::SetWorldSpecialPropertyEnabled(const WorldSpecialProperty property, const bool enabled) noexcept
{
    if (!m_pClientGame->SetWorldSpecialProperty(property, enabled))
        return false;

    if (!g_pNet->CanServerBitStream(eBitStreamVersion::WorldSpecialPropertyEvent))
        return true;

    if (auto stream = g_pNet->AllocateNetBitStream())
    {
        stream->WriteString(EnumToString(property));
        stream->WriteBit(enabled);
        g_pNet->SendPacket(PACKET_ID_PLAYER_WORLD_SPECIAL_PROPERTY, stream, PACKET_PRIORITY_HIGH, PACKET_RELIABILITY_RELIABLE_ORDERED);
        g_pNet->DeallocateNetBitStream(stream);
    }

    return true;
}

int CLuaWorldDefs::SetCloudsEnabled(lua_State* luaVM)
{
    //  bool setCloudsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCloudsEnabled(bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetJetpackMaxHeight(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetWorld()->GetJetpackMaxHeight());
    return 1;
}

int CLuaWorldDefs::GetAircraftMaxHeight(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetWorld()->GetAircraftMaxHeight());
    return 1;
}

int CLuaWorldDefs::GetAircraftMaxVelocity(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetWorld()->GetAircraftMaxVelocity());
    return 1;
}

int CLuaWorldDefs::GetOcclusionsEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pGame->GetWorld()->GetOcclusionsEnabled());
    return 1;
}

int CLuaWorldDefs::GetCloudsEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, CStaticFunctionDefinitions::GetCloudsEnabled());
    return 1;
}

int CLuaWorldDefs::SetTrafficLightState(lua_State* luaVM)
{
    //  bool setTrafficLightState ( int state )
    //  bool setTrafficLightState ( string state )
    //  bool setTrafficLightState ( string colorNS, string colorEW )

    CScriptArgReader argStream(luaVM);

    // Determine which version to parse
    if (argStream.NextIsNumber())
    {
        //  bool setTrafficLightState ( int state )
        int iState;
        argStream.ReadNumber(iState);

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetTrafficLightState(iState))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else if (!argStream.NextIsString(1))
    {
        //  bool setTrafficLightState ( string state )
        TrafficLight::EState eState;
        argStream.ReadEnumString(eState);

        if (!argStream.HasErrors())
        {
            if (eState == TrafficLight::AUTO)
            {
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked(false) && CStaticFunctionDefinitions::SetTrafficLightState(0);
                lua_pushboolean(luaVM, bOk);
                return 1;
            }
            else
            {
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked(true) && CStaticFunctionDefinitions::SetTrafficLightState(9);
                lua_pushboolean(luaVM, bOk);
                return 1;
            }
        }
    }
    else
    {
        //  bool setTrafficLightState ( string colorNS, string colorEW )
        TrafficLight::EColor eColorNS;
        TrafficLight::EColor eColorEW;
        argStream.ReadEnumString(eColorNS);
        argStream.ReadEnumString(eColorEW);

        if (!argStream.HasErrors())
        {
            unsigned char ucState = SharedUtil::GetTrafficLightStateFromColors(eColorNS, eColorEW);

            // Change it.
            bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked(true) && CStaticFunctionDefinitions::SetTrafficLightState(ucState);
            lua_pushboolean(luaVM, bOk);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetTrafficLightsLocked(lua_State* luaVM)
{
    //  bool setTrafficLightsLocked ( bool bLocked )
    bool bLocked;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bLocked);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetTrafficLightsLocked(bLocked))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetWindVelocity(lua_State* luaVM)
{
    float fX, fY, fZ;

    if (CStaticFunctionDefinitions::GetWindVelocity(fX, fY, fZ))
    {
        lua_pushnumber(luaVM, fX);
        lua_pushnumber(luaVM, fY);
        lua_pushnumber(luaVM, fZ);
        return 3;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetWindVelocity(lua_State* luaVM)
{
    //  bool setWindVelocity ( float velocityX, float velocityY, float velocityZ )
    CVector vecVelocity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecVelocity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetWindVelocity(vecVelocity.fX, vecVelocity.fY, vecVelocity.fZ))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetWindVelocity(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::RestoreWindVelocity())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetInteriorSoundsEnabled(lua_State* luaVM)
{
    lua_pushboolean(luaVM, g_pMultiplayer->GetInteriorSoundsEnabled());
    return 1;
}

int CLuaWorldDefs::SetInteriorSoundsEnabled(lua_State* luaVM)
{
    //  bool setInteriorSoundsEnabled ( bool enabled )
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetInteriorSoundsEnabled(bEnabled);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetInteriorFurnitureEnabled(lua_State* luaVM)
{
    //  bool getInteriorFurnitureEnabled ( int roomId )
    char cRoomId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(cRoomId);

    if (!argStream.HasErrors())
    {
        if (cRoomId >= 0 && cRoomId <= 4)
        {
            lua_pushboolean(luaVM, g_pMultiplayer->GetInteriorFurnitureEnabled(cRoomId));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaWorldDefs::SetInteriorFurnitureEnabled(lua_State* luaVM)
{
    //  bool setInteriorFurnitureEnabled ( int roomId, bool enabled )
    char cRoomId;
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(cRoomId);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (cRoomId >= 0 && cRoomId <= 4)
        {
            g_pMultiplayer->SetInteriorFurnitureEnabled(cRoomId, bEnabled);
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetRainLevel(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetWeather()->GetAmountOfRain());
    return 1;
}

int CLuaWorldDefs::SetRainLevel(lua_State* luaVM)
{
    //  bool setRainLevel ( float amount )
    float fRainLevel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fRainLevel);

    if (!argStream.HasErrors())
    {
        // Clamp amount of rain to avoid game freezing/crash
        fRainLevel = Clamp(0.0f, fRainLevel, 10.0f);

        g_pGame->GetWeather()->SetAmountOfRain(fRainLevel);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetRainLevel(lua_State* luaVM)
{
    g_pGame->GetWeather()->ResetAmountOfRain();

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetFarClipDistance(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pMultiplayer->GetFarClipDistance());
    return 1;
}

int CLuaWorldDefs::SetFarClipDistance(lua_State* luaVM)
{
    //  bool setFarClipDistance ( float distance )
    float fDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetFarClipDistance(fDistance);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetPedTargetingMarkerEnabled(lua_State* luaVM)
{
    //  bool setPedTargetingMarkerEnabled ( enabled )
    bool bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetPedTargetingMarkerEnabled(bEnabled);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::IsPedTargetingMarkerEnabled(lua_State* luaVM)
{
    //  bool isPedTargetingMarkerEnabled ( )
    lua_pushboolean(luaVM, g_pMultiplayer->IsPedTargetingMarkerEnabled());
    return 1;
}

int CLuaWorldDefs::ResetFarClipDistance(lua_State* luaVM)
{
    g_pMultiplayer->RestoreFarClipDistance();

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetNearClipDistance(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pMultiplayer->GetNearClipDistance());
    return 1;
}

int CLuaWorldDefs::SetNearClipDistance(lua_State* luaVM)
{
    //  bool setNearClipDistance ( float distance )
    float fDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetNearClipDistance(fDistance);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetNearClipDistance(lua_State* luaVM)
{
    g_pMultiplayer->RestoreNearClipDistance();

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetVehiclesLODDistance(lua_State* luaVM)
{
    //  float float getVehiclesLODDistance ( )
    float fVehiclesDistance, fTrainsPlanesDistance;

    g_pGame->GetSettings()->GetVehiclesLODDistance(fVehiclesDistance, fTrainsPlanesDistance);
    lua_pushnumber(luaVM, fVehiclesDistance);
    lua_pushnumber(luaVM, fTrainsPlanesDistance);
    return 2;
}

int CLuaWorldDefs::SetVehiclesLODDistance(lua_State* luaVM)
{
    //  bool setVehiclesLODDistance ( float vehiclesDistance, float trainsAndPlanesDistance = vehiclesDistance * 2.14 )
    float fVehiclesDistance, fTrainsPlanesDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fVehiclesDistance);
    fVehiclesDistance = Clamp(0.0f, fVehiclesDistance, 500.0f);

    // Default train distance is 2.14 times bigger than normal vehicles
    argStream.ReadNumber(fTrainsPlanesDistance, Clamp(0.0f, fVehiclesDistance * 2.14f, 500.0f));

    if (!argStream.HasErrors())
    {
        g_pGame->GetSettings()->SetVehiclesLODDistance(fVehiclesDistance, fTrainsPlanesDistance, true);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetVehiclesLODDistance(lua_State* luaVM)
{
    g_pGame->GetSettings()->ResetVehiclesLODDistance(true);
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetPedsLODDistance(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pGame->GetSettings()->GetPedsLODDistance());
    return 1;
}

int CLuaWorldDefs::SetPedsLODDistance(lua_State* luaVM)
{
    float fPedsDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fPedsDistance);

    if (!argStream.HasErrors())
    {
        fPedsDistance = Clamp(0.0f, fPedsDistance, 500.0f);
        g_pGame->GetSettings()->SetPedsLODDistance(fPedsDistance, true);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetPedsLODDistance(lua_State* luaVM)
{
    g_pGame->GetSettings()->ResetPedsLODDistance(true);
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetFogDistance(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pMultiplayer->GetFogDistance());
    return 1;
}

int CLuaWorldDefs::SetFogDistance(lua_State* luaVM)
{
    //  bool setFogDistance ( float distance )
    float fDistance;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fDistance);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetFogDistance(fDistance);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetFogDistance(lua_State* luaVM)
{
    g_pMultiplayer->RestoreFogDistance();

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetSunColor(lua_State* luaVM)
{
    unsigned char ucCoreRed, ucCoreGreen, ucCoreBlue, ucCoronaRed, ucCoronaGreen, ucCoronaBlue;

    g_pMultiplayer->GetSunColor(ucCoreRed, ucCoreGreen, ucCoreBlue, ucCoronaRed, ucCoronaGreen, ucCoronaBlue);

    lua_pushnumber(luaVM, ucCoreRed);
    lua_pushnumber(luaVM, ucCoreGreen);
    lua_pushnumber(luaVM, ucCoreBlue);
    lua_pushnumber(luaVM, ucCoronaRed);
    lua_pushnumber(luaVM, ucCoronaGreen);
    lua_pushnumber(luaVM, ucCoronaBlue);

    return 6;
}

int CLuaWorldDefs::SetSunColor(lua_State* luaVM)
{
    //  bool setSunColor ( int coreRed, int coreGreen, int coreBlue, int coronaRed, int coronaGreen, int coronaBlue )
    int iCoreRed;
    int iCoreGreen;
    int iCoreBlue;
    int iCoronaRed;
    int iCoronaGreen;
    int iCoronaBlue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iCoreRed);
    argStream.ReadNumber(iCoreGreen);
    argStream.ReadNumber(iCoreBlue);
    argStream.ReadNumber(iCoronaRed, iCoreRed);
    argStream.ReadNumber(iCoronaGreen, iCoreGreen);
    argStream.ReadNumber(iCoronaBlue, iCoreBlue);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetSunColor(iCoreRed, iCoreGreen, iCoreBlue, iCoronaRed, iCoronaGreen, iCoronaBlue);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::ResetSunColor(lua_State* luaVM)
{
    g_pMultiplayer->ResetSunColor();

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::GetSunSize(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pMultiplayer->GetSunSize());
    return 1;
}

int CLuaWorldDefs::SetSunSize(lua_State* luaVM)
{
    //  bool setSunSize ( float size )
    float fSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(fSize);

    if (!argStream.HasErrors())
    {
        g_pMultiplayer->SetSunSize(fSize);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::ResetSunSize(lua_State* luaVM)
{
    g_pMultiplayer->ResetSunSize();

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::CreateSWATRope(lua_State* luaVM)
{
    CVector          vecPosition;
    DWORD            dwDuration = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(dwDuration, 4000);

    if (!argStream.HasErrors())
    {
        // Returns a Rope ID?
        if (CStaticFunctionDefinitions::CreateSWATRope(vecPosition, dwDuration))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetBirdsEnabled(lua_State* luaVM)
{
    bool             bEnabled = false;
    CScriptArgReader argStream(luaVM);

    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetBirdsEnabled(bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetBirdsEnabled(lua_State* luaVM)
{
    if (CStaticFunctionDefinitions::GetBirdsEnabled())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::SetMoonSize(lua_State* luaVM)
{
    int iSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iSize);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetMoonSize(iSize))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetMoonSize(lua_State* luaVM)
{
    lua_pushnumber(luaVM, g_pMultiplayer->GetMoonSize());
    return 1;
}

int CLuaWorldDefs::ResetMoonSize(lua_State* luaVM)
{
    g_pMultiplayer->ResetMoonSize();
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaWorldDefs::SetFPSLimit(lua_State* luaVM)
{
    // bool setFPSLimit ( int fpsLimit )
    int iLimit;

    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(iLimit);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetFPSLimit(iLimit))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaWorldDefs::GetFPSLimit(lua_State* luaVM)
{
    int iLimit;
    if (CStaticFunctionDefinitions::GetFPSLimit(iLimit))
    {
        lua_pushnumber(luaVM, iLimit);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaWorldDefs::ResetColorFilter()
{
    g_pMultiplayer->ResetColorFilter();
    return true;
}

bool CLuaWorldDefs::SetColorFilter(uchar ucPass0Red, uchar ucPass0Green, uchar ucPass0Blue, uchar ucPass0Alpha, uchar ucPass1Red, uchar ucPass1Green,
                                   uchar ucPass1Blue, uchar ucPass1Alpha)
{
    unsigned long ulColor0 = COLOR_RGBA(ucPass0Red, ucPass0Green, ucPass0Blue, ucPass0Alpha);
    unsigned long ulColor1 = COLOR_RGBA(ucPass1Red, ucPass1Green, ucPass1Blue, ucPass1Alpha);
    g_pMultiplayer->SetColorFilter(ulColor0, ulColor1);
    return true;
}

CLuaMultiReturn<uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar> CLuaWorldDefs::GetColorFilter(bool isOriginal)
{
    SColor uColor0 = 0;
    SColor uColor1 = 0;
    g_pMultiplayer->GetColorFilter(uColor0.ulARGB, uColor1.ulARGB, isOriginal);
    return {
        uColor0.R, uColor0.G, uColor0.B, uColor0.A,
        uColor1.R, uColor1.G, uColor1.B, uColor1.A,
    };
}

bool CLuaWorldDefs::SetGrainMultiplier(eGrainMultiplierType type, float fMultiplier)
{
    g_pMultiplayer->SetGrainMultiplier(type, fMultiplier);
    return true;
}

bool CLuaWorldDefs::SetGrainLevel(uchar ucLevel)
{
    g_pMultiplayer->SetGrainLevel(ucLevel);
    return true;
}

bool CLuaWorldDefs::SetCoronaReflectionsEnabled(uchar ucEnabled)
{
    if (ucEnabled > 2)
        return false;

    g_pGame->GetSettings()->SetCoronaReflectionsControlledByScript(true);
    g_pGame->GetCoronas()->SetCoronaReflectionsEnabled(ucEnabled);
    return true;
}

uchar CLuaWorldDefs::GetCoronaReflectionsEnabled()
{
    return g_pGame->GetCoronas()->GetCoronaReflectionsEnabled();
}

bool CLuaWorldDefs::ResetCoronaReflectionsEnabled()
{
    g_pGame->GetSettings()->SetCoronaReflectionsControlledByScript(false);
    g_pGame->GetSettings()->ResetCoronaReflectionsEnabled();
    return true;
}

std::variant<bool, float, CLuaMultiReturn<float, float, float>> CLuaWorldDefs::GetWorldProperty(eWorldProperty property)
{
    switch (property)
    {
        case eWorldProperty::AMBIENT_COLOR:
        {
            float red, green, blue;
            g_pMultiplayer->GetAmbientColor(red, green, blue);
            return std::make_tuple((int16)(red * 255), (int16)(green * 255), (int16)(blue * 255));
        }
        case eWorldProperty::AMBIENT_OBJ_COLOR:
        {
            float red, green, blue;
            g_pMultiplayer->GetAmbientObjectColor(red, green, blue);
            return std::make_tuple((int16)(red * 255), (int16)(green * 255), (int16)(blue * 255));
        }
        case eWorldProperty::DIRECTIONAL_COLOR:
        {
            float red, green, blue;
            g_pMultiplayer->GetDirectionalColor(red, green, blue);
            return std::make_tuple((int16)(red * 255), (int16)(green * 255), (int16)(blue * 255));
        }
        case eWorldProperty::SPRITE_SIZE:
            return g_pMultiplayer->GetSpriteSize();
        case eWorldProperty::SPRITE_BRIGHTNESS:
            return g_pMultiplayer->GetSpriteBrightness();
        case eWorldProperty::POLE_SHADOW_STRENGTH:
            return (float)g_pMultiplayer->GetPoleShadowStrength();
        case eWorldProperty::SHADOW_STRENGTH:
            return (float)g_pMultiplayer->GetShadowStrength();
        case eWorldProperty::SHADOWS_OFFSET:
            return g_pMultiplayer->GetShadowsOffset();
        case eWorldProperty::LIGHTS_ON_GROUND:
            return g_pMultiplayer->GetLightsOnGroundBrightness();
        case eWorldProperty::LOW_CLOUDS_COLOR:
        {
            int16 red, green, blue;
            g_pMultiplayer->GetLowCloudsColor(red, green, blue);
            return std::make_tuple(red, green, blue);
        }
        case eWorldProperty::BOTTOM_CLOUDS_COLOR:
        {
            int16 red, green, blue;
            g_pMultiplayer->GetBottomCloudsColor(red, green, blue);
            return std::make_tuple(red, green, blue);
        }
        case eWorldProperty::CLOUDS_ALPHA1:
            return g_pMultiplayer->GetCloudsAlpha1();
        case eWorldProperty::ILLUMINATION:
            return g_pMultiplayer->GetIllumination();
        case eWorldProperty::WEATHER_WET_ROADS:
            return g_pGame->GetWeather()->GetWetRoads();
        case eWorldProperty::WEATHER_FOGGYNESS:
            return g_pGame->GetWeather()->GetFoggyness();
        case eWorldProperty::WEATHER_FOG:
            return g_pGame->GetWeather()->GetFog();
        case eWorldProperty::WEATHER_RAIN_FOG:
            return g_pGame->GetWeather()->GetRainFog();
        case eWorldProperty::WEATHER_WATER_FOG:
            return g_pGame->GetWeather()->GetWaterFog();
        case eWorldProperty::WEATHER_SANDSTORM:
            return g_pGame->GetWeather()->GetSandstorm();
        case eWorldProperty::WEATHER_RAINBOW:
            return g_pGame->GetWeather()->GetRainbow();
    }
    return false;
}

bool CLuaWorldDefs::SetWorldProperty(eWorldProperty property, float arg1, std::optional<float> arg2, std::optional<float> arg3)
{
    if (arg2.has_value() && arg3.has_value())
    {
        switch (property)
        {
            case eWorldProperty::AMBIENT_COLOR:
                return g_pMultiplayer->SetAmbientColor(arg1 / 255, arg2.value() / 255, arg3.value() / 255);
            case eWorldProperty::AMBIENT_OBJ_COLOR:
                return g_pMultiplayer->SetAmbientObjectColor(arg1 / 255, arg2.value() / 255, arg3.value() / 255);
            case eWorldProperty::DIRECTIONAL_COLOR:
                return g_pMultiplayer->SetDirectionalColor(arg1 / 255, arg2.value() / 255, arg3.value() / 255);
            case eWorldProperty::LOW_CLOUDS_COLOR:
                return g_pMultiplayer->SetLowCloudsColor((int16)arg1, (int16)arg2.value(), (int16)arg3.value());
            case eWorldProperty::BOTTOM_CLOUDS_COLOR:
                return g_pMultiplayer->SetBottomCloudsColor((int16)arg1, (int16)arg2.value(), (int16)arg3.value());
        }
        return false;
    }
    switch (property)
    {
        case eWorldProperty::SPRITE_SIZE:
            return g_pMultiplayer->SetSpriteSize(arg1);
        case eWorldProperty::SPRITE_BRIGHTNESS:
            return g_pMultiplayer->SetSpriteBrightness(arg1);
        case eWorldProperty::POLE_SHADOW_STRENGTH:
            return g_pMultiplayer->SetPoleShadowStrength(arg1);
        case eWorldProperty::SHADOW_STRENGTH:
            return g_pMultiplayer->SetShadowStrength(arg1);
        case eWorldProperty::SHADOWS_OFFSET:
            return g_pMultiplayer->SetShadowsOffset(arg1);
        case eWorldProperty::LIGHTS_ON_GROUND:
            return g_pMultiplayer->SetLightsOnGroundBrightness(arg1);
        case eWorldProperty::CLOUDS_ALPHA1:
            return g_pMultiplayer->SetCloudsAlpha1(arg1);
        case eWorldProperty::ILLUMINATION:
            return g_pMultiplayer->SetIllumination(arg1);
        case eWorldProperty::WEATHER_WET_ROADS:
            return g_pGame->GetWeather()->SetWetRoads(arg1);
        case eWorldProperty::WEATHER_FOGGYNESS:
            return g_pGame->GetWeather()->SetFoggyness(arg1);
        case eWorldProperty::WEATHER_FOG:
            return g_pGame->GetWeather()->SetFog(arg1);
        case eWorldProperty::WEATHER_RAIN_FOG:
            return g_pGame->GetWeather()->SetRainFog(arg1);
        case eWorldProperty::WEATHER_WATER_FOG:
            return g_pGame->GetWeather()->SetWaterFog(arg1);
        case eWorldProperty::WEATHER_SANDSTORM:
            return g_pGame->GetWeather()->SetSandstorm(arg1);
        case eWorldProperty::WEATHER_RAINBOW:
            return g_pGame->GetWeather()->SetRainbow(arg1);
    }
    return false;
}

bool CLuaWorldDefs::ResetWorldProperty(eWorldProperty property)
{
    switch (property)
    {
        case eWorldProperty::AMBIENT_COLOR:
            return g_pMultiplayer->ResetAmbientColor();
        case eWorldProperty::AMBIENT_OBJ_COLOR:
            return g_pMultiplayer->ResetAmbientObjectColor();
        case eWorldProperty::DIRECTIONAL_COLOR:
            return g_pMultiplayer->ResetDirectionalColor();
        case eWorldProperty::SPRITE_SIZE:
            return g_pMultiplayer->ResetSpriteSize();
        case eWorldProperty::SPRITE_BRIGHTNESS:
            return g_pMultiplayer->ResetSpriteBrightness();
        case eWorldProperty::POLE_SHADOW_STRENGTH:
            return g_pMultiplayer->ResetPoleShadowStrength();
        case eWorldProperty::SHADOW_STRENGTH:
            return g_pMultiplayer->ResetShadowStrength();
        case eWorldProperty::SHADOWS_OFFSET:
            return g_pMultiplayer->ResetShadowsOffset();
        case eWorldProperty::LIGHTS_ON_GROUND:
            return g_pMultiplayer->ResetLightsOnGroundBrightness();
        case eWorldProperty::LOW_CLOUDS_COLOR:
            return g_pMultiplayer->ResetLowCloudsColor();
        case eWorldProperty::BOTTOM_CLOUDS_COLOR:
            return g_pMultiplayer->ResetBottomCloudsColor();
        case eWorldProperty::CLOUDS_ALPHA1:
            return g_pMultiplayer->ResetCloudsAlpha1();
        case eWorldProperty::ILLUMINATION:
            return g_pMultiplayer->ResetIllumination();
        case eWorldProperty::WEATHER_WET_ROADS:
            return g_pGame->GetWeather()->ResetWetRoads();
        case eWorldProperty::WEATHER_FOGGYNESS:
            return g_pGame->GetWeather()->ResetFoggyness();
        case eWorldProperty::WEATHER_FOG:
            return g_pGame->GetWeather()->ResetFog();
        case eWorldProperty::WEATHER_RAIN_FOG:
            return g_pGame->GetWeather()->ResetRainFog();
        case eWorldProperty::WEATHER_WATER_FOG:
            return g_pGame->GetWeather()->ResetWaterFog();
        case eWorldProperty::WEATHER_SANDSTORM:
            return g_pGame->GetWeather()->ResetSandstorm();
        case eWorldProperty::WEATHER_RAINBOW:
            return g_pGame->GetWeather()->ResetRainbow();
    }
    return false;
}

bool CLuaWorldDefs::SetTimeFrozen(bool value) noexcept
{
    return g_pGame->GetClock()->SetTimeFrozen(value);
}

bool CLuaWorldDefs::IsTimeFrozen() noexcept
{
    return g_pGame->GetClock()->IsTimeFrozen();
}

bool CLuaWorldDefs::ResetTimeFrozen() noexcept
{
    return g_pGame->GetClock()->ResetTimeFrozen();
}

void CLuaWorldDefs::RemoveGameWorld()
{
    // We do not want to remove scripted buildings
    // But we need remove them from the buildings pool for a bit...
    m_pBuildingManager->DestroyAllForABit();

    // This function makes buildings backup without scripted buildings
    g_pGame->RemoveGameWorld();

    // ... And restore here
    m_pBuildingManager->RestoreDestroyed();
}

void CLuaWorldDefs::RestoreGameWorld()
{
    // We want to restore the game buildings to the same positions as they were before the backup.
    // Remove scripted buildings for a bit
    m_pBuildingManager->DestroyAllForABit();

    g_pGame->RestoreGameWorld();

    // ... And restore here
    m_pBuildingManager->RestoreDestroyedSafe();
}

bool CLuaWorldDefs::SetVolumetricShadowsEnabled(bool enable) noexcept
{
    g_pGame->GetSettings()->SetVolumetricShadowsEnabled(enable);
    return true;
}

bool CLuaWorldDefs::IsVolumetricShadowsEnabled() noexcept
{
    return g_pGame->GetSettings()->IsVolumetricShadowsEnabled();
}

bool CLuaWorldDefs::ResetVolumetricShadows() noexcept
{
    return g_pGame->GetSettings()->ResetVolumetricShadows();
}

void CLuaWorldDefs::ResetWorldProperties(std::optional<bool> resetSpecialWorldProperties, std::optional<bool> resetWorldProperties, std::optional<bool> resetWeatherProperties, std::optional<bool> resetLODs, std::optional<bool> resetSounds) noexcept
{
    g_pClientGame->ResetWorldProperties(ResetWorldPropsInfo{resetSpecialWorldProperties.value_or(true), resetWorldProperties.value_or(true), resetWeatherProperties.value_or(true), resetLODs.value_or(true), resetSounds.value_or(true)});
}

bool CLuaWorldDefs::SetDynamicPedShadowsEnabled(bool enable)
{
    g_pGame->GetSettings()->SetDynamicPedShadowsEnabled(enable);
    return true;
}

bool CLuaWorldDefs::IsDynamicPedShadowsEnabled() noexcept
{
    return g_pGame->GetSettings()->IsDynamicPedShadowsEnabled();
}

bool CLuaWorldDefs::ResetDynamicPedShadows() noexcept
{
    return g_pGame->GetSettings()->ResetDynamicPedShadows();
}

CLuaMultiReturn<bool, CClientEntity*, int, float, float, float, float, float, float, int, eEntityType> CLuaWorldDefs::TestSphereAgainstWorld(CVector sphereCenter, float radius, std::optional<CClientEntity*> ignoredEntity, std::optional<bool> checkBuildings, std::optional<bool> checkVehicles, std::optional<bool> checkPeds, std::optional<bool> checkObjects, std::optional<bool> checkDummies, std::optional<bool> cameraIgnore)
{
    STestSphereAgainstWorldResult result;
    CClientEntity* collidedEntity = nullptr;

    CEntity* entity = g_pGame->GetWorld()->TestSphereAgainstWorld(sphereCenter, radius, ignoredEntity.has_value() ? ignoredEntity.value()->GetGameEntity() : nullptr, checkBuildings.value_or(true), checkVehicles.value_or(true), checkPeds.value_or(true), checkObjects.value_or(true), checkDummies.value_or(true), cameraIgnore.value_or(false), result);
    if (entity)
        collidedEntity = reinterpret_cast<CClientEntity*>(entity->GetStoredPointer());

    return {result.collisionDetected, collidedEntity, result.modelID, result.entityPosition.fX, result.entityPosition.fY, result.entityPosition.fZ, ConvertRadiansToDegrees(result.entityRotation.fX), ConvertRadiansToDegrees(result.entityRotation.fY), ConvertRadiansToDegrees(result.entityRotation.fZ), result.lodID, result.type};
}

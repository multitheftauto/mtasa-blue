/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaWorldDefs : public CLuaDefs
{
public:
    static void LoadFunctions();


    LUA_DECLARE(GetTime);
    LUA_DECLARE(GetGroundPosition);
    LUA_DECLARE(GetRoofPosition);
    static std::variant<bool, CLuaMultiReturn<bool, float, float, const char*, const char*, float, float, float>> ProcessLineAgainstMesh(CClientEntity* e, CVector start, CVector end);
    LUA_DECLARE(ProcessLineOfSight);
    LUA_DECLARE(IsLineOfSightClear);
    LUA_DECLARE(GetWorldFromScreenPosition);
    LUA_DECLARE(GetScreenFromWorldPosition);
    LUA_DECLARE(GetWeather);
    LUA_DECLARE(GetZoneName);
    LUA_DECLARE(GetGravity);
    LUA_DECLARE(GetGameSpeed);
    LUA_DECLARE(GetMinuteDuration);
    LUA_DECLARE(GetWaveHeight);
    LUA_DECLARE(IsGarageOpen);
    LUA_DECLARE(GetGaragePosition);
    LUA_DECLARE(GetGarageSize);
    LUA_DECLARE(GetGarageBoundingBox);
    static bool IsWorldSpecialPropertyEnabled(WorldSpecialProperty property);
    LUA_DECLARE(GetBlurLevel);
    LUA_DECLARE(GetTrafficLightState);
    LUA_DECLARE(AreTrafficLightsLocked);
    LUA_DECLARE(GetJetpackMaxHeight);
    LUA_DECLARE(GetAircraftMaxHeight);
    LUA_DECLARE(GetAircraftMaxVelocity);
    LUA_DECLARE(GetOcclusionsEnabled);

    LUA_DECLARE(SetTime);
    LUA_DECLARE(GetSkyGradient);
    LUA_DECLARE(SetSkyGradient);
    LUA_DECLARE(ResetSkyGradient);
    LUA_DECLARE(GetHeatHaze);
    LUA_DECLARE(SetHeatHaze);
    LUA_DECLARE(ResetHeatHaze);
    LUA_DECLARE(SetWeather);
    LUA_DECLARE(SetWeatherBlended);
    LUA_DECLARE(SetGravity);
    LUA_DECLARE(SetGameSpeed);
    LUA_DECLARE(SetMinuteDuration);
    LUA_DECLARE(SetWaveHeight);
    LUA_DECLARE(SetGarageOpen);
    static bool SetWorldSpecialPropertyEnabled(WorldSpecialProperty property, bool isEnabled);
    LUA_DECLARE(SetBlurLevel);
    LUA_DECLARE(ResetBlurLevel);
    LUA_DECLARE(SetJetpackMaxHeight);
    LUA_DECLARE(SetCloudsEnabled);
    LUA_DECLARE(GetCloudsEnabled);
    LUA_DECLARE(SetTrafficLightState);
    LUA_DECLARE(SetTrafficLightsLocked);
    LUA_DECLARE(GetWindVelocity);
    LUA_DECLARE(SetWindVelocity);
    LUA_DECLARE(ResetWindVelocity);
    LUA_DECLARE(GetInteriorSoundsEnabled);
    LUA_DECLARE(SetInteriorSoundsEnabled);
    LUA_DECLARE(GetInteriorFurnitureEnabled);
    LUA_DECLARE(SetInteriorFurnitureEnabled);
    LUA_DECLARE(GetRainLevel);
    LUA_DECLARE(SetRainLevel);
    LUA_DECLARE(ResetRainLevel);
    LUA_DECLARE(GetFarClipDistance);
    LUA_DECLARE(SetFarClipDistance);
    LUA_DECLARE(ResetFarClipDistance);
    LUA_DECLARE(GetNearClipDistance);
    LUA_DECLARE(SetNearClipDistance);
    LUA_DECLARE(ResetNearClipDistance);
    LUA_DECLARE(GetVehiclesLODDistance);
    LUA_DECLARE(SetVehiclesLODDistance);
    LUA_DECLARE(ResetVehiclesLODDistance);
    LUA_DECLARE(GetPedsLODDistance);
    LUA_DECLARE(SetPedsLODDistance);
    LUA_DECLARE(ResetPedsLODDistance);
    LUA_DECLARE(GetFogDistance);
    LUA_DECLARE(SetFogDistance);
    LUA_DECLARE(ResetFogDistance);
    LUA_DECLARE(GetSunColor);
    LUA_DECLARE(SetSunColor);
    LUA_DECLARE(ResetSunColor);
    LUA_DECLARE(GetSunSize);
    LUA_DECLARE(SetSunSize);
    LUA_DECLARE(ResetSunSize);
    LUA_DECLARE(RemoveWorldBuilding);
    LUA_DECLARE(RestoreWorldBuildings);
    LUA_DECLARE(RestoreWorldBuilding);
    LUA_DECLARE(SetAircraftMaxHeight);
    LUA_DECLARE(SetAircraftMaxVelocity);
    LUA_DECLARE(SetOcclusionsEnabled);
    LUA_DECLARE(CreateSWATRope);
    LUA_DECLARE(SetBirdsEnabled);
    LUA_DECLARE(GetBirdsEnabled);
    LUA_DECLARE(SetPedTargetingMarkerEnabled);
    LUA_DECLARE(IsPedTargetingMarkerEnabled);
    LUA_DECLARE(SetMoonSize);
    LUA_DECLARE(GetMoonSize);
    LUA_DECLARE(ResetMoonSize);
    LUA_DECLARE(SetFPSLimit);
    LUA_DECLARE(GetFPSLimit);

    LUA_DECLARE(CreateExplosion);

    static bool                                                                    ResetColorFilter();
    static bool                                                                    SetColorFilter(uchar ucPass0Red, uchar ucPass0Green, uchar ucPass0Blue, uchar ucPass0Alpha,
                                                                                                  uchar ucPass1Red, uchar ucPass1Green, uchar ucPass1Blue, uchar ucPass1Alpha);
    static CLuaMultiReturn<uchar, uchar, uchar, uchar, uchar, uchar, uchar, uchar> GetColorFilter(bool isOriginal = false);

    static bool SetGrainMultiplier(eGrainMultiplierType type, float fMultiplier);
    static bool SetGrainLevel(uchar ucLevel);

    static bool  SetCoronaReflectionsEnabled(uchar ucEnabled);
    static uchar GetCoronaReflectionsEnabled();
    static bool  ResetCoronaReflectionsEnabled();

    static std::variant<bool, float, CLuaMultiReturn<float, float, float>> GetWorldProperty(eWorldProperty property);
    static bool SetWorldProperty(eWorldProperty property, float arg1, std::optional<float> arg2, std::optional<float> arg3);
    static bool ResetWorldProperty(eWorldProperty property);

    static bool SetTimeFrozen(bool value) noexcept;
    static bool IsTimeFrozen() noexcept;
    static bool ResetTimeFrozen() noexcept;
    
    static bool SetVolumetricShadowsEnabled(bool enable) noexcept;
    static bool IsVolumetricShadowsEnabled() noexcept;
    static bool ResetVolumetricShadows() noexcept;
    
    static void ResetWorldProperties(std::optional<bool> resetSpecialWorldProperties, std::optional<bool> resetWorldProperties, std::optional<bool> resetWeatherProperties, std::optional<bool> resetLODs, std::optional<bool> resetSounds) noexcept;

    static bool SetDynamicPedShadowsEnabled(bool enable);
    static bool IsDynamicPedShadowsEnabled() noexcept;
    static bool ResetDynamicPedShadows() noexcept;

    static CLuaMultiReturn<bool, CClientEntity*, int, float, float, float, float, float, float, int, eEntityType> TestSphereAgainstWorld(CVector sphereCenter, float radius, std::optional<CClientEntity*> ignoredEntity, std::optional<bool> checkBuildings, std::optional<bool> checkVehicles, std::optional<bool> checkPeds, std::optional<bool> checkObjects, std::optional<bool> checkDummies, std::optional<bool> cameraIgnore);

};


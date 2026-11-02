/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaCameraDefs.cpp
 *  PURPOSE:     Lua camera function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaCameraDefs.h"
#include "CGame.h"
#include "CPlayer.h"
#include "CPlayerCamera.h"
#include "CameraScriptShared.h"
#include "packets/CLuaPacket.h"
#include <net/rpc_enums.h>

#define MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS "1.5.8-9.20979"

void CLuaCameraDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Get functions
        {"getCameraMatrix", ArgumentParserWarn<false, GetCameraMatrix>},
        {"getCameraTarget", ArgumentParserWarn<false, GetCameraTarget>},
        {"getCameraInterior", ArgumentParserWarn<false, GetCameraInterior>},

        // Set functions
        {"setCameraMatrix", ArgumentParserWarn<false, SetCameraMatrix>},
        {"setCameraTarget", ArgumentParserWarn<false, SetCameraTarget>},
        {"setCameraInterior", ArgumentParserWarn<false, SetCameraInterior>},
        {"fadeCamera", ArgumentParserWarn<false, FadeCamera>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::variant<CLuaMultiReturn<float, float, float, float, float, float, float, float>, bool> CLuaCameraDefs::GetCameraMatrix(CPlayer* player)
{
    //  float cameraX, float cameraY, float cameraZ, float targetX, float targetY, float targetZ, float roll, float fov getCameraMatrix ( player thePlayer )
    CPlayerCamera* camera = player->GetCamera();

    if (!camera)
        return false;

    CVector position;
    camera->GetPosition(position);

    CVector lookAt;
    camera->GetLookAt(lookAt);

    return CLuaMultiReturn<float, float, float, float, float, float, float, float>{position.fX, position.fY, position.fZ,       lookAt.fX,
                                                                                   lookAt.fY,   lookAt.fZ,   camera->GetRoll(), camera->GetFOV()};
}

std::variant<CElement*, bool> CLuaCameraDefs::GetCameraTarget(CPlayer* player)
{
    //  element getCameraTarget ( player thePlayer )
    CPlayerCamera* camera = player->GetCamera();

    if (!camera)
        return false;

    if (camera->GetMode() == CAMERAMODE_PLAYER)
    {
        if (CElement* target = camera->GetTarget())
            return target;
    }

    return false;
}

std::variant<std::uint8_t, bool> CLuaCameraDefs::GetCameraInterior(CPlayer* player)
{
    //  int getCameraInterior ( player thePlayer )
    CPlayerCamera* camera = player->GetCamera();

    if (!camera)
        return false;

    return static_cast<std::uint8_t>(camera->GetInterior());
}

bool CLuaCameraDefs::SetCameraMatrix(CElement* element, std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> lookAt,
                                     std::optional<float> roll, std::optional<float> fov)
{
    //  bool setCameraMatrix ( player thePlayer, float positionX, float positionY, float positionZ [, float lookAtX, float lookAtY, float lookAtZ, float roll =
    //  0, float fov = 70 ] )
    CVector position;
    CVector lookAtValue;
    bool    lookAtValid = false;

    if (auto* matrix = std::get_if<CLuaMatrix*>(&matrixOrPosition))
    {
        position = (*matrix)->GetPosition();
        lookAtValue = (*matrix)->GetRotation();
        lookAtValid = true;
    }
    else
    {
        position = std::get<CVector>(matrixOrPosition);

        if (lookAt)
        {
            lookAtValue = *lookAt;
            lookAtValid = true;
        }
    }

    if (!CameraScriptShared::IsFiniteVector(position))
        throw std::invalid_argument("Invalid matrix/position (values out of range)");

    if (lookAtValid && !CameraScriptShared::IsFiniteVector(lookAtValue))
        throw std::invalid_argument("Invalid lookAt (values out of range)");

    const float rollValue = CameraScriptShared::NormalizeRoll(roll.value_or(0.0f));

    float fovValue = fov.value_or(CameraScriptShared::kDefaultFOV);
    if (fovValue <= 0.0f || fovValue >= 180.0f)
        fovValue = CameraScriptShared::kDefaultFOV;

    if (element->CountChildren() && element->IsCallPropagationEnabled())
    {
        CElementListSnapshotRef children = element->GetChildrenListSnapshot();
        for (CElementListSnapshot::const_iterator iter = children->begin(); iter != children->end(); iter++)
            if (!(*iter)->IsBeingDeleted())
                SetCameraMatrix(*iter, position, lookAtValid ? std::optional<CVector>{lookAtValue} : std::nullopt, rollValue, fovValue);
    }

    if (!IS_PLAYER(element))
        return false;

    CPlayer*       player = static_cast<CPlayer*>(element);
    CPlayerCamera* camera = player->GetCamera();

    if (!camera)
        return false;

    camera->SetMode(CAMERAMODE_FIXED);

    if (lookAtValid)
        camera->SetMatrix(position, lookAtValue);
    else
        camera->SetPosition(position);

    CVector syncLookAt;
    if (lookAtValid)
        syncLookAt = lookAtValue;
    else
    {
        camera->GetLookAt(syncLookAt);

        if (!CameraScriptShared::IsFiniteVector(syncLookAt))
            syncLookAt = position + CVector(0.0f, 1.0f, 0.0f);
    }

    camera->SetRoll(rollValue);
    camera->SetFOV(fovValue);

    CBitStream bitStream;
    bitStream.pBitStream->Write(camera->GenerateSyncTimeContext());
    bitStream.pBitStream->Write(position.fX);
    bitStream.pBitStream->Write(position.fY);
    bitStream.pBitStream->Write(position.fZ);
    bitStream.pBitStream->Write(syncLookAt.fX);
    bitStream.pBitStream->Write(syncLookAt.fY);
    bitStream.pBitStream->Write(syncLookAt.fZ);

    if (rollValue != 0.0f || fovValue != CameraScriptShared::kDefaultFOV)
    {
        bitStream.pBitStream->Write(rollValue);
        bitStream.pBitStream->Write(fovValue);
    }

    player->Send(CLuaPacket(SET_CAMERA_MATRIX, *bitStream.pBitStream));

    return true;
}

bool CLuaCameraDefs::SetCameraTarget(lua_State* luaVM, CElement* element, std::optional<CElement*> target)
{
    //  bool setCameraTarget ( player thePlayer [, element target = nil ] )
    if (target && *target && (*target)->GetType() != CElement::PLAYER)
    {
        CLuaMain*  luaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        CResource* resource = luaMain ? luaMain->GetResource() : nullptr;

        if (resource && resource->GetMinServerRequirement() < MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS)
        {
#if MTASA_VERSION_TYPE >= VERSION_TYPE_UNTESTED
            throw std::invalid_argument(SString("<min_mta_version> section in the meta.xml is incorrect or missing (expected at least server %s because %s)",
                                                MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS, "target is not a player"));
#endif
        }
    }

    CElement* targetValue = target.value_or(nullptr);

    if (element->CountChildren() && element->IsCallPropagationEnabled())
    {
        CElementListSnapshotRef children = element->GetChildrenListSnapshot();
        for (CElementListSnapshot::const_iterator iter = children->begin(); iter != children->end(); iter++)
            if (!(*iter)->IsBeingDeleted())
                SetCameraTarget(luaVM, *iter, targetValue);
    }

    if (!IS_PLAYER(element))
        return false;

    CPlayer*       player = static_cast<CPlayer*>(element);
    CPlayerCamera* camera = player->GetCamera();

    if (!camera)
        return false;

    CElement* effectiveTarget = targetValue ? targetValue : player;

    switch (effectiveTarget->GetType())
    {
        case CElement::PLAYER:
        case CElement::PED:
        case CElement::VEHICLE:
        {
            camera->SetMode(CAMERAMODE_PLAYER);
            camera->SetTarget(effectiveTarget);
            camera->SetRoll(0.0f);
            camera->SetFOV(CameraScriptShared::kDefaultFOV);

            CBitStream bitStream;
            bitStream.pBitStream->Write(camera->GenerateSyncTimeContext());
            bitStream.pBitStream->Write(effectiveTarget->GetID());

            player->Send(CLuaPacket(SET_CAMERA_TARGET, *bitStream.pBitStream));

            return true;
        }
        default:
            return false;
    }
}

bool CLuaCameraDefs::SetCameraInterior(CElement* element, std::uint8_t interior)
{
    //  bool setCameraInterior ( player thePlayer, int interior )
    if (element->CountChildren() && element->IsCallPropagationEnabled())
    {
        CElementListSnapshotRef children = element->GetChildrenListSnapshot();
        for (CElementListSnapshot::const_iterator iter = children->begin(); iter != children->end(); iter++)
            if (!(*iter)->IsBeingDeleted())
                SetCameraInterior(*iter, interior);
    }

    if (!IS_PLAYER(element))
        return false;

    CPlayer*       player = static_cast<CPlayer*>(element);
    CPlayerCamera* camera = player->GetCamera();

    if (!camera)
        return false;

    if (camera->GetInterior() == interior)
        return false;

    camera->SetInterior(interior);

    CBitStream bitStream;
    bitStream.pBitStream->Write(interior);

    player->Send(CLuaPacket(SET_CAMERA_INTERIOR, *bitStream.pBitStream));

    return true;
}

bool CLuaCameraDefs::FadeCamera(CElement* element, bool fadeIn, std::optional<float> fadeTime, std::optional<std::uint8_t> red,
                                std::optional<std::uint8_t> green, std::optional<std::uint8_t> blue)
{
    //  bool fadeCamera ( player thePlayer, bool fadeIn, [ float timeToFade = 1.0, int red = 0, int green = 0, int blue = 0 ] )
    const float        fadeTimeValue = fadeTime.value_or(1.0f);
    const std::uint8_t redValue = red.value_or(0);
    const std::uint8_t greenValue = green.value_or(0);
    const std::uint8_t blueValue = blue.value_or(0);

    if (element->CountChildren() && element->IsCallPropagationEnabled())
    {
        CElementListSnapshotRef children = element->GetChildrenListSnapshot();
        for (CElementListSnapshot::const_iterator iter = children->begin(); iter != children->end(); iter++)
            if (!(*iter)->IsBeingDeleted())
                FadeCamera(*iter, fadeIn, fadeTimeValue, redValue, greenValue, blueValue);
    }

    if (!IS_PLAYER(element))
        return false;

    CPlayer* player = static_cast<CPlayer*>(element);

    CBitStream bitStream;
    bitStream.pBitStream->Write(static_cast<unsigned char>(fadeIn ? 1 : 0));
    bitStream.pBitStream->Write(fadeTimeValue);

    if (!fadeIn)
    {
        bitStream.pBitStream->Write(redValue);
        bitStream.pBitStream->Write(greenValue);
        bitStream.pBitStream->Write(blueValue);
    }

    player->Send(CLuaPacket(FADE_CAMERA, *bitStream.pBitStream));

    return true;
}

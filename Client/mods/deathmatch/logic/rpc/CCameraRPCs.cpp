/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CCameraRPCs.cpp
 *  PURPOSE:     Camera remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CCameraRPCs.h"

void CCameraRPCs::LoadFunctions()
{
    AddHandler(SET_CAMERA_MATRIX, SetCameraMatrix, "SetCameraMatrix");
    AddHandler(SET_CAMERA_TARGET, SetCameraTarget, "SetCameraTarget");
    AddHandler(SET_CAMERA_INTERIOR, SetCameraInterior, "SetCameraInterior");
    AddHandler(FADE_CAMERA, FadeCamera, "FadeCamera");
}

void CCameraRPCs::SetCameraMatrix(NetBitStreamInterface& bitStream)
{
    if (!m_pCamera)
        return;

    if (bitStream.Version() >= 0x5E)
    {
        uchar ucTimeContext;
        if (!bitStream.Read(ucTimeContext))
        {
            return;
        }
        m_pCamera->SetSyncTimeContext(ucTimeContext);
    }

    CVector vecPosition, vecLookAt;
    float   fRoll = 0.0f;
    float   fFOV = 70.0f;
    
    if (!bitStream.Read(vecPosition.fX) || !bitStream.Read(vecPosition.fY) || !bitStream.Read(vecPosition.fZ) || 
        !bitStream.Read(vecLookAt.fX) || !bitStream.Read(vecLookAt.fY) || !bitStream.Read(vecLookAt.fZ))
    {
        return; // Invalid data
    }

    bitStream.Read(fRoll);
    bitStream.Read(fFOV);

    // Validate float values to prevent potential issues
    if (!std::isfinite(fRoll) || !std::isfinite(fFOV) || 
        !std::isfinite(vecPosition.fX) || !std::isfinite(vecPosition.fY) || !std::isfinite(vecPosition.fZ) ||
        !std::isfinite(vecLookAt.fX) || !std::isfinite(vecLookAt.fY) || !std::isfinite(vecLookAt.fZ))
    {
        return; // Invalid float values (NaN, infinity, etc.)
    }

    // Validate camera pointer before use
    if (!m_pCamera)
        return;

    if (!m_pCamera->IsInFixedMode())
        m_pCamera->ToggleCameraFixedMode(true);

    // Put the camera there
    m_pCamera->SetPosition(vecPosition);
    m_pCamera->SetFixedTarget(vecLookAt, fRoll);
    m_pCamera->SetFOV(fFOV);
}

void CCameraRPCs::SetCameraTarget(NetBitStreamInterface& bitStream)
{
    if (!m_pCamera)
        return;

    if (bitStream.Version() >= 0x5E)
    {
        uchar ucTimeContext;
        if (!bitStream.Read(ucTimeContext))
        {
            return;
        }
        m_pCamera->SetSyncTimeContext(ucTimeContext);
    }

    ElementID targetID;
    if (!bitStream.Read(targetID))
        return;

    // Validate camera pointer
    if (!m_pCamera)
        return;

    CClientEntity* pEntity = CElementIDs::GetElement(targetID);
    if (!pEntity)
        return;

    // Check if entity is being deleted - critical memory safety check
    if (pEntity->IsBeingDeleted())
        return;

    switch (pEntity->GetType())
    {
        case CCLIENTPLAYER:
        {
            CClientPlayer* pPlayer = static_cast<CClientPlayer*>(pEntity);
            if (pPlayer->IsLocalPlayer())
            {
                // Return the focus to the local player
                m_pCamera->SetFocusToLocalPlayer();
            }
            else
            {
                // Put the focus on that player
                m_pCamera->SetFocus(pPlayer, MODE_CAM_ON_A_STRING, false);
            }
            break;
        }
        case CCLIENTPED:
        case CCLIENTVEHICLE:
        {
            m_pCamera->SetFocus(pEntity, MODE_CAM_ON_A_STRING, false);
            break;
        }
        default:
            // Invalid entity type for camera target
            return;
    }
}

void CCameraRPCs::SetCameraInterior(NetBitStreamInterface& bitStream)
{
    // Read out the camera interior
    unsigned char ucInterior;
    if (!bitStream.Read(ucInterior))
        return;

    // Validate game pointer before use
    if (g_pGame && g_pGame->GetWorld())
    {
        g_pGame->GetWorld()->SetCurrentArea(ucInterior);
    }
}

void CCameraRPCs::FadeCamera(NetBitStreamInterface& bitStream)
{
    unsigned char ucFadeIn;
    float         fFadeTime = 1.0f;
    
    if (!bitStream.Read(ucFadeIn) || !bitStream.Read(fFadeTime))
        return;

    // Validate pointers before use
    if (!m_pCamera || !g_pClientGame)
        return;

    g_pClientGame->SetInitiallyFadedOut(false);

    if (ucFadeIn)
    {
        m_pCamera->FadeIn(fFadeTime);
        
        // Validate game and HUD pointers
        if (g_pGame && g_pGame->GetHud())
        {
            g_pGame->GetHud()->SetComponentVisible(HUD_AREA_NAME, !g_pClientGame->GetHudAreaNameDisabled());
        }
    }
    else
    {
        unsigned char ucRed = 0;
        unsigned char ucGreen = 0;
        unsigned char ucBlue = 0;

        if (!bitStream.Read(ucRed) || !bitStream.Read(ucGreen) || !bitStream.Read(ucBlue))
            return;

        m_pCamera->FadeOut(fFadeTime, ucRed, ucGreen, ucBlue);
        
        // Validate game and HUD pointers
        if (g_pGame && g_pGame->GetHud())
        {
            g_pGame->GetHud()->SetComponentVisible(HUD_AREA_NAME, false);
        }
    }
}

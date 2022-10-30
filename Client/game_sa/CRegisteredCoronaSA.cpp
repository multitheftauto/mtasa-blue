/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRegisteredCoronaSA.cpp
 *  PURPOSE:     Registered corona entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CCoronasSA.h"
#include "CGameSA.h"
#include "CRegisteredCoronaSA.h"

extern CGameSA* pGame;

CRegisteredCoronaSA::CRegisteredCoronaSA(CRegisteredCoronaSAInterface* coronaInterface)
{
    internalInterface = coronaInterface;
}

CVector* CRegisteredCoronaSA::GetPosition()
{
    return &internalInterface->Coordinates;
}

void CRegisteredCoronaSA::SetPosition(CVector* vector)
{
    MemCpyFast(&internalInterface->Coordinates, vector, sizeof(CVector));
}

float CRegisteredCoronaSA::GetSize()
{
    return internalInterface->Size;
}

void CRegisteredCoronaSA::SetSize(float fSize)
{
    internalInterface->Size = fSize;
}

float CRegisteredCoronaSA::GetRange()
{
    return internalInterface->Range;
}

void CRegisteredCoronaSA::SetRange(float fRange)
{
    internalInterface->Range = fRange;
}

float CRegisteredCoronaSA::GetPullTowardsCamera()
{
    return internalInterface->PullTowardsCam;
}

void CRegisteredCoronaSA::SetPullTowardsCamera(float fPullTowardsCamera)
{
    internalInterface->PullTowardsCam = fPullTowardsCamera;
}

void CRegisteredCoronaSA::SetColor(BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha)
{
    internalInterface->Red = Red;
    internalInterface->Green = Green;
    internalInterface->Blue = Blue;
    internalInterface->Intensity = Alpha;
    internalInterface->FadedIntensity = Alpha;
}

void CRegisteredCoronaSA::SetTexture(RwTexture* texture)
{
    internalInterface->pTex = texture;
}

void CRegisteredCoronaSA::SetTexture(eCoronaType texture)
{
    CCoronasSA* coronas = ((CCoronasSA*)pGame->GetCoronas());
    RwTexture*  tex = coronas->GetTexture(texture);
    if (tex)
        internalInterface->pTex = tex;
}

/**
 * Lens flare
 * \todo Test what the types are
 */
BYTE CRegisteredCoronaSA::GetFlareType()
{
    return internalInterface->FlareType;
}

void CRegisteredCoronaSA::SetFlareType(BYTE fFlareType)
{
    internalInterface->FlareType = fFlareType;
}

void CRegisteredCoronaSA::SetReflectionType(BYTE reflectionType)
{
    internalInterface->ReflectionType = reflectionType;
}

DWORD CRegisteredCoronaSA::GetID()
{
    return ((DWORD)internalInterface - ARRAY_CORONAS) / sizeof(CRegisteredCoronaSAInterface);
}

void CRegisteredCoronaSA::Init(DWORD Identifier)
{
    internalInterface->Identifier = Identifier;
    this->SetSize(2.5f);
    internalInterface->bIsAttachedToEntity = false;
    internalInterface->pEntityAttachedTo = NULL;
    internalInterface->NormalAngle = 0.0f;
    internalInterface->Range = 450.0f;
    internalInterface->PullTowardsCam = 1.5f;
    internalInterface->Red = 255;
    internalInterface->Green = 55;
    internalInterface->Blue = 255;
    internalInterface->Intensity = 255;
    internalInterface->FadedIntensity = 255;
    internalInterface->FlareType = 0;
    internalInterface->ReflectionType = 0;
    internalInterface->JustCreated = 1;
    internalInterface->RegisteredThisFrame = 1;            // won't appear in-game without this
}

void CRegisteredCoronaSA::Refresh()
{
    internalInterface->JustCreated = 1;
}

void CRegisteredCoronaSA::Disable()
{
    internalInterface->Identifier = 0;
    // internalInterface->Intensity = 0; // wasn't working before?
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientVehicle.cpp
 *  PURPOSE:     Vehicle entity class
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CBike.h>
#include <game/CBikeHandlingEntry.h>
#include <game/CBoat.h>
#include <game/CBoatHandlingEntry.h>
#include <game/CCarEnterExit.h>
#include <game/CDoor.h>
#include <game/CFlyingHandlingEntry.h>
#include <game/CHandlingEntry.h>
#include <game/CHandlingManager.h>
#include <game/CStreaming.h>

using std::list;

extern CClientGame*            g_pClientGame;
std::set<const CClientEntity*> ms_AttachedVehiclesToIgnore;

// To hide the ugly "pointer truncation from DWORD* to unsigned long warning
#pragma warning(disable:4311)

// Maximum distance between current position and target position (for interpolation)
// before we disable interpolation and warp to the position instead
#define VEHICLE_INTERPOLATION_WARP_THRESHOLD            15
#define VEHICLE_INTERPOLATION_WARP_THRESHOLD_FOR_SPEED  10

CClientVehicle::CClientVehicle(CClientManager* pManager, ElementID ID, unsigned short usModel, unsigned char ucVariation, unsigned char ucVariation2)
    : ClassInit(this), CClientStreamElement(pManager->GetVehicleStreamer(), ID)
{
    CClientEntityRefManager::AddEntityRefs(ENTITY_REF_DEBUG(this, "CClientVehicle"), &m_pDriver, &m_pOccupyingDriver, &m_pPreviousLink, &m_pNextLink,
                                           &m_pTowedVehicle, &m_pTowedByVehicle, &m_pPickedUpWinchEntity, NULL);

    // Initialize members
    m_pManager = pManager;
    m_pObjectManager = m_pManager->GetObjectManager();
    m_pVehicleManager = pManager->GetVehicleManager();
    m_pModelRequester = pManager->GetModelRequestManager();
    m_usModel = usModel;
    m_eVehicleType = CClientVehicleManager::GetVehicleType(usModel);
    m_bHasDamageModel = CClientVehicleManager::HasDamageModel(m_eVehicleType);
    m_pVehicle = NULL;
    m_pUpgrades = new CVehicleUpgrades(this);
    m_pClump = NULL;

    // Grab the model info
    m_pModelInfo = g_pGame->GetModelInfo(usModel);

    // Apply handling
    ushort usHandlingModelID = m_usModel;
    if (m_usModel < 400 || m_usModel > 611)
        usHandlingModelID = m_pModelInfo->GetParentID();

    m_pOriginalHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData(static_cast<eVehicleTypes>(usHandlingModelID));
    m_pHandlingEntry = g_pGame->GetHandlingManager()->CreateHandlingData();
    m_pHandlingEntry->Assign(m_pOriginalHandlingEntry);

    m_pOriginalFlyingHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalFlyingHandlingData(static_cast<eVehicleTypes>(usHandlingModelID));
    m_pFlyingHandlingEntry = g_pGame->GetHandlingManager()->CreateFlyingHandlingData();
    m_pFlyingHandlingEntry->Assign(m_pOriginalFlyingHandlingEntry);

    m_pOriginalBoatHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalBoatHandlingData(static_cast<eVehicleTypes>(usHandlingModelID));
    if (m_pOriginalBoatHandlingEntry)
    {
        m_pBoatHandlingEntry = g_pGame->GetHandlingManager()->CreateBoatHandlingData();
        m_pBoatHandlingEntry->Assign(m_pOriginalBoatHandlingEntry);
    }

    m_pOriginalBikeHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalBikeHandlingData(static_cast<eVehicleTypes>(usHandlingModelID));
    if (m_pOriginalBikeHandlingEntry)
    {
        m_pBikeHandlingEntry = g_pGame->GetHandlingManager()->CreateBikeHandlingData();
        m_pBikeHandlingEntry->Assign(m_pOriginalBikeHandlingEntry);
    }

    SetTypeName("vehicle");

    m_ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount(usModel);

    // Set our default properties
    m_pDriver = NULL;
    m_pOccupyingDriver = NULL;
    memset(&m_pPassengers[0], 0, sizeof(m_pPassengers));
    memset(&m_pOccupyingPassengers[0], 0, sizeof(m_pOccupyingPassengers));
    m_pPreviousLink = NULL;
    m_pNextLink = NULL;
    m_Matrix.vFront.fY = 1.0f;
    m_Matrix.vUp.fZ = 1.0f;
    m_Matrix.vRight.fX = 1.0f;
    m_dLastRotationTime = 0;
    m_fHealth = DEFAULT_VEHICLE_HEALTH;
    m_fTurretHorizontal = 0.0f;
    m_fTurretVertical = 0.0f;
    m_fGasPedal = 0.0f;
    m_bVisible = true;
    m_bIsCollisionEnabled = true;
    m_bEngineOn = false;
    m_bEngineBroken = false;
    m_bSireneOrAlarmActive = false;
    m_bLandingGearDown = true;
    m_usAdjustablePropertyValue = 0;
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_bAllowDoorRatioSetting[i] = true;
        m_fDoorOpenRatio[i] = 0.0f;
        m_doorInterp.fStart[i] = 0.0f;
        m_doorInterp.fTarget[i] = 0.0f;
        m_doorInterp.ulStartTime[i] = 0UL;
        m_doorInterp.ulTargetTime[i] = 0UL;
    }
    m_bSwingingDoorsAllowed = false;
    m_bDoorsLocked = false;
    m_bDoorsUndamageable = false;
    m_bCanShootPetrolTank = true;
    m_bCanBeTargettedByHeatSeekingMissiles = true;
    m_bColorSaved = false;
    m_bIsFrozen = false;
    m_bScriptFrozen = false;
    m_bFrozenWaitingForGroundToLoad = false;
    m_fGroundCheckTolerance = 0.f;
    m_fObjectsAroundTolerance = 0.f;
    GetInitialDoorStates(m_ucDoorStates);
    memset(&m_ucWheelStates[0], 0, sizeof(m_ucWheelStates));
    memset(&m_ucPanelStates[0], 0, sizeof(m_ucPanelStates));
    memset(&m_ucLightStates[0], 0, sizeof(m_ucLightStates));
    m_bCanBeDamaged = true;
    m_bSyncUnoccupiedDamage = false;
    m_bScriptCanBeDamaged = true;
    m_bTyresCanBurst = true;
    m_ucOverrideLights = 0;
    m_pTowedVehicle = NULL;
    m_pTowedByVehicle = NULL;
    m_eWinchType = WINCH_NONE;
    m_pPickedUpWinchEntity = NULL;
    m_ucPaintjob = 3;
    m_fDirtLevel = 0.0f;
    m_bSmokeTrail = false;
    m_bJustBlewUp = false;
    m_ucAlpha = 255;
    m_bAlphaChanged = false;
    m_blowAfterStreamIn = false;
    m_bIsOnGround = false;
    m_ulIllegalTowBreakTime = 0;
    m_LastSyncedData = new SLastSyncedVehData;
    m_bIsDerailed = false;
    m_bIsDerailable = true;
    m_bTrainDirection = false;
    m_fTrainSpeed = 0.0f;
    m_fTrainPosition = -1.0f;
    m_ucTrackID = 0xFF;
    m_bChainEngine = false;
    m_bTaxiLightOn = false;
    m_vecGravity = CVector(0.0f, 0.0f, -1.0f);
    m_HeadLightColor = SColorRGBA(255, 255, 255, 255);
    m_bHeliSearchLightVisible = false;
    m_fHeliRotorSpeed = 0.0f;
    m_fPlaneRotorSpeed = 0.0f;
    m_bHasCustomHandling = false;
    m_ucVariation = ucVariation;
    m_ucVariation2 = ucVariation2;
    m_bEnableHeliBladeCollisions = true;
    m_fNitroLevel = 1.0f;
    m_cNitroCount = 0;
    m_fWheelScale = 1.0f;

    for (unsigned int i = 0; i < MAX_WINDOWS; ++i)
    {
        m_bWindowOpen[i] = false;
    }

#ifdef MTA_DEBUG
    m_pLastSyncer = NULL;
    m_ulLastSyncTime = 0;
    m_szLastSyncType = "none";
#endif

    m_interp.rot.ulFinishTime = 0;
    m_interp.pos.ulFinishTime = 0;
    ResetInterpolation();

    // Check if we have landing gears
    m_bHasLandingGear = DoCheckHasLandingGear();
    m_bHasAdjustableProperty = CClientVehicleManager::HasAdjustableProperty(m_usModel);

    // Add this vehicle to the vehicle list
    m_pVehicleManager->AddToList(this);
    m_tSirenBeaconInfo.m_bSirenSilent = false;

    // clear our component data to regenerate
    m_ComponentData.clear();

    // Prepare the sirens
    RemoveVehicleSirens();

    // reset our fall through map count
    m_ucFellThroughMapCount = 1;

    // We've not yet been streamed in
    m_bJustStreamedIn = false;

    // We've not changed the wheel scale
    m_bWheelScaleChanged = false;
    m_clientModel = pManager->GetModelManager()->FindModelByID(usModel);
}

CClientVehicle::~CClientVehicle()
{
    // Unreference us
    m_pManager->UnreferenceEntity(this);

    // Unlink any towing attachments
    SetTowedVehicle(NULL);
    if (m_pTowedByVehicle)
        m_pTowedByVehicle->SetTowedVehicle(NULL);

    SetNextTrainCarriage(NULL);
    SetPreviousTrainCarriage(NULL);

    AttachTo(NULL);

    // Remove all our projectiles
    RemoveAllProjectiles();

    // Destroy the vehicle
    Destroy();

    // Make sure we haven't requested any model that will make us crash
    // when it's done loading.
    m_pModelRequester->Cancel(this, false);

    // Unreference us from the driving player model (if any)
    if (m_pDriver)
    {
        m_pDriver->SetVehicleInOutState(VEHICLE_INOUT_NONE);
        UnpairPedAndVehicle(m_pDriver, this);
    }

    // And the occupying ones eventually
    if (m_pOccupyingDriver)
    {
        m_pOccupyingDriver->SetVehicleInOutState(VEHICLE_INOUT_NONE);
        UnpairPedAndVehicle(m_pOccupyingDriver, this);
    }

    // And the passenger models
    int i;
    for (i = 0; i < (sizeof(m_pPassengers) / sizeof(CClientPed*)); i++)
    {
        if (m_pPassengers[i])
        {
            m_pPassengers[i]->m_uiOccupiedVehicleSeat = 0;
            m_pPassengers[i]->SetVehicleInOutState(VEHICLE_INOUT_NONE);
            UnpairPedAndVehicle(m_pPassengers[i], this);
        }
    }

    // Occupying passenger models
    for (i = 0; i < (sizeof(m_pOccupyingPassengers) / sizeof(CClientPed*)); i++)
    {
        if (m_pOccupyingPassengers[i])
        {
            m_pOccupyingPassengers[i]->m_uiOccupiedVehicleSeat = 0;
            m_pOccupyingPassengers[i]->SetVehicleInOutState(VEHICLE_INOUT_NONE);
            UnpairPedAndVehicle(m_pOccupyingPassengers[i], this);
        }
    }

    // Remove us from the vehicle list
    Unlink();

    delete m_pUpgrades;
    delete m_pHandlingEntry;
    delete m_pFlyingHandlingEntry;
    delete m_pBoatHandlingEntry;
    delete m_pBikeHandlingEntry;
    delete m_LastSyncedData;
    CClientEntityRefManager::RemoveEntityRefs(0, &m_pDriver, &m_pOccupyingDriver, &m_pPreviousLink, &m_pNextLink, &m_pTowedVehicle, &m_pTowedByVehicle,
                                              &m_pPickedUpWinchEntity, NULL);
    m_clientModel = nullptr;
}

void CClientVehicle::Unlink()
{
    m_pVehicleManager->RemoveFromLists(this);
}

void CClientVehicle::GetPosition(CVector& vecPosition) const
{
    if (m_bIsFrozen)
    {
        vecPosition = m_matFrozen.vPos;
    }
    // Is this a trailer being towed?
    else if (m_pTowedByVehicle)
    {
        // Is it streamed out or not attached properly?
        if (!m_pVehicle || !m_pVehicle->GetTowedByVehicle())
        {
            // Grab the position behind the vehicle (should take X/Y rotation into acount)
            // Prevent infinte recursion by ignoring attach link back to this towed vehicle (during GetPosition call)
            MapInsert(ms_AttachedVehiclesToIgnore, this);
            m_pTowedByVehicle->GetPosition(vecPosition);
            MapRemove(ms_AttachedVehiclesToIgnore, this);

            CVector vecRotation;
            m_pTowedByVehicle->GetRotationRadians(vecRotation);
            vecPosition.fX -= (5.0f * sin(vecRotation.fZ));
            vecPosition.fY -= (5.0f * cos(vecRotation.fZ));
        }
        else
        {
            vecPosition = *m_pVehicle->GetPosition();
        }
    }
    // Streamed in?
    else if (m_pVehicle)
    {
        vecPosition = *m_pVehicle->GetPosition();
    }
    // Attached to something?
    else if (m_pAttachedToEntity && !MapContains(ms_AttachedVehiclesToIgnore, m_pAttachedToEntity))
    {
        m_pAttachedToEntity->GetPosition(vecPosition);
        vecPosition += m_vecAttachedPosition;
    }
    else
    {
        vecPosition = m_Matrix.vPos;
    }
}

void CClientVehicle::SetPosition(const CVector& vecPosition, bool bResetInterpolation, bool bAllowGroundLoadFreeze)
{
    // Is the local player in the vehicle
    if (g_pClientGame->GetLocalPlayer() && g_pClientGame->GetLocalPlayer()->GetOccupiedVehicle() == this)
    {
        // If move is big enough, do ground checks
        float DistanceMoved = (m_Matrix.vPos - vecPosition).Length();
        if (DistanceMoved > 50 && !IsFrozen() && bAllowGroundLoadFreeze)
            SetFrozenWaitingForGroundToLoad(true, true);
    }

    if (m_pVehicle)
    {
        m_pVehicle->SetPosition(const_cast<CVector*>(&vecPosition));

        // Jax: can someone find a cleaner alternative for this, it fixes vehicles not being affected by gravity (supposed to be a flag used only on creation,
        // but isnt)
        if (!m_pDriver)
        {
            CVector vecMoveSpeed;
            m_pVehicle->GetMoveSpeed(&vecMoveSpeed);
            if (vecMoveSpeed.fX == 0.0f && vecMoveSpeed.fY == 0.0f && vecMoveSpeed.fZ == 0.0f)
            {
                vecMoveSpeed.fZ -= 0.01f;
                m_pVehicle->SetMoveSpeed(&vecMoveSpeed);
            }
        }
    }
    // Have we moved to a different position?
    if (m_Matrix.vPos != vecPosition)
    {
        // Store our new position
        m_Matrix.vPos = vecPosition;
        m_matFrozen.vPos = vecPosition;

        // Update our streaming position
        UpdateStreamPosition(vecPosition);
    }

    // If we have any occupants, update their positions
    // Make sure we dont update their position if they are getting out and have physically left the car
    for (int i = 0; i <= NUMELMS(m_pPassengers); i++)
        if (CClientPed* pOccupant = GetOccupant(i))
            if (pOccupant->GetVehicleInOutState() != VEHICLE_INOUT_GETTING_OUT || pOccupant->GetRealOccupiedVehicle())
                pOccupant->SetPosition(vecPosition);

    // Reset interpolation
    if (bResetInterpolation)
    {
        RemoveTargetPosition();

        // Tell GTA it should update the train rail position (set this here since we don't want to call this on interpolation)
        if (GetVehicleType() == CLIENTVEHICLE_TRAIN && !IsDerailed() && !IsStreamedIn())
        {
            m_fTrainPosition = -1.0f;
            m_ucTrackID = 0xFF;
        }
    }
}

void CClientVehicle::UpdatePedPositions(const CVector& vecPosition)
{
    // Have we moved to a different position?
    if (m_Matrix.vPos != vecPosition)
    {
        // Store our new position
        m_Matrix.vPos = vecPosition;
        m_matFrozen.vPos = vecPosition;

        // Update our streaming position
        UpdateStreamPosition(vecPosition);
    }

    // If we have any occupants, update their positions
    for (int i = 0; i <= NUMELMS(m_pPassengers); i++)
        if (CClientPed* pOccupant = GetOccupant(i))
            pOccupant->SetPosition(vecPosition);
}

void CClientVehicle::GetRotationDegrees(CVector& vecRotation) const
{
    // Grab our rotations in radians
    GetRotationRadians(vecRotation);
    ConvertRadiansToDegrees(vecRotation);
}

void CClientVehicle::GetRotationRadians(CVector& vecRotation) const
{
    // Grab the rotation in radians from the matrix
    CMatrix matTemp;
    GetMatrix(matTemp);
    g_pMultiplayer->ConvertMatrixToEulerAngles(matTemp, vecRotation.fX, vecRotation.fY, vecRotation.fZ);

    // ChrML: We flip the actual rotation direction so that the rotation is consistent with
    //        objects and players.
    vecRotation.fX = (2 * PI) - vecRotation.fX;
    vecRotation.fY = (2 * PI) - vecRotation.fY;
    vecRotation.fZ = (2 * PI) - vecRotation.fZ;
}

void CClientVehicle::SetRotationDegrees(const CVector& vecRotation, bool bResetInterpolation)
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fY = vecRotation.fY * 3.1415926535897932384626433832795f / 180.0f;
    vecTemp.fZ = vecRotation.fZ * 3.1415926535897932384626433832795f / 180.0f;

    // Set the rotation as radians
    SetRotationRadians(vecTemp, bResetInterpolation);
}

void CClientVehicle::SetRotationRadians(const CVector& vecRotation, bool bResetInterpolation)
{
    // Grab the matrix, apply the rotation to it and set it again
    // ChrML: We flip the actual rotation direction so that the rotation is consistent with
    //        objects and players.
    CMatrix matTemp;
    GetMatrix(matTemp);
    g_pMultiplayer->ConvertEulerAnglesToMatrix(matTemp, (2 * PI) - vecRotation.fX, (2 * PI) - vecRotation.fY, (2 * PI) - vecRotation.fZ);
    SetMatrix(matTemp);

    // Reset target rotation
    if (bResetInterpolation)
        RemoveTargetRotation();
}

void CClientVehicle::ReportMissionAudioEvent(unsigned short usSound)
{
    if (m_pVehicle)
    {
        //      g_pGame->GetAudio ()->ReportMissionAudioEvent ( m_pVehicle, usSound );
    }
}

bool CClientVehicle::SetTaxiLightOn(bool bLightOn)
{
    m_bTaxiLightOn = bLightOn;
    if (m_pVehicle)
    {
        m_pVehicle->SetTaxiLightOn(bLightOn);
        return true;
    }
    return false;
}

float CClientVehicle::GetDistanceFromCentreOfMassToBaseOfModel()
{
    return m_pVehicle ? m_pVehicle->GetDistanceFromCentreOfMassToBaseOfModel() : 0.0f;
}

bool CClientVehicle::GetMatrix(CMatrix& Matrix) const
{
    if (m_bIsFrozen)
    {
        Matrix = m_matFrozen;
    }
    else
    {
        if (m_pVehicle)
        {
            m_pVehicle->GetMatrix(&Matrix);
        }
        else
        {
            Matrix = m_Matrix;
        }
    }

    return true;
}

bool CClientVehicle::SetMatrix(const CMatrix& Matrix)
{
    if (m_pVehicle)
    {
        m_pVehicle->SetMatrix(const_cast<CMatrix*>(&Matrix));

        // If it is a boat, we need to call FixBoatOrientation or it won't move/rotate properly
        if (m_eVehicleType == CLIENTVEHICLE_BOAT)
            m_pVehicle->FixBoatOrientation();
    }

    // Have we moved to a different position?
    if (m_Matrix.vPos != Matrix.vPos)
    {
        // Update our streaming position
        UpdateStreamPosition(Matrix.vPos);
    }

    m_Matrix = Matrix;
    m_matFrozen = Matrix;

    // If we have any occupants, update their positions
    // Make sure we dont update their position if they are getting out and have physically left the car
    for (int i = 0; i <= NUMELMS(m_pPassengers); i++)
        if (CClientPed* pOccupant = GetOccupant(i))
            if (pOccupant->GetVehicleInOutState() != VEHICLE_INOUT_GETTING_OUT || pOccupant->GetRealOccupiedVehicle())
                pOccupant->SetPosition(m_Matrix.vPos);

    return true;
}

void CClientVehicle::GetMoveSpeed(CVector& vecMoveSpeed) const
{
    if (m_bIsFrozen)
    {
        vecMoveSpeed = CVector(0, 0, 0);
    }
    else
    {
        if (m_pVehicle)
        {
            m_pVehicle->GetMoveSpeed(&vecMoveSpeed);
        }
        else
        {
            vecMoveSpeed = m_vecMoveSpeed;
        }
    }
}

void CClientVehicle::SetMoveSpeed(const CVector& vecMoveSpeed)
{
    if (!m_bIsFrozen)
    {
        if (m_pVehicle)
            m_pVehicle->SetMoveSpeed(const_cast<CVector*>(&vecMoveSpeed));

        m_vecMoveSpeed = vecMoveSpeed;

        if (IsFrozenWaitingForGroundToLoad())
            m_vecWaitingForGroundSavedMoveSpeed = vecMoveSpeed;
    }
}

void CClientVehicle::GetTurnSpeed(CVector& vecTurnSpeed) const
{
    if (m_bIsFrozen)
        vecTurnSpeed = CVector(0, 0, 0);

    if (m_pVehicle)
        m_pVehicle->GetTurnSpeed(&vecTurnSpeed);
    else
        vecTurnSpeed = m_vecTurnSpeed;
}

void CClientVehicle::SetTurnSpeed(const CVector& vecTurnSpeed)
{
    if (!m_bIsFrozen)
    {
        if (m_pVehicle)
            m_pVehicle->SetTurnSpeed(const_cast<CVector*>(&vecTurnSpeed));

        m_vecTurnSpeed = vecTurnSpeed;

        if (IsFrozenWaitingForGroundToLoad())
            m_vecWaitingForGroundSavedTurnSpeed = vecTurnSpeed;
    }
}

bool CClientVehicle::IsVisible()
{
    return m_pVehicle ? m_pVehicle->IsVisible() : m_bVisible;
}

void CClientVehicle::SetVisible(bool bVisible)
{
    if (m_pVehicle)
        m_pVehicle->SetVisible(bVisible);

    m_bVisible = bVisible;
}

void CClientVehicle::SetDoorOpenRatioInterpolated(unsigned char ucDoor, float fRatio, unsigned long ulDelay)
{
    unsigned long ulTime = CClientTime::GetTime();
    m_doorInterp.fStart[ucDoor] = m_fDoorOpenRatio[ucDoor];
    m_doorInterp.fTarget[ucDoor] = fRatio;
    m_doorInterp.ulStartTime[ucDoor] = ulTime;
    m_doorInterp.ulTargetTime[ucDoor] = ulTime + ulDelay;
}

void CClientVehicle::ResetDoorInterpolation()
{
    for (unsigned char i = 0; i < 6; ++i)
    {
        if (m_doorInterp.ulTargetTime[i] != 0)
            SetDoorOpenRatio(i, m_doorInterp.fTarget[i], 0, true);
        m_doorInterp.ulTargetTime[i] = 0;
    }
}

void CClientVehicle::CancelDoorInterpolation(unsigned char ucDoor)
{
    m_doorInterp.ulTargetTime[ucDoor] = 0;
}

void CClientVehicle::ProcessDoorInterpolation()
{
    unsigned long ulTime = CClientTime::GetTime();

    for (unsigned char i = 0; i < 6; ++i)
    {
        if (m_doorInterp.ulTargetTime[i] != 0)
        {
            if (m_doorInterp.ulTargetTime[i] <= ulTime)
            {
                // Interpolation finished.
                SetDoorOpenRatio(i, m_doorInterp.fTarget[i], 0, true);
                m_doorInterp.ulTargetTime[i] = 0;
            }
            else
            {
                unsigned long ulElapsedTime = ulTime - m_doorInterp.ulStartTime[i];
                unsigned long ulDelay = m_doorInterp.ulTargetTime[i] - m_doorInterp.ulStartTime[i];
                float         fStep = ulElapsedTime / (float)ulDelay;
                float         fRatio = SharedUtil::Lerp(m_doorInterp.fStart[i], fStep, m_doorInterp.fTarget[i]);
                SetDoorOpenRatio(i, fRatio, 0, true);
            }
        }
    }
}

void CClientVehicle::SetDoorOpenRatio(unsigned char ucDoor, float fRatio, unsigned long ulDelay, bool bForced)
{
    unsigned char ucSeat;

    if (ucDoor <= 5)
    {
        bool bAllow = m_bAllowDoorRatioSetting[ucDoor];

        // Prevent setting the door angle ratio while a ped is entering/leaving the vehicle.
        if (bAllow && bForced == false)
        {
            switch (ucDoor)
            {
                case 2:
                    bAllow = m_pOccupyingDriver == 0;
                    break;
                case 3:
                case 4:
                case 5:
                    ucSeat = ucDoor - 2;
                    bAllow = m_pOccupyingPassengers[ucSeat] == 0;
                    break;
            }
        }

        if (bAllow)
        {
            if (ulDelay == 0UL)
            {
                if (m_pVehicle)
                    m_pVehicle->OpenDoor(ucDoor, fRatio, false);

                m_fDoorOpenRatio[ucDoor] = fRatio;
            }
            else
            {
                SetDoorOpenRatioInterpolated(ucDoor, fRatio, ulDelay);
            }
        }
    }
}

float CClientVehicle::GetDoorOpenRatio(unsigned char ucDoor)
{
    if (ucDoor <= 5)
    {
        if (m_pVehicle)
            return m_pVehicle->GetDoor(ucDoor)->GetAngleOpenRatio();

        return m_fDoorOpenRatio[ucDoor];
    }

    return 0.0f;
}

void CClientVehicle::SetSwingingDoorsAllowed(bool bAllowed)
{
    if (m_pVehicle)
        m_pVehicle->SetSwingingDoorsAllowed(bAllowed);

    m_bSwingingDoorsAllowed = bAllowed;
}

bool CClientVehicle::AreSwingingDoorsAllowed() const
{
    if (m_pVehicle)
        return m_pVehicle->AreSwingingDoorsAllowed();

    return m_bSwingingDoorsAllowed;
}

void CClientVehicle::AllowDoorRatioSetting(unsigned char ucDoor, bool bAllow, bool bAutoReallowAfterDelay)
{
    if (ucDoor < NUMELMS(m_bAllowDoorRatioSetting))
    {
        m_bAllowDoorRatioSetting[ucDoor] = bAllow;
        CancelDoorInterpolation(ucDoor);
        MapRemove(m_AutoReallowDoorRatioMap, (eDoors)ucDoor);
        if (!bAllow && bAutoReallowAfterDelay)
        {
            MapSet(m_AutoReallowDoorRatioMap, (eDoors)ucDoor, CTickCount::Now());
        }
    }
}

bool CClientVehicle::AreDoorsLocked()
{
    if (m_pVehicle)
        return m_pVehicle->AreDoorsLocked();

    return m_bDoorsLocked;
}

void CClientVehicle::SetDoorsLocked(bool bLocked)
{
    if (m_pVehicle)
        m_pVehicle->LockDoors(bLocked);

    m_bDoorsLocked = bLocked;
}

bool CClientVehicle::AreDoorsUndamageable()
{
    return m_pVehicle ? m_pVehicle->AreDoorsUndamageable() : m_bDoorsUndamageable;
}

void CClientVehicle::SetDoorsUndamageable(bool bUndamageable)
{
    if (m_pVehicle)
        m_pVehicle->SetDoorsUndamageable(bUndamageable);

    m_bDoorsUndamageable = bUndamageable;
}

float CClientVehicle::GetHealth() const
{
    return m_pVehicle ? m_pVehicle->GetHealth() : m_fHealth;
}

void CClientVehicle::SetHealth(float health)
{
    if (health < 0.0f || IsBlown())
        health = 0.0f;

    m_fHealth = health;

    if (m_pVehicle)
        m_pVehicle->SetHealth(health);
}

void CClientVehicle::Fix()
{
    if (m_pVehicle)
    {
        m_pVehicle->Fix();
        // Make sure its visible, if its supposed to be
        m_pVehicle->SetVisible(m_bVisible);
    }

    m_blowAfterStreamIn = false;

    if (m_blowState != VehicleBlowState::INTACT)
    {
        m_blowState = VehicleBlowState::INTACT;
        ReCreate();
    }

    SetHealth(DEFAULT_VEHICLE_HEALTH);

    SFixedArray<unsigned char, MAX_DOORS> ucDoorStates;
    GetInitialDoorStates(ucDoorStates);
    for (int i = 0; i < MAX_DOORS; i++)
        SetDoorStatus(i, ucDoorStates[i], true);
    for (int i = 0; i < MAX_PANELS; i++)
        SetPanelStatus(i, 0);
    for (int i = 0; i < MAX_LIGHTS; i++)
        SetLightStatus(i, 0);
    for (int i = 0; i < MAX_WHEELS; i++)
        SetWheelStatus(i, 0);

    // These components get a funny rotation when calling Fix() (unknown reason)
    struct
    {
        ushort      usModelId;
        const char* szComponentName;
    } const static fixRotationsList[] = {
        {422, "exhaust_ok"}, {436, "exhaust_ok"}, {440, "bump_front_dummy"}, {458, "exhaust_ok"}, {483, "exhaust_ok"},
        {485, "misc_b"},     {499, "exhaust_ok"}, {545, "exhaust_ok"},       {568, "misc_e"},     {603, "misc_a"},
    };

    const char* szFixComponentName = NULL;
    for (uint i = 0; i < NUMELMS(fixRotationsList); i++)
        if (GetModel() == fixRotationsList[i].usModelId)
            szFixComponentName = fixRotationsList[i].szComponentName;

    // Grab our component data
    std::map<SString, SVehicleComponentData>::iterator iter = m_ComponentData.begin();
    // Loop through our component data
    for (; iter != m_ComponentData.end(); iter++)
    {
        // store our string in a temporary variable
        SString strTemp = (*iter).first;

        // Do rotation fix if required
        if (szFixComponentName && strTemp == szFixComponentName)
        {
            SetComponentRotation(strTemp, (*iter).second.m_vecComponentRotation);
        }

        // is our position changed?
        if ((*iter).second.m_bPositionChanged)
        {
            // Make sure it's different
            if ((*iter).second.m_vecOriginalComponentPosition != (*iter).second.m_vecComponentPosition)
            {
                // apply our new position
                SetComponentPosition(strTemp, (*iter).second.m_vecComponentPosition);
            }
        }
        // is our rotation changed?
        if ((*iter).second.m_bRotationChanged)
        {
            // Make sure it's different
            if ((*iter).second.m_vecOriginalComponentRotation != (*iter).second.m_vecComponentRotation)
            {
                // apply our new rotation
                SetComponentRotation(strTemp, (*iter).second.m_vecComponentRotation);
            }
        }
        // is our scale changed?
        if ((*iter).second.m_bScaleChanged)
        {
            // Make sure it's different
            if ((*iter).second.m_vecOriginalComponentScale != (*iter).second.m_vecComponentScale)
            {
                // apply our new rotation
                SetComponentScale(strTemp, (*iter).second.m_vecComponentScale);
            }
        }

        // set our visibility
        SetComponentVisible(strTemp, (*iter).second.m_bVisible);
    }
}

void CClientVehicle::Blow(VehicleBlowFlags blow)
{
    if (m_blowState != VehicleBlowState::INTACT)
        return;

    m_blowState = (blow.withExplosion ? VehicleBlowState::AWAITING_EXPLOSION_SYNC : VehicleBlowState::BLOWN);
    m_fHealth = 0.0f;

    if (m_pVehicle)
    {
        // Make sure it can be damaged
        m_pVehicle->SetCanBeDamaged(true);

        // Grab our current speeds
        CVector vecMoveSpeed, vecTurnSpeed;
        GetMoveSpeed(vecMoveSpeed);
        GetTurnSpeed(vecTurnSpeed);

        // Set the health to 0
        m_pVehicle->SetHealth(0.0f);

        // "Fuck" the car completely, so we don't have weird client-side jumpyness because of differently synced wheel states on clients
        FuckCarCompletely(true);

        m_pVehicle->BlowUp(NULL, 0);

        // Blowing up a vehicle will cause an explosion in the original game code, but we have a hook in place,
        // which will prevent the explosion and forward the information to the server to relay it to everyone from there.
        // That hook may call further Lua events, which could result in a fixed vehicle and we have to check for that here.
        if (m_blowState == VehicleBlowState::INTACT)
            return;

        // And force the wheel states to "burst"
        SetWheelStatus(FRONT_LEFT_WHEEL, DT_WHEEL_BURST);
        SetWheelStatus(FRONT_RIGHT_WHEEL, DT_WHEEL_BURST);
        SetWheelStatus(REAR_LEFT_WHEEL, DT_WHEEL_BURST);
        SetWheelStatus(REAR_RIGHT_WHEEL, DT_WHEEL_BURST);

        if (!blow.withMovement)
        {
            // Make sure it doesn't change speeds (slightly cleaner for syncing)
            SetMoveSpeed(vecMoveSpeed);
            SetTurnSpeed(vecTurnSpeed);
        }

        // Restore the old can be damaged state
        CalcAndUpdateCanBeDamagedFlag();
    }
}

CVehicleColor& CClientVehicle::GetColor()
{
    if (m_pVehicle)
    {
        SColor colors[4];
        m_pVehicle->GetColor(&colors[0], &colors[1], &colors[2], &colors[3], 0);
        m_Color.SetRGBColors(colors[0], colors[1], colors[2], colors[3]);
    }
    return m_Color;
}

void CClientVehicle::SetColor(const CVehicleColor& color)
{
    m_Color = color;
    m_bColorSaved = true;

    if (m_pVehicle)
        m_pVehicle->SetColor(m_Color.GetRGBColor(0), m_Color.GetRGBColor(1), m_Color.GetRGBColor(2), m_Color.GetRGBColor(3), 0);
}

void CClientVehicle::GetTurretRotation(float& fHorizontal, float& fVertical)
{
    if (m_pVehicle)
    {
        // Car, plane or quad?
        if (m_eVehicleType == CLIENTVEHICLE_CAR || m_eVehicleType == CLIENTVEHICLE_PLANE || m_eVehicleType == CLIENTVEHICLE_QUADBIKE)
        {
            m_pVehicle->GetTurretRotation(&fHorizontal, &fVertical);
        }
        else
        {
            fHorizontal = 0;
            fVertical = 0;
        }
    }
    else
    {
        fHorizontal = m_fTurretHorizontal;
        fVertical = m_fTurretVertical;
    }
}

void CClientVehicle::SetTurretRotation(float fHorizontal, float fVertical)
{
    if (m_pVehicle)
    {
        // Car, plane or quad?
        if (m_eVehicleType == CLIENTVEHICLE_CAR || m_eVehicleType == CLIENTVEHICLE_PLANE || m_eVehicleType == CLIENTVEHICLE_QUADBIKE)
        {
            m_pVehicle->SetTurretRotation(fHorizontal, fVertical);
        }
    }
    m_fTurretHorizontal = fHorizontal;
    m_fTurretVertical = fVertical;
}

void CClientVehicle::SetModelBlocking(unsigned short usModel, unsigned char ucVariant, unsigned char ucVariant2)
{
    // Different vehicle ID than we have now?
    if (m_usModel != usModel)
    {
        // Destroy the old vehicle if we have one
        if (m_pVehicle)
            Destroy();

        // Get rid of our upgrades, they might be incompatible
        if (m_pUpgrades)
            m_pUpgrades->RemoveAll(false);

        // Are we swapping from a vehicle without doors?
        bool bResetWheelAndDoorStates = (!CClientVehicleManager::HasDoors(m_usModel) || m_eVehicleType != CClientVehicleManager::GetVehicleType(usModel));

        // Apply variant requirements
        if (ucVariant == 255 && ucVariant2 == 255)
            CClientVehicleManager::GetRandomVariation(usModel, ucVariant, ucVariant2);
        m_ucVariation = ucVariant;
        m_ucVariation2 = ucVariant2;

        // Set the new vehicle id and type
        eClientVehicleType eOldVehicleType = m_eVehicleType;
        m_usModel = usModel;
        if (m_clientModel && m_clientModel->GetModelID() != m_usModel)
            m_clientModel = nullptr;
        m_eVehicleType = CClientVehicleManager::GetVehicleType(usModel);
        m_bHasDamageModel = CClientVehicleManager::HasDamageModel(m_eVehicleType);

        if (bResetWheelAndDoorStates)
        {
            GetInitialDoorStates(m_ucDoorStates);
            memset(&m_ucWheelStates[0], 0, sizeof(m_ucWheelStates));
        }

        // If this is a train unlink if we're going to be a non-train
        if (eOldVehicleType == CLIENTVEHICLE_TRAIN && m_eVehicleType != CLIENTVEHICLE_TRAIN)
        {
            if (m_pNextLink != NULL)
            {
                m_pNextLink->SetPreviousTrainCarriage(NULL);
                m_pNextLink = NULL;
            }
            if (m_pPreviousLink != NULL)
            {
                m_pPreviousLink->SetNextTrainCarriage(NULL);
                m_pPreviousLink = NULL;
            }
        }

        // Check if we have landing gears and adjustable properties
        m_bHasLandingGear = DoCheckHasLandingGear();
        m_bHasAdjustableProperty = CClientVehicleManager::HasAdjustableProperty(m_usModel);
        m_usAdjustablePropertyValue = 0;

        // Grab the model info and the bounding box
        m_pModelInfo = g_pGame->GetModelInfo(usModel);
        m_ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount(usModel);

        // Reset handling to fit the vehicle
        if (IsLocalEntity() || !(usModel < 400 || usModel > 611))
        {
            ushort usHandlingModelID = usModel;
            if (usHandlingModelID < 400 || usHandlingModelID > 611)
                usHandlingModelID = m_pModelInfo->GetParentID();

            m_pOriginalHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData((eVehicleTypes)usHandlingModelID);
            m_pHandlingEntry->Assign(m_pOriginalHandlingEntry);

            m_pOriginalFlyingHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalFlyingHandlingData((eVehicleTypes)usHandlingModelID);
            m_pFlyingHandlingEntry->Assign(m_pOriginalFlyingHandlingEntry);

            m_pOriginalBoatHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalBoatHandlingData((eVehicleTypes)usHandlingModelID);
            if (m_pOriginalBoatHandlingEntry)
            {
                if (!m_pBoatHandlingEntry)
                    m_pBoatHandlingEntry = g_pGame->GetHandlingManager()->CreateBoatHandlingData();

                m_pBoatHandlingEntry->Assign(m_pOriginalBoatHandlingEntry);
            }

            m_pOriginalBikeHandlingEntry = g_pGame->GetHandlingManager()->GetOriginalBikeHandlingData((eVehicleTypes)usHandlingModelID);
            if (m_pOriginalBikeHandlingEntry)
            {
                if (!m_pBikeHandlingEntry)
                    m_pBikeHandlingEntry = g_pGame->GetHandlingManager()->CreateBikeHandlingData();

                m_pBikeHandlingEntry->Assign(m_pOriginalBikeHandlingEntry);
            }
        }

        ApplyHandling();

        SetSirenOrAlarmActive(false);

        // clear our component data to regenerate it
        m_ComponentData.clear();

        // Reset stored dummy positions
        m_copyDummyPositions = true;
        m_dummyPositions = {};

        // Create the vehicle if we're streamed in
        if (IsStreamedIn())
        {
            // Preload the model
            if (!m_pModelInfo->IsLoaded())
            {
                m_pModelInfo->Request(BLOCKING, "CClientVehicle::SetModelBlocking");
                m_pModelInfo->MakeCustomModel();
            }

            // Create the vehicle now. Don't prerequest the model ID for this func.
            Create();
        }
    }
}

void CClientVehicle::SetVariant(unsigned char ucVariant, unsigned char ucVariant2)
{
    m_ucVariation = ucVariant;
    m_ucVariation2 = ucVariant2;

    // clear our component data to regenerate it
    m_ComponentData.clear();

    ReCreate();
}

bool CClientVehicle::IsEngineBroken()
{
    return m_pVehicle ? m_pVehicle->IsEngineBroken() : m_bEngineBroken;
}

void CClientVehicle::SetEngineBroken(bool bEngineBroken)
{
    if (m_pVehicle)
    {
        m_pVehicle->SetEngineBroken(bEngineBroken);
        m_pVehicle->SetEngineOn(!bEngineBroken);
    }

    m_bEngineBroken = bEngineBroken;
}

bool CClientVehicle::IsEngineOn()
{
    return m_pVehicle ? m_pVehicle->IsEngineOn() : m_bEngineOn;
}

void CClientVehicle::SetEngineOn(bool bEngineOn)
{
    if (m_pVehicle)
        m_pVehicle->SetEngineOn(bEngineOn);

    m_bEngineOn = bEngineOn;
}

bool CClientVehicle::CanBeDamaged()
{
    return m_pVehicle ? m_pVehicle->GetCanBeDamaged() : m_bCanBeDamaged;
}

// This can be called frequently to ensure the correct setting gets to the SA vehicle
void CClientVehicle::CalcAndUpdateCanBeDamagedFlag()
{
    bool bCanBeDamaged = false;

    // CanBeDamaged if local driver or syncing unoccupiedVehicle
    if (m_pDriver && m_pDriver->IsLocalPlayer())
        bCanBeDamaged = true;

    if (m_bSyncUnoccupiedDamage)
        bCanBeDamaged = true;

    // Script override
    if (!m_bScriptCanBeDamaged)
        bCanBeDamaged = false;

    if (m_pVehicle)
        m_pVehicle->SetCanBeDamaged(bCanBeDamaged);

    m_bCanBeDamaged = bCanBeDamaged;
}

void CClientVehicle::SetScriptCanBeDamaged(bool bCanBeDamaged)
{
    // Needed so script doesn't interfere with syncing unoccupied vehicles
    m_bScriptCanBeDamaged = bCanBeDamaged;
    CalcAndUpdateCanBeDamagedFlag();
    CalcAndUpdateTyresCanBurstFlag();
}

void CClientVehicle::SetSyncUnoccupiedDamage(bool bCanBeDamaged)
{
    m_bSyncUnoccupiedDamage = bCanBeDamaged;
    CalcAndUpdateCanBeDamagedFlag();
    CalcAndUpdateTyresCanBurstFlag();
}

bool CClientVehicle::GetTyresCanBurst()
{
    return m_pVehicle ? (!m_pVehicle->GetTyresDontBurst()) : m_bTyresCanBurst;
}

// This can be called frequently to ensure the correct setting gets to the SA vehicle
void CClientVehicle::CalcAndUpdateTyresCanBurstFlag()
{
    bool bTyresCanBurst = false;

    // TyresCanBurst if local driver or syncing unoccupiedVehicle
    if (m_pDriver && m_pDriver->IsLocalPlayer())
        bTyresCanBurst = true;

    if (m_bSyncUnoccupiedDamage)
        bTyresCanBurst = true;

    // Script override
    if (!m_bScriptCanBeDamaged)
        bTyresCanBurst = false;

    if (m_pVehicle)
        m_pVehicle->SetTyresDontBurst(!bTyresCanBurst);

    m_bTyresCanBurst = bTyresCanBurst;
}

float CClientVehicle::GetGasPedal()
{
    return m_pVehicle ? m_pVehicle->GetGasPedal() : m_fGasPedal;
}

bool CClientVehicle::IsBelowWater() const
{
    CVector vecPosition;
    GetPosition(vecPosition);
    float fWaterLevel = 0.0f;

    if (g_pGame->GetWaterManager()->GetWaterLevel(vecPosition, &fWaterLevel, true, NULL))
    {
        if (vecPosition.fZ < fWaterLevel - 0.7)
        {
            return true;
        }
    }

    return false;
}

bool CClientVehicle::IsDrowning() const
{
    return m_pVehicle ? m_pVehicle->IsDrowning() : false;
}

bool CClientVehicle::IsDriven() const
{
    return m_pVehicle ? m_pVehicle->IsBeingDriven() : (GetOccupant(0) != nullptr);
}

bool CClientVehicle::IsUpsideDown() const
{
    if (m_pVehicle)
        return m_pVehicle->IsUpsideDown();

    // TODO: Figure out this using matrix?
    return false;
}

bool CClientVehicle::IsSirenOrAlarmActive()
{
    if (m_pVehicle)
        return m_pVehicle->IsSirenOrAlarmActive() ? true : false;

    return m_bSireneOrAlarmActive;
}

void CClientVehicle::SetSirenOrAlarmActive(bool bActive)
{
    if (m_pVehicle)
        m_pVehicle->SetSirenOrAlarmActive(bActive);

    m_bSireneOrAlarmActive = bActive;
}

float CClientVehicle::GetLandingGearPosition()
{
    return (m_pVehicle && m_bHasLandingGear) ? m_pVehicle->GetLandingGearPosition() : 0.0f;
}

void CClientVehicle::SetLandingGearPosition(float fPosition)
{
    if (m_pVehicle && m_bHasLandingGear)
        m_pVehicle->SetLandingGearPosition(fPosition);
}

bool CClientVehicle::IsLandingGearDown()
{
    if (!m_bHasLandingGear)
        return true;

    return m_pVehicle ? m_pVehicle->IsLandingGearDown() : m_bLandingGearDown;
}

void CClientVehicle::SetLandingGearDown(bool bLandingGearDown)
{
    if (m_bHasLandingGear)
    {
        if (m_pVehicle)
            m_pVehicle->SetLandingGearDown(bLandingGearDown);

        m_bLandingGearDown = bLandingGearDown;
    }
}

unsigned short CClientVehicle::GetAdjustablePropertyValue()
{
    unsigned short usPropertyValue;
    if (m_pVehicle)
    {
        usPropertyValue = m_pVehicle->GetAdjustablePropertyValue();
        // If it's a Hydra invert it with 5000 (as 0 is "forward"), so we can maintain a standard of 0 being "normal"
        if (m_usModel == VT_HYDRA)
            usPropertyValue = 5000 - usPropertyValue;
    }
    else
    {
        usPropertyValue = m_usAdjustablePropertyValue;
    }

    // Return it
    return usPropertyValue;
}

void CClientVehicle::SetAdjustablePropertyValue(unsigned short usValue)
{
    if (m_usModel == VT_HYDRA)
        usValue = 5000 - usValue;

    _SetAdjustablePropertyValue(usValue);
}

void CClientVehicle::_SetAdjustablePropertyValue(unsigned short usValue)
{
    // Set it
    if (m_pVehicle)
    {
        if (m_bHasAdjustableProperty)
        {
            m_pVehicle->SetAdjustablePropertyValue(usValue);

            // Update our collision for this adjustable?
            if (HasMovingCollision())
            {
                float fAngle = (float)usValue / 2499.0f;
                m_pVehicle->UpdateMovingCollision(fAngle);
            }
        }
    }
    m_usAdjustablePropertyValue = usValue;
}

bool CClientVehicle::HasMovingCollision()
{
    return (m_usModel == VT_FORKLIFT || m_usModel == VT_FIRELA || m_usModel == VT_ANDROM || m_usModel == VT_DUMPER || m_usModel == VT_DOZER ||
            m_usModel == VT_PACKER);
}

unsigned char CClientVehicle::GetDoorStatus(unsigned char ucDoor)
{
    if (ucDoor < MAX_DOORS)
    {
        if (m_pVehicle && HasDamageModel())
        {
            return m_pVehicle->GetDamageManager()->GetDoorStatus(static_cast<eDoors>(ucDoor));
        }

        return m_ucDoorStates[ucDoor];
    }

    return 0;
}

unsigned char CClientVehicle::GetWheelStatus(unsigned char ucWheel)
{
    if (ucWheel < MAX_WHEELS)
    {
        // Return our custom state?
        if (m_ucWheelStates[ucWheel] == DT_WHEEL_INTACT_COLLISIONLESS)
            return DT_WHEEL_INTACT_COLLISIONLESS;

        if (m_pVehicle)
        {
            if (HasDamageModel())
                return m_pVehicle->GetDamageManager()->GetWheelStatus(static_cast<eWheelPosition>(ucWheel));
            if (m_eVehicleType == CLIENTVEHICLE_BIKE && ucWheel < 2)
                return m_pVehicle->GetBikeWheelStatus(ucWheel);
        }

        return m_ucWheelStates[ucWheel];
    }

    return 0;
}

bool CClientVehicle::IsWheelCollided(unsigned char ucWheel)
{
    return m_pVehicle ? m_pVehicle->IsWheelCollided(ucWheel) : true;
}

int CClientVehicle::GetWheelFrictionState(unsigned char ucWheel)
{
    return m_pVehicle ? m_pVehicle->GetWheelFrictionState(ucWheel) : 0;
}

unsigned char CClientVehicle::GetPanelStatus(unsigned char ucPanel)
{
    if (ucPanel < MAX_PANELS)
    {
        if (m_pVehicle && HasDamageModel())
            return m_pVehicle->GetDamageManager()->GetPanelStatus(ucPanel);

        return m_ucPanelStates[ucPanel];
    }

    return 0;
}

unsigned char CClientVehicle::GetLightStatus(unsigned char ucLight)
{
    if (ucLight < MAX_LIGHTS)
    {
        if (m_pVehicle && HasDamageModel())
            return m_pVehicle->GetDamageManager()->GetLightStatus(ucLight);

        return m_ucLightStates[ucLight];
    }

    return 0;
}

void CClientVehicle::SetDoorStatus(unsigned char ucDoor, unsigned char ucStatus, bool spawnFlyingComponent)
{
    if (ucDoor < MAX_DOORS)
    {
        if (m_pVehicle && HasDamageModel())
            m_pVehicle->GetDamageManager()->SetDoorStatus(static_cast<eDoors>(ucDoor), ucStatus, spawnFlyingComponent);

        m_ucDoorStates[ucDoor] = ucStatus;
    }
}

void CClientVehicle::SetWheelStatus(unsigned char ucWheel, unsigned char ucStatus, bool bSilent)
{
    if (ucWheel < MAX_WHEELS)
    {
        if (m_pVehicle)
        {
            // Is our new status a burst tyre? and do we need to call BurstTyre?
            if (ucStatus == DT_WHEEL_BURST && !bSilent)
                m_pVehicle->BurstTyre(ucWheel);

            // Are we using our custom state?
            unsigned char ucGTAStatus = ucStatus;
            if (ucStatus == DT_WHEEL_INTACT_COLLISIONLESS)
                ucGTAStatus = DT_WHEEL_MISSING;

            // Do we have a damage model?
            if (HasDamageModel())
            {
                m_pVehicle->GetDamageManager()->SetWheelStatus((eWheelPosition)(ucWheel), ucGTAStatus);

                // Update the wheel's visibility
                m_pVehicle->SetWheelVisibility((eWheelPosition)ucWheel, ucStatus != DT_WHEEL_MISSING &&
                    (m_ComponentData.empty() || m_ComponentData[GetComponentNameForWheel(ucWheel)].m_bVisible));
            }
            else if (m_eVehicleType == CLIENTVEHICLE_BIKE && ucWheel < 2)
                m_pVehicle->SetBikeWheelStatus(ucWheel, ucGTAStatus);
        }
        m_ucWheelStates[ucWheel] = ucStatus;
    }
}

//
// Returns component name for eWheelPosition enum
//
SString CClientVehicle::GetComponentNameForWheel(unsigned char ucWheel) const noexcept
{
    switch (ucWheel)
    {
        case FRONT_LEFT_WHEEL:
            return "wheel_lf_dummy";
        case FRONT_RIGHT_WHEEL:
            return "wheel_rf_dummy";
        case REAR_LEFT_WHEEL:
            return "wheel_lb_dummy";
        case REAR_RIGHT_WHEEL:
            return "wheel_rb_dummy";
        default:
            return "";
    }
}

//
// Returns true if wheel should be invisible because of its state
//
bool CClientVehicle::GetWheelMissing(unsigned char ucWheel, const SString& strWheelName)
{
    // Use name if supplied
    if (strWheelName.BeginsWith("wheel"))
    {
        if (strWheelName == "wheel_lf_dummy")
            ucWheel = FRONT_LEFT_WHEEL;
        else if (strWheelName == "wheel_rf_dummy")
            ucWheel = FRONT_RIGHT_WHEEL;
        else if (strWheelName == "wheel_lb_dummy")
            ucWheel = REAR_LEFT_WHEEL;
        else if (strWheelName == "wheel_rb_dummy")
            ucWheel = REAR_RIGHT_WHEEL;
    }

    if (ucWheel < MAX_WHEELS)
    {
        if (HasDamageModel())
        {
            // Check the wheel's invisibility
            if (m_ucWheelStates[ucWheel] == DT_WHEEL_MISSING)
                return true;
        }
    }
    return false;
}

void CClientVehicle::SetPanelStatus(unsigned char ucPanel, unsigned char ucStatus)
{
    if (ucPanel < MAX_PANELS)
    {
        if (m_pVehicle && HasDamageModel())
            m_pVehicle->GetDamageManager()->SetPanelStatus(static_cast<ePanels>(ucPanel), ucStatus);

        m_ucPanelStates[ucPanel] = ucStatus;
    }
}

void CClientVehicle::SetLightStatus(unsigned char ucLight, unsigned char ucStatus)
{
    if (ucLight < MAX_LIGHTS)
    {
        if (m_pVehicle && HasDamageModel())
            m_pVehicle->GetDamageManager()->SetLightStatus(static_cast<eLights>(ucLight), ucStatus);

        m_ucLightStates[ucLight] = ucStatus;
    }
}

bool CClientVehicle::AreLightsOn()
{
    return m_pVehicle ? m_pVehicle->GetLightsOn() : false;
}

float CClientVehicle::GetHeliRotorSpeed()
{
    if (m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI)
        return m_pVehicle->GetHeliRotorSpeed();

    return m_fHeliRotorSpeed;
}

float CClientVehicle::GetPlaneRotorSpeed()
{
    if (m_pVehicle && m_eVehicleType == CLIENTVEHICLE_PLANE)
        return m_pVehicle->GetPlaneRotorSpeed();

    return m_fPlaneRotorSpeed;
}

void CClientVehicle::SetHeliRotorSpeed(float fSpeed)
{
    if (m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI)
        m_pVehicle->SetHeliRotorSpeed(fSpeed);

    m_fHeliRotorSpeed = fSpeed;
}

void CClientVehicle::SetPlaneRotorSpeed(float fSpeed)
{
    if (m_pVehicle && m_eVehicleType == CLIENTVEHICLE_PLANE)
        m_pVehicle->SetPlaneRotorSpeed(fSpeed);

    m_fPlaneRotorSpeed = fSpeed;
}

bool CClientVehicle::GetRotorSpeed(float& speed)
{
    if (m_eVehicleType == CLIENTVEHICLE_PLANE)
    {
        speed = GetPlaneRotorSpeed();
        return true;
    }
    else if (m_eVehicleType == CLIENTVEHICLE_HELI)
    {
        speed = GetHeliRotorSpeed();
        return true;
    }

    return false;
}

bool CClientVehicle::SetRotorSpeed(float fSpeed)
{
    switch (m_eVehicleType)
    {
        case CLIENTVEHICLE_HELI:
            SetHeliRotorSpeed(fSpeed);
            return true;
        case CLIENTVEHICLE_PLANE:
            SetPlaneRotorSpeed(fSpeed);
            return true;
        default:
            return false;
    }
}

bool CClientVehicle::SetWheelsRotation(float fRot1, float fRot2, float fRot3, float fRot4) noexcept
{
    if (!m_pVehicle)
        return false;

    return m_pVehicle->SetVehicleWheelRotation(fRot1, fRot2, fRot3, fRot4);
}

bool CClientVehicle::IsHeliSearchLightVisible()
{
    if (m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI)
        return m_pVehicle->IsHeliSearchLightVisible();

    return m_bHeliSearchLightVisible;
}

void CClientVehicle::SetHeliSearchLightVisible(bool bVisible)
{
    if (m_pVehicle && m_eVehicleType == CLIENTVEHICLE_HELI)
        m_pVehicle->SetHeliSearchLightVisible(bVisible);

    m_bHeliSearchLightVisible = bVisible;
}

void CClientVehicle::SetCollisionEnabled(bool bCollisionEnabled)
{
    if (m_pVehicle && m_bHasAdjustableProperty)
        m_pVehicle->SetUsesCollision(bCollisionEnabled);

    // Remove all contacts
    for (const auto& ped : m_Contacts)
        RemoveContact(ped);

    m_bIsCollisionEnabled = bCollisionEnabled;
}

bool CClientVehicle::GetCanShootPetrolTank()
{
    return m_pVehicle ? m_pVehicle->GetCanShootPetrolTank() : m_bCanShootPetrolTank;
}

void CClientVehicle::SetCanShootPetrolTank(bool bCanShoot)
{
    if (m_pVehicle)
        m_pVehicle->SetCanShootPetrolTank(bCanShoot);

    m_bCanShootPetrolTank = bCanShoot;
}

bool CClientVehicle::GetCanBeTargettedByHeatSeekingMissiles()
{
    return m_pVehicle ? m_pVehicle->GetCanBeTargettedByHeatSeekingMissiles() : m_bCanBeTargettedByHeatSeekingMissiles;
}

void CClientVehicle::SetCanBeTargettedByHeatSeekingMissiles(bool bEnabled)
{
    if (m_pVehicle)
        m_pVehicle->SetCanBeTargettedByHeatSeekingMissiles(bEnabled);

    m_bCanBeTargettedByHeatSeekingMissiles = bEnabled;
}

void CClientVehicle::SetAlpha(unsigned char ucAlpha)
{
    if (ucAlpha != m_ucAlpha)
    {
        if (m_pVehicle)
            m_pVehicle->SetAlpha(ucAlpha);

        m_ucAlpha = ucAlpha;
        m_bAlphaChanged = true;
    }
}

CClientPed* CClientVehicle::GetOccupant(int iSeat) const
{
    // Return the driver if the seat is 0
    if (iSeat == 0)
    {
        return (CClientPed*)(const CClientPed*)m_pDriver;
    }
    else if (iSeat <= (sizeof(m_pPassengers) / sizeof(CClientPed*)))
    {
        return m_pPassengers[iSeat - 1];
    }

    return NULL;
}

CClientPed* CClientVehicle::GetControllingPlayer()
{
    CClientPed* pControllingPlayer = m_pDriver;

    if (pControllingPlayer == NULL)
    {
        CClientVehicle* pCurrentVehicle = this;
        CClientVehicle* pTowedByVehicle = m_pTowedByVehicle;
        pControllingPlayer = pCurrentVehicle->GetOccupant(0);
        while (pTowedByVehicle)
        {
            pCurrentVehicle = pTowedByVehicle;
            pTowedByVehicle = pCurrentVehicle->GetTowedByVehicle();
            CClientPed* pCurrentDriver = pCurrentVehicle->GetOccupant(0);
            if (pCurrentDriver)
                pControllingPlayer = pCurrentDriver;
        }
    }

    // Trains
    if (GetVehicleType() == CLIENTVEHICLE_TRAIN)
    {
        CClientVehicle* pChainEngine = GetChainEngine();
        if (pChainEngine)
            pControllingPlayer = pChainEngine->GetOccupant(0);
    }

    return pControllingPlayer;
}

void CClientVehicle::ClearForOccupants()
{
    // TODO: Also check passenger seats for players
    // If there are people in the vehicle, remove them from the vehicle
    CClientPed* pPed = GetOccupant(0);
    if (pPed)
    {
        pPed->RemoveFromVehicle();
    }
}

void CClientVehicle::PlaceProperlyOnGround()
{
    if (m_pVehicle)
    {
        // Place it properly at the ground depending on what kind of vehicle it is
        if (m_eVehicleType == CLIENTVEHICLE_BMX || m_eVehicleType == CLIENTVEHICLE_BIKE)
        {
            m_pVehicle->PlaceBikeOnRoadProperly();
        }
        else if (m_eVehicleType != CLIENTVEHICLE_BOAT && m_eVehicleType != CLIENTVEHICLE_TRAIN)
        {
            m_pVehicle->PlaceAutomobileOnRoadProperly();
        }
    }
}

void CClientVehicle::FuckCarCompletely(bool bKeepWheels)
{
    if (m_pVehicle && HasDamageModel())
        m_pVehicle->GetDamageManager()->FuckCarCompletely(bKeepWheels);
}

void CClientVehicle::WorldIgnore(bool bWorldIgnore)
{
    if (bWorldIgnore)
    {
        if (m_pVehicle)
        {
            g_pGame->GetWorld()->IgnoreEntity(m_pVehicle);
        }
    }
    else
    {
        g_pGame->GetWorld()->IgnoreEntity(NULL);
    }
}

void CClientVehicle::FadeOut(bool bFadeOut)
{
    if (m_pVehicle)
        m_pVehicle->FadeOut(bFadeOut);
}

bool CClientVehicle::IsFadingOut()
{
    return m_pVehicle ? m_pVehicle->IsFadingOut() : false;
}

void CClientVehicle::SetFrozen(bool bFrozen)
{
    if (m_bScriptFrozen && bFrozen)
    {
        m_bIsFrozen = bFrozen;
        CVector vecTemp;
        if (m_pVehicle)
        {
            m_pVehicle->GetMatrix(&m_matFrozen);
            m_pVehicle->SetMoveSpeed(&vecTemp);
            m_pVehicle->SetTurnSpeed(&vecTemp);
        }
        else
        {
            m_matFrozen = m_Matrix;
            m_vecMoveSpeed = vecTemp;
            m_vecTurnSpeed = vecTemp;
        }
    }
    else if (!m_bScriptFrozen)
    {
        m_bIsFrozen = bFrozen;

        if (bFrozen)
        {
            CVector vecTemp;
            if (m_pVehicle)
            {
                m_pVehicle->GetMatrix(&m_matFrozen);
                m_pVehicle->SetMoveSpeed(&vecTemp);
                m_pVehicle->SetTurnSpeed(&vecTemp);
            }
            else
            {
                m_matFrozen = m_Matrix;
                m_vecMoveSpeed = vecTemp;
                m_vecTurnSpeed = vecTemp;
            }
        }
    }
}

bool CClientVehicle::IsFrozenWaitingForGroundToLoad() const
{
    return m_bFrozenWaitingForGroundToLoad;
}

void CClientVehicle::SetFrozenWaitingForGroundToLoad(bool bFrozen, bool bSuspendAsyncLoading)
{
    if (!g_pGame->IsASyncLoadingEnabled(true))
        return;

    if (m_bFrozenWaitingForGroundToLoad != bFrozen)
    {
        m_bFrozenWaitingForGroundToLoad = bFrozen;

        if (bFrozen)
        {
            if (bSuspendAsyncLoading)
            {
                // Set auto unsuspend time in case changes prevent second call
                g_pGame->SuspendASyncLoading(true, 5000);
            }
            m_fGroundCheckTolerance = 0.f;
            m_fObjectsAroundTolerance = -1.f;

            CVector vecTemp;
            if (m_pVehicle)
            {
                m_pVehicle->GetMatrix(&m_matFrozen);
                m_pVehicle->SetMoveSpeed(&vecTemp);
                m_pVehicle->SetTurnSpeed(&vecTemp);
            }
            else
            {
                m_matFrozen = m_Matrix;
                m_vecMoveSpeed = vecTemp;
                m_vecTurnSpeed = vecTemp;
            }
            m_vecWaitingForGroundSavedMoveSpeed = vecTemp;
            m_vecWaitingForGroundSavedTurnSpeed = vecTemp;
            m_bAsyncLoadingDisabled = bSuspendAsyncLoading;
        }
        else
        {
            // use the member variable here and ignore Suspend Async loading
            if (m_bAsyncLoadingDisabled)
            {
                g_pGame->SuspendASyncLoading(false);
            }
            m_vecMoveSpeed = m_vecWaitingForGroundSavedMoveSpeed;
            m_vecTurnSpeed = m_vecWaitingForGroundSavedTurnSpeed;
            if (m_pVehicle)
            {
                m_pVehicle->SetMoveSpeed(&m_vecMoveSpeed);
                m_pVehicle->SetTurnSpeed(&m_vecTurnSpeed);
            }
            m_bAsyncLoadingDisabled = false;
        }
    }
}

CClientVehicle* CClientVehicle::GetPreviousTrainCarriage()
{
    if (IsDerailed())
        return NULL;

    if (m_pVehicle)
    {
        CVehicle* pPreviousTrainCarriage = m_pVehicle->GetPreviousTrainCarriage();
        if (pPreviousTrainCarriage)
        {
            CPools*                    pPools = g_pGame->GetPools();
            CEntitySAInterface*        pInterface = pPreviousTrainCarriage->GetInterface();
            SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle((DWORD*)pInterface);
            if (pVehicleClientEntity && pVehicleClientEntity->pClientEntity)
            {
                return reinterpret_cast<CClientVehicle*>(pVehicleClientEntity->pClientEntity);
            }
        }
    }
    return m_pPreviousLink;
}

CClientVehicle* CClientVehicle::GetNextTrainCarriage()
{
    if (IsDerailed())
        return NULL;

    if (m_pVehicle)
    {
        CVehicle* pNextTrainCarriage = m_pVehicle->GetNextTrainCarriage();
        if (pNextTrainCarriage)
        {
            CPools*                    pPools = g_pGame->GetPools();
            CEntitySAInterface*        pInterface = pNextTrainCarriage->GetInterface();
            SClientEntity<CVehicleSA>* pVehicleClientEntity = pPools->GetVehicle((DWORD*)pInterface);
            if (pVehicleClientEntity && pVehicleClientEntity->pClientEntity)
            {
                return reinterpret_cast<CClientVehicle*>(pVehicleClientEntity->pClientEntity);
            }
        }
    }
    return m_pNextLink;
}

void CClientVehicle::SetPreviousTrainCarriage(CClientVehicle* pPrevious)
{
    // Tell the given vehicle we're the previous link and save the given vehicle as the next link
    m_pPreviousLink = pPrevious;

    if (pPrevious)
        pPrevious->m_pNextLink = this;

    // If both vehicles are streamed in, do the link
    if (m_pVehicle)
    {
        if (pPrevious && pPrevious->m_pVehicle)
            m_pVehicle->SetPreviousTrainCarriage(pPrevious->m_pVehicle);
        else
            m_pVehicle->SetPreviousTrainCarriage(NULL);
    }
}

void CClientVehicle::SetNextTrainCarriage(CClientVehicle* pNext)
{
    // Tell the given vehicle we're the previous link and save the given vehicle as the next link
    m_pNextLink = pNext;

    if (pNext)
        pNext->m_pPreviousLink = this;

    // If both vehicles are streamed in, do the link
    if (m_pVehicle)
    {
        if (pNext && pNext->m_pVehicle)
            m_pVehicle->SetNextTrainCarriage(pNext->m_pVehicle);
        else
            m_pVehicle->SetNextTrainCarriage(NULL);
    }
}

void CClientVehicle::SetIsChainEngine(bool bChainEngine, bool bTemporary)
{
    if (!bTemporary)
        m_bChainEngine = bChainEngine;

    if (m_pVehicle)
        m_pVehicle->SetIsChainEngine(bChainEngine);

    // Remove chain engine status from other carriages
    if (bChainEngine == true)
    {
        CClientVehicle* pCarriage = m_pNextLink;
        while (pCarriage)
        {
            pCarriage->SetIsChainEngine(false, bTemporary);
            pCarriage = pCarriage->m_pNextLink;
        }

        pCarriage = m_pPreviousLink;
        while (pCarriage)
        {
            pCarriage->SetIsChainEngine(false, bTemporary);
            pCarriage = pCarriage->m_pPreviousLink;
        }
    }
}

bool CClientVehicle::IsTrainConnectedTo(CClientVehicle* pTrailer)
{
    CClientVehicle* pVehicle = this;
    while (pVehicle)
    {
        if (pTrailer == pVehicle)
            return true;

        pVehicle = pVehicle->m_pNextLink;
    }

    pVehicle = this;
    while (pVehicle)
    {
        if (pTrailer == pVehicle)
            return true;

        pVehicle = pVehicle->m_pPreviousLink;
    }
    return false;
}

CClientVehicle* CClientVehicle::GetChainEngine()
{
    CClientVehicle* pChainEngine = this;
    while (pChainEngine)
    {
        if (pChainEngine->IsChainEngine())
            return pChainEngine;

        pChainEngine = pChainEngine->m_pNextLink;
    }

    pChainEngine = this;
    while (pChainEngine)
    {
        if (pChainEngine->IsChainEngine())
            return pChainEngine;

        pChainEngine = pChainEngine->m_pPreviousLink;
    }
    return pChainEngine;
}

bool CClientVehicle::IsDerailed()
{
    if (GetVehicleType() != CLIENTVEHICLE_TRAIN)
        return false;

    return m_pVehicle ? m_pVehicle->IsDerailed() : m_bIsDerailed;
}

void CClientVehicle::SetDerailed(bool bDerailed)
{
    if (GetVehicleType() != CLIENTVEHICLE_TRAIN)
        return;

    if (m_pVehicle && bDerailed != m_pVehicle->IsDerailed())
        m_pVehicle->SetDerailed(bDerailed);

    m_bIsDerailed = bDerailed;
}

bool CClientVehicle::IsDerailable()
{
    return m_pVehicle ? m_pVehicle->IsDerailable() : m_bIsDerailable;
}

void CClientVehicle::SetDerailable(bool bDerailable)
{
    if (m_pVehicle)
        m_pVehicle->SetDerailable(bDerailable);

    m_bIsDerailable = bDerailable;
}

bool CClientVehicle::GetTrainDirection()
{
    if (m_pVehicle)
        return m_pVehicle->GetTrainDirection();

    return m_bTrainDirection;
}

void CClientVehicle::SetTrainDirection(bool bDirection)
{
    if (m_pVehicle && GetVehicleType() == CLIENTVEHICLE_TRAIN)
        m_pVehicle->SetTrainDirection(bDirection);

    m_bTrainDirection = bDirection;
}

float CClientVehicle::GetTrainSpeed()
{
    return m_pVehicle ? m_pVehicle->GetTrainSpeed() : m_fTrainSpeed;
}

void CClientVehicle::SetTrainSpeed(float fSpeed)
{
    if (m_pVehicle && GetVehicleType() == CLIENTVEHICLE_TRAIN)
        m_pVehicle->SetTrainSpeed(fSpeed);

    m_fTrainSpeed = fSpeed;
}

float CClientVehicle::GetTrainPosition()
{
    return m_pVehicle ? m_pVehicle->GetTrainPosition() : m_fTrainPosition;
}

void CClientVehicle::SetTrainPosition(float fTrainPosition, bool bRecalcOnRailDistance)
{
    if (m_pVehicle && GetVehicleType() == CLIENTVEHICLE_TRAIN)
        m_pVehicle->SetTrainPosition(fTrainPosition, bRecalcOnRailDistance);

    m_fTrainPosition = fTrainPosition;
}

uchar CClientVehicle::GetTrainTrack()
{
    return m_pVehicle ? m_pVehicle->GetRailTrack() : m_ucTrackID;
}

void CClientVehicle::SetTrainTrack(uchar ucTrack)
{
    if (m_pVehicle && GetVehicleType() == CLIENTVEHICLE_TRAIN)
        m_pVehicle->SetRailTrack(ucTrack);

    m_ucTrackID = ucTrack;
}

void CClientVehicle::SetOverrideLights(unsigned char ucOverrideLights)
{
    if (m_pVehicle)
        m_pVehicle->SetOverrideLights(static_cast<unsigned int>(ucOverrideLights));

    m_ucOverrideLights = ucOverrideLights;
}

bool CClientVehicle::IsNitroInstalled()
{
    return this->GetUpgrades()->GetSlotState(8) != 0;
}

void CClientVehicle::StreamedInPulse()
{
    // Make sure the vehicle doesn't go too far down
    if (m_pVehicle)
    {
        if (m_bJustStreamedIn)
        {
            // Apply door & panel states. This cannot be done in Create()
            // due to issues upon setting the visibility of new RwFrames

            // Does this car have a damage model?
            if (HasDamageModel())
            {
                // Set the damage model doors
                CDamageManager* pDamageManager = m_pVehicle->GetDamageManager();

                for (int i = 0; i < MAX_DOORS; i++)
                    pDamageManager->SetDoorStatus(static_cast<eDoors>(i), m_ucDoorStates[i], true);
                for (int i = 0; i < MAX_PANELS; i++)
                    pDamageManager->SetPanelStatus(static_cast<ePanels>(i), m_ucPanelStates[i]);
                for (int i = 0; i < MAX_LIGHTS; i++)
                    pDamageManager->SetLightStatus(static_cast<eLights>(i), m_ucLightStates[i]);
            }

            m_bJustStreamedIn = false;
        }

        if (m_blowAfterStreamIn)
        {
            m_blowAfterStreamIn = false;

            VehicleBlowState previousBlowState = m_blowState;
            m_blowState = VehicleBlowState::INTACT;

            VehicleBlowFlags blow;
            blow.withMovement = false;
            blow.withExplosion = (previousBlowState == VehicleBlowState::AWAITING_EXPLOSION_SYNC);
            Blow(blow);

            if (m_blowState != VehicleBlowState::INTACT)
                m_blowState = previousBlowState;
        }

        // Handle door ratio auto reallowment
        if (!m_AutoReallowDoorRatioMap.empty())
        {
            for (std::map<eDoors, CTickCount>::iterator iter = m_AutoReallowDoorRatioMap.begin(); iter != m_AutoReallowDoorRatioMap.end();)
            {
                if ((CTickCount::Now() - iter->second).ToInt() > 4000)
                {
                    uchar ucDoor = iter->first;
                    m_AutoReallowDoorRatioMap.erase(iter++);
                    if (!m_bAllowDoorRatioSetting[ucDoor])
                        AllowDoorRatioSetting(ucDoor, true);
                }
                else
                    ++iter;
            }
        }

        // Are we an unmanned, invisible, blown-up plane?
        if (!GetOccupant() && m_eVehicleType == CLIENTVEHICLE_PLANE && IsBlown() && !m_pVehicle->IsVisible())
        {
            // Disable our collisions
            m_pVehicle->SetUsesCollision(false);
        }
        else
        {
            // Vehicles have a way of getting their cols back, so we have to force this each frame (not much overhead anyway)
            m_pVehicle->SetUsesCollision(m_bIsCollisionEnabled);
        }

        // Handle waiting for the ground to load
        if (IsFrozenWaitingForGroundToLoad())
            HandleWaitingForGroundToLoad();

        // If we are frozen, make sure we freeze our matrix and keep move/turn speed at 0,0,0
        if (m_bIsFrozen)
        {
            CVector vecTemp;
            m_pVehicle->SetMatrix(&m_matFrozen);
            m_pVehicle->SetMoveSpeed(&vecTemp);
            m_pVehicle->SetTurnSpeed(&vecTemp);
        }
        else
        {
            CVector vecPos = *m_pVehicle->GetPosition();
            // Cols been loaded for where the vehicle is? Only check this if it has no drivers.
            if ( m_pDriver ||
                 ( g_pGame->GetWorld ()->HasCollisionBeenLoaded ( &vecPos ) /*&&
                   m_pObjectManager->ObjectsAroundPointLoaded ( m_matFrozen.vPos, 200.0f, m_usDimension )*/ ) )
            {
                // Remember the matrix
                m_pVehicle->GetMatrix(&m_matFrozen);
            }
            else
            {
                // Force the position to the last remembered matrix (..and make sure gravity doesn't pull it down)

                if (!m_pTowedByVehicle && (GetVehicleType() != CLIENTVEHICLE_TRAIN || IsDerailed()))
                {
                    m_pVehicle->SetMatrix(&m_matFrozen);
                    CVector vec(0.0f, 0.0f, 0.0f);
                    m_pVehicle->SetMoveSpeed(&vec);
                }
                // Added by ChrML 27. Nov: Shouldn't cause any problems
                m_pVehicle->SetUsesCollision(false);
            }
        }

        // We dont interpolate attached trailers
        if (m_pTowedByVehicle)
        {
            RemoveTargetPosition();
            RemoveTargetRotation();
        }

        // Remove link in CClientVehicle structure if SA does it
        if (GetVehicleType() == CLIENTVEHICLE_TRAIN)
        {
            if (!GetNextTrainCarriage())
                m_pNextLink = NULL;

            if (!GetPreviousTrainCarriage())
                m_pPreviousLink = NULL;
        }

        CClientPed* pControllingPed = GetControllingPlayer();
        if (GetVehicleType() == CLIENTVEHICLE_TRAIN && (!pControllingPed || pControllingPed->GetType() != CCLIENTPLAYER))
        {
            // Apply chain engine's speed on its carriages (if chain engine isn't streamed in)
            CClientVehicle* pChainEngine = GetChainEngine();
            if (pChainEngine && pChainEngine != this && !pChainEngine->IsStreamedIn())
            {
                SetTrainSpeed(pChainEngine->GetTrainSpeed());
            }

            // Check if we need to update the train position (because of streaming)
            CVector vecPosition;
            float   fCarriageDistance = 20.0f;            // approximately || Todo: Find proper distance
            if (GetTrainDirection())
                fCarriageDistance = -fCarriageDistance;

            // Calculate and update new stream world position
            CClientVehicle* pCarriage = this;
            while (pCarriage->m_pNextLink && !pCarriage->m_pNextLink->IsStreamedIn())
            {
                float fNewTrainPosition = pCarriage->GetTrainPosition() + fCarriageDistance;
                pCarriage->m_pNextLink->SetTrainPosition(fNewTrainPosition);
                g_pGame->GetWorld()->FindWorldPositionForRailTrackPosition(fNewTrainPosition, GetTrainTrack(), &vecPosition);
                pCarriage->m_pNextLink->UpdatePedPositions(vecPosition);

                pCarriage = pCarriage->m_pNextLink;
            }

            pCarriage = this;
            while (pCarriage->m_pPreviousLink && !pCarriage->m_pPreviousLink->IsStreamedIn())
            {
                float fNewTrainPosition = pCarriage->GetTrainPosition() - fCarriageDistance;
                pCarriage->m_pPreviousLink->SetTrainPosition(fNewTrainPosition);
                g_pGame->GetWorld()->FindWorldPositionForRailTrackPosition(fNewTrainPosition, GetTrainTrack(), &vecPosition);
                pCarriage->m_pPreviousLink->UpdatePedPositions(vecPosition);

                pCarriage = pCarriage->m_pPreviousLink;
            }
        }

        // Limit burnout turn speed to ensure smoothness
        if (m_pDriver)
        {
            CControllerState cs;
            m_pDriver->GetControllerState(cs);
            bool bAcclerate = cs.ButtonCross > 128;
            bool bBrake = cs.ButtonSquare > 128;

            // Is doing burnout ?
            if (bAcclerate && bBrake)
            {
                CVector vecMoveSpeed;
                m_pVehicle->GetMoveSpeed(&vecMoveSpeed);
                if (fabsf(vecMoveSpeed.fX) < 0.06f * 2 && fabsf(vecMoveSpeed.fY) < 0.06f * 2 && fabsf(vecMoveSpeed.fZ) < 0.01f * 2)
                {
                    CVector vecTurnSpeed;
                    m_pVehicle->GetTurnSpeed(&vecTurnSpeed);
                    if (fabsf(vecTurnSpeed.fX) < 0.006f * 2 && fabsf(vecTurnSpeed.fY) < 0.006f * 2 && fabsf(vecTurnSpeed.fZ) < 0.04f * 2)
                    {
                        // Apply turn speed limit
                        float fLength = vecTurnSpeed.Normalize();
                        fLength = std::min(fLength, 0.02f);
                        vecTurnSpeed *= fLength;

                        m_pVehicle->SetTurnSpeed(&vecTurnSpeed);
                    }
                }
            }
        }

        Interpolate();
        ProcessDoorInterpolation();

        // Grab our current position
        CVector vecPosition = *m_pVehicle->GetPosition();

        if (m_pAttachedToEntity)
        {
            m_pAttachedToEntity->GetPosition(vecPosition);
            vecPosition += m_vecAttachedPosition;
        }

        // Have we moved?
        if (vecPosition != m_Matrix.vPos)
        {
            // If we're setting the position, check whether we're under-water.
            // If so, we need to set the Underwater flag so the render draw order is changed.
            m_pVehicle->SetUnderwater(IsBelowWater());

            // Store our new position
            m_Matrix.vPos = vecPosition;
            m_matFrozen.vPos = vecPosition;

            // Update our streaming position
            UpdateStreamPosition(vecPosition);
        }

        // Check installed nitro
        if (IsNitroInstalled())
        {
            // Nitro state changed?
            bool bActivated = (m_pVehicle->GetNitroLevel() < 0);
            if (m_bNitroActivated != bActivated)
            {
                CLuaArguments Arguments;
                Arguments.PushBoolean(bActivated);
                this->CallEvent("onClientVehicleNitroStateChange", Arguments, false);
            }

            m_bNitroActivated = bActivated;
        }

        // Update doors
        if (CClientVehicleManager::HasDoors(GetModel()))
        {
            for (unsigned char i = 0; i < 6; ++i)
            {
                CDoor* pDoor = m_pVehicle->GetDoor(i);
                if (pDoor)
                    m_fDoorOpenRatio[i] = pDoor->GetAngleOpenRatio();
            }
        }

        // Update landing gear state if this is a plane and we have no driver / pilot
        if (this->HasLandingGear() && m_pDriver == NULL)
        {
            m_pVehicle->UpdateLandingGearPosition();
        }
    }
}

void CClientVehicle::StreamIn(bool bInstantly)
{
    // We need to create now?
    if (bInstantly)
    {
        // Request its model blocking
        if (m_pModelRequester->RequestBlocking(m_usModel, "CClientVehicle::StreamIn - bInstantly"))
        {
            // Create us
            Create();
        }
        else
            NotifyUnableToCreate();
    }
    else
    {
        // Request its model.
        if (m_pModelRequester->Request(m_usModel, this))
        {
            // If it got loaded immediately, create the vehicle now.
            // Otherwise we create it when the model loaded callback calls us
            Create();
        }
        else
            NotifyUnableToCreate();
    }
}

void CClientVehicle::StreamOut()
{
    // Destroy the vehicle.
    Destroy();

    // Make sure we don't have any model requests
    // pending in the model request manager. If we
    // had and don't do this it could create us when
    // we're not streamed in.
    m_pModelRequester->Cancel(this, true);
}

bool CClientVehicle::DoCheckHasLandingGear()
{
    return (m_usModel == VT_ANDROM || m_usModel == VT_AT400 || m_usModel == VT_NEVADA || m_usModel == VT_RUSTLER || m_usModel == VT_SHAMAL ||
            m_usModel == VT_HYDRA || m_usModel == VT_STUNT);
}

void CClientVehicle::Create()
{
    // If the vehicle doesn't exist
    if (!m_pVehicle)
    {
        #ifdef MTA_DEBUG
        g_pCore->GetConsole()->Printf("CClientVehicle::Create %d", GetModel());
        #endif

        // Check again that the limit isn't reached. We are required to do so because
        // we load async. The streamer isn't always aware of our limits.
        if (CClientVehicleManager::IsVehicleLimitReached())
        {
            // Tell the streamer we could not create it
            NotifyUnableToCreate();
            return;
        }

        // Add a reference to the vehicle model we're creating.
        m_pModelInfo->ModelAddRef(BLOCKING, "CClientVehicle::Create");

        // Might want to make this settable by users? Could just leave it like this, don't mind.
        // Doesn't appear to work with trucks - only cars - stored string is up to 8 chars, will be reset after
        // each vehicle spawned of this model type (i.e. after AddVehicle below)
        if (!m_strRegPlate.empty())
            m_pModelInfo->SetCustomCarPlateText(m_strRegPlate.c_str());

        // Create the vehicle
        if (CClientVehicleManager::IsTrainModel(m_usModel))
        {
            DWORD dwModels[1];
            dwModels[0] = m_usModel;
            m_pVehicle = g_pGame->GetPools()->AddTrain(this, &m_Matrix.vPos, dwModels, 1, m_bTrainDirection, m_ucTrackID);
        }
        else
        {
            m_pVehicle = g_pGame->GetPools()->AddVehicle(this, static_cast<eVehicleTypes>(m_usModel), m_ucVariation, m_ucVariation2);
        }

        // Failed. Remove our reference to the vehicle model and return
        if (!m_pVehicle)
        {
            // Tell the streamer we could not create it
            NotifyUnableToCreate();

            m_pModelInfo->RemoveRef();
            return;
        }

        // Put our pointer in its custom data
        m_pVehicle->SetStoredPointer(this);

        /*if ( DoesNeedToWaitForGroundToLoad() )
        {
            // waiting for ground to load
            SetFrozenWaitingForGroundToLoad ( true, false );
        }*/

        // Jump straight to the target position if we have one
        if (HasTargetPosition())
        {
            GetTargetPosition(m_Matrix.vPos);
        }

        // Jump straight to the target rotation if we have one
        if (HasTargetRotation())
        {
            CVector vecTemp = m_interp.rot.vecTarget;
            ConvertDegreesToRadians(vecTemp);
            g_pMultiplayer->ConvertEulerAnglesToMatrix(m_Matrix, (2 * PI) - vecTemp.fX, (2 * PI) - vecTemp.fY, (2 * PI) - vecTemp.fZ);
        }

        // Got any settings to restore?
        m_pVehicle->SetMatrix(&m_Matrix);
        m_matFrozen = m_Matrix;
        SetMoveSpeed(m_vecMoveSpeed);
        SetTurnSpeed(m_vecTurnSpeed);
        m_pVehicle->SetVisible(m_bVisible);
        m_pVehicle->SetUsesCollision(m_bIsCollisionEnabled);
        m_pVehicle->SetEngineBroken(m_bEngineBroken);

        if (m_tSirenBeaconInfo.m_bOverrideSirens)
        {
            GiveVehicleSirens(m_tSirenBeaconInfo.m_ucSirenType, m_tSirenBeaconInfo.m_ucSirenCount);
            for (unsigned char i = 0; i < m_tSirenBeaconInfo.m_ucSirenCount; i++)
            {
                m_pVehicle->SetVehicleSirenPosition(i, m_tSirenBeaconInfo.m_tSirenInfo[i].m_vecSirenPositions);
                m_pVehicle->SetVehicleSirenMinimumAlpha(i, m_tSirenBeaconInfo.m_tSirenInfo[i].m_dwMinSirenAlpha);
                m_pVehicle->SetVehicleSirenColour(i, m_tSirenBeaconInfo.m_tSirenInfo[i].m_RGBBeaconColour);
            }
            SetVehicleFlags(m_tSirenBeaconInfo.m_b360Flag, m_tSirenBeaconInfo.m_bUseRandomiser, m_tSirenBeaconInfo.m_bDoLOSCheck,
                            m_tSirenBeaconInfo.m_bSirenSilent);
        }
        m_pVehicle->SetSirenOrAlarmActive(m_bSireneOrAlarmActive);
        SetLandingGearDown(m_bLandingGearDown);
        _SetAdjustablePropertyValue(m_usAdjustablePropertyValue);
        m_pVehicle->SetSwingingDoorsAllowed(m_bSwingingDoorsAllowed);
        m_pVehicle->LockDoors(m_bDoorsLocked);
        m_pVehicle->SetDoorsUndamageable(m_bDoorsUndamageable);
        m_pVehicle->SetCanShootPetrolTank(m_bCanShootPetrolTank);
        m_pVehicle->SetTaxiLightOn(m_bTaxiLightOn);
        m_pVehicle->SetCanBeTargettedByHeatSeekingMissiles(m_bCanBeTargettedByHeatSeekingMissiles);
        CalcAndUpdateTyresCanBurstFlag();

        if (GetVehicleType() == CLIENTVEHICLE_TRAIN)
        {
            m_pVehicle->SetDerailed(m_bIsDerailed);
            m_pVehicle->SetDerailable(m_bIsDerailable);
            m_pVehicle->SetTrainDirection(m_bTrainDirection);
            m_pVehicle->SetTrainSpeed(m_fTrainSpeed);
            if (m_ucTrackID >= 0)
                m_pVehicle->SetRailTrack(m_ucTrackID);

            if (m_fTrainPosition >= 0 && !m_bIsDerailed)
                m_pVehicle->SetTrainPosition(m_fTrainPosition, true);

            // Set matrix once more (to ensure that the rotation has been set properly)
            if (m_bIsDerailed)
                m_pVehicle->SetMatrix(&m_Matrix);

            if (m_bChainEngine)
                SetIsChainEngine(true);

            // Train carriages
            if (m_pNextLink && !m_bIsDerailed && !m_pNextLink->IsDerailed())
            {
                m_pVehicle->SetNextTrainCarriage(m_pNextLink->m_pVehicle);
                m_pNextLink->SetTrainTrack(GetTrainTrack());

                if (m_pNextLink->GetGameVehicle())
                {
                    SetTrainPosition(m_pNextLink->GetTrainPosition() - m_pVehicle->GetDistanceToCarriage(m_pNextLink->GetGameVehicle()), false);

                    m_pVehicle->AttachTrainCarriage(m_pNextLink->GetGameVehicle());
                }
            }
            if (m_pPreviousLink && !m_bIsDerailed && !m_pPreviousLink->IsDerailed())
            {
                m_pVehicle->SetPreviousTrainCarriage(m_pPreviousLink->m_pVehicle);
                this->SetTrainTrack(m_pPreviousLink->GetTrainTrack());
                if (m_pPreviousLink->GetGameVehicle())
                    m_pPreviousLink->GetGameVehicle()->AttachTrainCarriage(m_pVehicle);
            }
        }

        m_pVehicle->SetOverrideLights(m_ucOverrideLights);
        m_pVehicle->SetRemap(static_cast<unsigned int>(m_ucPaintjob));
        m_pVehicle->SetBodyDirtLevel(m_fDirtLevel);
        m_pVehicle->SetEngineOn(m_bEngineOn);
        m_pVehicle->SetAreaCode(m_ucInterior);
        m_pVehicle->SetSmokeTrailEnabled(m_bSmokeTrail);
        m_pVehicle->SetGravity(&m_vecGravity);
        m_pVehicle->SetHeadLightColor(m_HeadLightColor);
        m_pVehicle->SetRadioStatus(0);

        if (IsNitroInstalled())
        {
            m_pVehicle->SetNitroCount(m_cNitroCount);
            m_pVehicle->SetNitroLevel(m_fNitroLevel);
        }

        if (m_eVehicleType == CLIENTVEHICLE_HELI)
        {
            m_pVehicle->SetHeliRotorSpeed(m_fHeliRotorSpeed);
            m_pVehicle->SetHeliSearchLightVisible(m_bHeliSearchLightVisible);
        }

        m_pVehicle->SetUnderwater(IsBelowWater());

        // HACK: temp fix until windows are fixed using setAlpha
        if (m_bAlphaChanged)
            m_pVehicle->SetAlpha(m_ucAlpha);

        m_pVehicle->SetHealth(m_fHealth);
        m_blowAfterStreamIn = IsBlown();

        CalcAndUpdateCanBeDamagedFlag();

        if (IsLocalEntity() && !m_bColorSaved)
        {
            // On first create of local vehicle, save color chosen by GTA
            GetColor();
            m_bColorSaved = true;
        }

        // Restore the color
        m_pVehicle->SetColor(m_Color.GetRGBColor(0), m_Color.GetRGBColor(1), m_Color.GetRGBColor(2), m_Color.GetRGBColor(3), 0);

        // Restore turret rotation
        if (m_eVehicleType == CLIENTVEHICLE_CAR || m_eVehicleType == CLIENTVEHICLE_PLANE || m_eVehicleType == CLIENTVEHICLE_QUADBIKE)
        {
            m_pVehicle->SetTurretRotation(m_fTurretHorizontal, m_fTurretVertical);
        }

        for (int i = 0; i < MAX_WHEELS; i++)
            SetWheelStatus(i, m_ucWheelStates[i], true);

        // Eventually warp driver back in
        if (m_pDriver)
            m_pDriver->WarpIntoVehicle(this, 0);

        // Warp the passengers back in
        for (unsigned int i = 0; i < 8; i++)
        {
            if (m_pPassengers[i])
            {
                // Undefined passengers count?
                if (m_ucMaxPassengers != 255)
                    m_pPassengers[i]->WarpIntoVehicle(this, i + 1);
                else
                    m_pPassengers[i]->SetWarpInToVehicleRequired(false);

                if (m_pPassengers[i])
                    m_pPassengers[i]->StreamIn(true);
            }
        }

        // Reattach a towed vehicle?
        if (m_pTowedVehicle)
        {
            if (m_pTowedVehicle->IsTowableBy(this))
            {
                // Make sure that the trailer is streamed in
                if (!m_pTowedVehicle->GetGameVehicle())
                {
                    m_pTowedVehicle->StreamIn(true);
                }

                if (m_pTowedVehicle->GetGameVehicle())
                {
                    InternalSetTowLink(m_pTowedVehicle);
                }
            }
            else
            {
                m_pTowedVehicle->m_pTowedByVehicle = nullptr;

                // Stream-in the old unlinked trailer
                if (!m_pTowedVehicle->GetGameVehicle())
                {
                    m_pTowedVehicle->StreamIn(true);
                }

                m_pTowedVehicle = nullptr;
            }
        }

        // Reattach if we're being towed
        if (m_pTowedByVehicle)
        {
            if (IsTowableBy(m_pTowedByVehicle))
            {
                if (m_pTowedByVehicle->GetGameVehicle())
                {
                    m_pTowedByVehicle->InternalSetTowLink(this);
                }
            }
            else
            {
                m_pTowedByVehicle->m_pTowedVehicle = nullptr;
                m_pTowedByVehicle = nullptr;
            }
        }

        // Reattach to an entity + any entities attached to this
        ReattachEntities();

        // Give it a tap so it doesn't stick in the air if movespeed is standing still
        if (m_vecMoveSpeed.fX < 0.01f && m_vecMoveSpeed.fX > -0.01f && m_vecMoveSpeed.fY < 0.01f && m_vecMoveSpeed.fY > -0.01f && m_vecMoveSpeed.fZ < 0.01f &&
            m_vecMoveSpeed.fZ > -0.01f)
        {
            m_vecMoveSpeed = CVector(0.0f, 0.0f, 0.01f);
            m_pVehicle->SetMoveSpeed(&m_vecMoveSpeed);
        }

        // Validate
        m_pManager->RestoreEntity(this);

        // Set the frozen matrix to our position
        m_matFrozen = m_Matrix;

        // Reset the interpolation
        ResetInterpolation();
        ResetDoorInterpolation();

        for (unsigned char i = 0; i < 6; ++i)
            SetDoorOpenRatio(i, m_fDoorOpenRatio[i], 0, true);

        for (unsigned char i = 0; i < MAX_WINDOWS; ++i)
            SetWindowOpen(i, m_bWindowOpen[i]);

        // Re-apply handling entry
        if (m_pHandlingEntry)
        {
            m_pVehicle->SetHandlingData(m_pHandlingEntry);
            m_pVehicle->SetFlyingHandlingData(m_pFlyingHandlingEntry);

            switch (m_eVehicleType)
            {
                case CLIENTVEHICLE_BOAT:
                    dynamic_cast<CBoat*>(m_pVehicle)->SetBoatHandlingData(m_pBoatHandlingEntry);
                    break;
                case CLIENTVEHICLE_BIKE:
                case CLIENTVEHICLE_BMX:
                    dynamic_cast<CBike*>(m_pVehicle)->SetBikeHandlingData(m_pBikeHandlingEntry);
                    break;
            }

            if (m_bHasCustomHandling)
                ApplyHandling();
        }

        // Applying wheel upgrades can change these values.
        // We should keep track of the original values to restore them
        bool  bPreviousWheelScaleChanged = m_bWheelScaleChanged;
        float fPreviousWheelScale = m_fWheelScale;

        // Re-add all the upgrades - Has to be applied after handling *shrugs*
        if (m_pUpgrades)
            m_pUpgrades->ReAddAll();

        // Restore custom wheel scale
        if (bPreviousWheelScaleChanged)
        {
            m_pVehicle->SetWheelScale(fPreviousWheelScale);
            m_fWheelScale = fPreviousWheelScale;
            m_bWheelScaleChanged = true;
        }

        if (m_ComponentData.empty())
        {
            // grab our map of components
            std::map<SString, SVehicleFrame> componentMap = m_pVehicle->GetComponentMap();
            // get our beginning
            std::map<SString, SVehicleFrame>::iterator iter = componentMap.begin();
            // loop through all the components.... we don't care about the RwFrame we just want the names.
            for (; iter != componentMap.end(); iter++)
            {
                const SString&       strName = iter->first;
                const SVehicleFrame& frame = iter->second;

                SVehicleComponentData vehicleComponentData;

                // Find parent component name
                if (!frame.frameList.empty())
                {
                    RwFrame* pParentRwFrame = frame.frameList.back();
                    for (std::map<SString, SVehicleFrame>::const_iterator iter2 = componentMap.begin(); iter2 != componentMap.end(); iter2++)
                    {
                        if (iter2->second.pFrame == pParentRwFrame)
                        {
                            vehicleComponentData.m_strParentName = iter2->first;
                            break;
                        }
                    }
                }

                // Grab our start position
                GetComponentPosition((*iter).first, vehicleComponentData.m_vecComponentPosition);
                GetComponentRotation((*iter).first, vehicleComponentData.m_vecComponentRotation);
                GetComponentScale((*iter).first, vehicleComponentData.m_vecComponentScale);

                // copy it into our original positions
                vehicleComponentData.m_vecOriginalComponentPosition = vehicleComponentData.m_vecComponentPosition;
                vehicleComponentData.m_vecOriginalComponentRotation = vehicleComponentData.m_vecComponentRotation;
                vehicleComponentData.m_vecOriginalComponentScale = vehicleComponentData.m_vecComponentScale;

                // insert it into our component data list
                m_ComponentData.insert(std::pair<SString, SVehicleComponentData>((*iter).first, vehicleComponentData));

                // # prefix means hidden by default.
                if ((*iter).first[0] == '#')
                {
                    SetComponentVisible((*iter).first, false);
                }
            }
        }
        // Grab our component data
        std::map<SString, SVehicleComponentData>::iterator iter = m_ComponentData.begin();
        // Loop through our component data
        for (; iter != m_ComponentData.end(); iter++)
        {
            // store our string in a temporary variable
            SString strTemp = (*iter).first;
            // get our poisition and rotation and store it into
            // GetComponentPosition ( strTemp, (*iter).second.m_vecComponentPosition );
            // GetComponentRotation ( strTemp, (*iter).second.m_vecComponentRotation );
            // is our position changed?
            if ((*iter).second.m_bPositionChanged)
            {
                // Make sure it's different
                if ((*iter).second.m_vecOriginalComponentPosition != (*iter).second.m_vecComponentPosition)
                {
                    // apply our new position
                    SetComponentPosition(strTemp, (*iter).second.m_vecComponentPosition);
                }
            }
            // is our rotation changed?
            if ((*iter).second.m_bRotationChanged)
            {
                // Make sure it's different
                if ((*iter).second.m_vecOriginalComponentRotation != (*iter).second.m_vecComponentRotation)
                {
                    // apple our new rotation
                    SetComponentRotation(strTemp, (*iter).second.m_vecComponentRotation);
                }
            }
            // is our scale changed?
            if ((*iter).second.m_bScaleChanged)
            {
                // Make sure it's different
                if ((*iter).second.m_vecOriginalComponentScale != (*iter).second.m_vecComponentScale)
                {
                    // apple our new scale
                    SetComponentScale(strTemp, (*iter).second.m_vecComponentScale);
                }
            }
            // set our visibility
            SetComponentVisible(strTemp, (*iter).second.m_bVisible);
        }
        // store our spawn position in case we fall through the map
        m_matCreate = m_Matrix;

        // Copy or apply our vehicle dummy positions
        if (m_copyDummyPositions)
        {
            const CVector* positions = m_pVehicle->GetDummyPositions();
            std::copy(positions, positions + VEHICLE_DUMMY_COUNT, m_dummyPositions.begin());
        }
        else
        {
            for (size_t i = 0; i < VEHICLE_DUMMY_COUNT; ++i)
            {
                m_pVehicle->SetDummyPosition(static_cast<eVehicleDummies>(i), m_dummyPositions[i]);
            }
        }

        // We've just been streamed in
        m_bJustStreamedIn = true;

        // Tell the streamer we've created this object
        NotifyCreate();
    }
}

void CClientVehicle::Destroy()
{
    // If the vehicle exists
    if (m_pVehicle)
    {
        #ifdef MTA_DEBUG
        g_pCore->GetConsole()->Printf("CClientVehicle::Destroy %d", GetModel());
        #endif

        // Invalidate
        m_pManager->InvalidateEntity(this);

        // Store anything we allow GTA to change
        m_pVehicle->GetMatrix(&m_Matrix);
        m_pVehicle->GetMoveSpeed(&m_vecMoveSpeed);
        m_pVehicle->GetTurnSpeed(&m_vecTurnSpeed);
        m_fHealth = GetHealth();
        m_bSireneOrAlarmActive = m_pVehicle->IsSirenOrAlarmActive() ? true : false;
        m_bLandingGearDown = IsLandingGearDown();
        m_usAdjustablePropertyValue = m_pVehicle->GetAdjustablePropertyValue();
        m_bEngineOn = m_pVehicle->IsEngineOn();
        m_bIsOnGround = IsOnGround();
        m_fHeliRotorSpeed = GetHeliRotorSpeed();
        m_bHeliSearchLightVisible = IsHeliSearchLightVisible();
        m_pHandlingEntry = m_pVehicle->GetHandlingData();
        m_pFlyingHandlingEntry = m_pVehicle->GetFlyingHandlingData();

        switch (m_eVehicleType)
        {
            case CLIENTVEHICLE_BOAT:
                m_pBoatHandlingEntry = dynamic_cast<CBoat*>(m_pVehicle)->GetBoatHandlingData();
                break;
            case CLIENTVEHICLE_BIKE:
            case CLIENTVEHICLE_BMX:
                m_pBikeHandlingEntry = dynamic_cast<CBike*>(m_pVehicle)->GetBikeHandlingData();
                break;
            default:
                break;
        }

        if (m_eVehicleType == CLIENTVEHICLE_CAR || m_eVehicleType == CLIENTVEHICLE_PLANE || m_eVehicleType == CLIENTVEHICLE_QUADBIKE)
        {
            m_pVehicle->GetTurretRotation(&m_fTurretHorizontal, &m_fTurretVertical);
        }

        // This vehicle has a damage model?
        if (HasDamageModel())
        {
            // Grab the damage model
            CDamageManager* pDamageManager = m_pVehicle->GetDamageManager();

            for (int i = 0; i < MAX_DOORS; i++)
                m_ucDoorStates[i] = pDamageManager->GetDoorStatus(static_cast<eDoors>(i));
            for (int i = 0; i < MAX_PANELS; i++)
                m_ucPanelStates[i] = pDamageManager->GetPanelStatus(static_cast<ePanels>(i));
            for (int i = 0; i < MAX_LIGHTS; i++)
                m_ucLightStates[i] = pDamageManager->GetLightStatus(static_cast<eLights>(i));
        }
        for (int i = 0; i < MAX_WHEELS; i++)
            m_ucWheelStates[i] = GetWheelStatus(i);

        // Remove the driver from the vehicle
        CClientPed* pPed = GetOccupant(0);
        if (pPed)
        {
            // Only remove him physically. Don't let the ped update us
            pPed->InternalRemoveFromVehicle(m_pVehicle);
            if (!g_pClientGame->IsGlitchEnabled(CClientGame::GLITCH_KICKOUTOFVEHICLE_ONMODELREPLACE))
                pPed->SetWarpInToVehicleRequired(true);
        }

        // Remove all the passengers physically
        bool bWarpInToVehicleRequired = !g_pClientGame->IsGlitchEnabled(CClientGame::GLITCH_KICKOUTOFVEHICLE_ONMODELREPLACE);
        for (unsigned int i = 0; i < 8; i++)
        {
            CClientPed* pPassenger = m_pPassengers[i];
            if (pPassenger)
            {
                pPassenger->InternalRemoveFromVehicle(m_pVehicle);
                pPassenger->SetWarpInToVehicleRequired(bWarpInToVehicleRequired);
            }
        }

        // Do we have any occupying players? (that could be working on entering this vehicle)
        if (m_pOccupyingDriver && m_pOccupyingDriver->m_pOccupyingVehicle == this)
        {
            if (m_pOccupyingDriver->IsGettingIntoVehicle() || m_pOccupyingDriver->IsGettingOutOfVehicle())
            {
                m_pOccupyingDriver->RemoveFromVehicle();
            }
        }
        for (unsigned int i = 0; i < 8; i++)
        {
            if (m_pOccupyingPassengers[i] && m_pOccupyingPassengers[i]->m_pOccupyingVehicle == this)
            {
                if (m_pOccupyingPassengers[i]->IsGettingIntoVehicle() || m_pOccupyingPassengers[i]->IsGettingOutOfVehicle())
                {
                    m_pOccupyingPassengers[i]->RemoveFromVehicle();
                }
            }
        }

        if (GetTowedVehicle())
        {
            // Force the trailer to stream out
            GetTowedVehicle()->StreamOut();
        }

        if (m_pTowedByVehicle)
        {
            m_pVehicle->BreakTowLink();
        }

        if (GetVehicleType() == CLIENTVEHICLE_TRAIN)
        {
            m_bIsDerailed = IsDerailed();
            m_ucTrackID = m_pVehicle->GetRailTrack();
            m_fTrainPosition = m_pVehicle->GetTrainPosition();
            m_fTrainSpeed = m_pVehicle->GetTrainSpeed();

            if (m_pVehicle->IsChainEngine())
            {
                // Devolve chain engine state to next link (temporarily)
                if (m_pNextLink && m_pNextLink->GetGameVehicle())
                    m_pNextLink->SetIsChainEngine(true, true);
                else if (m_pPreviousLink && m_pPreviousLink->GetGameVehicle())
                    m_pPreviousLink->SetIsChainEngine(true, true);
            }

            // Unlink from chain
            if (m_pNextLink && m_pNextLink->GetGameVehicle())
                m_pNextLink->GetGameVehicle()->SetPreviousTrainCarriage(NULL);

            if (m_pPreviousLink && m_pPreviousLink->GetGameVehicle())
                m_pPreviousLink->GetGameVehicle()->SetNextTrainCarriage(NULL);
        }

        // Destroy the vehicle
        g_pGame->GetPools()->RemoveVehicle(m_pVehicle);
        m_pVehicle = NULL;

        // Remove reference to its model
        m_pModelInfo->RemoveRef();

        // reset our fall through map count
        m_ucFellThroughMapCount = 1;

        NotifyDestroy();
    }
}

void CClientVehicle::ReCreate()
{
    // Recreate the vehicle if it exists
    if (m_pVehicle)
    {
        Destroy();
        Create();
    }
}

void CClientVehicle::ModelRequestCallback(CModelInfo* pModelInfo)
{
    // Create the vehicle. The model is now loaded.
    Create();
}

void CClientVehicle::NotifyCreate()
{
    m_pVehicleManager->OnCreation(this);
    CClientStreamElement::NotifyCreate();
}

void CClientVehicle::NotifyDestroy()
{
    m_pVehicleManager->OnDestruction(this);
}

CClientVehicle* CClientVehicle::GetTowedVehicle()
{
    if (m_pVehicle)
    {
        CVehicle* pGameVehicle = m_pVehicle->GetTowedVehicle();
        if (pGameVehicle)
        {
            CPools*                    pPools = g_pGame->GetPools();
            SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)pGameVehicle->GetInterface());
            if (pVehicleEntity && pVehicleEntity->pClientEntity)
            {
                return reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity);
            }
        }
    }

    return m_pTowedVehicle;
}

CClientVehicle* CClientVehicle::GetRealTowedVehicle()
{
    if (m_pVehicle)
    {
        CVehicle* pGameVehicle = m_pVehicle->GetTowedVehicle();
        if (pGameVehicle)
        {
            CPools*                    pPools = g_pGame->GetPools();
            SClientEntity<CVehicleSA>* pVehicleEntity = pPools->GetVehicle((DWORD*)pGameVehicle->GetInterface());
            if (pVehicleEntity && pVehicleEntity->pClientEntity)
            {
                return reinterpret_cast<CClientVehicle*>(pVehicleEntity->pClientEntity);
            }
        }

        // This is the only difference from ::GetTowedVehicle
        return NULL;
    }

    return m_pTowedVehicle;
}

bool CClientVehicle::SetTowedVehicle(CClientVehicle* pVehicle, const CVector* vecRotationDegrees)
{
    // Train carriages
    if (this->GetVehicleType() == CLIENTVEHICLE_TRAIN && pVehicle == NULL)
    {
        if (m_pVehicle && m_pNextLink && m_pNextLink->GetGameVehicle())
            m_pVehicle->DetachTrainCarriage(m_pNextLink->GetGameVehicle());

        // Deattach our trailer
        if (m_pNextLink != NULL)
        {
            m_pNextLink->SetPreviousTrainCarriage(NULL);
        }

        SetNextTrainCarriage(NULL);
    }
    else if (this->GetVehicleType() == CLIENTVEHICLE_TRAIN && pVehicle->GetVehicleType() == CLIENTVEHICLE_TRAIN)
    {
        if (!m_pPreviousLink)
            SetIsChainEngine(true);

        CClientVehicle* pChainEngine = GetChainEngine();
        CVehicle*       pTowedGameVehicle = pVehicle->GetGameVehicle();
        SetNextTrainCarriage(pVehicle);

        if (pChainEngine)
        {
            pVehicle->SetTrainTrack(pChainEngine->GetTrainTrack());
            pVehicle->SetTrainPosition(pChainEngine->GetTrainPosition());
            pVehicle->SetTrainDirection(pChainEngine->GetTrainDirection());

            CVector vecPosition;
            pChainEngine->GetPosition(vecPosition);
            pVehicle->SetPosition(vecPosition);
        }

        if (m_pVehicle && pTowedGameVehicle)
        {
            m_pVehicle->AttachTrainCarriage(pTowedGameVehicle);
        }

        return true;
    }
    else if (GetVehicleType() == CLIENTVEHICLE_TRAIN || (pVehicle && !pVehicle->IsTowableBy(this)))
    {
        return false;
    }

    if (pVehicle == m_pTowedVehicle)
        return true;

    // Do we already have a towed vehicle?
    if (m_pTowedVehicle && pVehicle != m_pTowedVehicle)
    {
        // Remove it
        CVehicle* pGameVehicle = m_pTowedVehicle->GetGameVehicle();
        if (pGameVehicle && m_pVehicle)
            pGameVehicle->BreakTowLink();
        m_pTowedVehicle->m_pTowedByVehicle = NULL;
        m_pTowedVehicle = NULL;
    }

    // Do we have a new one to set?
    if (pVehicle)
    {
        // Are we trying to establish a circular loop? (this would freeze everything up)
        CClientVehicle* pCircTestVehicle = pVehicle;
        while (pCircTestVehicle)
        {
            if (pCircTestVehicle == this)
                return false;
            pCircTestVehicle = pCircTestVehicle->m_pTowedVehicle;
        }

        pVehicle->m_pTowedByVehicle = this;

        // Add it
        if (m_pVehicle)
        {
            CVehicle* pGameVehicle = pVehicle->GetGameVehicle();

            if (pGameVehicle)
            {
                // Both vehicles are streamed in
                if (m_pVehicle->GetTowedVehicle() != pGameVehicle)
                {
                    if (vecRotationDegrees)
                    {
                        pVehicle->SetRotationDegrees(*vecRotationDegrees);
                    }
                    else
                    {
                        // Apply the vehicle's rotation to the trailer
                        CVector vecRotationDegrees;
                        GetRotationDegrees(vecRotationDegrees);
                        pVehicle->SetRotationDegrees(vecRotationDegrees);
                    }
                    InternalSetTowLink(pVehicle);
                }
            }
            else
            {
                // If only the towing vehicle is streamed in, force the towed vehicle to stream in
                pVehicle->StreamIn(true);
            }
        }
        else
        {
            // If the towing vehicle is not streamed in, the towed vehicle can't be streamed in,
            // so we move it to the towed position.
            CVector vecPosition;
            pVehicle->GetPosition(vecPosition);
            pVehicle->UpdateStreamPosition(vecPosition);
        }
    }
    else
        m_ulIllegalTowBreakTime = 0;

    m_pTowedVehicle = pVehicle;
    return true;
}

bool CClientVehicle::InternalSetTowLink(CClientVehicle* pTrailer)
{
    CVehicle* pGameVehicle = pTrailer->GetGameVehicle();
    if (!pGameVehicle || !m_pVehicle)
        return false;

    // Get the position
    CVector* pTrailerPosition = pGameVehicle->GetPosition();
    CVector* pVehiclePosition = m_pVehicle->GetPosition();

    // Get hitch and tow (world) position
    CVector vecHitchPosition, vecTowBarPosition;
    pGameVehicle->GetTowHitchPos(&vecHitchPosition);
    m_pVehicle->GetTowBarPos(&vecTowBarPosition, pGameVehicle);

    // Calculate the new position (rotation should be set already)
    CVector vecOffset = vecHitchPosition - *pTrailerPosition;
    CVector vecDest = vecTowBarPosition - vecOffset;
    pTrailer->SetPosition(vecDest);

    // Apply the towed-by-vehicle's velocity to the trailer
    CVector vecMoveSpeed;
    this->GetMoveSpeed(vecMoveSpeed);
    pTrailer->SetMoveSpeed(vecMoveSpeed);

    // SA can attach the trailer now
    pGameVehicle->SetTowLink(m_pVehicle);

    pTrailer->PlaceProperlyOnGround();            // Probably not needed

    return true;
}

bool CClientVehicle::IsTowableBy(CClientVehicle* towingVehicle)
{
    return GetModelInfo()->IsTowableBy(towingVehicle->GetModelInfo());
}

bool CClientVehicle::SetWinchType(eWinchType winchType)
{
    if (GetModel() == VT_LEVIATHN)            // Leviathan
    {
        if (m_pVehicle)
        {
            if (m_eWinchType == WINCH_NONE)
            {
                m_pVehicle->SetWinchType(winchType);
                m_eWinchType = winchType;
            }
            return true;
        }
        else
        {
            m_eWinchType = winchType;
            return true;
        }
    }

    return false;
}

bool CClientVehicle::PickupEntityWithWinch(CClientEntity* pEntity)
{
    if (m_pVehicle)
    {
        if (m_eWinchType != WINCH_NONE)
        {
            CEntity*          pGameEntity = NULL;
            eClientEntityType entityType = pEntity->GetType();
            switch (entityType)
            {
                case CCLIENTOBJECT:
                {
                    CClientObject* pObject = static_cast<CClientObject*>(pEntity);
                    pGameEntity = pObject->GetGameObject();
                    break;
                }
                case CCLIENTPED:
                case CCLIENTPLAYER:
                {
                    CClientPed* pModel = static_cast<CClientPed*>(pEntity);
                    pGameEntity = pModel->GetGameEntity();
                    break;
                }
                case CCLIENTVEHICLE:
                {
                    CClientVehicle* pVehicle = static_cast<CClientVehicle*>(pEntity);
                    pGameEntity = pVehicle->GetGameVehicle();
                    break;
                }
            }

            if (pGameEntity)
            {
                m_pVehicle->PickupEntityWithWinch(pGameEntity);
                m_pPickedUpWinchEntity = pEntity;

                return true;
            }
        }
    }

    return false;
}

bool CClientVehicle::ReleasePickedUpEntityWithWinch()
{
    if (m_pVehicle)
    {
        if (m_pPickedUpWinchEntity)
        {
            m_pVehicle->ReleasePickedUpEntityWithWinch();
            m_pPickedUpWinchEntity = NULL;

            return true;
        }
    }

    return false;
}

void CClientVehicle::SetRopeHeightForHeli(float fRopeHeight)
{
    if (m_pVehicle)
        m_pVehicle->SetRopeHeightForHeli(fRopeHeight);
}

CClientEntity* CClientVehicle::GetPickedUpEntityWithWinch()
{
    CClientEntity* pEntity = m_pPickedUpWinchEntity;
    if (m_pVehicle)
    {
        CPhysical* pPhysical = m_pVehicle->QueryPickedUpEntityWithWinch();
        if (pPhysical)
        {
            CPools* pPools = g_pGame->GetPools();
            pEntity = pPools->GetClientEntity((DWORD*)pPhysical->GetInterface());
        }
    }

    return pEntity;
}

bool CClientVehicle::SetRegPlate(const char* szPlate)
{
    if (szPlate)
    {
        SString strPlateText = SStringX(szPlate).Left(8);
        if (strPlateText != m_strRegPlate)
        {
            m_strRegPlate = strPlateText;
            if (m_pVehicle)
            {
                m_pVehicle->SetPlateText(m_strRegPlate);
            }
            return true;
        }
    }
    return false;
}

unsigned char CClientVehicle::GetPaintjob()
{
    if (m_pVehicle)
    {
        int iRemap = m_pVehicle->GetRemapIndex();
        return (iRemap == -1) ? 3 : iRemap;
    }

    return m_ucPaintjob;
}

void CClientVehicle::SetPaintjob(unsigned char ucPaintjob)
{
    if (ucPaintjob != m_ucPaintjob && ucPaintjob <= 4)
    {
        if (m_pVehicle)
        {
            m_pVehicle->SetRemap(static_cast<unsigned int>(ucPaintjob));
        }
        m_ucPaintjob = ucPaintjob;
    }
}

float CClientVehicle::GetDirtLevel()
{
    return m_pVehicle ? m_pVehicle->GetBodyDirtLevel() : m_fDirtLevel;
}

void CClientVehicle::SetDirtLevel(float fDirtLevel)
{
    if (m_pVehicle)
        m_pVehicle->SetBodyDirtLevel(fDirtLevel);

    m_fDirtLevel = fDirtLevel;
}
bool CClientVehicle::IsOnWater()
{
    if (m_pVehicle)
    {
        float   fWaterLevel;
        CVector vecPosition, vecTemp;
        GetPosition(vecPosition);
        float fDistToBaseOfModel = vecPosition.fZ - m_pVehicle->GetDistanceFromCentreOfMassToBaseOfModel();
        if (g_pGame->GetWaterManager()->GetWaterLevel(vecPosition, &fWaterLevel, true, &vecTemp))
        {
            if (fDistToBaseOfModel <= fWaterLevel)
            {
                return true;
            }
        }
    }
    return false;
}

bool CClientVehicle::IsInWater()
{
    if (m_pModelInfo)
    {
        CBoundingBox* pBoundingBox = m_pModelInfo->GetBoundingBox();
        if (pBoundingBox)
        {
            CVector vecMin = pBoundingBox->vecBoundMin;
            CVector vecPosition, vecTemp;
            GetPosition(vecPosition);
            vecMin += vecPosition;
            float fWaterLevel;
            if (g_pGame->GetWaterManager()->GetWaterLevel(vecPosition, &fWaterLevel, true, &vecTemp))
            {
                if (vecPosition.fZ <= fWaterLevel)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

float CClientVehicle::GetDistanceFromGround()
{
    CVector vecPosition;
    GetPosition(vecPosition);
    float fGroundLevel = static_cast<float>(g_pGame->GetWorld()->FindGroundZFor3DPosition(&vecPosition));

    CBoundingBox* pBoundingBox = m_pModelInfo->GetBoundingBox();
    if (pBoundingBox)
        fGroundLevel -= pBoundingBox->vecBoundMin.fZ + pBoundingBox->vecBoundOffset.fZ;

    return (vecPosition.fZ - fGroundLevel);
}

bool CClientVehicle::IsOnGround()
{
    if (m_pModelInfo)
    {
        CBoundingBox* pBoundingBox = m_pModelInfo->GetBoundingBox();
        if (pBoundingBox)
        {
            CVector vecMin = pBoundingBox->vecBoundMin;
            CVector vecPosition;
            GetPosition(vecPosition);
            vecMin += vecPosition;
            float fGroundLevel = static_cast<float>(g_pGame->GetWorld()->FindGroundZFor3DPosition(&vecPosition));

            /* Is the lowest point of the bounding box lower than 0.5 above the floor,
            or is the lowest point of the bounding box higher than 0.3 below the floor */
            return ((fGroundLevel > vecMin.fZ && (fGroundLevel - vecMin.fZ) < 0.5f) || (vecMin.fZ > fGroundLevel && (vecMin.fZ - fGroundLevel) < 0.3f));
        }
    }
    return m_bIsOnGround;
}

void CClientVehicle::LockSteering(bool bLock)
{
    // STATUS_TRAIN_MOVING or STATUS_PLAYER_DISABLED will do. STATUS_TRAIN_NOT_MOVING is neater but will screw up planes (turns off the engine).

    eEntityStatus Status = m_pVehicle->GetEntityStatus();

    if (bLock && Status != STATUS_TRAIN_MOVING)
    {
        m_NormalStatus = Status;
        m_pVehicle->SetEntityStatus(STATUS_TRAIN_MOVING);
    }
    else if (!bLock && Status == STATUS_TRAIN_MOVING)
    {
        m_pVehicle->SetEntityStatus(m_NormalStatus);
    }

    return;
}

bool CClientVehicle::IsSmokeTrailEnabled()
{
    return m_pVehicle ? m_pVehicle->IsSmokeTrailEnabled() : m_bSmokeTrail;
}

void CClientVehicle::SetSmokeTrailEnabled(bool bEnabled)
{
    if (m_pVehicle)
        m_pVehicle->SetSmokeTrailEnabled(bEnabled);

    m_bSmokeTrail = bEnabled;
}

void CClientVehicle::ResetInterpolation()
{
    if (HasTargetPosition())
        SetPosition(m_interp.pos.vecTarget);
    if (HasTargetRotation())
        SetRotationDegrees(m_interp.rot.vecTarget);
    m_interp.pos.ulFinishTime = 0;
    m_interp.rot.ulFinishTime = 0;
}

void CClientVehicle::Interpolate()
{
    // Interpolate it if: It has a driver and it's not local and we're not syncing it or
    //                    It has no driver and we're not syncing it.
    if ((m_pDriver && !m_pDriver->IsLocalPlayer() && !static_cast<CDeathmatchVehicle*>(this)->IsSyncing()) ||
        (!m_pDriver && !static_cast<CDeathmatchVehicle*>(this)->IsSyncing()))
    {
        UpdateTargetPosition();
        UpdateTargetRotation();
    }
    else
    {
        // Otherwize make sure we have no interpolation stuff stored
        RemoveTargetPosition();
        RemoveTargetRotation();
    }
}

void CClientVehicle::GetInitialDoorStates(SFixedArray<unsigned char, MAX_DOORS>& ucOutDoorStates)
{
    switch (m_usModel)
    {
        case VT_BAGGAGE:
        case VT_BANDITO:
        case VT_BFINJECT:
        case VT_CADDY:
        case VT_DOZER:
        case VT_FORKLIFT:
        case VT_KART:
        case VT_MOWER:
        case VT_QUAD:
        case VT_RCBANDIT:
        case VT_RCCAM:
        case VT_RCGOBLIN:
        case VT_RCRAIDER:
        case VT_RCTIGER:
        case VT_TRACTOR:
        case VT_VORTEX:
        case VT_BLOODRA:
            memset(&ucOutDoorStates[0], DT_DOOR_MISSING, MAX_DOORS);

            // Keep the bonet and boot intact
            ucOutDoorStates[0] = ucOutDoorStates[1] = DT_DOOR_INTACT;
            break;
        default:
            memset(&ucOutDoorStates[0], DT_DOOR_INTACT, MAX_DOORS);
    }
}

void CClientVehicle::SetTargetPosition(const CVector& vecTargetPosition, unsigned long ulDelay, bool bValidVelocityZ, float fVelocityZ)
{
    // Are we streamed in?
    if (m_pVehicle)
    {
        UpdateTargetPosition();
        UpdateUnderFloorFix(vecTargetPosition, bValidVelocityZ, fVelocityZ);

        unsigned long ulTime = CClientTime::GetTime();
        CVector       vecLocalPosition;
        GetPosition(vecLocalPosition);

#ifdef MTA_DEBUG
        m_interp.pos.vecStart = vecLocalPosition;
#endif
        m_interp.pos.vecTarget = vecTargetPosition;
        // Calculate the relative error
        m_interp.pos.vecError = vecTargetPosition - vecLocalPosition;

        // Extrapolation
        const SVehExtrapolateSettings& vehExtrapolate = g_pClientGame->GetVehExtrapolateSettings();
        if (vehExtrapolate.bEnabled)
        {
            // Base amount to account for something
            int iExtrapolateMs = vehExtrapolate.iBaseMs;

            if (CClientPlayer* pPlayerDriver = DynamicCast<CClientPlayer>((CClientEntity*)m_pDriver))
                iExtrapolateMs += pPlayerDriver->GetLatency() * vehExtrapolate.iScalePercent / 110;

            // Limit amount
            iExtrapolateMs = Clamp(0, iExtrapolateMs, vehExtrapolate.iMaxMs);

            CVector vecVelocity;
            GetMoveSpeed(vecVelocity);
            vecVelocity *= 50.f * iExtrapolateMs * (1 / 1000.f);
            m_interp.pos.vecError += vecVelocity;
        }

        // Apply the error over 400ms (i.e. 1/4 per 100ms )
        m_interp.pos.vecError *= Lerp<const float>(0.25f, UnlerpClamped(100, ulDelay, 400), 1.0f);

        // Get the interpolation interval
        m_interp.pos.ulStartTime = ulTime;
        m_interp.pos.ulFinishTime = ulTime + ulDelay;

        // Initialize the interpolation
        m_interp.pos.fLastAlpha = 0.0f;
    }
    else
    {
        // Update our position now
        SetPosition(vecTargetPosition);
    }
}

void CClientVehicle::RemoveTargetPosition()
{
    m_interp.pos.ulFinishTime = 0;
}

void CClientVehicle::SetTargetRotation(const CVector& vecRotation, unsigned long ulDelay)
{
    // Are we streamed in?
    if (m_pVehicle)
    {
        UpdateTargetRotation();

        unsigned long ulTime = CClientTime::GetTime();
        CVector       vecLocalRotation;
        GetRotationDegrees(vecLocalRotation);

#ifdef MTA_DEBUG
        m_interp.rot.vecStart = vecLocalRotation;
#endif
        m_interp.rot.vecTarget = vecRotation;
        // Get the error
        m_interp.rot.vecError.fX = GetOffsetDegrees(vecLocalRotation.fX, vecRotation.fX);
        m_interp.rot.vecError.fY = GetOffsetDegrees(vecLocalRotation.fY, vecRotation.fY);
        m_interp.rot.vecError.fZ = GetOffsetDegrees(vecLocalRotation.fZ, vecRotation.fZ);

        // Apply the error over 250ms (i.e. 2/5 per 100ms )
        m_interp.rot.vecError *= Lerp<const float>(0.40f, UnlerpClamped(100, ulDelay, 400), 1.0f);

        // Get the interpolation interval
        m_interp.rot.ulStartTime = ulTime;
        m_interp.rot.ulFinishTime = ulTime + ulDelay;

        // Initialize the interpolation
        m_interp.rot.fLastAlpha = 0.0f;
    }
    else
    {
        // Update our rotation now
        SetRotationDegrees(vecRotation);
    }
}

void CClientVehicle::RemoveTargetRotation()
{
    m_interp.rot.ulFinishTime = 0;
}

void CClientVehicle::UpdateTargetPosition()
{
    if (HasTargetPosition())
    {
        // Grab the current game position
        CVector vecCurrentPosition;
        GetPosition(vecCurrentPosition);

        // Get the factor of time spent from the interpolation start
        // to the current time.
        unsigned long ulCurrentTime = CClientTime::GetTime();
        float         fAlpha = SharedUtil::Unlerp(m_interp.pos.ulStartTime, ulCurrentTime, m_interp.pos.ulFinishTime);

        // Don't let it overcompensate the error too much
        fAlpha = SharedUtil::Clamp(0.0f, fAlpha, 1.5f);

        // Get the current error portion to compensate
        float fCurrentAlpha = fAlpha - m_interp.pos.fLastAlpha;
        m_interp.pos.fLastAlpha = fAlpha;

        // Apply the error compensation
        CVector vecCompensation = SharedUtil::Lerp(CVector(), fCurrentAlpha, m_interp.pos.vecError);

        // If we finished compensating the error, finish it for the next pulse
        if (fAlpha == 1.5f)
        {
            m_interp.pos.ulFinishTime = 0;
        }

        CVector vecNewPosition = vecCurrentPosition + vecCompensation;

        // Check if the distance to interpolate is too far.
        CVector vecVelocity;
        GetMoveSpeed(vecVelocity);
        float fThreshold = (VEHICLE_INTERPOLATION_WARP_THRESHOLD + VEHICLE_INTERPOLATION_WARP_THRESHOLD_FOR_SPEED * vecVelocity.Length()) *
                           g_pGame->GetGameSpeed() * TICK_RATE / 100;

        // There is a reason to have this condition this way: To prevent NaNs generating new NaNs after interpolating (Comparing with NaNs always results to
        // false).
        if (!((vecCurrentPosition - m_interp.pos.vecTarget).Length() <= fThreshold))
        {
            // Abort all interpolation
            m_interp.pos.ulFinishTime = 0;
            vecNewPosition = m_interp.pos.vecTarget;

            if (HasTargetRotation())
                SetRotationDegrees(m_interp.rot.vecTarget);
            m_interp.rot.ulFinishTime = 0;
        }

        SetPosition(vecNewPosition, false);

        if (!m_bIsCollisionEnabled)
        {
            if (m_eVehicleType != CLIENTVEHICLE_HELI && m_eVehicleType != CLIENTVEHICLE_BOAT)
            {
                // Ghostmode upwards movement compensation
                CVector MoveSpeed;
                m_pVehicle->GetMoveSpeed(&MoveSpeed);
                float SpeedXY = CVector(MoveSpeed.fX, MoveSpeed.fY, 0).Length();
                if (MoveSpeed.fZ > 0.00 && MoveSpeed.fZ < 0.02 && MoveSpeed.fZ > SpeedXY)
                    MoveSpeed.fZ = SpeedXY;
                m_pVehicle->SetMoveSpeed(&MoveSpeed);
            }
        }

#ifdef MTA_DEBUG
        if (g_pClientGame->IsShowingInterpolation() && g_pClientGame->GetLocalPlayer() && g_pClientGame->GetLocalPlayer()->GetOccupiedVehicle() == this)
        {
            // DEBUG
            SString strBuffer(
                "-== Vehicle interpolation ==-\n"
                "vecStart: %f %f %f\n"
                "vecTarget: %f %f %f\n"
                "Position: %f %f %f\n"
                "Error: %f %f %f\n"
                "Alpha: %f\n"
                "Interpolating: %s\n",
                m_interp.pos.vecStart.fX, m_interp.pos.vecStart.fY, m_interp.pos.vecStart.fZ, m_interp.pos.vecTarget.fX, m_interp.pos.vecTarget.fY,
                m_interp.pos.vecTarget.fZ, vecNewPosition.fX, vecNewPosition.fY, vecNewPosition.fZ, m_interp.pos.vecError.fX, m_interp.pos.vecError.fY,
                m_interp.pos.vecError.fZ, fAlpha, (m_interp.pos.ulFinishTime == 0 ? "no" : "yes"));
            g_pClientGame->GetManager()->GetDisplayManager()->DrawText2D(strBuffer, CVector(0.45f, 0.05f, 0), 1.0f, 0xFFFFFFFF);
        }
#endif

        // Update our contact players
        CVector vecPlayerPosition;
        CVector vecOffset;
        for (uint i = 0; i < m_Contacts.size(); i++)
        {
            CClientPed* pModel = m_Contacts[i];
            pModel->GetPosition(vecPlayerPosition);
            vecOffset = vecPlayerPosition - vecCurrentPosition;
            vecPlayerPosition = vecNewPosition + vecOffset;
            pModel->SetPosition(vecPlayerPosition);
        }
    }
}

void CClientVehicle::UpdateTargetRotation()
{
    // Do we have a rotation to move towards? and are we streamed in?
    if (HasTargetRotation())
    {
        // Grab the current game rotation
        CVector vecCurrentRotation;
        GetRotationDegrees(vecCurrentRotation);

        // Get the factor of time spent from the interpolation start
        // to the current time.
        unsigned long ulCurrentTime = CClientTime::GetTime();
        float         fAlpha = SharedUtil::Unlerp(m_interp.rot.ulStartTime, ulCurrentTime, m_interp.rot.ulFinishTime);

        // Don't let it to overcompensate the error
        fAlpha = SharedUtil::Clamp(0.0f, fAlpha, 1.0f);

        // Get the current error portion to compensate
        float fCurrentAlpha = fAlpha - m_interp.rot.fLastAlpha;
        m_interp.rot.fLastAlpha = fAlpha;

        CVector vecCompensation = SharedUtil::Lerp(CVector(), fCurrentAlpha, m_interp.rot.vecError);

        // If we finished compensating the error, finish it for the next pulse
        if (fAlpha == 1.0f)
        {
            m_interp.rot.ulFinishTime = 0;
        }

        SetRotationDegrees(vecCurrentRotation + vecCompensation, false);
    }
}

// Cars under road fix hack
void CClientVehicle::UpdateUnderFloorFix(const CVector& vecTargetPosition, bool bValidVelocityZ, float fVelocityZ)
{
    CVector vecLocalPosition;
    GetPosition(vecLocalPosition);

    bool bForceLocalZ = false;
    if (bValidVelocityZ && m_eVehicleType != CLIENTVEHICLE_HELI && m_eVehicleType != CLIENTVEHICLE_PLANE)
    {
        // If remote z higher by too much and remote not doing any z movement, warp local z coord
        float fDeltaZ = vecTargetPosition.fZ - vecLocalPosition.fZ;
        if (fDeltaZ > 0.4f && fDeltaZ < 10.0f)
        {
            if (fabsf(fVelocityZ) < 0.01f)
            {
                bForceLocalZ = true;
            }
        }
    }

    // Only force z coord if needed for at least two consecutive calls
    if (!bForceLocalZ)
        m_uiForceLocalZCounter = 0;
    else if (m_uiForceLocalZCounter++ > 1)
    {
        vecLocalPosition.fZ = vecTargetPosition.fZ;
        SetPosition(vecLocalPosition);
    }
}

bool CClientVehicle::IsEnterable()
{
    if (m_pVehicle)
    {
        // Server vehicle?
        if (!IsLocalEntity())
        {
            if (GetHealth() > 0.0f)
            {
                if (!IsInWater() || (GetVehicleType() == CLIENTVEHICLE_BOAT || m_usModel == 447 /* sea sparrow */
                                     || m_usModel == 417                                        /* levithan */
                                     || m_usModel == 460 /* skimmer */))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool CClientVehicle::HasRadio()
{
    if (m_eVehicleType != CLIENTVEHICLE_BMX)
        return true;
    return false;
}

bool CClientVehicle::HasPoliceRadio()
{
    switch (m_usModel)
    {
        case VT_COPCARLA:
        case VT_COPCARSF:
        case VT_COPCARVG:
        case VT_COPCARRU:
        case VT_POLMAV:
        case VT_COPBIKE:
        case VT_SWATVAN:
            return true;
            break;
        default:
            break;
    }
    return false;
}

void CClientVehicle::RemoveAllProjectiles()
{
    CClientProjectile*                 pProjectile = NULL;
    list<CClientProjectile*>::iterator iter = m_Projectiles.begin();
    for (; iter != m_Projectiles.end(); iter++)
    {
        pProjectile = *iter;
        pProjectile->m_pCreator = NULL;
        g_pClientGame->GetElementDeleter()->Delete(pProjectile);
    }
    m_Projectiles.clear();
}

void CClientVehicle::SetGravity(const CVector& vecGravity)
{
    if (m_pVehicle)
        m_pVehicle->SetGravity(&vecGravity);

    m_vecGravity = vecGravity;
}

SColor CClientVehicle::GetHeadLightColor()
{
    if (m_pVehicle)
        return m_pVehicle->GetHeadLightColor();

    return m_HeadLightColor;
}

int CClientVehicle::GetCurrentGear()
{
    return m_pVehicle ? m_pVehicle->GetCurrentGear() : 0;
}

void CClientVehicle::SetHeadLightColor(const SColor color)
{
    if (m_pVehicle)
        m_pVehicle->SetHeadLightColor(color);

    m_HeadLightColor = color;
}

//
// Below here is basically awful.
// But there you go.
//

#if OCCUPY_DEBUG_INFO
    #define INFO(x)    g_pCore->GetConsole ()->Printf x
    #define WARN(x)    g_pCore->GetConsole ()->Printf x
#else
    #define INFO(x)    {}
    #define WARN(x)    {}
#endif

std::string GetPlayerName(CClientPed* pClientPed)
{
    if (!pClientPed)
        return "null";
    if (IS_PLAYER(pClientPed))
    {
        CClientPlayer* pPlayer = static_cast<CClientPlayer*>(pClientPed);
        return pPlayer->GetNick();
    }
    return "ped";
}

//
// Make a ped become an occupied driver/passenger
// Static function
//
void CClientVehicle::SetPedOccupiedVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor)
{
    INFO(("SetPedOccupiedVehicle:%s in vehicle:0x%08x  seat:%d  door:%u", GetPlayerName(pClientPed).c_str(), pVehicle, uiSeat, ucDoor));

    if (!pClientPed || !pVehicle)
        return;

    // Clear ped from any current occupied seat in this vehicle
    if (pClientPed->m_pOccupiedVehicle == pVehicle)
    {
        if (pVehicle->m_pDriver == pClientPed)
            pVehicle->m_pDriver = NULL;

        for (int i = 0; i < NUMELMS(pVehicle->m_pPassengers); i++)
            if (pVehicle->m_pPassengers[i] == pClientPed)
                pVehicle->m_pPassengers[i] = NULL;
    }

    // Vehicle vars
    if (uiSeat == 0)
    {
        if (pVehicle->m_pDriver && pVehicle->m_pDriver != pClientPed)
        {
            WARN(("Emergency occupied driver eject by %s on %s\n", GetPlayerName(pClientPed).c_str(), GetPlayerName(pVehicle->m_pDriver).c_str()));
            UnpairPedAndVehicle(pVehicle->m_pDriver, pVehicle);
        }
        pVehicle->m_pDriver = pClientPed;
    }
    else
    {
        assert(uiSeat <= NUMELMS(pVehicle->m_pPassengers));
        if (pVehicle->m_pPassengers[uiSeat - 1] && pVehicle->m_pPassengers[uiSeat - 1] != pClientPed)
        {
            WARN(("Emergency occupied passenger eject by %s on %s\n", GetPlayerName(pClientPed).c_str(),
                  GetPlayerName(pVehicle->m_pPassengers[uiSeat - 1]).c_str()));
            UnpairPedAndVehicle(pVehicle->m_pPassengers[uiSeat - 1], pVehicle);
        }
        pVehicle->m_pPassengers[uiSeat - 1] = pClientPed;
    }

    // Ped vars
    pClientPed->m_pOccupiedVehicle = pVehicle;
    pClientPed->m_uiOccupiedVehicleSeat = uiSeat;

    if (ucDoor != 0xFF)
        pVehicle->AllowDoorRatioSetting(ucDoor, true);
    else if (uiSeat < 4)
        pVehicle->AllowDoorRatioSetting(uiSeat + 2, true);

    // Checks
    ValidatePedAndVehiclePair(pClientPed, pVehicle);
}

//
// Make a ped become an occupying driver/passenger
// Static function
//
void CClientVehicle::SetPedOccupyingVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor)
{
    INFO(("SetPedOccupyingVehicle:%s in vehicle:0x%08x  seat:%d  door:%u", GetPlayerName(pClientPed).c_str(), pVehicle, uiSeat, ucDoor));

    if (!pClientPed || !pVehicle)
        return;

    // Clear ped from any current occupying seat in this vehicle
    if (pClientPed->m_pOccupyingVehicle == pVehicle)
    {
        if (pVehicle->m_pOccupyingDriver == pClientPed)
            pVehicle->m_pOccupyingDriver = NULL;

        for (int i = 0; i < NUMELMS(pVehicle->m_pOccupyingPassengers); i++)
            if (pVehicle->m_pOccupyingPassengers[i] == pClientPed)
                pVehicle->m_pOccupyingPassengers[i] = NULL;
    }

    // Vehicle vars
    if (uiSeat == 0)
    {
        if (pVehicle->m_pOccupyingDriver && pVehicle->m_pOccupyingDriver != pClientPed)
        {
            WARN(("Emergency occupying driver eject by %s on %s\n", GetPlayerName(pClientPed).c_str(), GetPlayerName(pVehicle->m_pOccupyingDriver).c_str()));
            UnpairPedAndVehicle(pVehicle->m_pOccupyingDriver, pVehicle);
        }
        pVehicle->m_pOccupyingDriver = pClientPed;
    }
    else
    {
        assert(uiSeat <= NUMELMS(pVehicle->m_pOccupyingPassengers));
        if (pVehicle->m_pOccupyingPassengers[uiSeat - 1] && pVehicle->m_pOccupyingPassengers[uiSeat - 1] != pClientPed)
        {
            WARN(("Emergency occupying passenger eject by %s on %s\n", GetPlayerName(pClientPed).c_str(),
                  GetPlayerName(pVehicle->m_pOccupyingPassengers[uiSeat - 1]).c_str()));
            UnpairPedAndVehicle(pVehicle->m_pOccupyingPassengers[uiSeat - 1], pVehicle);
        }
        pVehicle->m_pOccupyingPassengers[uiSeat - 1] = pClientPed;
    }

    // Ped vars
    pClientPed->m_pOccupyingVehicle = pVehicle;
    //  if ( uiSeat >= 0 && uiSeat < 8 )
    //      pClientPed->m_uiOccupyingSeat = uiSeat;

    if (ucDoor != 0xFF)
        pVehicle->AllowDoorRatioSetting(ucDoor, false);

    // Checks
    ValidatePedAndVehiclePair(pClientPed, pVehicle);
}

//
// Check ped <> vehicle pointers
// Static function
//
void CClientVehicle::ValidatePedAndVehiclePair(CClientPed* pClientPed, CClientVehicle* pVehicle)
{
#if MTA_DEBUG
    // Occupied
    // Vehicle vars
    if (pVehicle->m_pDriver)
        assert(pVehicle->m_pDriver->m_pOccupiedVehicle == pVehicle);

    for (int i = 0; i < NUMELMS(pVehicle->m_pPassengers); i++)
        if (pVehicle->m_pPassengers[i])
            assert(pVehicle->m_pPassengers[i]->m_pOccupiedVehicle == pVehicle);

    // Ped vars
    if (pClientPed->m_pOccupiedVehicle)
    {
        // Make sure refed once by vehicle
        int iCount = 0;
        if (pClientPed->m_pOccupiedVehicle->m_pDriver == pClientPed)
            iCount++;

        for (int i = 0; i < NUMELMS(pClientPed->m_pOccupiedVehicle->m_pPassengers); i++)
            if (pClientPed->m_pOccupiedVehicle->m_pPassengers[i] == pClientPed)
                iCount++;

        assert(iCount == 1);
    }

    // Occupying
    // Vehicle vars
    if (pVehicle->m_pOccupyingDriver)
        assert(pVehicle->m_pOccupyingDriver->m_pOccupyingVehicle == pVehicle);

    for (int i = 0; i < NUMELMS(pVehicle->m_pOccupyingPassengers); i++)
        if (pVehicle->m_pOccupyingPassengers[i])
            assert(pVehicle->m_pOccupyingPassengers[i]->m_pOccupyingVehicle == pVehicle);

    // Ped vars
    if (pClientPed->m_pOccupyingVehicle)
    {
        // Make sure refed once by vehicle
        int iCount = 0;
        if (pClientPed->m_pOccupyingVehicle->m_pOccupyingDriver == pClientPed)
            iCount++;

        for (int i = 0; i < NUMELMS(pClientPed->m_pOccupyingVehicle->m_pOccupyingPassengers); i++)
            if (pClientPed->m_pOccupyingVehicle->m_pOccupyingPassengers[i] == pClientPed)
                iCount++;

        assert(iCount == 1);
    }
#endif
}

//
// Make sure there is no association between a ped and a vehicle
// Static function
//
void CClientVehicle::UnpairPedAndVehicle(CClientPed* pClientPed, CClientVehicle* pVehicle)
{
    if (!pClientPed || !pVehicle)
        return;

    // Checks
    ValidatePedAndVehiclePair(pClientPed, pVehicle);

    // Occupied
    // Vehicle vars
    if (pVehicle->m_pDriver == pClientPed)
    {
        INFO(("UnpairPedAndVehicle: m_pDriver:%s from vehicle:0x%08x", GetPlayerName(pClientPed).c_str(), pVehicle));
        pVehicle->m_pDriver = NULL;
    }

    for (int i = 0; i < NUMELMS(pVehicle->m_pPassengers); i++)
        if (pVehicle->m_pPassengers[i] == pClientPed)
        {
            INFO(("UnpairPedAndVehicle: m_pPassenger:%s seat:%d from vehicle:0x%08x", GetPlayerName(pClientPed).c_str(), i + 1, pVehicle));
            pVehicle->m_pPassengers[i] = NULL;
        }

    // Ped vars
    if (pClientPed->m_pOccupiedVehicle == pVehicle)
    {
        INFO(("UnpairPedAndVehicle: pClientPed:%s from m_pOccupiedVehicle:0x%08x", GetPlayerName(pClientPed).c_str(), pVehicle));
        if (pClientPed->m_ucLeavingDoor != 0xFF)
        {
            pVehicle->AllowDoorRatioSetting(pClientPed->m_ucLeavingDoor, true);
            pClientPed->m_ucLeavingDoor = 0xFF;
        }
        pClientPed->m_pOccupiedVehicle = NULL;
        pClientPed->m_uiOccupiedVehicleSeat = 0xFF;
    }

    // Occupying
    // Vehicle vars
    if (pVehicle->m_pOccupyingDriver == pClientPed)
    {
        INFO(("UnpairPedAndVehicle: m_pOccupyingDriver:%s from vehicle:0x%08x", GetPlayerName(pClientPed).c_str(), pVehicle));
        pVehicle->m_pOccupyingDriver = NULL;
    }

    for (int i = 0; i < NUMELMS(pVehicle->m_pOccupyingPassengers); i++)
        if (pVehicle->m_pOccupyingPassengers[i] == pClientPed)
        {
            INFO(("UnpairPedAndVehicle: m_pOccupyingPassenger:%s seat:%d from vehicle:0x%08x", GetPlayerName(pClientPed).c_str(), i + 1, pVehicle));
            pVehicle->m_pOccupyingPassengers[i] = NULL;
        }

    // Ped vars
    if (pClientPed->m_pOccupyingVehicle == pVehicle)
    {
        INFO(("UnpairPedAndVehicle: pClientPed:%s from m_pOccupyingVehicle:0x%08x", GetPlayerName(pClientPed).c_str(), pVehicle));
        pClientPed->m_pOccupyingVehicle = NULL;
        // pClientPed->m_uiOccupyingSeat = 0xFF;
    }
}

//
// Make sure there is no association between a ped and its vehicle
// Static function
//
void CClientVehicle::UnpairPedAndVehicle(CClientPed* pClientPed)
{
    UnpairPedAndVehicle(pClientPed, pClientPed->GetOccupiedVehicle());
    UnpairPedAndVehicle(pClientPed, pClientPed->m_pOccupyingVehicle);
    UnpairPedAndVehicle(pClientPed, pClientPed->GetRealOccupiedVehicle());

    if (pClientPed->m_pOccupiedVehicle)
    {
        WARN(("*** Unexpected m_pOccupiedVehicle:0x%08x for %s\n", pClientPed->m_pOccupiedVehicle, GetPlayerName(pClientPed).c_str()));
        pClientPed->m_pOccupiedVehicle = NULL;
    }

    if (pClientPed->m_pOccupyingVehicle)
    {
        WARN(("*** Unexpected m_pOccupyingVehicle:0x%08x for %s\n", pClientPed->m_pOccupyingVehicle, GetPlayerName(pClientPed).c_str()));
        pClientPed->m_pOccupyingVehicle = NULL;
    }
}

void CClientVehicle::ApplyHandling()
{
    m_bHasCustomHandling = true;

    if (!m_pVehicle)
        return;

    m_pVehicle->RecalculateHandling();

    if (m_eVehicleType == CLIENTVEHICLE_BMX || m_eVehicleType == CLIENTVEHICLE_BIKE)
        dynamic_cast<CBike*>(m_pVehicle)->RecalculateBikeHandling();
}

CHandlingEntry* CClientVehicle::GetHandlingData()
{
    if (m_pVehicle)
        return m_pVehicle->GetHandlingData();
    else if (m_pHandlingEntry)
        return m_pHandlingEntry;

    return nullptr;
}

CFlyingHandlingEntry* CClientVehicle::GetFlyingHandlingData()
{
    if (m_pVehicle)
        return m_pVehicle->GetFlyingHandlingData();
    else if (m_pFlyingHandlingEntry)
        return m_pFlyingHandlingEntry;

    return nullptr;
}

CBoatHandlingEntry* CClientVehicle::GetBoatHandlingData()
{
    if (m_eVehicleType != CLIENTVEHICLE_BOAT)
        return nullptr;

    if (m_pVehicle)
        return reinterpret_cast<CBoat*>(m_pVehicle)->GetBoatHandlingData();
    else if (m_pBoatHandlingEntry)
        return m_pBoatHandlingEntry;

    return nullptr;
}

CBikeHandlingEntry* CClientVehicle::GetBikeHandlingData()
{
    if (m_eVehicleType != CLIENTVEHICLE_BIKE && m_eVehicleType != CLIENTVEHICLE_BMX)
        return nullptr;

    if (m_pVehicle)
        return reinterpret_cast<CBike*>(m_pVehicle)->GetBikeHandlingData();
    else if (m_pBikeHandlingEntry)
        return m_pBikeHandlingEntry;

    return nullptr;
}

CSphere CClientVehicle::GetWorldBoundingSphere()
{
    CSphere     sphere;
    CModelInfo* pModelInfo = g_pGame->GetModelInfo(GetModel());
    if (pModelInfo)
    {
        CBoundingBox* pBoundingBox = pModelInfo->GetBoundingBox();
        if (pBoundingBox)
        {
            sphere.vecPosition = pBoundingBox->vecBoundOffset;
            sphere.fRadius = pBoundingBox->fRadius;
        }
    }
    sphere.vecPosition += GetStreamPosition();
    return sphere;
}

// Currently, this should only be called if the local player is, or was just in the vehicle
void CClientVehicle::HandleWaitingForGroundToLoad()
{
    // Check if near any MTA objects
    bool    bNearObject = false;
    CVector vecPosition;
    GetPosition(vecPosition);
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery(result, CSphere(vecPosition + CVector(0, 0, -3), 5));
    for (CClientEntityResult::const_iterator it = result.begin(); it != result.end(); ++it)
    {
        if ((*it)->GetType() == CCLIENTOBJECT)
        {
            bNearObject = true;
            break;
        }
    }

    if (!bNearObject)
    {
        // If not near any MTA objects, then don't bother waiting
        SetFrozenWaitingForGroundToLoad(false, true);
        #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
        OutputDebugLine("[AsyncLoading]   FreezeUntilCollisionLoaded - Early stop");
        #endif
        return;
    }

    // Reset position
    CVector vecTemp;
    m_pVehicle->SetMatrix(&m_matFrozen);
    m_pVehicle->SetMoveSpeed(&vecTemp);
    m_pVehicle->SetTurnSpeed(&vecTemp);
    m_vecMoveSpeed = vecTemp;
    m_vecTurnSpeed = vecTemp;

    // Load load load
    if (GetModelInfo())
        g_pGame->GetStreaming()->LoadAllRequestedModels(false, "CClientVehicle::HandleWaitingForGroundToLoad");

    // Start out with a fairly big radius to check, and shrink it down over time
    float fUseRadius = 50.0f * (1.f - std::max(0.f, m_fObjectsAroundTolerance));

    // Gather up some flags
    CClientObjectManager* pObjectManager = g_pClientGame->GetObjectManager();
    bool                  bASync = g_pGame->IsASyncLoadingEnabled();
    bool                  bMTAObjLimit = pObjectManager->IsObjectLimitReached();
    bool                  bHasModel = GetModelInfo() != NULL;
    #ifndef ASYNC_LOADING_DEBUG_OUTPUTA
    bool bMTALoaded = pObjectManager->ObjectsAroundPointLoaded(vecPosition, fUseRadius, m_usDimension);
    #else
    SString strAround;
    bool    bMTALoaded = pObjectManager->ObjectsAroundPointLoaded(vecPosition, fUseRadius, m_usDimension, &strAround);
    #endif

    #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
    SString status = SString(
        "%2.2f,%2.2f,%2.2f  bASync:%d   bHasModel:%d   bMTALoaded:%d   bMTAObjLimit:%d   m_fGroundCheckTolerance:%2.2f   m_fObjectsAroundTolerance:%2.2f  "
        "fUseRadius:%2.1f",
        vecPosition.fX, vecPosition.fY, vecPosition.fZ, bASync, bHasModel, bMTALoaded, bMTAObjLimit, m_fGroundCheckTolerance, m_fObjectsAroundTolerance,
        fUseRadius);
    #endif

    // See if ground is ready
    if ((!bHasModel || !bMTALoaded) && m_fObjectsAroundTolerance < 1.f)
    {
        m_fGroundCheckTolerance = 0.f;
        m_fObjectsAroundTolerance = std::min(1.f, m_fObjectsAroundTolerance + 0.01f);
        #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
        status += ("  FreezeUntilCollisionLoaded - wait");
        #endif
    }
    else
    {
        // Models should be loaded, but sometimes the collision is still not ready
        // Do a ground distance check to make sure.
        // Make the check tolerance larger with each passing frame
        m_fGroundCheckTolerance = std::min(1.f, m_fGroundCheckTolerance + 0.01f);
        float fDist = GetDistanceFromGround();
        float fUseDist = fDist * (1.f - m_fGroundCheckTolerance);
        if (fUseDist > -0.2f && fUseDist < 1.5f)
            SetFrozenWaitingForGroundToLoad(false, true);

        #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
        status += (SString("  GetDistanceFromGround:  fDist:%2.2f   fUseDist:%2.2f", fDist, fUseDist));
        #endif

        // Stop waiting after 3 frames, if the object limit has not been reached. (bASync should always be false here)
        if (m_fGroundCheckTolerance > 0.03f /*&& !bMTAObjLimit*/ && !bASync)
            SetFrozenWaitingForGroundToLoad(false, true);
    }

    #ifdef ASYNC_LOADING_DEBUG_OUTPUTA
    OutputDebugLine(SStringX("[AsyncLoading] ") + status);
    g_pCore->GetGraphics()->DrawString(10, 220, -1, 1, status);

    std::vector<SString> lineList;
    strAround.Split("\n", lineList);
    for (unsigned int i = 0; i < lineList.size(); i++)
        g_pCore->GetGraphics()->DrawString(10, 230 + i * 10, -1, 1, lineList[i]);
    #endif
}

bool CClientVehicle::GiveVehicleSirens(unsigned char ucSirenType, unsigned char ucSirenCount)
{
    m_tSirenBeaconInfo.m_bOverrideSirens = true;
    m_tSirenBeaconInfo.m_ucSirenType = ucSirenType;
    m_tSirenBeaconInfo.m_ucSirenCount = ucSirenCount;

    if (m_pVehicle)
        m_pVehicle->GiveVehicleSirens(ucSirenType, ucSirenCount);

    return true;
}
void CClientVehicle::SetVehicleSirenPosition(unsigned char ucSirenID, CVector vecPos)
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_vecSirenPositions = vecPos;

    if (m_pVehicle)
        m_pVehicle->SetVehicleSirenPosition(ucSirenID, vecPos);
}

void CClientVehicle::SetVehicleSirenMinimumAlpha(unsigned char ucSirenID, DWORD dwPercentage)
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_dwMinSirenAlpha = dwPercentage;

    if (m_pVehicle)
        m_pVehicle->SetVehicleSirenMinimumAlpha(ucSirenID, dwPercentage);
}

void CClientVehicle::SetVehicleSirenColour(unsigned char ucSirenID, SColor tVehicleSirenColour)
{
    m_tSirenBeaconInfo.m_tSirenInfo[ucSirenID].m_RGBBeaconColour = tVehicleSirenColour;

    if (m_pVehicle)
        m_pVehicle->SetVehicleSirenColour(ucSirenID, tVehicleSirenColour);
}

void CClientVehicle::SetVehicleFlags(bool bEnable360, bool bEnableRandomiser, bool bEnableLOSCheck, bool bEnableSilent)
{
    m_tSirenBeaconInfo.m_b360Flag = bEnable360;
    m_tSirenBeaconInfo.m_bDoLOSCheck = bEnableLOSCheck;
    m_tSirenBeaconInfo.m_bUseRandomiser = bEnableRandomiser;
    m_tSirenBeaconInfo.m_bSirenSilent = bEnableSilent;

    if (m_pVehicle)
        m_pVehicle->SetVehicleFlags(bEnable360, bEnableRandomiser, bEnableLOSCheck, bEnableSilent);
}

void CClientVehicle::RemoveVehicleSirens()
{
    if (m_pVehicle)
        m_pVehicle->RemoveVehicleSirens();

    m_tSirenBeaconInfo.m_bOverrideSirens = false;
    SetSirenOrAlarmActive(false);

    for (unsigned char i = 0; i < 7; i++)
    {
        SetVehicleSirenPosition(i, CVector(0, 0, 0));
        SetVehicleSirenMinimumAlpha(i, 0);
        SetVehicleSirenColour(i, SColor());
    }

    m_tSirenBeaconInfo.m_ucSirenCount = 0;
}

bool CClientVehicle::SetComponentPosition(const SString& vehicleComponent, CVector vecPosition, EComponentBaseType inputBase)
{
    // Ensure position is parent relative
    ConvertComponentPositionBase(vehicleComponent, vecPosition, inputBase, EComponentBase::PARENT);

    if (m_pVehicle)
    {
        // set our position on the model
        if (m_pVehicle->SetComponentPosition(vehicleComponent, vecPosition))
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_vecComponentPosition = vecPosition;
            m_ComponentData[vehicleComponent].m_bPositionChanged = true;

            return true;
        }
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_vecComponentPosition = vecPosition;
            m_ComponentData[vehicleComponent].m_bPositionChanged = true;
            return true;
        }
    }
    return false;
}

bool CClientVehicle::GetComponentPosition(const SString& vehicleComponent, CVector& vecPosition, EComponentBaseType outputBase)
{
    if (m_pVehicle)
    {
        // fill our position from the actual position
        if (m_pVehicle->GetComponentPosition(vehicleComponent, vecPosition))
        {
            // Convert to required base
            ConvertComponentPositionBase(vehicleComponent, vecPosition, EComponentBase::PARENT, outputBase);
            return true;
        }
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // fill our position from the cached position
            vecPosition = m_ComponentData[vehicleComponent].m_vecComponentPosition;

            // Convert to required base
            ConvertComponentPositionBase(vehicleComponent, vecPosition, EComponentBase::PARENT, outputBase);
            return true;
        }
    }
    return false;
}

bool CClientVehicle::SetComponentRotation(const SString& vehicleComponent, CVector vecRotation, EComponentBaseType inputBase)
{
    // Ensure rotation is parent relative
    ConvertComponentRotationBase(vehicleComponent, vecRotation, inputBase, EComponentBase::PARENT);

    if (m_pVehicle)
    {
        // set our rotation on the model
        if (m_pVehicle->SetComponentRotation(vehicleComponent, vecRotation))
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_vecComponentRotation = vecRotation;
            m_ComponentData[vehicleComponent].m_bRotationChanged = true;

            return true;
        }
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_vecComponentRotation = vecRotation;
            m_ComponentData[vehicleComponent].m_bRotationChanged = true;
            return true;
        }
    }
    return false;
}

bool CClientVehicle::GetComponentRotation(const SString& vehicleComponent, CVector& vecRotation, EComponentBaseType outputBase)
{
    if (m_pVehicle)
    {
        // fill our rotation from the actual rotation
        bool bResult = m_pVehicle->GetComponentRotation(vehicleComponent, vecRotation);

        // Convert to required base
        ConvertComponentRotationBase(vehicleComponent, vecRotation, EComponentBase::PARENT, outputBase);

        return bResult;
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // fill our rotation from the cached rotation
            vecRotation = m_ComponentData[vehicleComponent].m_vecComponentRotation;

            // Convert to required base
            ConvertComponentRotationBase(vehicleComponent, vecRotation, EComponentBase::PARENT, outputBase);
            return true;
        }
    }
    return false;
}

bool CClientVehicle::SetComponentScale(const SString& vehicleComponent, CVector vecScale, EComponentBaseType inputBase)
{
    // Ensure scale is parent relative
    ConvertComponentScaleBase(vehicleComponent, vecScale, inputBase, EComponentBase::PARENT);

    if (m_pVehicle)
    {
        // set our rotation on the model
        if (m_pVehicle->SetComponentScale(vehicleComponent, vecScale))
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_vecComponentScale = vecScale;
            m_ComponentData[vehicleComponent].m_bScaleChanged = true;

            return true;
        }
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_vecComponentScale = vecScale;
            m_ComponentData[vehicleComponent].m_bScaleChanged = true;
            return true;
        }
    }
    return false;
}

bool CClientVehicle::GetComponentScale(const SString& vehicleComponent, CVector& vecScale, EComponentBaseType outputBase)
{
    if (m_pVehicle)
    {
        // fill our scale from the actual rotation
        bool bResult = m_pVehicle->GetComponentScale(vehicleComponent, vecScale);

        // Convert to required base
        ConvertComponentScaleBase(vehicleComponent, vecScale, EComponentBase::PARENT, outputBase);

        return bResult;
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // fill our rotation from the cached rotation
            vecScale = m_ComponentData[vehicleComponent].m_vecComponentScale;

            // Convert to required base
            ConvertComponentScaleBase(vehicleComponent, vecScale, EComponentBase::PARENT, outputBase);
            return true;
        }
    }
    return false;
}

bool CClientVehicle::ResetComponentScale(const SString& vehicleComponent)
{
    // set our rotation on the model
    if (SetComponentScale(vehicleComponent, m_ComponentData[vehicleComponent].m_vecOriginalComponentScale))
    {
        // update our cache
        m_ComponentData[vehicleComponent].m_bScaleChanged = false;
        return true;
    }
    return false;
}
bool CClientVehicle::ResetComponentRotation(const SString& vehicleComponent)
{
    // set our rotation on the model
    if (SetComponentRotation(vehicleComponent, m_ComponentData[vehicleComponent].m_vecOriginalComponentRotation))
    {
        // update our cache
        m_ComponentData[vehicleComponent].m_bRotationChanged = false;
        return true;
    }
    return false;
}
bool CClientVehicle::ResetComponentPosition(const SString& vehicleComponent)
{
    // set our position on the model
    if (SetComponentPosition(vehicleComponent, m_ComponentData[vehicleComponent].m_vecOriginalComponentPosition))
    {
        // update our cache
        m_ComponentData[vehicleComponent].m_bPositionChanged = false;
        return true;
    }
    return false;
}

//
// Get transform from component parent to the model root
//
void CClientVehicle::GetComponentParentToRootMatrix(const SString& vehicleComponent, CMatrix& matOutParentToRoot)
{
    if (m_pVehicle)
    {
        if (m_pVehicle->GetComponentParentToRootMatrix(vehicleComponent, matOutParentToRoot))
        {
            return;
        }
    }
    else
    {
        // Get first parent
        SVehicleComponentData* pComponentData = MapFind(m_ComponentData, vehicleComponent);
        if (pComponentData)
        {
            pComponentData = MapFind(m_ComponentData, pComponentData->m_strParentName);
        }

        // Combine transforms of parent components (limit to 10 in case of problems)
        CMatrix matCombo;
        for (uint i = 0; pComponentData && i < 10; i++)
        {
            CMatrix matFrame(pComponentData->m_vecComponentPosition, pComponentData->m_vecComponentRotation, pComponentData->m_vecComponentScale);
            matCombo = matCombo * matFrame;
            pComponentData = MapFind(m_ComponentData, pComponentData->m_strParentName);
        }

        matOutParentToRoot = matCombo;
    }
}

//
// Change what a component rotation/position/matrix is relative to
//
void CClientVehicle::ConvertComponentRotationBase(const SString& vehicleComponent, CVector& vecRotation, EComponentBaseType inputBase,
                                                  EComponentBaseType outputBase)
{
    if (inputBase != outputBase)
    {
        CMatrix matTemp(CVector(), vecRotation);
        ConvertComponentMatrixBase(vehicleComponent, matTemp, inputBase, outputBase);
        vecRotation = matTemp.GetRotation();
    }
}

void CClientVehicle::ConvertComponentPositionBase(const SString& vehicleComponent, CVector& vecPosition, EComponentBaseType inputBase,
                                                  EComponentBaseType outputBase)
{
    if (inputBase != outputBase)
    {
        CMatrix matTemp(vecPosition);
        ConvertComponentMatrixBase(vehicleComponent, matTemp, inputBase, outputBase);
        vecPosition = matTemp.GetPosition();
    }
}

void CClientVehicle::ConvertComponentScaleBase(const SString& vehicleComponent, CVector& vecScale, EComponentBaseType inputBase, EComponentBaseType outputBase)
{
    if (inputBase != outputBase)
    {
        CMatrix matTemp(CVector(), CVector(), vecScale);
        ConvertComponentMatrixBase(vehicleComponent, matTemp, inputBase, outputBase);
        vecScale = matTemp.GetScale();
    }
}

void CClientVehicle::ConvertComponentMatrixBase(const SString& vehicleComponent, CMatrix& matOrientation, EComponentBaseType inputBase,
                                                EComponentBaseType outputBase)
{
    if (inputBase == outputBase)
        return;

    if (inputBase == EComponentBaseType::PARENT)
    {
        if (outputBase == EComponentBaseType::ROOT)
        {
            // Parent relative to root relative
            CMatrix matParentToRoot;
            GetComponentParentToRootMatrix(vehicleComponent, matParentToRoot);
            matOrientation = matOrientation * matParentToRoot;
        }
        else if (outputBase == EComponentBaseType::WORLD)
        {
            // Parent relative to world
            CMatrix matParentToRoot;
            GetComponentParentToRootMatrix(vehicleComponent, matParentToRoot);
            matOrientation = matOrientation * matParentToRoot;

            CMatrix matRootToWorld;
            GetMatrix(matRootToWorld);
            matOrientation = matOrientation * matRootToWorld;
        }
    }
    else if (inputBase == EComponentBaseType::ROOT)
    {
        if (outputBase == EComponentBaseType::PARENT)
        {
            // Root relative to parent relative
            CMatrix matParentToRoot;
            GetComponentParentToRootMatrix(vehicleComponent, matParentToRoot);
            matOrientation = matOrientation * matParentToRoot.Inverse();
        }
        else if (outputBase == EComponentBaseType::WORLD)
        {
            // Root relative to world
            CMatrix matRootToWorld;
            GetMatrix(matRootToWorld);
            matOrientation = matOrientation * matRootToWorld;
        }
    }
    else if (inputBase == EComponentBaseType::WORLD)
    {
        if (outputBase == EComponentBaseType::PARENT)
        {
            // World to parent relative
            CMatrix matRootToWorld;
            GetMatrix(matRootToWorld);
            matOrientation = matOrientation * matRootToWorld.Inverse();

            CMatrix matParentToRoot;
            GetComponentParentToRootMatrix(vehicleComponent, matParentToRoot);
            matOrientation = matOrientation * matParentToRoot.Inverse();
        }
        else if (outputBase == EComponentBaseType::ROOT)
        {
            // World to root relative
            CMatrix matRootToWorld;
            GetMatrix(matRootToWorld);
            matOrientation = matOrientation * matRootToWorld.Inverse();
        }
    }
}

bool CClientVehicle::SetComponentVisible(const SString& vehicleComponent, bool bVisible)
{
    // Check if wheel invisibility override is in operation due to setting of wheel states
    if (bVisible && GetWheelMissing(UCHAR_INVALID_INDEX, vehicleComponent))
        bVisible = false;

    if (m_pVehicle)
    {
        if (m_pVehicle->SetComponentVisible(vehicleComponent, bVisible))
        {
            // update our cache
            m_ComponentData[vehicleComponent].m_bVisible = bVisible;

            // set our visibility on the model
            m_pVehicle->SetComponentVisible(vehicleComponent, bVisible);
            return true;
        }
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // store our visible variable to the cached data
            m_ComponentData[vehicleComponent].m_bVisible = bVisible;
            return true;
        }
    }
    return false;
}

bool CClientVehicle::GetComponentVisible(const SString& vehicleComponent, bool& bVisible)
{
    if (m_pVehicle)
    {
        // fill our visible variable from the actual position
        return m_pVehicle->GetComponentVisible(vehicleComponent, bVisible);
    }
    else
    {
        if (m_ComponentData.find(vehicleComponent) != m_ComponentData.end())
        {
            // fill our visible variable from the cached data
            bVisible = m_ComponentData[vehicleComponent].m_bVisible;
            return true;
        }
    }
    return false;
}

bool CClientVehicle::DoesSupportUpgrade(const SString& strFrameName)
{
    return m_pVehicle ? m_pVehicle->DoesSupportUpgrade(strFrameName) : true;
}

bool CClientVehicle::OnVehicleFallThroughMap()
{
    // if we have fallen through the map a small number of times
    if (m_ucFellThroughMapCount <= 2)
    {
        // make sure we haven't moved much if at all
        if (IsFrozen() == false && DistanceBetweenPoints2D(m_matCreate.GetPosition(), m_Matrix.GetPosition()) < 3)
        {
            // increase our fell through map count
            m_ucFellThroughMapCount++;
            // warp us to our initial position of creation
            SetPosition(m_matCreate.GetPosition());
            // warp us to our initial position of creation
            SetRotationRadians(m_matCreate.GetRotation());
            // handled
            return true;
        }
    }
    // unhandled
    return false;
}

bool CClientVehicle::GetDummyPosition(eVehicleDummies dummy, CVector& position) const
{
    if (dummy >= 0 && dummy < VEHICLE_DUMMY_COUNT)
    {
        position = m_dummyPositions[dummy];
        return true;
    }

    return false;
}

bool CClientVehicle::SetDummyPosition(eVehicleDummies dummy, const CVector& position)
{
    if (dummy >= 0 && dummy < VEHICLE_DUMMY_COUNT)
    {
        m_dummyPositions[dummy] = position;
        m_copyDummyPositions = false;

        return m_pVehicle ? m_pVehicle->SetDummyPosition(dummy, position) : true;
    }

    return false;
}

bool CClientVehicle::ResetDummyPositions()
{
    if (m_pVehicle)
    {
        std::array<CVector, VEHICLE_DUMMY_COUNT> positions;

        if (!m_pModelInfo->GetVehicleDummyPositions(positions))
            return false;

        for (size_t i = 0; i < positions.size(); ++i)
        {
            SetDummyPosition(static_cast<eVehicleDummies>(i), positions[i]);
        }

        return true;
    }
    else
    {
        if (m_copyDummyPositions)
            return false;

        m_copyDummyPositions = true;
        m_dummyPositions = {};
        return true;
    }
}

bool CClientVehicle::DoesNeedToWaitForGroundToLoad()
{
    if (!g_pGame->IsASyncLoadingEnabled())
        return false;

    // Let CClientPed handle it if our driver is the local player
    if (m_pDriver == g_pClientGame->GetLocalPlayer())
        return false;

    // If we're in water we won't need to wait for the ground to load
    if (m_LastSyncedData != NULL && m_LastSyncedData->bIsInWater == true)
        return false;

    // Check for MTA objects around our position
    CVector vecPosition;
    GetPosition(vecPosition);
    CClientObjectManager* pObjectManager = g_pClientGame->GetObjectManager();

    return !pObjectManager->ObjectsAroundPointLoaded(vecPosition, 50.0f, m_usDimension);
}

void CClientVehicle::SetNitroLevel(float fNitroLevel)
{
    if (m_pVehicle)
        m_pVehicle->SetNitroLevel(fNitroLevel);

    m_fNitroLevel = fNitroLevel;
}

float CClientVehicle::GetNitroLevel()
{
    return m_pVehicle ? m_pVehicle->GetNitroLevel() : m_fNitroLevel;
}

void CClientVehicle::SetNitroCount(char cNitroCount)
{
    if (m_pVehicle)
        m_pVehicle->SetNitroCount(cNitroCount);

    m_cNitroCount = cNitroCount;
}

char CClientVehicle::GetNitroCount()
{
    if (m_pVehicle)
        return m_pVehicle->GetNitroCount();

    return m_cNitroCount;
}

bool CClientVehicle::SetWindowOpen(uchar ucWindow, bool bOpen)
{
    if (ucWindow < MAX_WINDOWS)
    {
        m_bWindowOpen[ucWindow] = bOpen;
        return m_pVehicle ? m_pVehicle->SetWindowOpenFlagState(ucWindow, bOpen) : true;
    }

    return false;
}

bool CClientVehicle::IsWindowOpen(uchar ucWindow)
{
    return (ucWindow < MAX_WINDOWS) ? m_bWindowOpen[ucWindow] : false;
}

void CClientVehicle::SetWheelScale(float fWheelScale)
{
    if (m_pVehicle)
        m_pVehicle->SetWheelScale(fWheelScale);

    m_fWheelScale = fWheelScale;
    m_bWheelScaleChanged = true;
}

float CClientVehicle::GetWheelScale()
{
    return m_pVehicle ? m_pVehicle->GetWheelScale() : m_fWheelScale;
}

// This function is meant to be called after GTA resets wheel scale
// (i.e. after installing a wheel upgrade)
void CClientVehicle::ResetWheelScale()
{
    assert(m_pUpgrades);

    // The calculation of the default wheel scale is based on original GTA code at functions
    // 0x6E3290 (CVehicle::AddVehicleUpgrade) and 0x6DF930 (CVehicle::RemoveVehicleUpgrade)
    if (m_pUpgrades->GetSlotState(12) != 0)
        m_fWheelScale = m_pModelInfo->GetVehicleWheelSize(eResizableVehicleWheelGroup::FRONT_AXLE);
    else
        m_fWheelScale = 1.0f;

    m_bWheelScaleChanged = false;
}

bool CClientVehicle::SpawnFlyingComponent(const eCarNodes& nodeID, const eCarComponentCollisionTypes& collisionType, std::int32_t removalTime)
{
    if (!m_pVehicle)
        return false;

    return m_pVehicle->SpawnFlyingComponent(nodeID, collisionType, removalTime);
}
 
CVector CClientVehicle::GetEntryPoint(std::uint32_t entryPointIndex)
{
    static const uint32_t lookup[4] = {10, 8, 11, 9};
    assert(entryPointIndex < 4);
    const std::uint32_t saDoorIndex = lookup[entryPointIndex];

    CVector      entryPoint;
    CVehicle*    gameVehicle = GetGameVehicle();

    g_pGame->GetCarEnterExit()->GetPositionToOpenCarDoor(entryPoint, gameVehicle, saDoorIndex);

    return entryPoint;
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <algorithm>
#include <game/CPools.h>
#include <game/CTrainTrackManager.h>
#include "../game_sa/CTrainSA.h"

////////////////////////////////////////////////////////////////////////
// CTrain::ProcessControl
//
// The game only ever expects trackIDs 0-3 (its 4 built-in tracks), and indexes fixed-size
// arrays with that ID without any bounds checking. This hook sits right at the top of the
// function: for a normal train it re-executes the few bytes it had to overwrite and lets the
// original code run untouched, but for a train on a custom track (trackID >= 4) it skips the
// original function entirely and moves the train along our own track data instead, since the
// original code would read out of bounds on those fixed arrays.
//
// Stations, passenger boarding and the derail-on-sharp-turn check are all part of the vanilla
// station/track system and don't apply to a track a script drew, so those are left out here.
//
//     0x6F86A0 | 83 EC 7C          | sub  esp, 7Ch
//     0x6F86A3 | 53                | push ebx
//     0x6F86A4 | 55                | push ebp
// >>> 0x6F86A5 | 56                | push esi
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTrain__ProcessControl_Custom  0x6F86A0
#define HOOKSIZE_CTrain__ProcessControl_Custom 5
static DWORD CONTINUE_CTrain__ProcessControl_Custom = 0x6F86A5;

static bool _cdecl IsCustomTrackTrain(CTrainSAInterface* train)
{
    return train->m_ucRailTrackID >= CTrainTrackManager::FIRST_CUSTOM_TRACK_ID;
}

static void _cdecl ProcessCustomTrackTrain(CTrainSAInterface* train)
{
    if (train->trainFlags.bIsDerailed)
        return;

    CTrainTrack* pTrack = pGameInterface->GetTrainTrackManager()->GetTrainTrack(train->m_ucRailTrackID);
    if (!pTrack)
        return;  // The track was destroyed (or never existed); leave the train where it is

    const std::vector<STrainTrackNodeSA>& nodes = pTrack->GetNodes();
    if (nodes.size() < 2)
        return;

    std::size_t numNodes = nodes.size();
    bool        bLooped = pTrack->GetLastNodesLinked();
    float       fTotalLength = pTrack->GetTotalLength();
    float       fTimeStep = pGameInterface->GetTimeStep();

    // Following carriages just mirror the one ahead; only the lead carriage actually advances
    if (train->m_prevCarriage)
    {
        train->m_fTrainSpeed = train->m_prevCarriage->m_fTrainSpeed;
        train->m_fTrainRailDistance = train->m_prevCarriage->m_fTrainRailDistance - train->m_fDistanceToNextCarriage;
    }
    else
    {
        train->m_fTrainSpeed *= std::pow(0.999750018119812f, fTimeStep);
        train->m_fTrainRailDistance += fTimeStep * train->m_fTrainSpeed;
    }

    // A looped track wraps the distance around; an open one just stops the train at either end
    if (bLooped)
    {
        while (train->m_fTrainRailDistance < 0.0f)
            train->m_fTrainRailDistance += fTotalLength;
        while (train->m_fTrainRailDistance >= fTotalLength)
            train->m_fTrainRailDistance -= fTotalLength;
    }
    else
    {
        train->m_fTrainRailDistance = std::clamp(train->m_fTrainRailDistance, 0.0f, fTotalLength);
    }

    // Find the pair of nodes the current rail distance falls between
    std::size_t currentIndex = numNodes - 1;
    for (std::size_t i = 0; i < numNodes; i++)
    {
        std::size_t nextOfI = (i + 1) % numNodes;
        float       fNextDistance = (nextOfI == 0) ? fTotalLength : nodes[nextOfI].distanceFromStart;
        if (train->m_fTrainRailDistance >= nodes[i].distanceFromStart && train->m_fTrainRailDistance <= fNextDistance)
        {
            currentIndex = i;
            break;
        }
    }

    std::size_t nextIndex = (currentIndex + 1) % numNodes;
    if (nextIndex == 0 && !bLooped)
        nextIndex = currentIndex;

    const CVector& vecCurrentNode = nodes[currentIndex].position;
    const CVector& vecNextNode = nodes[nextIndex].position;

    float fSegmentStart = nodes[currentIndex].distanceFromStart;
    float fSegmentEnd = (currentIndex == numNodes - 1) ? fTotalLength : nodes[nextIndex].distanceFromStart;
    float fSegmentLength = fSegmentEnd - fSegmentStart;
    float fAlpha = (fSegmentLength > 0.0f) ? (train->m_fTrainRailDistance - fSegmentStart) / fSegmentLength : 0.0f;
    fAlpha = std::clamp(fAlpha, 0.0f, 1.0f);

    CVector vecPosition = vecCurrentNode * (1.0f - fAlpha) + vecNextNode * fAlpha;

    CMatrix matrix;
    matrix.vPos = vecPosition;
    matrix.vFront = vecNextNode - vecCurrentNode;
    if (matrix.vFront.Length() > 0.0001f)
        matrix.vFront.Normalize();
    else
        matrix.vFront = CVector(0.0f, 1.0f, 0.0f);

    if (!train->trainFlags.bClockwiseDirection)
        matrix.vFront = matrix.vFront * -1.0f;

    CVector vecWorldUp(0.0f, 0.0f, 1.0f);
    matrix.vRight = matrix.vFront;
    matrix.vRight.CrossProduct(&vecWorldUp);
    if (matrix.vRight.Length() > 0.0001f)
        matrix.vRight.Normalize();
    else
        matrix.vRight = CVector(1.0f, 0.0f, 0.0f);

    matrix.vUp = matrix.vRight;
    matrix.vUp.CrossProduct(&matrix.vFront);

    SClientEntity<CVehicleSA>* pClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)train);
    if (pClientEntity && pClientEntity->pEntity)
        pClientEntity->pEntity->SetMatrix(&matrix);
}

static void __declspec(naked) HOOK_CTrain__ProcessControl_Custom()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push ecx
        call IsCustomTrackTrain
        add esp, 4
        test al, al
        popad
        jz isDefaultTrack

        push ecx
        call ProcessCustomTrackTrain
        add esp, 4
        retn

    isDefaultTrack:
        sub esp, 0x7C
        push ebx
        push ebp
        jmp CONTINUE_CTrain__ProcessControl_Custom
    }
    // clang-format on
}

void CMultiplayerSA::InitHooks_TrainTracks()
{
    EZHookInstall(CTrain__ProcessControl_Custom);
}

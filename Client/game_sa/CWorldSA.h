/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWorldSA.h
 *  PURPOSE:     Header file for game world
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CWorld.h>
#include <cstdint>
#include <vector>

#define FUNC_Add                               0x563220
#define FUNC_Remove                            0x563280
#define FUNC_ProcessLineOfSight                0x56BA00
#define FUNC_FindGroundZFor3DCoord             0x5696C0
#define FUNC_IsLineOfSightClear                0x56A490
#define FUNC_HasCollisionBeenLoaded            0x410CE0
#define FUNC_RemoveBuildingsNotInArea          0x4094B0
#define FUNC_RemoveReferencesToDeletedObject   0x565510
#define FUNC_COcclusion_ProcessBeforeRendering 0x7201C0
#define VAR_COcclusion_NumActiveOccluders      0xC73CC0

// COcclusion::AddOne is __cdecl with 11 stack arguments: centre xyz, three full extents, three euler
// angles in degrees, an unused flag and the interior selector as a byte. It appends an 18 byte entry
// to one of two fixed arrays and returns nothing, dropping the call silently when the array is full.
// The centre is stored as int16 in quarter units, the extents are truncated to whole units first and
// the angles are byte angles of 360/256 degrees; all of them truncate toward zero.
#define FUNC_COcclusion_Init                0x71DCA0
#define FUNC_COcclusion_AddOne              0x71DCD0
#define VAR_COcclusion_NumOccluders         0xC73F98
#define ARRAY_COcclusion_Occluders          0xC73FA0
#define VAR_COcclusion_NumInteriorOccluders 0xC73CC4
#define ARRAY_COcclusion_InteriorOccluders  0xC73CC8

// The exterior count is only a bump allocator that the renderer never reads: membership of the
// exterior array is carried by singly linked lists threaded through the 16 bit word at entry+0x10,
// whose heads and cursor live here. The interior array has no lists and is scanned up to its count.
// Editing the lists by hand can cross link them, which hangs the render loop, so every removal
// rebuilds the array from a snapshot instead.
// Four 16 bit words four bytes apart: the far list head, the near list head, the walk cursor and the
// entry before it. COcclusion::Init writes 0xFFFF to all four.
#define VAR_COcclusion_ListHeads 0x8D5D68

#define COCCLUSION_MAX_OCCLUDERS                     1000
#define COCCLUSION_MAX_INTERIOR_OCCLUDERS            40
#define COCCLUSION_ENTRY_SIZE                        18
#define CALL_CCullZones_FindTunnelAttributesForCoors 0x55570D
#define FUNC_CWorld_FindPositionForTrackPosition     0x6F59E0
#define FUNC_CWorld_TestSphereAgainstWorld           0x569E20

#define VAR_IgnoredEntity           0xB7CD68
#define VAR_currArea                0xB72914
#define ARRAY_StreamSectors         0xB7D0B8
#define NUM_StreamSectorRows        120
#define NUM_StreamSectorCols        120
#define ARRAY_LodPtrLists           0xB99EB8
#define NUM_LodPtrListRows          30
#define NUM_LodPtrListCols          30
#define ARRAY_StreamRepeatSectors   0xB992B8
#define NUM_StreamRepeatSectorRows  16
#define NUM_StreamRepeatSectorCols  16
#define VAR_fJetpackMaxHeight       0x8703D8
#define VTBL_CBUILDING              0x8585C8
#define VAR_CWorld_bIncludeCarTires 0xB7CD70

class CWorldSA : public CWorld
{
public:
    CWorldSA();
    void  InstallHooks();
    void  Add(CEntity* entity, eDebugCaller CallerId);
    void  Add(CEntitySAInterface* entityInterface, eDebugCaller CallerId);
    void  Remove(CEntity* entity, eDebugCaller CallerId);
    void  Remove(CEntitySAInterface* entityInterface, eDebugCaller CallerId);
    void  RemoveReferencesToDeletedObject(CEntitySAInterface* entity);
    auto  ProcessLineAgainstMesh(CEntitySAInterface* e, CVector start, CVector end) -> SProcessLineOfSightMaterialInfoResult override;
    bool  ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity, const SLineOfSightFlags flags,
                             SLineOfSightBuildingResult* pBuildingResult, SProcessLineOfSightMaterialInfoResult* outMatInfo = nullptr);
    void  IgnoreEntity(CEntity* entity);
    float FindGroundZFor3DPosition(CVector* vecPosition);
    float FindRoofZFor3DCoord(CVector* pvecPosition, bool* pbOutResult);
    bool  IsLineOfSightClear(const CVector* vecStart, const CVector* vecEnd, const SLineOfSightFlags flags);
    bool  HasCollisionBeenLoaded(CVector* vecPosition);
    DWORD GetCurrentArea();
    void  SetCurrentArea(DWORD dwArea);
    void  SetJetpackMaxHeight(float fHeight);
    float GetJetpackMaxHeight();
    void  SetAircraftMaxHeight(float fHeight);
    float GetAircraftMaxHeight();
    void  SetAircraftMaxVelocity(float fVelocity);
    float GetAircraftMaxVelocity();
    void  SetOcclusionsEnabled(bool bEnabled);
    bool  GetOcclusionsEnabled();
    bool  AddOccluder(const CVector& vecPosition, const CVector& vecSize, const CVector& vecRotation, bool bInterior, void* pChangeSource, uint& uiOutId);
    bool  RemoveOccluder(uint uiId, void* pChangeSource);
    bool  RestoreOccluder(uint uiId, void* pChangeSource);
    uint  RemoveOccludersInRadius(const CVector& vecPosition, float fRadius, bool bInterior, void* pChangeSource);
    uint  RestoreOccludersInRadius(const CVector& vecPosition, float fRadius, bool bInterior, void* pChangeSource);
    void  GetOccluders(bool bInterior, std::vector<SOccluderInfo>& outList);
    void  GetOccluderCapacity(bool bInterior, uint& uiOutUsed, uint& uiOutFree);
    void  UndoOccluderChanges(void* pChangeSource = nullptr);
    void  FindWorldPositionForRailTrackPosition(float fRailTrackPosition, int iTrackId, CVector* pOutVecPosition);
    int   FindClosestRailTrackNode(const CVector& vecPosition, uchar& ucOutTrackId, float& fOutRailDistance);
    bool  CalculateImpactPosition(const CVector& vecInputStart, CVector& vecInputEnd);

    CSurfaceType* GetSurfaceInfo() override;
    void          ResetAllSurfaceInfo() override;
    bool          ResetSurfaceInfo(short sSurfaceID) override;

    CEntity* TestSphereAgainstWorld(const CVector& sphereCenter, float radius, CEntity* ignoredEntity, bool checkBuildings, bool checkVehicles, bool checkPeds,
                                    bool checkObjects, bool checkDummies, bool cameraIgnore, STestSphereAgainstWorldResult& result) override;

private:
    // Every scripted occluder is kept here and replayed through COcclusion::AddOne after the vanilla
    // array is restored from the snapshot, so the engine always builds the lists itself.
    struct SScriptedOccluder
    {
        uint    uiId;
        CVector vecPosition;
        CVector vecSize;
        CVector vecRotation;
        bool    bInterior;
        void*   pChangeSource;
    };

    // The map's own occluders are kept exactly as the loader left them. Disabling one is a set
    // membership on the MTA side, so a rebuild can leave it out and hand its slot to a script.
    struct SVanillaOccluder
    {
        uint            uiId;
        bool            bInterior;
        std::uint8_t    aBytes[COCCLUSION_ENTRY_SIZE];
        std::set<void*> setDisabledBy;
        bool            bActive{true};
    };

    void        CaptureOccluderBaseline();
    void        RebuildOccluders();
    void        RestartOccluderListWalk();
    uint        CountEnabledVanillaOccluders(bool bInterior) const;
    uint        CountScriptedOccluders(bool bInterior) const;
    bool        IsOccluderPoolFull(bool bInterior) const;
    static uint GetOccluderPoolMax(bool bInterior);

    // True when lifting this source's mark is the one that puts the occluder back in the array
    static bool WouldReEnable(const SVanillaOccluder& occluder, void* pChangeSource)
    {
        return occluder.setDisabledBy.size() == 1 && occluder.setDisabledBy.count(pChangeSource) == 1;
    }

    bool                           m_bOccluderBaselineTaken = false;
    uint                           m_uiNextOccluderId = 1;
    std::vector<SScriptedOccluder> m_ScriptedOccluders;
    std::vector<SVanillaOccluder>  m_VanillaOccluders;

    float         m_fAircraftMaxHeight;
    CSurfaceType* m_pSurfaceInfo;
};

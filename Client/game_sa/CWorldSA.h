/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWorldSA.h
 *  PURPOSE:     Header file for game world
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CWorld.h>

#define FUNC_Add                                            0x563220
#define FUNC_Remove                                         0x563280
#define FUNC_ProcessLineOfSight                             0x56BA00
#define FUNC_FindGroundZFor3DCoord                          0x5696C0
#define FUNC_IsLineOfSightClear                             0x56A490
#define FUNC_HasCollisionBeenLoaded                         0x410CE0
#define FUNC_RemoveBuildingsNotInArea                       0x4094B0
#define FUNC_RemoveReferencesToDeletedObject                0x565510
#define FUNC_COcclusion_ProcessBeforeRendering              0x7201C0
#define VAR_COcclusion_NumActiveOccluders                   0xC73CC0
#define CALL_CCullZones_FindTunnelAttributesForCoors        0x55570D
#define FUNC_CWorld_FindPositionForTrackPosition            0x6F59E0
#define FUNC_CWorld_TestSphereAgainstWorld                  0x569E20

#define VAR_IgnoredEntity                                   0xB7CD68
#define VAR_currArea                                        0xB72914
#define ARRAY_StreamSectors                                 0xB7D0B8
#define NUM_StreamSectorRows                                120
#define NUM_StreamSectorCols                                120
#define ARRAY_StreamRepeatSectors                           0xB992B8
#define NUM_StreamRepeatSectorRows                          16
#define NUM_StreamRepeatSectorCols                          16
#define VAR_fJetpackMaxHeight                               0x8703D8
#define VTBL_CBUILDING                                      0x8585C8
#define VAR_CWorld_bIncludeCarTires                         0xB7CD70

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
    void  FindWorldPositionForRailTrackPosition(float fRailTrackPosition, int iTrackId, CVector* pOutVecPosition);
    int   FindClosestRailTrackNode(const CVector& vecPosition, uchar& ucOutTrackId, float& fOutRailDistance);
    bool  CalculateImpactPosition(const CVector& vecInputStart, CVector& vecInputEnd);

    CSurfaceType* GetSurfaceInfo() override;
    void          ResetAllSurfaceInfo() override;
    bool          ResetSurfaceInfo(short sSurfaceID) override;

    CEntity* TestSphereAgainstWorld(const CVector& sphereCenter, float radius, CEntity* ignoredEntity, bool checkBuildings, bool checkVehicles, bool checkPeds, bool checkObjects, bool checkDummies, bool cameraIgnore, STestSphereAgainstWorldResult& result) override;

private:
    float                                                     m_fAircraftMaxHeight;
    CSurfaceType*                                             m_pSurfaceInfo;
};

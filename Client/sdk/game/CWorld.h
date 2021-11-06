/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWorld.h
 *  PURPOSE:     Game world interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CEntity.h"
#include "CColPoint.h"

struct SLineOfSightFlags
{
    SLineOfSightFlags()
        : bCheckBuildings(true),
          bCheckVehicles(true),
          bCheckPeds(true),
          bCheckObjects(true),
          bCheckDummies(true),
          bSeeThroughStuff(false),
          bIgnoreSomeObjectsForCamera(false),
          bShootThroughStuff(false),
          bCheckCarTires(false)
    {
    }
    bool bCheckBuildings;
    bool bCheckVehicles;
    bool bCheckPeds;
    bool bCheckObjects;
    bool bCheckDummies;
    bool bSeeThroughStuff;
    bool bIgnoreSomeObjectsForCamera;
    bool bShootThroughStuff;            // not used for IsLineOfSightClear
    bool bCheckCarTires;
};

struct SLineOfSightBuildingResult
{
    SLineOfSightBuildingResult() : bValid(false) {}
    bool                bValid;
    ushort              usModelID;
    ushort              usLODModelID;
    CVector             vecPosition;
    CVector             vecRotation;
    CEntitySAInterface* pInterface;
};

struct SBuildingRemoval
{
    SBuildingRemoval()
    {
        m_pBinaryRemoveList = new std::list<CEntitySAInterface*>;
        m_pDataRemoveList = new std::list<CEntitySAInterface*>;
        m_usModel = 0;
        m_vecPos = CVector(0, 0, 0);
        m_fRadius = 0.0f;
        m_cInterior = -1;
    }

    ~SBuildingRemoval()
    {
        delete m_pBinaryRemoveList;
        delete m_pDataRemoveList;
    }

    void AddBinaryBuilding(CEntitySAInterface* pInterface)
    {
        // Add to list of binary buildings for this removal
        m_pBinaryRemoveList->push_back(pInterface);
    }
    void AddDataBuilding(CEntitySAInterface* pInterface)
    {
        // Add to list of data buildings for this removal
        m_pDataRemoveList->push_back(pInterface);
    }

    unsigned short                  m_usModel;
    CVector                         m_vecPos;
    float                           m_fRadius;
    char                            m_cInterior;
    std::list<CEntitySAInterface*>* m_pBinaryRemoveList;
    std::list<CEntitySAInterface*>* m_pDataRemoveList;
};
struct SIPLInst
{
    CVector m_pPosition;
    CVector m_pRotation;
    float   m_fRotationCont;
    WORD    m_nModelIndex;
    BYTE    m_nInterior;
    BYTE    m_bLOD;
};

struct sDataBuildingRemovalItem
{
    sDataBuildingRemovalItem(CEntitySAInterface* pInterface, bool bData)
    {
        m_pInterface = pInterface;
        m_iCount = 0;
    }
    void                AddCount() { m_iCount++; }
    void                RemoveCount() { m_iCount--; }
    CEntitySAInterface* m_pInterface;
    int                 m_iCount;
};
struct sBuildingRemovalItem
{
    sBuildingRemovalItem(CEntitySAInterface* pInterface, bool bData)
    {
        m_pInterface = pInterface;
        m_iCount = 0;
    }
    void                AddCount() { m_iCount++; }
    void                RemoveCount() { m_iCount--; }
    CEntitySAInterface* m_pInterface;
    int                 m_iCount;
};

enum eDebugCaller
{
    CEntity_SetMatrix,
    CEntity_SetOrientation,
    CEntity_FixBoatOrientation,
    BuildingRemoval,
    BuildingRemoval2,
    BuildingRemoval3,
    BuildingRemoval4,
    BuildingRemoval5,
    Building_Restore,
    Building_Restore2,
    BuildingRemovalReset,
    BuildingRemovalReset2,
    CVehicle_Destructor,
    CPlayerPed_Constructor,
    CPlayerPed_Destructor,
    CPickup_Constructor,
    CPickup_Destructor,
    CObject_Constructor,
    CObject_Destructor,
    CCivPed_Constructor,
    CCivPed_Destructor,
    CBuilding_Destructor,

};

enum eSurfaceProperties
{
    SURFACE_PROPERTY_AUDIO,
    SURFACE_PROPERTY_STEPWATERSPLASH,
    SURFACE_PROPERTY_STEPDUST,
    SURFACE_PROPERTY_CLIMBING,
    SURFACE_PROPERTY_BULLETEFFECT,
    SURFACE_PROPERTY_SHOOTTHROUGH,
    SURFACE_PROPERTY_SEETHROUGH,
    SURFACE_PROPERTY_SKIDMARKTYPE,
    SURFACE_PROPERTY_TYREGRIP,
    SURFACE_PROPERTY_WETGRIP,
    SURFACE_PROPERTY_ADHESIONGROUP,
    SURFACE_PROPERTY_WHEELEFFECT,
    SURFACE_PROPERTY_FRACTIONEFFECT,
    SURFACE_PROPERTY_STAIRS,
    SURFACE_PROPERTY_ROUGHNESS,
    SURFACE_PROPERTY_STEEPSLOPE,
    SURFACE_PROPERTY_GLASS,
    SURFACE_PROPERTY_PAVEMENT,
    SURFACE_PROPERTY_SOFTLANDING,
    SURFACE_PROPERTY_FOOTEFFECT,
    SURFACE_PROPERTY_CREATEOBJECTS,
    SURFACE_PROPERTY_CREATEPLANTS,
};

enum eSurfaceBulletEffect
{
    SURFACE_BULLET_EFFECT_DISABLED,
    SURFACE_BULLET_EFFECT_METAL,
    SURFACE_BULLET_EFFECT_SAND,
    SURFACE_BULLET_EFFECT_WOOD,
    SURFACE_BULLET_EFFECT_CONCRETE,
};

enum eSurfaceAudio
{
    SURFACE_AUDIO_CONCRETE = 10,
    SURFACE_AUDIO_GRASS,
    SURFACE_AUDIO_SAND,
    SURFACE_AUDIO_GRAVEL,
    SURFACE_AUDIO_WOOD,
    SURFACE_AUDIO_WATER,
    SURFACE_AUDIO_METAL,
};

enum eSurfaceWheelEffect
{
    SURFACE_WHEEL_EFFECT_DISABLED,
    SURFACE_WHEEL_EFFECT_GRASS,
    SURFACE_WHEEL_EFFECT_GRAVEL,
    SURFACE_WHEEL_EFFECT_MUD,
    SURFACE_WHEEL_EFFECT_SAND,
    SURFACE_WHEEL_EFFECT_DUST,
};

enum eSurfaceSkidMarkType
{
    SURFACE_SKID_MARK_ASPHALT,
    SURFACE_SKID_MARK_DIRT,
    SURFACE_SKID_MARK_DUST,
    SURFACE_SKID_MARK_DISABLED,
};

enum eSurfaceAdhesionGroup
{
    SURFACE_ADHESION_GROUP_RUBBER,
    SURFACE_ADHESION_GROUP_HARD,
    SURFACE_ADHESION_GROUP_ROAD,
    SURFACE_ADHESION_GROUP_LOOSE,
    SURFACE_ADHESION_GROUP_SAND,
    SURFACE_ADHESION_GROUP_WET,
};

class SurfaceInfo_c
{
public:
    uint8_t  m_tyreGrip;
    uint8_t  m_wetGrip;            // 2
    uint16_t pad;                  // 4
    union
    {
        struct            // size 8
        {
            uint32_t flags[2];
        };
        struct            // size = 51
        {
            uint32_t m_adhesionGroup : 3;             // 1 - 3
            uint32_t m_skidmarkType : 2;              // 4 - 5
            uint32_t m_frictionEffect : 3;            // 6 - 8
            uint32_t m_bulletFx : 3;                  // 9 - 11
            uint32_t m_softLanding : 1;               // 12
            uint32_t m_seeThrough : 1;                // 13
            uint32_t m_shootThrough : 1;              // 14
            uint32_t m_sand : 1;                      // 15
            uint32_t m_water : 1;
            uint32_t m_shallowWater : 1;            // unknown effect
            uint32_t m_beach : 1;
            uint32_t m_steepSlope : 1;
            uint32_t m_glass : 1;            // 20
            uint32_t m_stairs : 1;
            uint32_t m_skateable : 1;
            uint32_t m_pavement : 1;
            uint32_t m_roughness : 2;                // 24 - 25
            uint32_t m_flammability : 2;             // 26 - 27
            uint32_t m_createsSparks : 1;            // 28
            uint32_t m_cantSprintOn : 1;             // 29
            uint32_t m_leavesFootsteps : 1;
            uint32_t m_producesFootDust : 1;
            uint32_t m_makesCarDirty : 1;            // 32

            uint32_t m_makesCarClean : 1;            // 1
            uint32_t m_createsWheelGrass : 1;
            uint32_t m_createsWheelGravel : 1;
            uint32_t m_createsWheelMud : 1;
            uint32_t m_createsWheelDust : 1;             // 5
            uint32_t m_createsWheelSand : 1;             // no effect
            uint32_t m_createsWheelSpray : 1;            // crash
            uint32_t m_createsPlants : 1;                // 8
            uint32_t m_createsObjects : 1;
            uint32_t m_canClimb : 1;                 // 10
            uint32_t m_audioConcrete : 1;            // 11
            uint32_t m_audioGrass : 1;
            uint32_t m_audioSand : 1;            // 13
            uint32_t m_audioGravel : 1;
            uint32_t m_audioWood : 1;
            uint32_t m_audioWater : 1;
            uint32_t m_audioMetal : 1;
            uint32_t m_audioLongGrass : 1;            // 18
            uint32_t m_audioTile : 1;
        };
    };
    void setFlagEnabled(short flagsGroup, short sFlagID, bool bEnabled, short usForNext = 1)
    {
        for (usForNext--; usForNext >= 0; usForNext--)
        {
            if (bEnabled)
                flags[flagsGroup] |= 1UL << (sFlagID + usForNext);
            else
                flags[flagsGroup] &= ~(1UL << (sFlagID + usForNext));
        }
    }
    bool getFlagEnabled(char flagsGroup, short sFlagID) { return ((flags[flagsGroup] >> sFlagID) & 1U) == 1; }
};

struct CSurfaceType
{
    float         m_adhesivaeLimits[6][6];
    SurfaceInfo_c surfType[179];
};

class CWorld
{
public:
    virtual void Add(CEntity* entity, eDebugCaller CallerId) = 0;
    virtual void Remove(CEntity* entity, eDebugCaller CallerId) = 0;
    virtual void Remove(CEntitySAInterface* entityInterface, eDebugCaller CallerId) = 0;
    virtual bool ProcessLineOfSight(const CVector* vecStart, const CVector* vecEnd, CColPoint** colCollision, CEntity** CollisionEntity,
                                    const SLineOfSightFlags flags = SLineOfSightFlags(), SLineOfSightBuildingResult* pBuildingResult = NULL) = 0;
    // THIS FUNCTION IS INCOMPLETE AND SHOULD NOT BE USED ----------v
    virtual bool  TestLineSphere(CVector* vecStart, CVector* vecEnd, CVector* vecSphereCenter, float fSphereRadius, CColPoint** colCollision) = 0;
    virtual void  IgnoreEntity(CEntity* entity) = 0;
    virtual BYTE  GetLevelFromPosition(CVector* vecPosition) = 0;
    virtual float FindGroundZForPosition(float fX, float fY) = 0;
    virtual float FindGroundZFor3DPosition(CVector* vecPosition) = 0;
    virtual float FindRoofZFor3DCoord(CVector* pvecPosition, bool* pbOutResult) = 0;
    virtual void  LoadMapAroundPoint(CVector* vecPosition, float fRadius) = 0;
    virtual bool  IsLineOfSightClear(const CVector* vecStart, const CVector* vecEnd, const SLineOfSightFlags flags = SLineOfSightFlags()) = 0;
    virtual bool  HasCollisionBeenLoaded(CVector* vecPosition) = 0;
    virtual DWORD GetCurrentArea() = 0;
    virtual void  SetCurrentArea(DWORD dwArea) = 0;
    virtual void  SetJetpackMaxHeight(float fHeight) = 0;
    virtual float GetJetpackMaxHeight() = 0;
    virtual void  SetAircraftMaxHeight(float fHeight) = 0;
    virtual float GetAircraftMaxHeight() = 0;
    virtual void  SetAircraftMaxVelocity(float fVelocity) = 0;
    virtual float GetAircraftMaxVelocity() = 0;
    virtual void  SetOcclusionsEnabled(bool bEnabled) = 0;
    virtual bool  GetOcclusionsEnabled() = 0;
    virtual void  FindWorldPositionForRailTrackPosition(float fRailTrackPosition, int iTrackId, CVector* pOutVecPosition) = 0;
    virtual int   FindClosestRailTrackNode(const CVector& vecPosition, uchar& ucOutTrackId, float& fOutRailDistance) = 0;

    virtual void RemoveBuilding(unsigned short usModelToRemove, float fDistance, float fX, float fY, float fZ, char cInterior, uint* pOutAmount = NULL) = 0;
    virtual bool IsRemovedModelInRadius(SIPLInst* pInst) = 0;
    virtual bool IsModelRemoved(unsigned short usModelID) = 0;
    virtual void ClearRemovedBuildingLists(uint* pOutAmount = NULL) = 0;
    virtual bool RestoreBuilding(unsigned short usModelToRestore, float fDistance, float fX, float fY, float fZ, char cInterior, uint* pOutAmount = NULL) = 0;
    virtual SBuildingRemoval* GetBuildingRemoval(CEntitySAInterface* pInterface) = 0;
    virtual void              AddDataBuilding(CEntitySAInterface* pInterface) = 0;
    virtual void              AddBinaryBuilding(CEntitySAInterface* pInterface) = 0;
    virtual void              RemoveWorldBuildingFromLists(CEntitySAInterface* pInterface) = 0;
    virtual bool              IsObjectRemoved(CEntitySAInterface* pInterface) = 0;
    virtual bool              IsDataModelRemoved(unsigned short usModelID) = 0;
    virtual bool              IsEntityRemoved(CEntitySAInterface* pInterface) = 0;
    virtual bool              CalculateImpactPosition(const CVector& vecInputStart, CVector& vecInputEnd) = 0;

    virtual CSurfaceType* GetSurfaceInfo() = 0;
    virtual void          ResetAllSurfaceInfo() = 0;
    virtual bool          ResetSurfaceInfo(short sSurfaceID) = 0;
};

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWorldSA.h
*  PURPOSE:     Header file for game world
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_WORLD
#define __CGAMESA_WORLD

#define FUNC_Add                                            0x563220 // ##SA##
#define FUNC_Remove                                         0x563280 // ##SA##
#define FUNC_ProcessLineOfSight                             0x56BA00 // ##SA##
#define FUNC_FindGroundZFor3DCoord                          0x5696C0 // ##SA##
#define FUNC_FindGroundZForCoord                            0x569660 // ##SA##
#define FUNC_CTimer_Stop                                    0x561AA0 // ##SA##
#define FUNC_CTimer_Update                                  0x561B10 // ##SA##
#define FUNC_CRenderer_RequestObjectsInDirection            0x555CB0 // ##SA##
#define FUNC_CStreaming_LoadScene                           0x40EB70 // ##SA##
#define FUNC_IsLineOfSightClear                             0x56A490 // ##SA##
#define FUNC_HasCollisionBeenLoaded                         0x410CE0 // ##SA##
#define FUNC_RemoveBuildingsNotInArea                       0x4094B0 // ##SA##
#define FUNC_RemoveReferencesToDeletedObject                0x565510 // ##SA##

// CCol...
#define FUNC_CColLine_Constructor                           0x40EF50 // ##SA##
#define FUNC_CColSphere_Set                                 0x40FD10 // ##SA##

#define VAR_IgnoredEntity                                   0xB7CD68 // ##SA##
#define VAR_currArea                                        0xB72914 // ##SA##
#define ARRAY_StreamSectors                                 0xB7D0B8
#define NUM_StreamSectorRows                                120
#define NUM_StreamSectorCols                                120
#define ARRAY_StreamRepeatSectors                           0xB992B8
#define NUM_StreamRepeatSectorRows                          16
#define NUM_StreamRepeatSectorCols                          16
#define VAR_fJetpackMaxHeight                               0x8703D8
#define VAR_fAircraftMaxHeight                              0x8594DC


#include <game/CWorld.h>
#include "CEntitySA.h"
#include "CBuildingSA.h"
#include <google/dense_hash_map>
class CWorldSA : public CWorld
{
public:
    CWorldSA ( );
    void        Add                       ( CEntity * entity );
    void        Add                       ( CEntitySAInterface * entityInterface );
    void        Remove                    ( CEntity * entity );
    void        Remove                    ( CEntitySAInterface * entityInterface );
    void        RemoveReferencesToDeletedObject ( CEntitySAInterface * entity );
    bool        ProcessLineOfSight        ( const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, CEntity ** CollisionEntity, const SLineOfSightFlags flags, SLineOfSightBuildingResult* pBuildingResult );
    bool        TestLineSphere            ( CVector * vecStart, CVector * vecEnd, CVector * vecSphereCenter, float fSphereRadius, CColPoint ** colCollision );
    //bool      ProcessLineOfSight        ( CVector * vecStart, CVector * vecEnd, CColPoint * colCollision, CEntity * CollisionEntity );
    void        IgnoreEntity              ( CEntity * entity );
    BYTE        GetLevelFromPosition      ( CVector * vecPosition );
    float       FindGroundZForPosition    ( float fX, float fY );
    float       FindGroundZFor3DPosition  ( CVector * vecPosition );
    void        LoadMapAroundPoint        ( CVector * vecPosition, float fRadius );
    bool        IsLineOfSightClear        ( const CVector * vecStart, const CVector * vecEnd, const SLineOfSightFlags flags );
    bool        HasCollisionBeenLoaded    ( CVector * vecPosition );
    DWORD       GetCurrentArea            ( void );
    void        SetCurrentArea            ( DWORD dwArea );
    void        SetJetpackMaxHeight       ( float fHeight );
    float       GetJetpackMaxHeight       ( void );
    void        SetAircraftMaxHeight      ( float fHeight );
    float       GetAircraftMaxHeight      ( void );

    /**
     * \todo Add FindObjectsKindaColliding (see 0x430577)
     * \todo Add CameraToIgnoreThisObject (0x563F40)
     * \todo Add ClearForRestart (0x564360)
     * \todo Add Shutdown (0x564050)
     * \todo Add ClearExcitingStuffFromArea (0x56A0D0)
     * \todo Add ExtinguishAllCarFiresInArea (0x566950)
     * \todo Add FindLowestZForCoord (0x5697F0)
     * \todo Add FindNearestObjectOfType (see 0x46D5FD)
     * \todo Add FindRoofZFor3DCoord (0x569750)
     * \todo Add GetIsLineOfSightClear (0x56A490)
     * \todo Add ProcessVerticalLine (0x5674E0)
     * \todo Add RemoveReferencesToDeletedObject (0x565510)
     * \todo Add SetAllCarsCanBeDamaged (0x5668F0)
     * \todo Add TestSphereAgainstWorld (0x569E20)

     * \todo One Day: ClearPedsFromArea, SetCarsOnFire, SetPedsChoking, SetPedsOnFire, SetWorldOnFire
     * StopAllLawEnforcersInTheirTracks

     */
    void                RemoveBuilding              ( unsigned short usModelToRemove, float fDistance, float fX, float fY, float fZ );
    bool                IsRemovedModelInRadius      ( SIPLInst* pInst );
    bool                IsModelRemoved              ( unsigned short modelID );
    void                ClearRemovedBuildingLists   ( void );
    bool                RestoreBuilding             ( unsigned short usModelToRestore, float fDistance, float fX, float fY, float fZ );
    SBuildingRemoval*   GetBuildingRemoval          ( CEntitySAInterface * pInterface );
private:
    std::list< SBuildingRemoval* >                              *m_pBuildings;
    std::list< unsigned short >                                *m_pRemovedObjects;
};

#endif

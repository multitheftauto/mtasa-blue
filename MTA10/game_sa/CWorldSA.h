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
#define VAR_fWindSpeedX										0xC813E0
#define VAR_fWindSpeedY										0xC813E4
#define VAR_fWindSpeedZ										0xC813E8
#define VAR_fFarClipDistance                                0xB7C4F0
#define VAR_fFogDistance                                    0xB7C4F4
#define VAR_ucSunCoreR                                      0xB7C4D0
#define VAR_ucSunCoreG                                      0xB7C4D2
#define VAR_ucSunCoreB                                      0xB7C4D4
#define VAR_ucSunCoronaR                                    0xB7C4D6
#define VAR_ucSunCoronaG                                    0xB7C4D8
#define VAR_ucSunCoronaB                                    0xB7C4DA
#define ADDR_WindSpeedSetX									0x72C616
#define ADDR_WindSpeedSetY									0x72C622
#define ADDR_WindSpeedSetZ									0x72C636
#define ADDR_WindSpeedSetX2									0x72C40C
#define ADDR_WindSpeedSetY2									0x72C417
#define ADDR_WindSpeedSetZ2									0x72C4EF


#include <game/CWorld.h>
#include "CEntitySA.h"

class CWorldSA : public CWorld
{
public:
    void        Add                       ( CEntity * entity );
    void        Add                       ( CEntitySAInterface * entityInterface );
    void        Remove                    ( CEntity * entity );
    void        Remove                    ( CEntitySAInterface * entityInterface );
    void        RemoveReferencesToDeletedObject ( CEntitySAInterface * entity );
    bool        ProcessLineOfSight        ( const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, CEntity ** CollisionEntity = NULL, bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true, bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false, bool bIgnoreSomeObjectsForCamera = false, bool bShootThroughStuff = false );
    bool        TestLineSphere            ( CVector * vecStart, CVector * vecEnd, CVector * vecSphereCenter, float fSphereRadius, CColPoint ** colCollision );
    //bool      ProcessLineOfSight        ( CVector * vecStart, CVector * vecEnd, CColPoint * colCollision, CEntity * CollisionEntity );
    void        IgnoreEntity              ( CEntity * entity );
    BYTE        GetLevelFromPosition      ( CVector * vecPosition );
    float       FindGroundZForPosition    ( float fX, float fY );
    float       FindGroundZFor3DPosition  ( CVector * vecPosition );
    void        LoadMapAroundPoint        ( CVector * vecPosition, float fRadius );
    bool        IsLineOfSightClear        ( CVector * vecStart, CVector * vecEnd, bool bCheckBuildings = true, bool bCheckVehicles = true, bool bCheckPeds = true, bool bCheckObjects = true, bool bCheckDummies = true, bool bSeeThroughStuff = false, bool bIgnoreSomeObjectsForCamera = false );
    bool        HasCollisionBeenLoaded    ( CVector * vecPosition );
    DWORD       GetCurrentArea            ( void );
    void        SetCurrentArea            ( DWORD dwArea );
    void        SetJetpackMaxHeight       ( float fHeight );
    float       GetJetpackMaxHeight       ( void );
    void        SetWindVelocity           ( float fX, float fY, float fZ );
    void        GetWindVelocity           ( float& fX, float& fY, float& fZ );
    void        RestoreWindVelocity       ( void );
    float       GetFarClipDistance        ( void );
    void        SetFarClipDistance        ( float fDistance );
    void        RestoreFarClipDistance    ( void );
    float       GetFogDistance            ( void );
    void        SetFogDistance            ( float fDistance );
    void        RestoreFogDistance        ( void );
    void        GetSunColor               ( unsigned char& ucCoreRed, unsigned char& ucCoreGreen, unsigned char& ucCoreBlue, unsigned char& ucCoronaRed, unsigned char& ucCoronaGreen, unsigned char& ucCoronaBlue );
    void        SetSunColor               ( unsigned char ucCoreRed, unsigned char ucCoreGreen, unsigned char ucCoreBlue, unsigned char ucCoronaRed, unsigned char ucCoronaGreen, unsigned char ucCoronaBlue );
    void        ResetSunColor             ( );

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
};

#endif

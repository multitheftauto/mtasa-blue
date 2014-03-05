/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/COffsets.cpp
*  PURPOSE:     Grand Theft Auto executable offsets class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void COffsetsMP::Initialize10EU ( void )
{
    InitializeCommon10 ();

    CMultiplayerSA::ADDR_CursorHiding = 0x74821D;
    CMultiplayerSA::ADDR_GotFocus = 0x74804E;
}


void COffsetsMP::Initialize10US ( void )
{
    InitializeCommon10 ();

    CMultiplayerSA::ADDR_CursorHiding = 0x7481CD;
    CMultiplayerSA::ADDR_GotFocus = 0x748054;
}


void COffsetsMP::Initialize11 ( void )
{
    CMultiplayerSA::HOOKPOS_CExplosion_AddExplosion =                                       0x737280; // 1.01
    CMultiplayerSA::HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow =                  0x70636B; // 1.01
    CMultiplayerSA::HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic =                      0x6FFB8B; // 1.01
    CMultiplayerSA::HOOKPOS_CTrain_ProcessControl_Derail =                                  0x6F8DBA; // 1.01???? ACHTUNG!
    CMultiplayerSA::FUNC_CAudioEngine__DisplayRadioStationName =                            0x5074A0; // 1.01
    CMultiplayerSA::FUNC_CPlayerInfoBase =                                                  0xB7CD98;
    
}


void COffsetsMP::Initialize20 ( void )
{
    // TODO
}


void COffsetsMP::InitializeCommon10 ( void )
{
    CMultiplayerSA::HOOKPOS_CExplosion_AddExplosion =                                       0x736A50;
    CMultiplayerSA::HOOKPOS_CRealTimeShadowManager__ReturnRealTimeShadow =                  0x705B3B;
    CMultiplayerSA::HOOKPOS_CCustomRoadsignMgr__RenderRoadsignAtomic =                      0x6FF35B;
    CMultiplayerSA::HOOKPOS_Trailer_BreakTowLink =                                          0x6E0027;
    CMultiplayerSA::HOOKPOS_CRadar__DrawRadarGangOverlay =                                  0x586650;
    CMultiplayerSA::HOOKPOS_CTaskComplexJump__CreateSubTask =                               0x67DABE;
    CMultiplayerSA::HOOKPOS_CTrain_ProcessControl_Derail =                                  0x6F8DBA;
    CMultiplayerSA::HOOKPOS_CObject_Render =                                                0x59F1ED;
    CMultiplayerSA::HOOKPOS_EndWorldColors =                                                0x561795;
    CMultiplayerSA::HOOKPOS_CWorld_ProcessVerticalLineSectorList =                          0x563357;
    CMultiplayerSA::HOOKPOS_ComputeDamageResponse_StartChoking =                            0x4C05B9;
    CMultiplayerSA::HOOKPOS_CAutomobile__ProcessSwingingDoor =                              0x6A9DAF;

    CMultiplayerSA::FUNC_CAudioEngine__DisplayRadioStationName =                            0x507030;
    CMultiplayerSA::FUNC_CPlayerInfoBase =                                                  0xB7CD98;
}

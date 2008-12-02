/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedModelInfoSA.h
*  PURPOSE:     Header file for ped modelinfo class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPedModelInfoSA_H
#define __CPedModelInfoSA_H

#include "CModelInfoSA.h"
#include <game/CPedModelInfo.h>

class ePedStats;
typedef long AssocGroupID;

#define		FUNC_SetMotionAnimGroup				            0x5b3580

// CPedModelInfo:
// +36 = Motion anim group (AssocGroupID, long)
// +40 = Default ped type (long)
// +44 = Default ped stats (ePedStats)
// +48 = Can drive cars (byte)
// +50 = Ped flags (short)
// +52 = Hit col model (CColModel*)
// +56 = First radio station
// +57 = Second radio station
// +58 = Race (byte)
// +60 = Audio ped type (short)
// +62 = First voice
// +64 = Last voice
// +66 = Next voice (short)

class CPedModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    BYTE                pad [ 4 ];              // 32   
    AssocGroupID        motionAnimGroup;        // 36   Motion anim group (AssocGroupID, long)
    long                pedType;                // 40   Default ped type (long)
    ePedStats *         pedStats;               // 44   Default ped stats (ePedStats)
    BYTE                bCanDriveCars;          // 48   Can drive cars (byte)
    BYTE                pad2 [ 1 ];             // 49   
    short               pedFlags;               // 50   Ped flags (short)
    CColModel *         pHitColModel;           // 52   Hit col model (CColModel*)
    BYTE                bFirstRadioStation;     // 56   First radio station
    BYTE                bSecondRadioStation;    // 57   Second radio station
    BYTE                bIsInRace;              // 58   Race (byte)
    BYTE                pad3 [ 1 ];             // 59   
    short               sVoiceType;             // 60   Voice type
    short               sFirstVoice;            // 62   First voice
    short               sLastVoice;             // 64   Last voice
    short               sNextVoice;             // 66   Next voice
};

class CPedModelInfoSA : public CModelInfoSA, public CPedModelInfo
{
public:
    CPedModelInfoSAInterface *      GetPedModelInfoInterface   ( void )        { return reinterpret_cast < CPedModelInfoSAInterface * > ( m_pInterface ); }

	void							SetMotionAnimGroup	       ( AssocGroupId animGroup );
};

#endif
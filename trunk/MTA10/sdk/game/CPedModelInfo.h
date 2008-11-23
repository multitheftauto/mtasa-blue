/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CPedModelInfo.h
*  PURPOSE:		Ped model information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPedModelInfo_H
#define __CPedModelInfo_H

typedef unsigned long AssocGroupId;

// Ped voice categories
enum eVoiceGens
{
    VOICE_GEN = 0,      // Generic ped voices
    VOICE_EMG = 1,      // Emergency ped voices
    VOICE_PLY = 2,      // CJ non-mission ped voices
    VOICE_GNG = 3,      // Gang and special ped voices
    VOICE_GFD = 4       // Shop and girlfriend ped voices
};

// Ped audio types
    // PED_TYPE_GEN
    // PED_TYPE_EMG
    // PED_TYPE_PLAYER
    // PED_TYPE_GANG
    // PED_TYPE_GFD


#define PED_VOICE_BANK_LENGTH   20

class CPedModelInfo
{
public:
    virtual void							SetMotionAnimGroup	( AssocGroupId animGroup ) = 0;
};

#endif
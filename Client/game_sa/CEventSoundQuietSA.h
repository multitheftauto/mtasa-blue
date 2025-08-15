/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CEventSoundQuietSA.h
 *  PURPOSE:     Header file for damage event class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include <cstdint>
#include "CVector.h"

class CEntitySAInterface;

class CEventSoundQuietSAInterface
{
public:
    std::uint8_t parent[0x14]; // CEventEditableResponse (inherited class)
    CEntitySAInterface* entity{nullptr};
    float               localSoundLevel{};
    std::uint32_t       startTimeInMs{};
    CVector             position{};

    CEventSoundQuietSAInterface(CEntitySAInterface* ped, float volume, std::uint32_t startTimeInMs = 0, CVector& pos = CVector()) 
    {
        ((void*(__thiscall*)(CEventSoundQuietSAInterface*, CEntitySAInterface*, float, int, CVector*))0x5E05B0)(this, ped, volume, startTimeInMs, &pos);
    }

    ~CEventSoundQuietSAInterface()
    {
        ((void(__thiscall*)(void*))0x5DEA00)(this);
    }
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CVisibilityPlugins.h
 *  PURPOSE:     Visibility plugins interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define ATOMIC_ID_FLAG_TWO_VERSIONS_UNDAMAGED   1
#define ATOMIC_ID_FLAG_TWO_VERSIONS_DAMAGED     2

struct RpClump;
struct RpAtomic;
struct RwObject;

class CVisibilityPlugins
{
public:
    virtual void SetClumpAlpha(RpClump* pClump, int iAlpha) = 0;
    virtual int  GetAtomicId(RwObject* pAtomic) = 0;
    virtual void SetAtomicId(RpAtomic* atomic, int id) = 0;

    virtual bool InsertEntityIntoEntityList(void* entity, float distance, void* callback) = 0;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSpatialDatabase.h
 *  PURPOSE:
 *
 *****************************************************************************/

class CElement;

#pragma once

// Bounding sphere z position for 2d objects
#define SPATIAL_2D_Z    0

// Result of a Query
class CElementResult : public std::vector<CElement*>
{
public:
};

//
// CSpatialDatabase
//
class CSpatialDatabase
{
public:
    virtual ~CSpatialDatabase() {}

    virtual void UpdateEntity(CElement* pEntity) = 0;
    virtual void RemoveEntity(CElement* pEntity) = 0;
    virtual bool IsEntityPresent(CElement* pEntity) = 0;
    virtual void SphereQuery(CElementResult& outResult, const CSphere& sphere) = 0;
    virtual void AllQuery(CElementResult& outResult) = 0;
};

CSpatialDatabase* GetSpatialDatabase();

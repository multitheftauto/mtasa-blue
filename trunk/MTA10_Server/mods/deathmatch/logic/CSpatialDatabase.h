/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSpatialDatabase.h
*  PURPOSE:     
*  DEVELOPERS:  
*
*****************************************************************************/

class CElement;

#ifndef __CSPATIALDATABASE_H
#define __CSPATIALDATABASE_H

// Debugging flags
#ifdef MTA_DEBUG
    #define SPATIAL_DATABASE_TESTS
    //#define SPATIAL_DATABASE_DEBUG_OUTPUTA
    //#define SPATIAL_DATABASE_DEBUG_OUTPUTB
    //#define SPATIAL_DATABASE_DEBUG_OUTPUTC
#else
    //#define SPATIAL_DATABASE_TESTS
    //#define SPATIAL_DATABASE_DEBUG_OUTPUTA
    //#define SPATIAL_DATABASE_DEBUG_OUTPUTB
    //#define SPATIAL_DATABASE_DEBUG_OUTPUTC
#endif

// Bounding sphere z position for 2d objects
#define SPATIAL_2D_Z    0

// Result of a Query
class CElementResult : public std::vector < CElement* >
{
public:
};

//
// CSpatialDatabase
//
class CSpatialDatabase
{
public:
    virtual void        UpdateEntity        ( CElement* pEntity ) = 0;
    virtual void        RemoveEntity        ( CElement* pEntity ) = 0;
    virtual bool        IsEntityPresent     ( CElement* pEntity ) = 0;
    virtual void        SphereQuery         ( CElementResult& outResult, const CSphere& sphere ) = 0;
    virtual void        AllQuery            ( CElementResult& outResult ) = 0;
};

CSpatialDatabase* GetSpatialDatabase ();


#endif

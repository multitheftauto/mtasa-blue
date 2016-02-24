/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSpatialDatabase.h
*  PURPOSE:     
*  DEVELOPERS:  
*
*****************************************************************************/

class CClientEntity;

#ifndef __CCLIENTSPATIALDATABASE_H
#define __CCLIENTSPATIALDATABASE_H

// Bounding sphere z position for 2d objects
#define SPATIAL_2D_Z    0

// Result of a Query
class CClientEntityResult : public std::vector < CClientEntity* >
{
public:
};

//
// CClientSpatialDatabase
//
class CClientSpatialDatabase
{
public:
    virtual void        UpdateEntity        ( CClientEntity* pEntity ) = 0;
    virtual void        RemoveEntity        ( CClientEntity* pEntity ) = 0;
    virtual bool        IsEntityPresent     ( CClientEntity* pEntity ) = 0;
    virtual void        SphereQuery         ( CClientEntityResult& outResult, const CSphere& sphere ) = 0;
    virtual void        AllQuery            ( CClientEntityResult& outResult ) = 0;
};

CClientSpatialDatabase* GetClientSpatialDatabase ();


#endif

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

class CPlayerCamera;


// Bounding sphere z position for 2d objects
#define SPATIAL_2D_Z    0

// Result of a Query
class CCameraQueryResult : public std::vector < CPlayerCamera* >
{
public:
};

//
// CCameraSpatialDatabase
//
class CCameraSpatialDatabase
{
public:
    virtual             ~CCameraSpatialDatabase   ( void ) {}

    virtual void        UpdateItem          ( CPlayerCamera* pCamera ) = 0;
    virtual void        RemoveItem          ( CPlayerCamera* pCamera ) = 0;
    virtual bool        IsItemPresent       ( CPlayerCamera* pCamera ) = 0;
    virtual void        SphereQuery         ( CCameraQueryResult& outResult, const CSphere& sphere ) = 0;
    virtual void        AllQuery            ( CCameraQueryResult& outResult ) = 0;
};

CCameraSpatialDatabase* GetCameraSpatialDatabase ();


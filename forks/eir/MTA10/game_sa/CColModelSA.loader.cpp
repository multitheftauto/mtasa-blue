/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.loader.cpp
*  PURPOSE:     Collision model entity
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

// Private functions.
_mallocNew_t     _mallocNew =            (_mallocNew_t)invalid_ptr;


static void _BoundingBox_Constructor( CBoundingBox& bounds )
{
    bounds.vecBoundMin = CVector( 1.0f, 1.0f, 1.0f );
    bounds.vecBoundMax = CVector( -1.0f, -1.0f, -1.0f );
}

// Version 1 structs for backwards compatibility.
struct TBounds
{
    float fRadius;
    CVector center;
    CVector min, max;
};

struct TSphere
{
    float radius;
    CVector center;
    CColSurfaceSA surface;
};

inline short ColFloatToShort( float val )   { return (short)( val * 128.0f ); }
inline float ColShortToFlaot( short val )   { return (float)val / 128.0f; }

struct TFace
{
    unsigned int a, b, c;
    CColSurfaceSA surface;
};

struct ColHeaderVersion1    //size: 40 bytes
{
    TBounds         boundingBox;                // 0
};
C_ASSERT( sizeof( ColHeaderVersion1 ) == 40 );

// The documentation presented at gtamodding is partially wrong for San Andreas.
struct ColHeaderVersion2    //size: 76 bytes
{
    CBoundingBox    boundingBox;

    unsigned short  numColSpheres;              // 40
    unsigned short  numColBoxes;                // 42
    unsigned short  numColMeshFaces;            // 44
    unsigned char   numWheels;                  // 46
    unsigned char   pad;                        // 47

    union
    {
        unsigned int        flags;              // 48

        struct
        {
            unsigned int    unkFlag1 : 1;       // 48
            unsigned int    unkFlag2 : 1;           
            unsigned int    unkFlag3 : 1;
            unsigned int    unkFlag4 : 1;       
        };
    };

    unsigned int    offsetColSpheres;           // 52
    unsigned int    offsetColBoxes;             // 56
    unsigned int    offsetSuspensionLines;      // 60
    unsigned int    offsetColMeshVertices;      // 64
    unsigned int    offsetColMeshFaces;         // 68
    unsigned int    offsetTrianglePlanes;       // 72
};
C_ASSERT( sizeof( ColHeaderVersion2 ) == 76 );

struct ColHeaderVersion3 : public ColHeaderVersion2     //size: 88 bytes
{
    int             numShadowMeshFaces;         // 76

    unsigned int    offsetShadowMeshVertices;   // 80
    unsigned int    offsetShadowMeshFaces;      // 84
};
C_ASSERT( sizeof( ColHeaderVersion3 ) == 88 );

struct ColHeaderVersion4 : public ColHeaderVersion3
{
    int             unk;                        // 88
};
C_ASSERT( sizeof( ColHeaderVersion4 ) == 92 );

unsigned int CColDataSA::CalculateNumberOfShadowMeshVertices( void ) const
{
    // Calculates the number of shadow mesh vertices.
    // The number of shadow mesh vertices is equal to the highest index used in the shadow mesh.
    unsigned int numVertices = 0;

    if ( numShadowMeshFaces != 0 )
    {
        if ( numShadowMeshFaces > 0 )
        {
            for ( int n = 0; n < numShadowMeshFaces; n++ )
            {
                CColShadowMeshFaceSA *face = pShadowMeshFaces + n;

                if ( numVertices < face->a )
                {
                    numVertices = face->a;
                }

                if ( numVertices < face->b )
                {
                    numVertices = face->b;
                }

                if ( numVertices < face->c )
                {
                    numVertices = face->c;
                }
            }
        }

        numVertices++;
    }

    return numVertices;
}

template <typename structType, int minus_offset>
inline structType* GetCollisionDataFromSegment( void *segment, unsigned int offset )
{
    if ( offset != 0 )
    {
        return (structType*)( (char*)segment + ( offset - minus_offset ) );
    }

    return NULL;
}

void __cdecl LoadCollisionModel( const char *pBuffer, CColModelSAInterface *pCollision, const char *colName )
{
    const ColHeaderVersion1& colHeader = *(const ColHeaderVersion1*)pBuffer;

    pCollision->m_bounds.fRadius = colHeader.boundingBox.fRadius;
    pCollision->m_bounds.vecBoundOffset = colHeader.boundingBox.center;
    pCollision->m_bounds.vecBoundMax = colHeader.boundingBox.max;
    pCollision->m_bounds.vecBoundMin = colHeader.boundingBox.min;

    CColDataSA *colData = new (_mallocNew( sizeof( CColDataSA ) )) CColDataSA;

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion1 );

    short numColSpheres = *(unsigned short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numSpheres = numColSpheres;

    if ( numColSpheres > 0 )
    {
        void *mem = RwMalloc( sizeof( CColSphereSA ) * numColSpheres, 0 );

        for ( int n = 0; n < numColSpheres; n++ )
        {
            const TSphere& oldSphere = *(const TSphere*)pBuffer;

            new ((CColSphereSA*)mem + n)
                CColSphereSA( oldSphere.radius, oldSphere.center, oldSphere.surface.material, oldSphere.surface.flags, oldSphere.surface.unknown );

            ( (const TSphere*&)pBuffer )++;
        }

        colData->pColSpheres = (CColSphereSA*)mem;
    }
    else
        colData->pColSpheres = NULL;

    char numWheels = *(char*)pBuffer;

    pBuffer += sizeof( int );

    colData->ucNumWheels = numWheels;

    if ( numWheels > 0 )
    {
        // We do not care about suspension line data of the old version.
        pBuffer += ( sizeof( CVector ) * 2 ) * numWheels;
    }
    else
    {
        colData->pSuspensionLines = NULL;
    }

    // Always come with no suspension line data.
    colData->ucNumWheels = 0;
    colData->pSuspensionLines = NULL;

    short numColBoxes = *(short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numBoxes = numColBoxes;

    if ( numColBoxes > 0 )
    {
        void *mem = RwMalloc( sizeof( CColBoxSA ) * numColBoxes, 0 );

        for ( int n = 0; n < numColBoxes; n++ )
        {
            const CColBoxSA& colBox = *(const CColBoxSA*)pBuffer;

            new ((CColBoxSA*)mem + n)
                CColBoxSA( colBox.min, colBox.max, colBox.surface.material, colBox.surface.flags, colBox.surface.unknown );

            ( (const CColBoxSA*&)pBuffer )++;
        }

        colData->pColBoxes = (CColBoxSA*)mem;
    }
    else
        colData->pColBoxes = NULL;

    int numColVertices = *(int*)pBuffer;

    pBuffer += sizeof( int );

    if ( numColVertices > 0 )
    {
        void *mem = RwMalloc( sizeof( CColVertexSA ) * numColVertices, 0 );

        for ( int n = 0; n < numColVertices; n++ )
        {
            const CVector& oldVertex = *(const CVector*)pBuffer;

            new ((CColVertexSA*)mem + n) CColVertexSA(
                ColFloatToShort( oldVertex.fX ),
                ColFloatToShort( oldVertex.fY ),
                ColFloatToShort( oldVertex.fZ )
            );

            ( (const CVector*&)pBuffer )++;
        }

        colData->pColVertices = (CColVertexSA*)mem;
    }
    else
        colData->pColVertices = NULL;

    short numColFaces = *(short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numTriangles = numColFaces;

    if ( numColFaces > 0 )
    {
        void *mem = RwMalloc( sizeof( CColTriangleSA ) * numColFaces, 0 );

        colData->pColTriangles = (CColTriangleSA*)mem;

        int actualNumFaces = 0;

        for ( int n = 0; n < numColFaces; n++ )
        {
            const TFace& oldFace = *(const TFace*)pBuffer;

            CColTriangleSA& newFace = *( (CColTriangleSA*)mem + n );
            newFace.v1 = oldFace.a;
            newFace.v2 = oldFace.b;
            newFace.v3 = oldFace.c;
            newFace.material = oldFace.surface.material;
            newFace.lighting = oldFace.surface.light;

            actualNumFaces++;

            ( (const TFace*&)pBuffer )++;
        }

        colData->numTriangles = actualNumFaces;
    }
    else
        colData->pColTriangles = NULL;

    // Version 1 has no shadow mesh.
    colData->hasShadowMeshFaces = false;
    colData->pShadowMeshFaces = NULL;
    colData->pShadowMeshVertices = NULL;

    colData->numShadowMeshVertices = 0;

    // Check whether this collision has anything collidable.
    pCollision->m_unk = ( colData->numSpheres != 0 || colData->numBoxes != 0 || colData->numTriangles != 0 );
}

void __cdecl LoadCollisionModelVer2( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    CBoundingBox bounds;

    _BoundingBox_Constructor( bounds );

    ColHeaderVersion2 colHeader = *(ColHeaderVersion2*)pBuffer;

    // Set up the collision bounding box.
    bounds.vecBoundOffset = colHeader.boundingBox.vecBoundOffset;
    bounds.vecBoundMin = colHeader.boundingBox.vecBoundMin;
    bounds.vecBoundMax = colHeader.boundingBox.vecBoundMax;
    bounds.fRadius = colHeader.boundingBox.fRadius;

    pCollision->m_bounds = bounds;

    if ( IsNotNeutral( pCollision->m_unk, colHeader.unkFlag2 ) )
        pCollision->m_unk = !pCollision->m_unk;

    bufferSize -= sizeof( ColHeaderVersion2 );

    if ( bufferSize == 0 )
        return;

    void *mem = RwMalloc( bufferSize + sizeof( CColDataSA ), 0 );

    CColDataSA *colData = (CColDataSA*)mem;

    pCollision->pColData = colData;

    char *colBuffer = (char*)( (CColDataSA*)mem + 1 );

    pBuffer += sizeof( ColHeaderVersion2 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numTriangles = colHeader.numColMeshFaces;

    colData->unkFlag1 = false;
    colData->hasShadowMeshFaces = false;

    if ( IsNotNeutral( colData->unkFlag2, colHeader.unkFlag4 ) )
        colData->unkFlag2 = !colData->unkFlag2;

    // Read actual collision data now.
    colData->pColSpheres =          GetCollisionDataFromSegment <CColSphereSA, 56>      ( mem, colHeader.offsetColSpheres );
    colData->pColBoxes =            GetCollisionDataFromSegment <CColBoxSA, 56>         ( mem, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     GetCollisionDataFromSegment <void, 56>              ( mem, colHeader.offsetSuspensionLines );
    colData->pColVertices =         GetCollisionDataFromSegment <CColVertexSA, 56>      ( mem, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        GetCollisionDataFromSegment <CColTriangleSA, 56>    ( mem, colHeader.offsetColMeshFaces );

    // Null out data that is not available in version 2
    colData->pColTrianglePlanes = NULL;

    colData->hasShadowMeshFaces = false;
    colData->pShadowMeshVertices = NULL;
    colData->pShadowMeshFaces = NULL;

    colData->numShadowMeshVertices = 0;

    pCollision->m_initWithColData = true;
}

void __cdecl LoadCollisionModelVer3( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    CBoundingBox bounds;

    _BoundingBox_Constructor( bounds );

    ColHeaderVersion3 colHeader = *(ColHeaderVersion3*)pBuffer;

    // Set up the collision bounding box.
    bounds.vecBoundOffset = colHeader.boundingBox.vecBoundOffset;
    bounds.vecBoundMin = colHeader.boundingBox.vecBoundMin;
    bounds.vecBoundMax = colHeader.boundingBox.vecBoundMax;
    bounds.fRadius = colHeader.boundingBox.fRadius;

    pCollision->m_bounds = bounds;

    if ( IsNotNeutral( pCollision->m_unk, colHeader.unkFlag2 ) )
        pCollision->m_unk = !pCollision->m_unk;

    bufferSize -= sizeof( ColHeaderVersion3 );

    if ( bufferSize == 0 )
        return;

    void *mem = RwMalloc( bufferSize + sizeof( CColDataSA ), 0 );

    CColDataSA *colData = (CColDataSA*)mem;

    pCollision->pColData = colData;

    char *colBuffer = (char*)( (CColDataSA*)mem + 1 );

    pBuffer += sizeof( ColHeaderVersion3 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numTriangles = colHeader.numColMeshFaces;

    colData->unkFlag1 = false;

    if ( IsNotNeutral( colData->unkFlag2, colHeader.unkFlag4 ) )
        colData->unkFlag2 = !colData->unkFlag2;

    colData->numShadowMeshFaces = colHeader.numShadowMeshFaces;

    // Read actual collision data now.
    colData->pColSpheres =          GetCollisionDataFromSegment <CColSphereSA, 68>          ( mem, colHeader.offsetColSpheres );
    colData->pColBoxes =            GetCollisionDataFromSegment <CColBoxSA, 68>             ( mem, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     GetCollisionDataFromSegment <void, 68>                  ( mem, colHeader.offsetSuspensionLines );
    colData->pColVertices =         GetCollisionDataFromSegment <CColVertexSA, 68>          ( mem, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        GetCollisionDataFromSegment <CColTriangleSA, 68>        ( mem, colHeader.offsetColMeshFaces );

    colData->pShadowMeshVertices =  GetCollisionDataFromSegment <void, 68>                  ( mem, colHeader.offsetShadowMeshVertices );
    colData->pShadowMeshFaces =     GetCollisionDataFromSegment <CColShadowMeshFaceSA, 68>  ( mem, colHeader.offsetShadowMeshFaces );

    // Set up the shadow mesh.
    colData->hasShadowMeshFaces =
        ( colHeader.offsetShadowMeshFaces != 0 && colHeader.offsetShadowMeshVertices != 0 && colHeader.numShadowMeshFaces > 0 );

    unsigned int numShadowMeshVertices = 0;

    if ( colData->hasShadowMeshFaces )
        numShadowMeshVertices = colData->CalculateNumberOfShadowMeshVertices();

    colData->numShadowMeshVertices = numShadowMeshVertices;

    // Triangle planes not used in this version.
    colData->pColTrianglePlanes = NULL;

    pCollision->m_initWithColData = true;
}

void __cdecl LoadCollisionModelVer4( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    CBoundingBox bounds;

    _BoundingBox_Constructor( bounds );

    ColHeaderVersion4 colHeader = *(ColHeaderVersion4*)pBuffer;

    // Set up the collision bounding box.
    bounds.vecBoundOffset = colHeader.boundingBox.vecBoundOffset;
    bounds.vecBoundMin = colHeader.boundingBox.vecBoundMin;
    bounds.vecBoundMax = colHeader.boundingBox.vecBoundMax;
    bounds.fRadius = colHeader.boundingBox.fRadius;

    pCollision->m_bounds = bounds;

    if ( IsNotNeutral( pCollision->m_unk, colHeader.unkFlag2 ) )
        pCollision->m_unk = !pCollision->m_unk;

    bufferSize -= sizeof( ColHeaderVersion4 );

    if ( bufferSize == 0 )
        return;

    void *mem = RwMalloc( bufferSize + sizeof( CColDataSA ), 0 );

    CColDataSA *colData = (CColDataSA*)mem;

    pCollision->pColData = colData;

    char *colBuffer = (char*)( (CColDataSA*)mem + 1 );

    pBuffer += sizeof( ColHeaderVersion4 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numTriangles = colHeader.numColMeshFaces;

    colData->unkFlag1 = false;

    if ( IsNotNeutral( colData->unkFlag2, colHeader.unkFlag4 ) )
        colData->unkFlag2 = !colData->unkFlag2;

    colData->numShadowMeshFaces = colHeader.numShadowMeshFaces;

    // Read actual collision data now.
    colData->pColSpheres =          GetCollisionDataFromSegment <CColSphereSA, 72>          ( mem, colHeader.offsetColSpheres );
    colData->pColBoxes =            GetCollisionDataFromSegment <CColBoxSA, 72>             ( mem, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     GetCollisionDataFromSegment <void, 72>                  ( mem, colHeader.offsetSuspensionLines );
    colData->pColVertices =         GetCollisionDataFromSegment <CColVertexSA, 72>          ( mem, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        GetCollisionDataFromSegment <CColTriangleSA, 72>        ( mem, colHeader.offsetColMeshFaces );

    colData->pShadowMeshVertices =  GetCollisionDataFromSegment <void, 72>                  ( mem, colHeader.offsetShadowMeshVertices );
    colData->pShadowMeshFaces =     GetCollisionDataFromSegment <CColShadowMeshFaceSA, 72>  ( mem, colHeader.offsetShadowMeshFaces );

    // Set up the shadow mesh.
    colData->hasShadowMeshFaces =
        ( colHeader.offsetShadowMeshFaces != 0 && colHeader.offsetShadowMeshVertices != 0 && colHeader.numShadowMeshFaces > 0 );

    unsigned int numShadowMeshVertices = 0;

    if ( colData->hasShadowMeshFaces )
        numShadowMeshVertices = colData->CalculateNumberOfShadowMeshVertices();

    colData->numShadowMeshVertices = numShadowMeshVertices;

    // Triangle planes not used in this version.
    colData->pColTrianglePlanes = NULL;

    pCollision->m_initWithColData = true;
}
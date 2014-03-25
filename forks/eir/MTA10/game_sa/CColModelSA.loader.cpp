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
_mallocNew_t    _mallocNew =            (_mallocNew_t)invalid_ptr;
_freeMemGame_t  _freeMemGame =          (_freeMemGame_t)invalid_ptr;


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

// Float packing.
inline short ColFloatToShort( float val )   { return (short)( val * 128.0f ); }
inline float ColShortToFloat( short val )   { return (float)val / 128.0f; }

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

    unsigned short  numColSpheres;                      // 40
    unsigned short  numColBoxes;                        // 42
    unsigned short  numColMeshFaces;                    // 44
    unsigned char   numWheels;                          // 46
    unsigned char   pad;                                // 47

    union
    {
        unsigned int        flags;                      // 48

        struct
        {
            unsigned int    unkFlag1 : 1;               // 48
            unsigned int    hasAnythingCollidable : 1;           
            unsigned int    unkFlag3 : 1;
            unsigned int    hasFaceGroups : 1;       
            unsigned int    hasShadowMeshFaces : 1;     // unused flag
        };
    };

    unsigned int    offsetColSpheres;                   // 52
    unsigned int    offsetColBoxes;                     // 56
    unsigned int    offsetSuspensionLines;              // 60
    unsigned int    offsetColMeshVertices;              // 64
    unsigned int    offsetColMeshFaces;                 // 68
    unsigned int    offsetTrianglePlanes;               // 72
};
C_ASSERT( sizeof( ColHeaderVersion2 ) == 76 );

struct ColHeaderVersion3 : public ColHeaderVersion2     //size: 88 bytes
{
    int             numShadowMeshFaces;         // 76

    unsigned int    offsetShadowMeshVertices;   // 80
    unsigned int    offsetShadowMeshFaces;      // 84
};
C_ASSERT( sizeof( ColHeaderVersion3 ) == 88 );

struct ColHeaderVersion4 : public ColHeaderVersion3     //size: 92 bytes
{
    int             unk;                        // 88
};
C_ASSERT( sizeof( ColHeaderVersion4 ) == 92 );

unsigned int CColDataSA::CalculateNumberOfShadowMeshVertices( void ) const
{
    // Calculates the number of shadow mesh vertices.
    // The number of shadow mesh vertices is equal to the highest index used in the shadow mesh (+1).
    unsigned int numVertices = 0;

    if ( numShadowMeshFaces != 0 )
    {
        if ( numShadowMeshFaces > 0 )
        {
            for ( int n = 0; n < numShadowMeshFaces; n++ )
            {
                CColTriangleSA *face = pShadowMeshFaces + n;

                if ( numVertices < face->v1 )
                {
                    numVertices = face->v1;
                }

                if ( numVertices < face->v2 )
                {
                    numVertices = face->v2;
                }

                if ( numVertices < face->v3 )
                {
                    numVertices = face->v3;
                }
            }
        }

        numVertices++;
    }

    return numVertices;
}

unsigned int CColDataSA::CalculateNumberOfCollisionMeshVertices( void ) const
{
    // Does exactly the same as CalculateNumberOfShadowMeshVertices but does it
    // for the collision mesh.
    unsigned int numVertices = 0;

    if ( numTriangles != 0 )
    {
        if ( numTriangles > 0 )
        {
            for ( int n = 0; n < numTriangles; n++ )
            {
                CColTriangleSA *face = pColTriangles + n;

                if ( numVertices < face->v1 )
                {
                    numVertices = face->v1;
                }

                if ( numVertices < face->v2 )
                {
                    numVertices = face->v2;
                }

                if ( numVertices < face->v3 )
                {
                    numVertices = face->v3;
                }
            }
        }

        numVertices++;
    }

    return numVertices;
}

template <typename segmentHeaderType>
struct SegmentAllocator
{
    inline void AllocateDataSegment( int segmentSize, segmentHeaderType*& header, char*& segmentData )
    {
        void *mem = RwMalloc( segmentSize + sizeof( segmentHeaderType ), 0 );

        header = (segmentHeaderType*)mem;

        segmentData = (char*)( (CColDataSA*)mem + 1 );
    }

    template <typename structType, typename headerType>
    inline structType* GetDataFromSegment( segmentHeaderType *segment, unsigned int offset )
    {
        if ( offset != 0 )
        {
            return (structType*)(
                (char*)segment + (
                    offset -
                        ( sizeof(headerType) -
                            ( sizeof(segmentHeaderType) -
                                ( sizeof(ColModelFileHeader) - 4 )
                        )
                    )
                )
            );
        }

        return NULL;
    }

    struct SegmentDistributor
    {
        size_t cur_offset;

        inline SegmentDistributor( void )
        {
            cur_offset = 0;
        }

        template <typename structType>
        inline structType* ObtainSegmentData( segmentHeaderType *header, int structCount )
        {
            structType *dataArray = NULL;

            if ( structCount != 0 )
            {
                size_t segmentDataSize = structCount * sizeof( structType );

                dataArray = (structType*)( (char*)( header + 1 ) + cur_offset );
            }

            return dataArray;
        }
    };

    inline SegmentDistributor GetDistributor( void )
    {
        return SegmentDistributor();
    }
};

template <typename structType>
inline structType* AllocateDataCount( unsigned int count )
{
    if ( count == 0 )
        return NULL;

    return (structType*)RwMalloc( count * sizeof( structType ), 0 );
}

template <typename structType>
inline void CopyArray( const structType *srcArray, structType *dstArray, size_t count )
{
    if ( srcArray && dstArray )
    {
        for ( size_t n = 0; n < count; n++ )
        {
            dstArray[ n ] = srcArray[ n ];
        }
    }
}

template <typename streamDataType, typename outputType, typename processorType>
inline outputType* ProcessArrayFromStream( const char*& streamPtr, int arrayCount, processorType& proc )
{
    if ( arrayCount > 0 )
    {
        void *mem = RwMalloc( sizeof( outputType ) * arrayCount, 0 );

        for ( int n = 0; n < arrayCount; n++ )
        {
            const streamDataType& streamData = *(const streamDataType*)streamPtr;

            proc.CreateInstance( streamData, (outputType*)mem + n );

            ( (const streamDataType*&)streamPtr )++;
        }

        proc.SetActualCount( arrayCount );

        return (outputType*)mem;
    }
    
    return NULL;
}

struct TSphereProc
{
    inline void CreateInstance( const TSphere& oldSphere, CColSphereSA *newSphere )
    {
        new (newSphere) CColSphereSA
            ( oldSphere.radius, oldSphere.center, oldSphere.surface.material, oldSphere.surface.flags, oldSphere.surface.unknown );
    }

    inline void SetActualCount( int count )
    {
        return;
    }
};

struct TBoxProc
{
    inline void CreateInstance( const CColBoxSA& colBox, CColBoxSA *newBox )
    {
        new (newBox)
            CColBoxSA( colBox.min, colBox.max, colBox.surface.material, colBox.surface.flags, colBox.surface.unknown );
    }

    inline void SetActualCount( int count )
    {
        return;
    }
};

struct TVertexProc
{
    inline void CreateInstance( const CVector& oldVertex, CColVertexSA *newVertex )
    {
        new (newVertex) CColVertexSA(
            ColFloatToShort( oldVertex.fX ),
            ColFloatToShort( oldVertex.fY ),
            ColFloatToShort( oldVertex.fZ )
        );
    }

    inline void SetActualCount( int count )
    {
        return;
    }
};

struct TFaceProc
{
    CColDataSA *colData;
    int actualCount;

    inline TFaceProc( CColDataSA *colData ) : colData( colData )
    {
        actualCount = 0;
    }

    inline void CreateInstance( const TFace& oldFace, CColTriangleSA *newFacePtr )
    {
        CColTriangleSA& newFace = *newFacePtr;
        newFace.v1 = oldFace.a;
        newFace.v2 = oldFace.b;
        newFace.v3 = oldFace.c;
        newFace.material = oldFace.surface.material;
        newFace.lighting = oldFace.surface.light;
    }

    inline void SetActualCount( int count )
    {
        colData->numTriangles = actualCount;
    }
};

void __cdecl LoadCollisionModel( const char *pBuffer, CColModelSAInterface *pCollision, const char *colName )
{
    const ColHeaderVersion1& colHeader = *(const ColHeaderVersion1*)pBuffer;

    // Copy the bounding box data over.
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

    {
        TSphereProc proc;

        colData->pColSpheres = ProcessArrayFromStream <TSphere, CColSphereSA> ( pBuffer, numColSpheres, proc );
    }

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

    {
        TBoxProc proc;

        colData->pColBoxes = ProcessArrayFromStream <CColBoxSA, CColBoxSA> ( pBuffer, numColBoxes, proc );
    }

    int numColVertices = *(int*)pBuffer;

    pBuffer += sizeof( int );

    {
        TVertexProc proc;

        colData->pColVertices = ProcessArrayFromStream <CVector, CColVertexSA> ( pBuffer, numColVertices, proc );
    }

    short numColFaces = *(short*)pBuffer;

    pBuffer += sizeof( int );

    colData->numTriangles = numColFaces;

    {
        TFaceProc proc( colData );

        colData->pColTriangles = ProcessArrayFromStream <TFace, CColTriangleSA> ( pBuffer, numColFaces, proc );
    }

    // Version 1 has no shadow mesh.
    colData->hasShadowMeshFaces = false;
    colData->pShadowMeshFaces = NULL;
    colData->pShadowMeshVertices = NULL;

    colData->numShadowMeshVertices = 0;

    // Check whether this collision has anything collidable.
    pCollision->m_isCollidable = ( colData->numSpheres != 0 || colData->numBoxes != 0 || colData->numTriangles != 0 );
}

void __cdecl LoadCollisionModelVer2( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    ColHeaderVersion2 colHeader = *(ColHeaderVersion2*)pBuffer;

    // Set up the collision bounding box.
    pCollision->m_bounds = colHeader.boundingBox;

    pCollision->m_isCollidable = colHeader.hasAnythingCollidable;

    bufferSize -= sizeof( ColHeaderVersion2 );

    if ( bufferSize == 0 )
        return;

    // Allocate the data segment.
    CColDataSA *colData = NULL;
    char *colBuffer = NULL;

    SegmentAllocator <CColDataSA> segAlloc;

    segAlloc.AllocateDataSegment( bufferSize, colData, colBuffer );

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion2 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numTriangles = colHeader.numColMeshFaces;

    colData->unkFlag1 = false;
    colData->hasShadowMeshFaces = false;
    colData->hasFaceGroups = colHeader.hasFaceGroups;

    // Read actual collision data now.
    colData->pColSpheres =          segAlloc.GetDataFromSegment <CColSphereSA, ColHeaderVersion2>           ( colData, colHeader.offsetColSpheres );
    colData->pColBoxes =            segAlloc.GetDataFromSegment <CColBoxSA, ColHeaderVersion2>              ( colData, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     segAlloc.GetDataFromSegment <CColSuspensionLineSA, ColHeaderVersion2>   ( colData, colHeader.offsetSuspensionLines );
    colData->pColVertices =         segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion2>           ( colData, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion2>         ( colData, colHeader.offsetColMeshFaces );

    // Null out data that is not available in version 2
    colData->pColTrianglePlanes = NULL;

    colData->hasShadowMeshFaces = false;
    colData->pShadowMeshVertices = NULL;
    colData->pShadowMeshFaces = NULL;

    colData->numShadowMeshVertices = 0;

    pCollision->m_isColDataSegmented = true;
}

void __cdecl LoadCollisionModelVer3( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    ColHeaderVersion3 colHeader = *(ColHeaderVersion3*)pBuffer;

    // Set up the collision bounding box.
    pCollision->m_bounds = colHeader.boundingBox;

    pCollision->m_isCollidable = colHeader.hasAnythingCollidable;

    bufferSize -= sizeof( ColHeaderVersion3 );

    if ( bufferSize == 0 )
        return;

    CColDataSA *colData = NULL;
    char *colBuffer = NULL;

    SegmentAllocator <CColDataSA> segAlloc;

    segAlloc.AllocateDataSegment( bufferSize, colData, colBuffer );

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion3 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numTriangles = colHeader.numColMeshFaces;

    colData->unkFlag1 = false;
    colData->hasFaceGroups = colHeader.hasFaceGroups;
    colData->numShadowMeshFaces = colHeader.numShadowMeshFaces;

    // Read actual collision data now.
    colData->pColSpheres =          segAlloc.GetDataFromSegment <CColSphereSA, ColHeaderVersion3>           ( colData, colHeader.offsetColSpheres );
    colData->pColBoxes =            segAlloc.GetDataFromSegment <CColBoxSA, ColHeaderVersion3>              ( colData, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     segAlloc.GetDataFromSegment <CColSuspensionLineSA, ColHeaderVersion3>   ( colData, colHeader.offsetSuspensionLines );
    colData->pColVertices =         segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion3>           ( colData, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion3>         ( colData, colHeader.offsetColMeshFaces );

    colData->pShadowMeshVertices =  segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion3>           ( colData, colHeader.offsetShadowMeshVertices );
    colData->pShadowMeshFaces =     segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion3>         ( colData, colHeader.offsetShadowMeshFaces );

    // Set up the shadow mesh.
    colData->hasShadowMeshFaces =
        ( colHeader.offsetShadowMeshFaces != 0 && colHeader.offsetShadowMeshVertices != 0 && colHeader.numShadowMeshFaces > 0 );

    unsigned int numShadowMeshVertices = 0;

    if ( colData->hasShadowMeshFaces )
        numShadowMeshVertices = colData->CalculateNumberOfShadowMeshVertices();

    colData->numShadowMeshVertices = numShadowMeshVertices;

    // Triangle planes not used in this version.
    colData->pColTrianglePlanes = NULL;

    pCollision->m_isColDataSegmented = true;
}

void __cdecl LoadCollisionModelVer4( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName )
{
    ColHeaderVersion4 colHeader = *(ColHeaderVersion4*)pBuffer;

    // Set up the collision bounding box.
    pCollision->m_bounds = colHeader.boundingBox;

    pCollision->m_isCollidable = colHeader.hasAnythingCollidable;

    bufferSize -= sizeof( ColHeaderVersion4 );

    if ( bufferSize == 0 )
        return;

    CColDataSA *colData = NULL;
    char *colBuffer = NULL;

    SegmentAllocator <CColDataSA> segAlloc;

    segAlloc.AllocateDataSegment( bufferSize, colData, colBuffer );

    pCollision->pColData = colData;

    pBuffer += sizeof( ColHeaderVersion4 );

    memcpy( colBuffer, pBuffer, bufferSize );

    // Initialize the collision data.
    colData->numSpheres = colHeader.numColSpheres;
    colData->numBoxes = colHeader.numColBoxes;
    colData->ucNumWheels = colHeader.numWheels;
    colData->numTriangles = colHeader.numColMeshFaces;

    colData->unkFlag1 = false;
    colData->hasFaceGroups = colHeader.hasFaceGroups;
    colData->numShadowMeshFaces = colHeader.numShadowMeshFaces;

    // Read actual collision data now.
    colData->pColSpheres =          segAlloc.GetDataFromSegment <CColSphereSA, ColHeaderVersion4>           ( colData, colHeader.offsetColSpheres );
    colData->pColBoxes =            segAlloc.GetDataFromSegment <CColBoxSA, ColHeaderVersion4>              ( colData, colHeader.offsetColBoxes );
    colData->pSuspensionLines =     segAlloc.GetDataFromSegment <CColSuspensionLineSA, ColHeaderVersion4>   ( colData, colHeader.offsetSuspensionLines );
    colData->pColVertices =         segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion4>           ( colData, colHeader.offsetColMeshVertices );
    colData->pColTriangles =        segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion4>         ( colData, colHeader.offsetColMeshFaces );

    colData->pShadowMeshVertices =  segAlloc.GetDataFromSegment <CColVertexSA, ColHeaderVersion4>           ( colData, colHeader.offsetShadowMeshVertices );
    colData->pShadowMeshFaces =     segAlloc.GetDataFromSegment <CColTriangleSA, ColHeaderVersion4>         ( colData, colHeader.offsetShadowMeshFaces );

    // Set up the shadow mesh.
    colData->hasShadowMeshFaces =
        ( colHeader.offsetShadowMeshFaces != 0 && colHeader.offsetShadowMeshVertices != 0 && colHeader.numShadowMeshFaces > 0 );

    unsigned int numShadowMeshVertices = 0;

    if ( colData->hasShadowMeshFaces )
        numShadowMeshVertices = colData->CalculateNumberOfShadowMeshVertices();

    colData->numShadowMeshVertices = numShadowMeshVertices;

    // Triangle planes not used in this version.
    colData->pColTrianglePlanes = NULL;

    pCollision->m_isColDataSegmented = true;
}

// MTA extension.
CColModelSAInterface* CColModelSAInterface::Clone( void )
{
    // To clone the interface, we must follow the rules of allocation.
    // We cannot simply copy the fields.
    CColModelSAInterface *clone = new CColModelSAInterface;

    // If allocation failed, we bail.
    if ( !clone )
        return NULL;

    // Copy bounding information.
    clone->m_bounds = this->m_bounds;
    
    // Clone collision data.
    CColDataSA *srcColData = this->pColData;
    CColDataSA *dstColData = NULL;

    CColSphereSA            *dstColSphereArray = NULL;
    CColBoxSA               *dstColBoxArray = NULL;
    CColSuspensionLineSA    *dstColSuspensionLineArray = NULL;
    CColVertexSA            *dstColVertexArray = NULL;
    CColTriangleSA          *dstColTriangleArray = NULL;
    CColTrianglePlaneSA     *dstColTrianglePlaneArray = NULL;

    CColVertexSA            *dstColShadowMeshVertexArray = NULL;
    CColTriangleSA          *dstColShadowMeshFaceArray = NULL;

    int numSpheres = srcColData->numSpheres;
    int numBoxes = srcColData->numBoxes;
    int numWheels = srcColData->ucNumWheels;
    int numTriangles = srcColData->numTriangles;

    int numVertices = srcColData->CalculateNumberOfCollisionMeshVertices();

    int numShadowMeshVertices = srcColData->numShadowMeshVertices;
    int numShadowMeshFaces = srcColData->numShadowMeshFaces;

    // Allocate depending on allocation method.
    size_t sphereArraySize = sizeof( CColSphereSA ) * numSpheres;
    size_t boxArraySize = sizeof( CColBoxSA ) * numBoxes;
    size_t suspensionLineArraySize = sizeof( CColSuspensionLineSA ) * numWheels;
    size_t vertexArraySize = sizeof( CColVertexSA ) * numVertices;
    size_t triangleArraySize = sizeof( CColTriangleSA ) * numTriangles;

    if ( this->m_isColDataSegmented )
    {
        // Calculate the size of all data.
        size_t dataBufferSize = 
            sphereArraySize +
            boxArraySize +
            suspensionLineArraySize +
            vertexArraySize +
            triangleArraySize;

        SegmentAllocator <CColDataSA> segAlloc;

        char *segBuffer;

        segAlloc.AllocateDataSegment( dataBufferSize, dstColData, segBuffer );

        // Set up the data pointers.
        SegmentAllocator <CColDataSA>::SegmentDistributor segDistr = segAlloc.GetDistributor();

        dstColSphereArray =             segDistr.ObtainSegmentData <CColSphereSA>           ( dstColData, numSpheres );
        dstColBoxArray =                segDistr.ObtainSegmentData <CColBoxSA>              ( dstColData, numBoxes );
        dstColSuspensionLineArray =     segDistr.ObtainSegmentData <CColSuspensionLineSA>   ( dstColData, numWheels );
        dstColVertexArray =             segDistr.ObtainSegmentData <CColVertexSA>           ( dstColData, numVertices );
        dstColTriangleArray =           segDistr.ObtainSegmentData <CColTriangleSA>         ( dstColData, numTriangles );

        dstColShadowMeshVertexArray =   segDistr.ObtainSegmentData <CColVertexSA>           ( dstColData, numShadowMeshVertices );
        dstColShadowMeshFaceArray =     segDistr.ObtainSegmentData <CColTriangleSA>         ( dstColData, numShadowMeshFaces );
    }
    else
    {
        // We allocate separate buffers for each data.
        CColDataSA *colData = new (_mallocNew( sizeof( CColDataSA ) )) CColDataSA;

        dstColSphereArray =             AllocateDataCount <CColSphereSA>            ( numSpheres );
        dstColBoxArray =                AllocateDataCount <CColBoxSA>               ( numBoxes );
        dstColSuspensionLineArray =     AllocateDataCount <CColSuspensionLineSA>    ( numWheels );
        dstColVertexArray =             AllocateDataCount <CColVertexSA>            ( numVertices );
        dstColTriangleArray =           AllocateDataCount <CColTriangleSA>          ( numTriangles );

        dstColShadowMeshVertexArray =   AllocateDataCount <CColVertexSA>            ( numShadowMeshVertices );
        dstColShadowMeshFaceArray =     AllocateDataCount <CColTriangleSA>          ( numShadowMeshFaces );
    }
    clone->m_isColDataSegmented = this->m_isColDataSegmented;

    // Copy over general stuff.
    dstColData->numSpheres = numSpheres;
    dstColData->numBoxes = numBoxes;
    dstColData->ucNumWheels = numWheels;
    dstColData->numTriangles = numTriangles;

    dstColData->numShadowMeshVertices = numShadowMeshVertices;
    dstColData->numShadowMeshFaces = numShadowMeshFaces;

    dstColData->unkFlag1 = srcColData->unkFlag1;
    dstColData->hasFaceGroups = srcColData->hasFaceGroups;
    dstColData->hasShadowMeshFaces = srcColData->hasShadowMeshFaces;

    // Copy over collision objects.
    CopyArray( srcColData->pColSpheres,             dstColSphereArray, numSpheres );
    CopyArray( srcColData->pColBoxes,               dstColBoxArray, numBoxes );
    CopyArray( srcColData->pSuspensionLines,        dstColSuspensionLineArray, numWheels );
    CopyArray( srcColData->pColVertices,            dstColVertexArray, numVertices );
    CopyArray( srcColData->pColTriangles,           dstColTriangleArray, numTriangles );

    CopyArray( srcColData->pShadowMeshVertices,     dstColShadowMeshVertexArray, numShadowMeshVertices );
    CopyArray( srcColData->pShadowMeshFaces,        dstColShadowMeshFaceArray, numShadowMeshFaces );

    // Set the array pointers to the destination struct.
    dstColData->pColSpheres = dstColSphereArray;
    dstColData->pColBoxes = dstColBoxArray;
    dstColData->pSuspensionLines = dstColSuspensionLineArray;
    dstColData->pColVertices = dstColVertexArray;
    dstColData->pColTriangles = dstColTriangleArray;
    
    dstColData->pShadowMeshVertices = dstColShadowMeshVertexArray;
    dstColData->pShadowMeshFaces = dstColShadowMeshFaceArray;

    // Copy over remaining settings.
    clone->m_isCollidable = this->m_isCollidable;

    clone->pColData = dstColData;
    
    // Set up individual data.
    clone->m_colPoolIndex = 0;  // index zero is dummy index. we do not belong to any sector.
    return clone;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F6E0
CColDataSA::trianglePlanesListNode* CColDataSA::GetTrianglePlanesListNode( void )
{
    return *(trianglePlanesListNode**)( ALIGN( (unsigned int)pColTrianglePlanes + numTriangles * 10, 4, 4 ) );
}

inline CColDataSA::trianglePlanesListNode& GetFreeTrianglePlanesNode( void )
{
    return *(CColDataSA::trianglePlanesListNode*)0x00965944;
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00416400
void CColDataSA::SegmentedClear( void )
{
    // Deallocate the triangle planes array.
    if ( pColTrianglePlanes )
    {
        // Get the triangle planes list node.
        trianglePlanesListNode *node = GetTrianglePlanesListNode();

        LIST_REMOVE( *node );
        LIST_APPEND( GetFreeTrianglePlanesNode(), *node );

        RwFree( pColTrianglePlanes );

        pColTrianglePlanes = NULL;
    }
}

template <typename structType>
inline void SafeFree( structType *data )
{
    if ( data )
    {
        RwFree( data );
    }
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F070
void CColDataSA::UnsegmentedClear( void )
{
    SafeFree( pColSpheres );
    SafeFree( pColBoxes );
    SafeFree( pSuspensionLines );
    SafeFree( pColVertices );
    SafeFree( pColTriangles );

    SafeFree( pShadowMeshVertices );
    SafeFree( pShadowMeshFaces );

    SegmentedClear();

    // Clear data fields.
    numSpheres = 0;
    ucNumWheels = 0;
    numBoxes = 0;
    numTriangles = 0;

    pColSpheres = NULL;
    pColBoxes = NULL;
    pSuspensionLines = NULL;
    pColVertices = NULL;
    pColTriangles = NULL;

    pShadowMeshVertices = NULL;
    pShadowMeshFaces = NULL;
}
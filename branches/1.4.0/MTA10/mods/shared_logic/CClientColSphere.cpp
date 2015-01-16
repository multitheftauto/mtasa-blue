/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColSphere.cpp
*  PURPOSE:     Sphere-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

namespace
{
    //
    // CClientColSphere helpers
    //

    // Used in CreateSphereFaces 
    struct SFace
    {
        SFace ( const CVector& a, const CVector& b, const CVector& c ) : a ( a ), b ( b ), c ( c ) {}
        CVector a, b, c;
    };

    //
    // Create faces for a sphere 
    //
    void CreateSphereFaces ( std::vector < SFace >& faceList, int iIterations )
    {
        int numFaces = (int)( pow ( 4.0, iIterations ) * 8 );
        faceList.clear ();
        faceList.reserve ( numFaces );

        // Initial octahedron
        static SFixedArray < CVector, 6 > vecPoints = { CVector(0,0,1),  CVector(0,0,-1),  CVector(-1,-1,0),  CVector(1,-1,0),  CVector(1,1,0), CVector(-1,1,0) };
        static const SFixedArray < WORD, 24 > indices = { 0, 3, 4,  0, 4, 5,  0, 5, 2,  0, 2, 3,  1, 4, 3,  1, 5, 4,  1, 2, 5,  1, 3, 2 };

        for ( uint i = 0 ; i < NUMELMS( vecPoints ) ; i ++ )
            vecPoints[ i ].Normalize ();

        for ( uint i = 0 ; i < NUMELMS( indices ) ; i += 3 )
            faceList.push_back ( SFace ( vecPoints[ indices[i] ],  vecPoints[ indices[i + 1] ], vecPoints[ indices[i + 2] ] ) );

        // For each iteration
        while ( iIterations-- )
        {
            // Divide each face into 4
            for ( int i = faceList.size () - 1 ; i >= 0 ; i-- )
            {
                // Get the three face points
                CVector a = faceList[i].a;
                CVector b = faceList[i].b;
                CVector c = faceList[i].c;

                // Make three inner points
                CVector a2 = ( a + b ) * 0.5f;
                CVector b2 = ( b + c ) * 0.5f;
                CVector c2 = ( c + a ) * 0.5f;

                // Keep points to the edge of the unit sphere
                a2.Normalize ();
                b2.Normalize ();
                c2.Normalize ();

                // Replace the original face with the first sub-face
                faceList[i] = SFace ( a2, b2, c2 );
 
                // Add the three other sub-faces to the end of the list
                faceList.push_back ( SFace ( a, a2, c2 ) );
                faceList.push_back ( SFace ( b, a2, b2 ) );
                faceList.push_back ( SFace ( c, b2, c2 ) );
            }
        }     
    }

    //
    // For holding the lines of an optimized wireframe sphere
    //
    struct SWireModel
    {
        std::vector < CVector > vertexList;

        // Add a line if it is unique
        void AddLine ( const CVector& from, const CVector& to )
        {
            // Conform order
            if ( from.fX > to.fX ||
                ( from.fX == to.fX && from.fY > to.fY || 
                ( from.fY == to.fY && from.fZ > to.fZ ) ) )
                return AddLine( to, from );

            // Find existing line
            for ( int i = 0 ; i < (int)vertexList.size () - 1  ; i += 2 )
                if ( ( from - vertexList[ i ] ).LengthSquared () < 0.00001f )
                    if ( ( to - vertexList[ i + 1 ] ).LengthSquared () < 0.00001f )
                        return; // Duplicated

            // Add new line
            vertexList.push_back ( from );
            vertexList.push_back ( to );
        }
    };

    const SWireModel& GetSphereWireModel ( int iterations )
    {
        static std::map < uint, SWireModel > wireModelMap;

        // Find existing
        SWireModel* pWireModel = MapFind ( wireModelMap, iterations );
        if ( pWireModel )
            return *pWireModel;

        // Add new
        MapSet ( wireModelMap, iterations, SWireModel () );
        SWireModel& wireModel = *MapFind ( wireModelMap, iterations );

        std::vector < SFace > faceList;
        CreateSphereFaces ( faceList, iterations );

        // Create big vertex/line list
        for ( uint i = 0 ; i < faceList.size () ; i++ )
        {
            wireModel.AddLine( faceList[i].a, faceList[i].b );
            wireModel.AddLine( faceList[i].b, faceList[i].c );
            wireModel.AddLine( faceList[i].c, faceList[i].a );
        }
        return wireModel;
    }
}


CClientColSphere::CClientColSphere ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius ) : ClassInit ( this ), CClientColShape ( pManager, ID )
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    UpdateSpatialData ();
}


bool CClientColSphere::DoHitDetection ( const CVector& vecNowPosition, float fRadius )
{
    // Do a simple distance check between now position and our position 
    return IsPointNearPoint3D ( vecNowPosition, m_vecPosition, fRadius + m_fRadius );
}


CSphere CClientColSphere::GetWorldBoundingSphere ( void )
{
    return CSphere ( m_vecPosition, m_fRadius );
}


//
// Draw wireframe sphere
//
void CClientColSphere::DebugRender ( const CVector& vecPosition, float fDrawRadius )
{
    SColorARGB color ( 64, 255, 0, 0 );
    float fLineWidth = 4.f + pow ( m_fRadius, 0.5f );
    CGraphicsInterface* pGraphics = g_pCore->GetGraphics ();

    uint iterations = Clamp ( 1, Round ( pow ( m_fRadius, 0.25f ) * 0.75f ), 4 );

    const SWireModel& model = GetSphereWireModel ( iterations );

    for ( uint i = 0 ; i < model.vertexList.size () ; i += 2 )
    {
        const CVector& vecBegin = model.vertexList[ i ] * m_fRadius + m_vecPosition;
        const CVector& vecEnd = model.vertexList[ i + 1 ] * m_fRadius + m_vecPosition;
        pGraphics->DrawLine3DQueued ( vecBegin, vecEnd, fLineWidth, color, false );
    }
}

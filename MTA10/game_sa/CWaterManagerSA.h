/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CWaterManagerSA.h
*  PURPOSE:		Control the lakes and seas
*  DEVELOPERS:	arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATERMANAGERSA_H
#define __CWATERMANAGERSA_H

#include "CWaterSA.h"

#define FUNC_ReadWaterConfiguration        0x6EAE80         // ()
#define FUNC_CreateWaterVertex             0x6E5A40         // (word x, word y, float z, float unknown, float height, word flow)
#define FUNC_CreateWaterQuad               0x6E7EF0         // (word x1, word y1, float z1, float unknown1, float height1, word flow1, word x2, ..., word x3, ..., word x4, ..., word flags)
#define FUNC_CreateWaterTriangle           0x6E7D40         // (word x1, word y1, float z1, float unknown1, float height1, word flow1, word x2, ..., word x3, ..., word flags)
#define FUNC_BuildWaterIndex               0x6E7B30         // ()
#define FUNC_AddWaterPolyToZone            0x6E5750         // (zoneColumn, zoneRow, polyID, polyType)
#define FUNC_GetWaterLevel                 0x6EB690
#define FUNC_TestLineAgainstWater          0x6E61B0

typedef void (__cdecl *ReadWaterConfiguration_t)();
typedef WORD (__cdecl *CreateWaterVertex_t)(short sX, short sY, float fZ, float fUnknown, float fHeight, WORD wFlow);
typedef void (__cdecl *BuildWaterIndex_t)();
typedef bool (__cdecl *TestLineAgainstWater_t)(float fStartX, float fStartY, float fStartZ, float fEndX, float fEndY, float fEndZ, CVector* pvecCollision);
typedef bool (__cdecl *GetWaterLevel_t)(float fX, float fY, float fZ, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown);

#define VAR_NumWaterVertices               0xC2288C
#define VAR_NumWaterQuads                  0xC22888
#define VAR_NumWaterTriangles              0xC22884
#define VAR_NumWaterZonePolys              0xC215F0

#define NUM_DefWaterVertices               0x3FD        // Sizes of pools reserved in gta_sa.exe
#define NUM_DefWaterQuads                  0x12D
#define NUM_DefWaterTriangles              0x6
#define NUM_DefWaterZonePolys              0x2BC
#define NUM_NewWaterVertices               0x600        // Sizes of replacement pools in MTA
#define NUM_NewWaterQuads                  0x200
#define NUM_NewWaterTriangles              0x20
#define NUM_NewWaterZonePolys              0x400
#define NUM_WaterZones                     (12 * 12)

#define ARRAY_WaterVertices                0xC22910
#define ARRAY_WaterQuads                   0xC21C90
#define ARRAY_WaterTriangles               0xC22854
#define ARRAY_WaterZones                   0xC21B70
#define ARRAY_WaterZonePolys               0xC215F8

// -------------------------------
// SA interfaces

class CWaterPolyEntrySAInterface
{
public:
    WORD m_wValue;
};

// -------------------------------
// Interface wrappers

class CWaterZoneSA
{
public:
                                     CWaterZoneSA      () { m_pInterface = NULL; }
                                     CWaterZoneSA      ( CWaterPolyEntrySAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterPolyEntrySAInterface*      GetInterface      () { return m_pInterface; }
    void                             SetInterface      ( CWaterPolyEntrySAInterface* pInterface ) { m_pInterface = pInterface; }

    CWaterPolyEntrySAInterface*      AddPoly           ( EWaterPolyType type, WORD wID );
    CWaterPolyEntrySAInterface*      AddPoly           ( CWaterPoly* pPoly );
    bool                             RemovePoly        ( EWaterPolyType type, WORD wID );
    bool                             RemovePoly        ( CWaterPoly* pPoly );

    class iterator
    {
    public:
                                     iterator          ();
                                     iterator          ( CWaterZoneSA* pZone );
                                     iterator          ( iterator& other );
        iterator&                    operator=         ( iterator& other );
        void                         operator++        ();
        void                         operator--        ();
        iterator                     operator+         ( int n );
        iterator                     operator-         ( int n );
        int                          operator-         ( iterator& other );
        bool                         operator==        ( iterator& other );
        bool                         operator!=        ( iterator& other );
        CWaterPolySA*                operator*         ();
                                     operator CWaterPolyEntrySAInterface* ();
        
    private:
        CWaterPolyEntrySAInterface*  m_pFirst;
        CWaterPolyEntrySAInterface*  m_pCurrent;
        bool                         m_bSinglePoly;
    };

    iterator                         begin             ();
    iterator                         end               ();
    int                              GetID             ();

private:
    CWaterPolyEntrySAInterface*      m_pInterface;
};

// -------------------------------
// Change trackkeepers

class CWaterChange
{
public:
    virtual void                     Undo              ( void* pChangedObject ) = 0;
};

class CWaterChangeVertexMove : public CWaterChange
{
public:
                                     CWaterChangeVertexMove ( CWaterVertex* pVertex ) { pVertex->GetPosition ( m_vecOriginalPosition ); }
    void                             Undo              ( void* pChangedObject );
private:
    CVector                          m_vecOriginalPosition;
};

// -------------------------------
// Manager

class CWaterManagerSA : public CWaterManager
{
public:
                                     CWaterManagerSA   ();
                                     ~CWaterManagerSA  ();

    void                             RelocatePools     ();
    void                             InstallHooks      ();
    CWaterZoneSA*                    GetZoneContaining ( float fX, float fY );
    void                             GetZonesContaining ( CWaterPoly* pPoly, std::vector < CWaterZoneSA* >& out );
    void                             GetZonesContaining ( CVector& v1, CVector& v2, CVector& v3, std::vector < CWaterZoneSA* >& out );

    CWaterVertex*                    CreateVertex      ( CVector& vecPosition );

    CWaterPoly*                      GetPolyAtPoint    ( CVector& vecPosition );
    CWaterPoly*                      CreateQuad        ( CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow = false );
    CWaterPoly*                      CreateTriangle    ( CVector& vec1, CVector& vec2, CVector& vec3, bool bShallow = false );
    bool                             DeletePoly        ( CWaterPoly* pPoly );

    bool                             GetWaterLevel     ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown );
    bool                             SetWaterLevel     ( CVector* pvecPosition, float fLevel, void* pChangeSource = NULL );
    bool                             TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector* vecCollision );

    void                             AddChange         ( void* pChangeSource, void* pChangedObject, CWaterChange* pChange );
    void                             UndoChanges       ( void* pChangeSource = NULL );
    void                             RebuildIndex      ();
    void                             Reset             ();

private:
    std::vector < CWaterVertexSA >   m_Vertices;
    std::vector < CWaterQuadSA >     m_Quads;
    std::vector < CWaterTriangleSA > m_Triangles;
    std::vector < CWaterZoneSA >     m_Zones;

    CWaterVertexSAInterface          m_VertexPool[NUM_NewWaterVertices];
    CWaterQuadSAInterface            m_QuadPool[NUM_NewWaterQuads];
    CWaterTriangleSAInterface        m_TrianglePool[NUM_NewWaterTriangles];
    CWaterPolyEntrySAInterface       m_ZonePolyPool[NUM_NewWaterZonePolys];

    static DWORD                     m_VertexXrefs[];
    static DWORD                     m_QuadXrefs[];
    static DWORD                     m_TriangleXrefs[];
    static DWORD                     m_ZonePolyXrefs[];

    std::map < void*, std::map < void*, CWaterChange* > > m_Changes;

    friend class                     CWaterVertexSA;
    friend class                     CWaterPolySA;
    friend class                     CWaterQuadSA;
    friend class                     CWaterTriangleSA;
    friend class                     CWaterZoneSA;
    friend class                     CWaterZoneSA::iterator;
};

#endif

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWaterManagerSA.h
 *  PURPOSE:     Control the lakes and seas
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CWaterManager.h>
#include "CWaterSA.h"

#define DEFAULT_WATER_LEVEL                0.0f
#define DEFAULT_WAVE_LEVEL                 0.0f

#define FUNC_ReadWaterConfiguration        0x6EAE80         // ()
#define FUNC_CreateWaterVertex             0x6E5A40         // (word x, word y, float z, float unknown, float height, word flow)
#define FUNC_CreateWaterQuad               0x6E7EF0         // (word x1, word y1, float z1, float unknown1, float height1, word flow1, word x2, ..., word x3, ..., word x4, ..., word flags)
#define FUNC_CreateWaterTriangle           0x6E7D40         // (word x1, word y1, float z1, float unknown1, float height1, word flow1, word x2, ..., word x3, ..., word flags)
#define FUNC_BuildWaterIndex               0x6E7B30         // ()
#define FUNC_AddWaterPolyToZone            0x6E5750         // (zoneColumn, zoneRow, polyID, polyType)
#define FUNC_GetWaterLevel                 0x6EB690
#define FUNC_TestLineAgainstWater          0x6E61B0

typedef void(__cdecl* ReadWaterConfiguration_t)();
typedef WORD(__cdecl* CreateWaterVertex_t)(long lX, long lY, float fZ, float fUnknown, float fHeight, WORD wFlow);
typedef void(__cdecl* BuildWaterIndex_t)();
typedef bool(__cdecl* TestLineAgainstWater_t)(float fStartX, float fStartY, float fStartZ, float fEndX, float fEndY, float fEndZ, CVector* pvecCollision);
typedef bool(__cdecl* GetWaterLevel_t)(float fX, float fY, float fZ, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown);

#define VAR_NumWaterVertices               0xC2288C
#define VAR_NumWaterQuads                  0xC22888
#define VAR_NumWaterTriangles              0xC22884
#define VAR_NumWaterZonePolys              0xC215F0
#define VAR_WaveLevel                      0xC812E8

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
    CWaterZoneSA() { m_pInterface = NULL; }
    CWaterZoneSA(CWaterPolyEntrySAInterface* pInterface) { m_pInterface = pInterface; }

    CWaterPolyEntrySAInterface* GetInterface() { return m_pInterface; }
    void                        SetInterface(CWaterPolyEntrySAInterface* pInterface) { m_pInterface = pInterface; }

    CWaterPolyEntrySAInterface* AddPoly(EWaterPolyType type, WORD wID);
    CWaterPolyEntrySAInterface* AddPoly(CWaterPoly* pPoly);
    bool                        RemovePoly(EWaterPolyType type, WORD wID);
    bool                        RemovePoly(CWaterPoly* pPoly);

    class iterator
    {
    public:
        iterator();
        iterator(CWaterZoneSA* pZone);
        iterator(const iterator& other);
        iterator&     operator=(const iterator& other);
        void          operator++();
        void          operator--();
        iterator      operator+(int n);
        iterator      operator-(int n);
        int           operator-(iterator& other);
        bool          operator==(const iterator& other);
        bool          operator!=(const iterator& other);
        CWaterPolySA* operator*();
                      operator CWaterPolyEntrySAInterface*();

    private:
        CWaterPolyEntrySAInterface* m_pFirst;
        CWaterPolyEntrySAInterface* m_pCurrent;
        bool                        m_bSinglePoly;
    };

    iterator begin();
    iterator end();
    int      GetID();

private:
    CWaterPolyEntrySAInterface* m_pInterface;
};

// -------------------------------
// Change trackkeepers

class CWaterChange
{
public:
    virtual void Undo(void* pChangedObject) = 0;
};

class CWaterChangeVertexMove : public CWaterChange
{
public:
    CWaterChangeVertexMove(CWaterVertex* pVertex) { pVertex->GetPosition(m_vecOriginalPosition); }
    void Undo(void* pChangedObject);

private:
    CVector m_vecOriginalPosition;
};

// -------------------------------
// Manager

class CWaterManagerSA : public CWaterManager
{
public:
    CWaterManagerSA();
    ~CWaterManagerSA();

    void          RelocatePools();
    void          InstallHooks();
    CWaterZoneSA* GetZone(int iCol, int iRow);
    CWaterZoneSA* GetZoneContaining(float fX, float fY);
    void          GetZonesContaining(CWaterPoly* pPoly, std::vector<CWaterZoneSA*>& out);
    void          GetZonesContaining(const CVector& v1, const CVector& v2, const CVector& v3, std::vector<CWaterZoneSA*>& out);
    void          GetZonesIntersecting(const CVector& startPos, const CVector& endPos, std::vector<CWaterZoneSA*>& vecOut);

    CWaterVertex* CreateVertex(const CVector& vecPosition);

    CWaterPoly* GetPolyAtPoint(const CVector& vecPosition);
    CWaterPoly* CreateQuad(const CVector& vecBL, const CVector& vecBR, const CVector& vecTL, const CVector& vecTR, bool bShallow = false);
    CWaterPoly* CreateTriangle(const CVector& vec1, const CVector& vec2, const CVector& vec3, bool bShallow = false);
    bool        DeletePoly(CWaterPoly* pPoly);

    bool GetWaterLevel(const CVector& vecPosition, float* pfLevel, bool ignoreDistanceToWaterThreshold, CVector* pvecUnknown);

    bool SetWorldWaterLevel(float fLevel, void* pChangeSource, bool bIncludeWorldNonSeaLevel, bool bIncludeWorldSeaLevel, bool bIncludeOutsideWorldLevel);
    bool SetPositionWaterLevel(const CVector& vecPosition, float fLevel, void* pChangeSource);
    bool SetPolyWaterLevel(CWaterPoly* pPoly, float fLevel, void* pChangeSource);
    void SetOutsideWorldWaterLevel(float fLevel);
    void ResetWorldWaterLevel();

    float GetWaveLevel();
    void  SetWaveLevel(float fWaveLevel);
    void  SetWaterDrawnLast(bool bEnable);
    bool  IsWaterDrawnLast();

    bool IsPointOutsideOfGameArea(const CVector& vecPos);
    bool TestLineAgainstWater(const CVector& vecStart, const CVector& vecEnd, CVector* vecCollision);

    void AddChange(void* pChangeSource, void* pChangedObject, CWaterChange* pChange);
    void UndoChanges(void* pChangeSource = NULL);
    void RebuildIndex();
    void Reset();
    void UpdateRenderOrderRequirement();

private:
    CWaterVertexSA   m_Vertices[NUM_NewWaterVertices];
    CWaterQuadSA     m_Quads[NUM_NewWaterQuads];
    CWaterTriangleSA m_Triangles[NUM_NewWaterTriangles];
    CWaterZoneSA     m_Zones[NUM_WaterZones];

    CWaterVertexSAInterface    m_VertexPool[NUM_NewWaterVertices];
    CWaterQuadSAInterface      m_QuadPool[NUM_NewWaterQuads];
    CWaterTriangleSAInterface  m_TrianglePool[NUM_NewWaterTriangles];
    CWaterPolyEntrySAInterface m_ZonePolyPool[NUM_NewWaterZonePolys];

    static DWORD m_VertexXrefs[];
    static DWORD m_QuadXrefs[];
    static DWORD m_TriangleXrefs[];
    static DWORD m_ZonePolyXrefs[];

    std::map<void*, std::map<void*, CWaterChange*> > m_Changes;
    bool                                             m_bAltRenderOrder;
    bool                                             m_bInitializedVertices;
    int                                              m_iActivePolyCount;
    bool                                             m_bWaterDrawnLast;

    friend class CWaterVertexSA;
    friend class CWaterPolySA;
    friend class CWaterQuadSA;
    friend class CWaterTriangleSA;
    friend class CWaterZoneSA;
    friend class CWaterZoneSA::iterator;
};

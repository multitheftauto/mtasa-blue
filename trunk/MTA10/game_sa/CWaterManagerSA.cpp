/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CWaterManagerSA.cpp
*  PURPOSE:		Control the lakes and seas
*  DEVELOPERS:	arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define POLYENTRY_TYPE(entry) ( (entry)->m_wValue >> 14 )
#define POLYENTRY_ID(entry) ( (entry)->m_wValue & 0x3FFF )
#define MAKE_POLYENTRY(type, id) (WORD)( ((type) << 14) | (id) )

DWORD CWaterManagerSA::m_VertexXrefs[] = {
    0x6E5B6E, 0x6E5BC3, 0x6E5BF7, 0x6E5EA3, 0x6E5ED7, 0x6E5F84, 0x6E5F8B, 0x6E6487,
	0x6E64A7, 0x6E65E4, 0x6E6608, 0x6E7B9B, 0x6E7BBC, 0x6E7C51, 0x6E7C73, 0x6E7E11,
	0x6E7E18, 0x6E7E3A, 0x6E7E41, 0x6E7E5A, 0x6E7E61, 0x6E811C, 0x6E8127, 0x6E8143,
	0x6E814E, 0x6E9E2F, 0x6E9E63, 0x6EA00A, 0x6EA04C, 0x6EA08E, 0x6EFC1F, 0x6EFC5E,
	0x6EFC95, 0x6EFDF9, 0x6EFE31, 0x6E5B36, 0x6E5B75, 0x6E5C15, 0x6E5C45, 0x6E5EFC,
	0x6E5F06, 0x6E64BF, 0x6E64E0, 0x6E6624, 0x6E6649, 0x6E7BDC, 0x6E7BF6, 0x6E7C58,
	0x6E7C8E, 0x6E7DF8, 0x6E7E08, 0x6E7E31, 0x6E801C, 0x6E8029, 0x6E804B, 0x6E8060,
	0x6E807B, 0x6E8088, 0x6E809A, 0x6E80BF, 0x6E80D8, 0x6E80FD, 0x6E9E8D, 0x6E9EC1,
	0x6E9FFF, 0x6EA03D, 0x6EA07F, 0x6EFC18, 0x6EFC57, 0x6EFC8E, 0x6EFDCB, 0x6EFE2A,
	0x6E5863, 0x6E58B7, 0x6E5945, 0x6E598F, 0x6E5B7B, 0x6E5CA9, 0x6E5CB3, 0x6E5CBB,
	0x6E5CC7, 0x6E5CD3, 0x6E5D83, 0x6E5D8E, 0x6E5D96, 0x6E5DA3, 0x6E5DAE, 0x6E5FC4,
	0x6E5FD0, 0x6E5FDE, 0x6E5FE4, 0x6E5FF0, 0x6E60AE, 0x6E60BA, 0x6E60C4, 0x6E60CA,
	0x6E60D4, 0x6E9FC7, 0x6EFBF6, 0x6EFC35, 0x6EFC6C, 0x6EFD90, 0x6EFDC2, 0x6EFDF0,
	0x6EFE1E, 0x6E5CEF, 0x6E5CFD, 0x6E5D06, 0x6E5D0D, 0x6E5D1A, 0x6E5DCB, 0x6E5DD9,
	0x6E5DE2, 0x6E5DE9, 0x6E5DF4, 0x6E600C, 0x6E601A, 0x6E6025, 0x6E602C, 0x6E6039,
	0x6E60F0, 0x6E60FE, 0x6E6107, 0x6E610E, 0x6E611B, 0x6E9F09, 0x6E9F41, 0x6E9F71,
	0x6E9FA1, 0x6E5D30, 0x6E5D3E, 0x6E5D47, 0x6E5D4E, 0x6E5D5B, 0x6E5E0B, 0x6E5E19,
	0x6E5E22, 0x6E5E29, 0x6E5E34, 0x6E604F, 0x6E605D, 0x6E6068, 0x6E606F, 0x6E607C,
	0x6E6131, 0x6E613F, 0x6E6148, 0x6E614F, 0x6E615C, 0x6E9F20, 0x6E9F54, 0x6E9F84,
	0x6E9FB4, 0x6EA0EE, 0x6EA149, 0x6EA193, 0x6EA1CB, 0x6EA0F5, 0x6EA150, 0x6EA19A,
	0x6EA1D2, 0x000000
};

DWORD CWaterManagerSA::m_QuadXrefs[] = {
    0x6E5859, 0x6E5985, 0x6E7B88, 0x6E816A, 0x6E8635, 0x6E8709, 0x6E9E1D, 0x6E8171,
	0x6E8178, 0x6EFCCF, 0x6E817F, 0x6E5849, 0x6E588A, 0x6E5979, 0x6E59B0, 0x6E8185,
	0x6E8199, 0x6E81AD, 0x6E81B5, 0x6E81CB, 0x000000
};

DWORD CWaterManagerSA::m_TriangleXrefs[] = {
    0x6E58AD, 0x6E593B, 0x6E7C44, 0x6E7E7F, 0x6E8673, 0x6E86E4, 0x6EFC27, 0x6E7C64,
	0x6E7E8B, 0x6EFBEA, 0x6E7C7F, 0x6E7E93, 0x6EFBE2, 0x6E589D, 0x6E58DE, 0x6E592F,
	0x6E5966, 0x6E7E9A, 0x6E7EA8, 0x6E7EBD, 0x6E7EC6, 0x6E7EDA, 0x6EFBD3, 0x6EFCA3,
	0x000000
};

DWORD CWaterManagerSA::m_ZonePolyXrefs[] = {
    0x6E57B2, 0x6E57AA, 0x6E57C8, 0x6E58F2, 0x6E86A1, 0x6E6387, 0x6E8699, 0x6E57DE,
	0x6E57E8, 0x000000
};

CWaterManagerSA* g_pWaterManager = NULL;

// -----------------------------------------------------
// Vertices

void CWaterVertexSA::GetPosition ( CVector& vec )
{
    vec.fX = (float)m_pInterface->m_sX;
    vec.fY = (float)m_pInterface->m_sY;
    vec.fZ = m_pInterface->m_fZ;
}

void CWaterVertexSA::SetPosition ( CVector& vec, void* pChangeSource )
{
    if ( pChangeSource )
        g_pWaterManager->AddChange ( pChangeSource, this, new CWaterChangeVertexMove ( this ) );
    m_pInterface->m_sX = (short)vec.fX;
    m_pInterface->m_sY = (short)vec.fY;
    m_pInterface->m_fZ = vec.fZ;
}

// -----------------------------------------------------
// Polygons

void CWaterQuadSA::SetInterface ( CWaterPolySAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_wID = (WORD) ( ((DWORD)pInterface - (DWORD)g_pWaterManager->m_QuadPool) / sizeof ( CWaterQuadSAInterface ) );

    CWaterZoneSA* pZone = g_pWaterManager->GetZoneContaining ( this );
    assert ( pZone );
    CWaterZoneSA::iterator it;
    for ( it = pZone->begin (); *it; ++it )
    {
        if ( (*it)->GetID () == m_wID )
        {
            m_pEntryInterface = (CWaterPolyEntrySAInterface *)it;
            return;
        }
    }
    assert ( false );
}

void CWaterTriangleSA::SetInterface ( CWaterPolySAInterface* pInterface )
{
    m_pInterface = pInterface;
    m_wID = (WORD) ( ((DWORD)pInterface - (DWORD)g_pWaterManager->m_TrianglePool) / sizeof ( CWaterTriangleSAInterface ) );

    CWaterZoneSA* pZone = g_pWaterManager->GetZoneContaining ( this );
    assert ( pZone );
    CWaterZoneSA::iterator it;
    for ( it = pZone->begin (); *it; ++it )
    {
        if ( (*it)->GetID () == m_wID )
        {
            m_pEntryInterface = (CWaterPolyEntrySAInterface *)it;
            return;
        }
    }
    assert ( false );
}

CWaterVertex* CWaterPolySA::GetVertex ( int index )
{
    if ( index < 0 || index >= GetNumVertices () )
        return NULL;

    return &g_pWaterManager->m_Vertices [
        GetType () == WATER_POLY_QUAD ? ((CWaterQuadSA *)this)->GetInterface ()->m_wVertexIDs[index]
                                  : ((CWaterTriangleSA *)this)->GetInterface ()->m_wVertexIDs[index]
    ];
}

bool CWaterPolySA::ContainsPoint ( float fX, float fY )
{
    bool bInside = false;
    int numVertices = GetNumVertices ();
    WORD* pwVertexIDs =
        GetType () == WATER_POLY_QUAD ? ((CWaterQuadSA *)this)->GetInterface ()->m_wVertexIDs
                                  : ((CWaterTriangleSA *)this)->GetInterface ()->m_wVertexIDs;
    
    CWaterVertexSA* pFrom;
    CWaterVertexSA* pTo;
    CVector vecFrom;
    CVector vecTo;
    int next[4];
    if ( GetType () == WATER_POLY_QUAD )
    {
        next[0] = 1;
        next[1] = 3;
        next[2] = 0;
        next[3] = 2;
    }
    else
    {
        next[0] = 1;
        next[1] = 2;
        next[2] = 0;
    }
    for ( int i = 0; i < numVertices; i++ )
    {
        pFrom = &g_pWaterManager->m_Vertices [ pwVertexIDs[i] ];
        pTo   = &g_pWaterManager->m_Vertices [ pwVertexIDs[next[i]] ];
        pFrom->GetPosition ( vecFrom );
        pTo->GetPosition ( vecTo );
        
        if ( (vecFrom.fY > fY) != (vecTo.fY > fY) &&
             fX < vecFrom.fX + (vecTo.fX - vecFrom.fX) * (fY - vecFrom.fX) / (vecTo.fY - vecFrom.fY) )
           bInside = !bInside;
    }
    return bInside;
}

// -----------------------------------------------------
// Water zone iterator (iterates over polys in a zone)

CWaterZoneSA::iterator::iterator ()
{
    m_pCurrent = NULL;
    m_pFirst = NULL;
    m_bSinglePoly = false;
}

CWaterZoneSA::iterator::iterator ( CWaterZoneSA* pZone )
{
    if ( POLYENTRY_TYPE ( pZone->GetInterface () ) == WATER_POLY_LIST )
    {
        m_pCurrent = &g_pWaterManager->m_ZonePolyPool [ POLYENTRY_ID ( pZone->GetInterface () ) ];
        m_bSinglePoly = false;
    }
    else
    {
        m_pCurrent = pZone->GetInterface ();
        m_bSinglePoly = true;
    }
    m_pFirst = m_pCurrent;
}

CWaterZoneSA::iterator::iterator ( CWaterZoneSA::iterator& other )
{
    operator= ( other );
}

CWaterZoneSA::iterator& CWaterZoneSA::iterator::operator= ( CWaterZoneSA::iterator& other )
{
    m_pCurrent = other.m_pCurrent;
    m_pFirst = other.m_pFirst;
    m_bSinglePoly = other.m_bSinglePoly;
    return *this;
}

void CWaterZoneSA::iterator::operator++ ()
{
    m_pCurrent++;
}

void CWaterZoneSA::iterator::operator-- ()
{
    m_pCurrent--;
}

CWaterZoneSA::iterator CWaterZoneSA::iterator::operator+ ( int n )
{
    iterator it ( *this );
    it.m_pCurrent += n;
    return it;
}

CWaterZoneSA::iterator CWaterZoneSA::iterator::operator- ( int n )
{
    iterator it ( *this );
    it.m_pCurrent -= n;
    return it;
}

int CWaterZoneSA::iterator::operator- ( CWaterZoneSA::iterator& other )
{
    return other.m_pCurrent - m_pCurrent;
}

bool CWaterZoneSA::iterator::operator== ( CWaterZoneSA::iterator& other )
{
    return m_pCurrent == other.m_pCurrent;
}

bool CWaterZoneSA::iterator::operator!= ( CWaterZoneSA::iterator& other )
{
    return m_pCurrent != other.m_pCurrent;
}

CWaterPolySA* CWaterZoneSA::iterator::operator* ()
{
    if ( (m_bSinglePoly && m_pCurrent != m_pFirst) || m_pCurrent->m_wValue == 0 )
        return NULL;

    if ( POLYENTRY_TYPE ( m_pCurrent ) == WATER_POLY_QUAD )
    {
        return &g_pWaterManager->m_Quads [ POLYENTRY_ID ( m_pCurrent ) ];
    }
    else if ( POLYENTRY_TYPE ( m_pCurrent ) == WATER_POLY_TRIANGLE )
    {
        return &g_pWaterManager->m_Triangles [ POLYENTRY_ID ( m_pCurrent ) ];
    }
    return NULL;
}

CWaterZoneSA::iterator::operator CWaterPolyEntrySAInterface* ()
{
    if ( (m_bSinglePoly && m_pCurrent != m_pFirst) || m_pCurrent->m_wValue == 0 )
        return NULL;

    return m_pCurrent;
}

CWaterZoneSA::iterator CWaterZoneSA::begin ()
{
    return iterator ( this );
}

CWaterZoneSA::iterator CWaterZoneSA::end ()
{
    iterator it ( this );
    while ( *it )
        ++it;
    return it;
}

// -----------------------------------------------------
// Water zones

CWaterPolyEntrySAInterface* CWaterZoneSA::AddPoly ( CWaterPoly* pPoly )
{
    return AddPoly ( pPoly->GetType (), pPoly->GetID () );
}

CWaterPolyEntrySAInterface* CWaterZoneSA::AddPoly ( EWaterPolyType type, WORD wID )
{
    if ( m_pInterface->m_wValue == 0 )
    {
        m_pInterface->m_wValue = MAKE_POLYENTRY ( type, wID );
        return m_pInterface;
    }
    else if ( POLYENTRY_TYPE ( m_pInterface ) != WATER_POLY_LIST )
    {
        if ( *(DWORD *)VAR_NumWaterZonePolys + 3 > NUM_NewWaterZonePolys )
            return NULL;
        
        WORD wOffset = *(WORD *)VAR_NumWaterZonePolys;
        g_pWaterManager->m_ZonePolyPool [ wOffset ].m_wValue = m_pInterface->m_wValue;
        g_pWaterManager->m_ZonePolyPool [ wOffset + 1 ].m_wValue = MAKE_POLYENTRY ( type, wID );
        g_pWaterManager->m_ZonePolyPool [ wOffset + 2 ].m_wValue = 0;
        m_pInterface->m_wValue = MAKE_POLYENTRY ( WATER_POLY_LIST, wOffset );

        CWaterPolyEntrySAInterface* pZoneInterface = m_pInterface + 1;
        for ( ; pZoneInterface < &((CWaterPolyEntrySAInterface *)ARRAY_WaterZones) [ NUM_WaterZones ]; pZoneInterface++ )
            pZoneInterface->m_wValue += 3;

        *(DWORD *)VAR_NumWaterZonePolys += 3;
        return &g_pWaterManager->m_ZonePolyPool [ wOffset + 1 ];
    }
    else
    {
        if ( *(DWORD *)VAR_NumWaterZonePolys + 1 > NUM_NewWaterZonePolys )
            return NULL;

        iterator it = end ();
        CWaterPolyEntrySAInterface* pZoneEnd = (CWaterPolyEntrySAInterface *)it;
        CWaterPolyEntrySAInterface* pEntry = &g_pWaterManager->m_ZonePolyPool [ *(DWORD *)VAR_NumWaterZonePolys ];
        while ( pEntry > pZoneEnd )
        {
            pEntry->m_wValue = (pEntry - 1)->m_wValue;
            pEntry--;
        }
        pZoneEnd->m_wValue = MAKE_POLYENTRY ( type, wID );

        CWaterPolyEntrySAInterface* pZoneInterface = m_pInterface + 1;
        for ( ; pZoneInterface != &((CWaterPolyEntrySAInterface *)ARRAY_WaterZones) [ NUM_WaterZones ]; pZoneInterface++ )
            pZoneInterface->m_wValue++;

        (*(DWORD *)VAR_NumWaterZonePolys)++;
        return pZoneEnd;
    }
}

bool CWaterZoneSA::RemovePoly ( CWaterPoly* pPoly )
{
    return RemovePoly ( pPoly->GetType (), pPoly->GetID () );
}

bool CWaterZoneSA::RemovePoly ( EWaterPolyType type, WORD wID )
{
    if ( m_pInterface->m_wValue == 0 )
    {
        return false;
    }
    else if ( POLYENTRY_TYPE ( m_pInterface ) != WATER_POLY_LIST )
    {
        if ( POLYENTRY_ID ( m_pInterface ) == wID )
        {
            m_pInterface->m_wValue = 0;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        CWaterPolyEntrySAInterface* pEntries = (CWaterPolyEntrySAInterface *)begin ();
        CWaterPolyEntrySAInterface* pEnd = &g_pWaterManager->m_ZonePolyPool [ *(DWORD *)VAR_NumWaterZonePolys ];
        if ( end () - begin () == 2 )
        {
            if ( pEntries [ 0 ].m_wValue == MAKE_POLYENTRY ( type, wID ) ||
                 pEntries [ 1 ].m_wValue == MAKE_POLYENTRY ( type, wID ) )
            {
                if ( pEntries [ 0 ].m_wValue == MAKE_POLYENTRY ( type, wID ) )
                    m_pInterface->m_wValue = pEntries [ 1 ].m_wValue;
                else
                    m_pInterface->m_wValue = pEntries [ 0 ].m_wValue;

                CWaterPolyEntrySAInterface* pEntry = pEntries + 3;
                for ( ; pEntry < pEnd; pEntry++ )
                    (pEntry - 3)->m_wValue = pEntry->m_wValue;

                CWaterPolyEntrySAInterface* pZoneInterface = m_pInterface + 1;
                for ( ; pZoneInterface < &((CWaterPolyEntrySAInterface *)ARRAY_WaterZones) [ NUM_WaterZones ]; pZoneInterface++ )
                    pZoneInterface->m_wValue -= 3;
                *(DWORD *)VAR_NumWaterZonePolys -= 3;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            for ( iterator it = begin (); *it; ++it )
            {
                if ( (*it)->GetType () == type && (*it)->GetID () == wID )
                {
                    CWaterPolyEntrySAInterface* pEntry = (CWaterPolyEntrySAInterface *)it + 1;
                    for ( ; pEntry < pEnd; pEntry++ )
                        (pEntry - 1)->m_wValue = pEntry->m_wValue;

                    CWaterPolyEntrySAInterface* pZoneInterface = m_pInterface + 1;
                    for ( ; pZoneInterface < &((CWaterPolyEntrySAInterface *)ARRAY_WaterZones) [ NUM_WaterZones ]; pZoneInterface++ )
                        pZoneInterface->m_wValue--;
                    (*(DWORD *)VAR_NumWaterZonePolys)--;
                    return true;
                }
            }
            return false;
        }
    }
}

// -----------------------------------------------------
// Change trackkeepers

CWaterChangeVertexMove::CWaterChangeVertexMove ( CWaterVertex* pVertex )
{
    pVertex->GetPosition ( m_vecOriginalPosition );
}

void CWaterChangeVertexMove::Undo ( void* pChangedObject )
{
    ((CWaterVertexSA *)pChangedObject)->SetPosition ( m_vecOriginalPosition );
}

// -----------------------------------------------------
// Manager

CWaterManagerSA::CWaterManagerSA ()
    : m_Vertices  ( NUM_DefWaterVertices ),
      m_Quads     ( NUM_DefWaterQuads ),
      m_Triangles ( NUM_DefWaterTriangles ),
      m_Zones     ( NUM_WaterZones )
{
    g_pWaterManager = this;
    RelocatePools ();
}

CWaterManagerSA::~CWaterManagerSA ()
{
    UndoChanges ();
    g_pWaterManager = NULL;
}

void CWaterManagerSA::RelocatePools ()
{
    DWORD* pXrefGroups[] = { m_VertexXrefs, m_QuadXrefs, m_TriangleXrefs, m_ZonePolyXrefs, 0 };
    void* pNewPools[] = { m_VertexPool, m_QuadPool, m_TrianglePool, m_ZonePolyPool, 0 };

    DWORD** pXrefGroup = NULL;
    void* pNewPool = NULL;
    for ( int i = 0; pXrefGroup = (DWORD **)pXrefGroups [ i ]; i++ )
    {
        pNewPool = pNewPools [ i ];
        DWORD dwDelta = (DWORD)pNewPool - *pXrefGroup [ 0 ];
        for ( DWORD** ppXref = pXrefGroup; *ppXref; ppXref++ )
        {
            **ppXref += dwDelta;
        }
    }
}

CWaterPoly* CWaterManagerSA::GetPolyAtPoint ( CVector& vecPosition )
{
    if ( vecPosition.fX < -3000.0f || vecPosition.fX > 3000.0f || vecPosition.fY < -3000.0f || vecPosition.fY > 3000.0f )
        return NULL;

    CWaterZoneSA* pZone = GetZoneContaining ( vecPosition.fX, vecPosition.fY );
    if ( !pZone )
        return NULL;

    CWaterZoneSA::iterator it;
    for ( it = pZone->begin (); *it; ++it )
    {
        if ( (*it)->ContainsPoint ( vecPosition.fX, vecPosition.fY ) )
        {
            return *it;
        }
    }
    return NULL;
}

CWaterPoly* CWaterManagerSA::CreateQuad ( CVector& vecTL, CVector& vecTR, CVector& vecBR, CVector& vecBL, bool bShallow )
{
    if ( vecTL.fX >= vecTR.fX || vecBL.fX >= vecBR.fX ||
         vecTL.fY <= vecBL.fY || vecTR.fY <= vecBR.fY ||
         vecTL.fX < -3000.0f || vecTL.fX > 3000.0f || vecTL.fY < -3000.0f || vecTL.fY > 3000.0f ||
         vecTR.fX < -3000.0f || vecTR.fX > 3000.0f || vecTR.fY < -3000.0f || vecTR.fY > 3000.0f ||
         vecBL.fX < -3000.0f || vecBL.fX > 3000.0f || vecBL.fY < -3000.0f || vecBL.fY > 3000.0f ||
         vecBR.fX < -3000.0f || vecBR.fX > 3000.0f || vecBR.fY < -3000.0f || vecBR.fY > 3000.0f )
        return NULL;

    if ( *(DWORD *)VAR_NumWaterVertices + 4 > NUM_NewWaterVertices ||
         *(DWORD *)VAR_NumWaterQuads + 1 > NUM_NewWaterQuads ||
         *(DWORD *)VAR_NumWaterZonePolys + 2 > NUM_NewWaterZonePolys )
        return NULL;

    CWaterZoneSA* pZone = g_pWaterManager->GetZoneContaining ( vecBL, vecBR, vecTL );
    if ( !pZone )
        return NULL;

    WORD wV1 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vecBL.fX, (short)vecBL.fY, vecBL.fZ, 0.0f, 0.0f, 0 );
    WORD wV2 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vecBR.fX, (short)vecBR.fY, vecBR.fZ, 0.0f, 0.0f, 0 );
    WORD wV3 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vecTL.fX, (short)vecTL.fY, vecTL.fZ, 0.0f, 0.0f, 0 );
    WORD wV4 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vecTR.fX, (short)vecTR.fY, vecTR.fZ, 0.0f, 0.0f, 0 );

    CWaterQuadSAInterface* pInterface = &g_pWaterManager->m_QuadPool [ *(DWORD *)VAR_NumWaterQuads ];
    pInterface->m_wVertexIDs [ 0 ] = wV1;
    pInterface->m_wVertexIDs [ 1 ] = wV2;
    pInterface->m_wVertexIDs [ 2 ] = wV3;
    pInterface->m_wVertexIDs [ 3 ] = wV4;
    pInterface->m_wFlags = WATER_VISIBLE;
    if ( bShallow )
        pInterface->m_wFlags |= WATER_SHALLOW;

    pZone->AddPoly ( WATER_POLY_QUAD, *(WORD *)VAR_NumWaterQuads );

    (*(DWORD *)VAR_NumWaterQuads)++;
    if ( g_pWaterManager->m_Quads.size () < *(DWORD *)VAR_NumWaterQuads )
        g_pWaterManager->m_Quads.push_back ( CWaterQuadSA () );
    CWaterQuadSA* pPoly = &g_pWaterManager->m_Quads [ *(DWORD *)VAR_NumWaterQuads - 1 ];
    pPoly->SetInterface ( pInterface );
    return pPoly;
}

CWaterPoly* CWaterManagerSA::CreateTriangle ( CVector& vec1, CVector& vec2, CVector& vec3, bool bShallow )
{
    if ( vec1.fX >= vec2.fX || vec1.fY == vec3.fY || vec2.fY == vec3.fY ||
         (vec1.fY < vec3.fY) != (vec2.fY < vec3.fY) ||
         vec1.fX < -3000.0f || vec1.fX > 3000.0f || vec1.fY < -3000.0f || vec1.fY > 3000.0f ||
         vec2.fX < -3000.0f || vec2.fX > 3000.0f || vec2.fY < -3000.0f || vec2.fY > 3000.0f ||
         vec3.fX < -3000.0f || vec3.fX > 3000.0f || vec3.fY < -3000.0f || vec3.fY > 3000.0f )
        return NULL;

    if ( *(DWORD *)VAR_NumWaterVertices + 4 > NUM_NewWaterVertices ||
         *(DWORD *)VAR_NumWaterTriangles + 1 > NUM_NewWaterTriangles ||
         *(DWORD *)VAR_NumWaterZonePolys + 2 > NUM_NewWaterZonePolys )
        return NULL;

    CWaterZoneSA* pZone = g_pWaterManager->GetZoneContaining ( vec1, vec2, vec3 );
    if ( !pZone )
        return NULL;

    WORD wV1 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vec1.fX, (short)vec1.fY, vec1.fZ, 0.0f, 0.0f, 0 );
    WORD wV2 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vec2.fX, (short)vec2.fY, vec2.fZ, 0.0f, 0.0f, 0 );
    WORD wV3 = ( (CreateWaterVertex_t) FUNC_CreateWaterVertex )( (short)vec3.fX, (short)vec3.fY, vec3.fZ, 0.0f, 0.0f, 0 );

    CWaterTriangleSAInterface* pInterface = &g_pWaterManager->m_TrianglePool [ *(DWORD *)VAR_NumWaterTriangles ];
    pInterface->m_wVertexIDs [ 0 ] = wV1;
    pInterface->m_wVertexIDs [ 1 ] = wV2;
    pInterface->m_wVertexIDs [ 2 ] = wV3;
    pInterface->m_wFlags = WATER_VISIBLE;
    if ( bShallow )
        pInterface->m_wFlags |= WATER_SHALLOW;

    pZone->AddPoly ( WATER_POLY_TRIANGLE, *(WORD *)VAR_NumWaterTriangles );

    (*(DWORD *)VAR_NumWaterTriangles)++;
    if ( g_pWaterManager->m_Triangles.size () < *(DWORD *)VAR_NumWaterTriangles )
        g_pWaterManager->m_Triangles.push_back ( CWaterTriangleSA () );
    CWaterTriangleSA* pPoly = &g_pWaterManager->m_Triangles [ *(DWORD *)VAR_NumWaterTriangles - 1 ];
    pPoly->SetInterface ( pInterface );
    return pPoly;
}

bool CWaterManagerSA::DeletePoly ( CWaterPoly* pPoly )
{
    CWaterZoneSA* pZone = GetZoneContaining ( pPoly );
    if ( !pZone || !pZone->RemovePoly ( pPoly ) )
        return false;

    CWaterPolySAInterface* pInterface = ((CWaterPolySA *)pPoly)->GetInterface ();
    if ( pPoly->GetType () == WATER_POLY_QUAD )
    {
        memcpy ( pInterface, pInterface + 1, (DWORD)&m_QuadPool [ *(DWORD *)VAR_NumWaterQuads ] - (DWORD)(pInterface + 1) );
        (*(DWORD *)VAR_NumWaterQuads)--;
    }
    else
    {
        memcpy ( pInterface, pInterface + 1, (DWORD)&m_TrianglePool [ *(DWORD *)VAR_NumWaterQuads ] - (DWORD)(pInterface + 1) );
        (*(DWORD *)VAR_NumWaterTriangles)--;
    }
    return true;
}

CWaterZoneSA* CWaterManagerSA::GetZoneContaining ( float fX, float fY )
{
    if ( fX < -3000.0f || fX > 3000.0f || fY < -3000.0f || fY > 3000.0f )
        return NULL;

    if ( fX == 3000.0f )
        fX = 2999.0f;
    if ( fY == 3000.0f )
        fY = 2999.0f;

    int zoneID = 12*((int)(fX + 3000.0f) / 500) + (int)(fY + 3000.0f) / 500;
    return &m_Zones [ zoneID ];
}

CWaterZoneSA* CWaterManagerSA::GetZoneContaining ( CWaterPoly* pPoly )
{
    CVector v1;
    CVector v2;
    CVector v3;

    pPoly->GetVertex ( 0 )->GetPosition ( v1 );
    pPoly->GetVertex ( 1 )->GetPosition ( v2 );
    pPoly->GetVertex ( 2 )->GetPosition ( v3 );

    return GetZoneContaining ( v1, v2, v3 );
}

CWaterZoneSA* CWaterManagerSA::GetZoneContaining ( CVector& v1, CVector& v2, CVector& v3 )
{
    float fColumnLeft = -3000.0f;
    for ( int column = 0; column < 12; column++ )
    {
        float fRowBottom = -3000.0f;
        for ( int row = 0; row < 12; row++ )
        {
            if ( v2.fX >= fColumnLeft && v1.fX < fColumnLeft + 500.0f && std::max<float>(v1.fY, v3.fY) >= fRowBottom && std::min<float>(v1.fY, v3.fY) < fRowBottom + 500.0f )
                return &m_Zones [ column*12 + row ];
            fRowBottom += 500.0f;
        }
        fColumnLeft += 500.0f;
    }
    return NULL;
}

bool CWaterManagerSA::GetWaterLevel ( CVector& vecPosition, float* pfLevel, bool bCheckWaves, CVector* pvecUnknown )
{
    return ( (GetWaterLevel_t) FUNC_GetWaterLevel )
        ( vecPosition.fX, vecPosition.fY, vecPosition.fZ, pfLevel, bCheckWaves, pvecUnknown );
}

bool CWaterManagerSA::SetWaterLevel ( CVector& vecPosition, float fLevel, void* pChangeSource )
{
    CWaterPoly* pPoly = GetPolyAtPoint ( vecPosition );
    if ( !pPoly )
        return false;

    CVector vecVertexPos;
    for ( int i = 0; i < pPoly->GetNumVertices (); i++ )
    {
        pPoly->GetVertex ( i )->GetPosition ( vecVertexPos );
        vecVertexPos.fZ = fLevel;
        pPoly->GetVertex ( i )->SetPosition ( vecVertexPos, pChangeSource );
    }
    return true;
}

bool CWaterManagerSA::TestLineAgainstWater ( CVector& vecStart, CVector& vecEnd, CVector* vecCollision )
{
    return ( (TestLineAgainstWater_t) FUNC_TestLineAgainstWater )
        ( vecEnd.fX, vecEnd.fY, vecEnd.fZ, vecStart.fX, vecStart.fY, vecStart.fZ, vecCollision );
}

void CWaterManagerSA::AddChange ( void *pChangeSource, void* pChangedObject, CWaterChange* pChange )
{
    if ( !pChangeSource )
        return;

    std::map < void*, std::map < void*, CWaterChange* > >::iterator sourceIt = m_Changes.find ( pChangeSource );
    if ( sourceIt == m_Changes.end () )
        sourceIt = m_Changes.insert ( make_pair ( pChangeSource, std::map < void*, CWaterChange* > () ) ).first;
    
    std::map < void*, CWaterChange* >::iterator changeIt = sourceIt->second.find ( pChangedObject );
    if ( changeIt == sourceIt->second.end () )
    {
        sourceIt->second.insert ( make_pair ( pChangedObject, pChange ) );
    }
    else
    {
        delete changeIt->second;
        changeIt->second = pChange;
    }
}

void CWaterManagerSA::UndoChanges ( void* pChangeSource )
{
    if ( pChangeSource == NULL )
    {
        std::map < void*, std::map < void*, CWaterChange* > >::iterator sourceIt = m_Changes.begin ();
        for ( ; sourceIt != m_Changes.end (); sourceIt++ )
        {
            if ( sourceIt->first )
                UndoChanges ( sourceIt->first );
        }
    }
    else
    {
        std::map < void*, std::map < void*, CWaterChange* > >::iterator sourceIt = m_Changes.find ( pChangeSource );
        if ( sourceIt == m_Changes.end () )
            return;
        std::map < void*, CWaterChange* >::iterator changeIt = sourceIt->second.begin ();
        for ( ; changeIt != sourceIt->second.end (); changeIt++ )
        {
            changeIt->second->Undo ( changeIt->first );
            delete changeIt->second;
        }
        m_Changes.erase ( sourceIt );
    }
}

void CWaterManagerSA::Reset ()
{
    memset ( m_QuadPool, 0, sizeof ( m_QuadPool ) );
    memset ( m_TrianglePool, 0, sizeof ( m_TrianglePool ) );

    ( (ReadWaterConfiguration_t) FUNC_ReadWaterConfiguration )();

    m_Zones.resize ( NUM_WaterZones );
    for ( DWORD i = 0; i < NUM_WaterZones; i++ )
        m_Zones [ i ].SetInterface ( &((CWaterPolyEntrySAInterface *)ARRAY_WaterZones) [ i ] );

    m_Vertices.resize ( NUM_DefWaterVertices );
    for ( i = 0; i < NUM_DefWaterVertices; i++ )
        m_Vertices [ i ].SetInterface ( &m_VertexPool [ i ] );

    m_Quads.resize ( NUM_DefWaterQuads );
    for ( i = 0; i < NUM_DefWaterQuads; i++ )
        m_Quads [ i ].SetInterface ( &m_QuadPool [ i ] );

    m_Triangles.resize ( NUM_DefWaterTriangles );
    for ( i = 0; i < NUM_DefWaterTriangles; i++ )
        m_Triangles [ i ].SetInterface ( &m_TrianglePool [ i ] );
}

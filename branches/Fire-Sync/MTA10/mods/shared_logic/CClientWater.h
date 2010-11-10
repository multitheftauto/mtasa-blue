/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWater.h
*  PURPOSE:     Water polygon class
*  DEVELOPERS:  arc_
*
*****************************************************************************/

#ifndef __CCLIENTWATER_H
#define __CCLIENTWATER_H

class CClientWater : public CClientEntity
{
public:
                               CClientWater           ( CClientManager* pManager, ElementID ID, CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow = false );
                               CClientWater           ( CClientManager* pManager, ElementID ID, CVector& vecL, CVector& vecR, CVector& vecTB, bool bShallow = false );
                               ~CClientWater          ();

    bool                       Valid                  () { return m_pPoly != NULL; }

    eClientEntityType          GetType                () const { return CCLIENTWATER; }
    int                        GetNumVertices         () const;
    void                       GetPosition            ( CVector& vecPosition ) const;
    bool                       GetVertexPosition      ( int iVertexIndex, CVector& vecPosition );
    void                       SetPosition            ( const CVector& vecPosition );
    bool                       SetVertexPosition      ( int iVertexIndex, CVector& vecPosition, void* pChangeSource = NULL );
    void                       Unlink                 ();

private:
    CWaterPoly*                m_pPoly;
    CClientWaterManager*       m_pWaterManager;

    friend CClientWaterManager;
};

#endif
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
    DECLARE_CLASS( CClientWater, CClientEntity )
public:
                               CClientWater           ( CClientManager* pManager, ElementID ID, CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow = false );
                               CClientWater           ( CClientManager* pManager, ElementID ID, CVector& vecL, CVector& vecR, CVector& vecTB, bool bShallow = false );
                               ~CClientWater          ();

    bool                       Create                 ( void );
    bool                       Destroy                ( void );
    bool                       Exists                 () { return m_pPoly != NULL; }

    eClientEntityType          GetType                () const { return CCLIENTWATER; }
    int                        GetNumVertices         () const;
    void                       GetPosition            ( CVector& vecPosition ) const;
    bool                       GetVertexPosition      ( int iVertexIndex, CVector& vecPosition );
    void                       SetPosition            ( const CVector& vecPosition );
    bool                       SetVertexPosition      ( int iVertexIndex, CVector& vecPosition, void* pChangeSource = NULL );
    void                       Unlink                 ();

    void                       SetDimension           (unsigned short usDimension);
    void                       RelateDimension        (unsigned short usWorldDimension);

private:
    CWaterPoly*                m_pPoly;
    CClientWaterManager*       m_pWaterManager;
    bool                       m_bTriangle;    // Is this water a triangle or a quad type?
    bool                       m_bShallow; // Shallow water?
    CVector                    m_vec1;
    CVector                    m_vec2;
    CVector                    m_vec3;
    CVector                    m_vec4;
    std::vector<CVector>       m_Verticies; // List of verticies for this water
    unsigned short             m_usDimension;

    friend class CClientWaterManager;
};

#endif
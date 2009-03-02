/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColTube.h
*  PURPOSE:     Tube-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTCOLTUBE_H
#define __CCLIENTCOLTUBE_H

class CClientColTube : public CClientColShape
{
public:
                            CClientColTube          ( CClientManager* pManager, ElementID ID );
                            CClientColTube          ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius, float fHeight );

    eColShapeType           GetShapeType            ( void )            { return COLSHAPE_TUBE
        ; }
    bool                    DoHitDetection          ( const CVector& vecNowPosition, float fRadius );

    inline float            GetRadius               ( void )            { return m_fRadius; };
    inline void             SetRadius               ( float fRadius )   { m_fRadius = fRadius; };
    inline float            GetHeight               ( void )            { return m_fHeight; };
    inline void             SetHeight               ( float fHeight )   { m_fHeight = fHeight; };

protected:
    float                   m_fRadius;
    float                   m_fHeight;
};

#endif

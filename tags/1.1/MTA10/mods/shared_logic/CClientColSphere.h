/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColSphere.h
*  PURPOSE:     Sphere-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTCOLSPHERE_H
#define __CCLIENTCOLSPHERE_H

class CClientColSphere : public CClientColShape
{
    DECLARE_CLASS( CClientColSphere, CClientColShape )
public:
                    CClientColSphere        ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius );

    virtual CSphere GetWorldBoundingSphere  ( void );

    eColShapeType   GetShapeType            ( void )            { return COLSHAPE_SPHERE; }

    bool            DoHitDetection          ( const CVector& vecNowPosition, float fRadius );

    float           GetRadius               ( void )            { return m_fRadius; };
    void            SetRadius               ( float fRadius )   { m_fRadius = fRadius; SizeChanged (); }

protected:
    float           m_fRadius;
};

#endif

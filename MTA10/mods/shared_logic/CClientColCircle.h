/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCircle.h
*  PURPOSE:     Circle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTCOLCIRCLE_H
#define __CCLIENTCOLCIRCLE_H

class CClientColCircle : public CClientColShape
{
public:
                    CClientColCircle        ( CClientManager* pManager, ElementID ID );
                    CClientColCircle        ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, float fRadius );

    eColShapeType   GetShapeType            ( void )            { return COLSHAPE_CIRCLE; }

    bool            DoHitDetection          ( const CVector& vecNowPosition, float fRadius );

    float           GetRadius               ( void )            { return m_fRadius; };
    void            SetRadius               ( float fRadius )   { m_fRadius = fRadius; };

protected:
    float           m_fRadius;
};

#endif

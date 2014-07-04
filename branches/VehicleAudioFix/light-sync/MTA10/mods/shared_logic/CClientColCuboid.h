/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCircle.h
*  PURPOSE:     Cuboid-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTCOLCUBOID_H
#define __CCLIENTCOLCUBOID_H

class CClientColCuboid : public CClientColShape
{
    DECLARE_CLASS( CClientColCuboid, CClientColShape )
public:
                            CClientColCuboid        ( CClientManager* pManager, ElementID ID, const CVector& vecPosition, const CVector& vecSize );

    virtual CSphere         GetWorldBoundingSphere  ( void );

    eColShapeType           GetShapeType            ( void )                    { return COLSHAPE_CUBOID; }

    bool                    DoHitDetection          ( const CVector& vecNowPosition, float fRadius );

    inline const CVector&   GetSize                 ( void )                    { return m_vecSize; };
    inline void             SetSize                 ( const CVector& vecSize )  { m_vecSize = vecSize; SizeChanged (); }

protected:
    CVector                 m_vecSize;
};

#endif

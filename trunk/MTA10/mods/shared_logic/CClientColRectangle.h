/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColRectangle.h
*  PURPOSE:     Rectangle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTCOLRECTANGLE_H
#define __CCLIENTCOLRECTANGLE_H

class CClientColRectangle : public CClientColShape
{
    DECLARE_CLASS( CClientColRectangle, CClientColShape )
public:
                            CClientColRectangle     ( CClientManager* pManager, ElementID ID, const CVector2D& vecPosition, const CVector2D& vecSize );

    virtual CSphere         GetWorldBoundingSphere  ( void );
    virtual void            DebugRender             ( const CVector& vecPosition, float fDrawRadius );

    eColShapeType           GetShapeType            ( void )            { return COLSHAPE_RECTANGLE; }

    bool                    DoHitDetection          ( const CVector& vecNowPosition, float fRadius );

    inline const CVector2D& GetSize                 ( void )                        { return m_vecSize; };
    inline void             SetSize                 ( const CVector2D& vecSize )    { m_vecSize = vecSize; SizeChanged (); };

protected:
    CVector2D               m_vecSize;
};

#endif
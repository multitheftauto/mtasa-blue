/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColCuboid.h
*  PURPOSE:     Cube-shaped collision entity class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOLCUBOID_H
#define __CCOLCUBOID_H

#include "CColShape.h"

class CColCuboid : public CColShape
{
public:
                            CColCuboid      ( CColManager* pManager, CElement* pParent, const CVector& vecPosition, const CVector& vecSize, CXMLNode* pNode = NULL );

    virtual CSphere         GetWorldBoundingSphere  ( void );

    eColShapeType           GetShapeType    ( void )            { return COLSHAPE_CUBOID; }

    bool                    DoHitDetection  ( const CVector& vecLastPosition, const CVector& vecNowPosition, float fRadius );

    inline const CVector&   GetSize         ( void )                    { return m_vecSize; };
    inline void             SetSize         ( const CVector& vecSize )  { m_vecSize = vecSize; SizeChanged (); };

protected:
    bool                    ReadSpecialData ( void );

    CVector                 m_vecSize;
};

#endif

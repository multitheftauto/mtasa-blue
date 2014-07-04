/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CBuildingRemoval.h
*  PURPOSE:     Header file for building removal.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CBUILDINGREMOVAL_H
#define __CBUILDINGREMOVAL_H

class CBuildingRemoval
{
public:
                                CBuildingRemoval                 ( unsigned short usModel, float fRadius, const CVector& vecPos, char cInterior );
                                ~CBuildingRemoval                ( void );

    float                       GetRadius                        ( void )                       { return m_fRadius; }
    const CVector&              GetPosition                      ( void )                       { return m_vecPos; }
    unsigned short              GetModel                         ( void )                       { return m_usModel; }
    char                        GetInterior                      ( void )                       { return m_cInterior; }
private:
    unsigned short m_usModel;
    CVector m_vecPos;
    float m_fRadius;
    char m_cInterior;

};
#endif
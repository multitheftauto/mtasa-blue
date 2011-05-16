/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDummy.h
*  PURPOSE:     Dummy entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDUMMY_H
#define __CDUMMY_H

#include "CElement.h"

class CDummy : public CElement
{
public:
                            CDummy              ( class CGroups* pGroups, CElement* pParent, CXMLNode* pNode = NULL );
                            ~CDummy             ( void );

    bool                    IsEntity            ( void )                    { return true; }

    void                    Unlink              ( void );
    bool                    ReadSpecialData     ( void );

private:
    class CGroups*          m_pGroups;
};

#endif

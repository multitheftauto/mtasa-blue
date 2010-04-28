/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimBlendHierarchySA.cpp
*  PURPOSE:     Header file for animation blend hierarchy class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/* CAnimBlendHierarchy: data tree on animation blending?*/

#ifndef __CAnimBlendHierarchySA_H
#define __CAnimBlendHierarchySA_H

#include <game/CAnimBlendHierarchy.h>
#include "Common.h"

#define FUNC_CAnimBlendHierarchy_GetAnimSequence    0x4ce8f0
#define FUNC_CAnimBlendHierarchy_GetAnimSequences   0x4d1350

class CAnimBlendSequence;

class CAnimBlendHierarchySAInterface
{
public:
    int                                             GetIndex ( void );
    int                                             iHashKey;
    CAnimBlendSequence *                            pSequences;
    unsigned short                                  usNumSequences;
    BYTE                                            pad;
    bool                                            bRunningCompressed;
    int                                             iAnimBlockID;
    float                                           fTotalTime;
    //class CLink<class CAnimBlendHierarchy *> *      pLinkPtr;
};

class CAnimBlendHierarchySA : public CAnimBlendHierarchy
{
public:
                                            CAnimBlendHierarchySA   ( CAnimBlendHierarchySAInterface * pInterface )     { m_pInterface = pInterface; }

    CAnimBlendHierarchySAInterface *        GetInterface            ( void )    { return m_pInterface; }
    int                                     GetAnimBlockID          ( void )    { return m_pInterface->iAnimBlockID; }

protected:
    CAnimBlendHierarchySAInterface *        m_pInterface;
};

#endif

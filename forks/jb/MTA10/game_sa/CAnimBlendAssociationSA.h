/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimBlendAssociationSA.h
*  PURPOSE:     Header file for animation blend association class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/*CAnimBlendAssociation: 1 per running animation?*/

#ifndef __CAnimBlendAssociationSA_H
#define __CAnimBlendAssociationSA_H

#include <game/CAnimBlendAssociation.h>
#include "Common.h"

class CAnimBlendAssocGroupSA;
class CAnimBlendHierarchySAInterface;

class CAnimBlendAssociationSAInterface
{
public:
    void*                               vtbl; // 0
    uint32                              pad1; // 4
    uint32                              pad2; // 8
    uint16                              uiAnimBlendNodeCount; // 12
    short                               sAnimGroup; // 14
    DWORD *                             pAnimBlendNodeArray; // 16 [[ CAnimBlendNode pp?
    CAnimBlendHierarchySAInterface *    pAnimHierarchy;  // 20
    float                               fBlendAmount;                                       // 24
    float                               fBlendDelta;                                        // 28
    float                               fCurrentTime;                                       // 32
    float                               fSpeed;                                             // 36
    float                               fTimeStep;                                          // 40
    short                               sAnimID;                                            // 44
    short                               sFlags;     // or1 = started?, or64 = referenced?   // 46
    // callback stuff
    uint32                              pad3; // kind of a regulator var
    uint32*                             pCallback;
    uint32                              uiCallbackArg; // it's passed to callback
};
C_ASSERT(sizeof(CAnimBlendAssociationSAInterface) == 0x3C);

class CAnimBlendAssociationSA : public CAnimBlendAssociation
{
public:
                                        CAnimBlendAssociationSA ( CAnimBlendAssociationSAInterface * pInterface )   { m_pInterface = pInterface; }

    CAnimBlendAssociationSAInterface *  GetInterface            ( void )                                            { return m_pInterface; }
    AssocGroupId                        GetAnimGroup            ( void )                                            { return ( AssocGroupId ) m_pInterface->sAnimGroup; }
    AnimationId                         GetAnimID               ( void )                                            { return ( AnimationId ) m_pInterface->sAnimID; }
    CAnimBlendHierarchy *               GetAnimHierarchy        ( void );

    inline float                        GetBlendAmount          ( void )            { return m_pInterface->fBlendAmount; }
    inline void                         SetBlendAmount          ( float fAmount )   { m_pInterface->fBlendAmount = fAmount; }

    void                                SetCurrentProgress      ( float fProgress );

protected:
    CAnimBlendAssociationSAInterface *  m_pInterface;
};

#endif

/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAnimBlendAssociationSA.h
*  PURPOSE:		Header file for animation blend association class
*  DEVELOPERS:	Jax <>
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
    BYTE                                pad [ 14 ];
    short                               sAnimGroup;
    DWORD *                             pAnimBlendNodeArray; // CAnimBlendNode pp?
    CAnimBlendHierarchySAInterface *    pAnimHierarchy;
    float                               fBlendAmount;
    float                               fBlendDelta;
    float                               fCurrentTime;
    float                               fSpeed;
    float                               fTimeStep;
    short                               sAnimID;
    short                               sFlags;     // or1 = started?, or64 = referenced?
    DWORD *                             pCallback;
};

class CAnimBlendAssociationSA : public CAnimBlendAssociation
{
public:
                                        CAnimBlendAssociationSA ( CAnimBlendAssociationSAInterface * pInterface )   { m_pInterface = pInterface; }

    CAnimBlendAssociationSAInterface *  GetInterface            ( void )                                            { return m_pInterface; }
    AssocGroupId                        GetAnimGroup            ( void )                                            { return ( AssocGroupId ) m_pInterface->sAnimGroup; }
    AnimationId                         GetAnimID               ( void )                                            { return ( AnimationId ) m_pInterface->sAnimID; }
    CAnimBlendHierarchy *               GetAnimHierarchy        ( void );

protected:
    CAnimBlendAssociationSAInterface *  m_pInterface;
};

#endif
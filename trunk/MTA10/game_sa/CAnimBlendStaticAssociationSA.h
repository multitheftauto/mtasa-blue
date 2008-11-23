/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAnimBlendStaticAssociationSA.h
*  PURPOSE:		Header file for animation blend static association class
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

/* CAnimBlendStaticAssociation: 1 per uncompressed animation?*/

#ifndef __CAnimBlendStaticAssociationSA_H
#define __CAnimBlendStaticAssociationSA_H

#include <game/CAnimBlendStaticAssociation.h>
#include "Common.h"

class CAnimBlendAssocGroupSA;
class CAnimBlendHierarchySAInterface;
class CAnimBlendHierarchy;

class CAnimBlendStaticAssociationSAInterface
{
public:
    BYTE                                pad [ 6 ];
    short                               sAnimID;
    short                               sAnimGroup;
    short                               sFlags;
    BYTE                                pad2 [ 4 ];
    CAnimBlendHierarchySAInterface *    pAnimHeirarchy;
};

class CAnimBlendStaticAssociationSA : public CAnimBlendStaticAssociation
{
public:
                                        CAnimBlendStaticAssociationSA       ( CAnimBlendStaticAssociationSAInterface * pInterface )     { m_pInterface = pInterface; }

    CAnimBlendHierarchy *               GetAnimHierachy                     ( void );

protected:
    CAnimBlendStaticAssociationSAInterface *   m_pInterface;
};

#endif
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

#define FUNC_CAnimBlendAssociation_UpdateBlend          0x4d1490
#define FUNC_CAnimBlendAssociation_SetDeleteCallback    0x4cebc0
#define FUNC_CAnimBlendAssociation_SetFinishCallback    0x4cebe0

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
                                        CAnimBlendAssociationSA ( CAnimBlendAssociationSAInterface * pInterface );

    CAnimBlendAssociationSAInterface *  GetInterface            ( void )                                            { return m_pInterface; }
    AssocGroupId                        GetAnimGroup            ( void )                                            { return ( AssocGroupId ) m_pInterface->sAnimGroup; }
    AnimationId                         GetAnimID               ( void )                                            { return ( AnimationId ) m_pInterface->sAnimID; }
    CAnimBlendHierarchy *               GetAnimHierarchy        ( void );

    static void                         StaticDeleteCallback    ( CAnimBlendAssociationSAInterface * pInterface, void * pCallbackData );
    void                                SetDeleteCallback       ( CALLBACK_CAnimBlendAssoc Callback, void * pData );
    
    static void                         StaticFinishCallback    ( CAnimBlendAssociationSAInterface * pInterface, void * pCallbackData );
    void                                SetFinishCallback       ( CALLBACK_CAnimBlendAssoc Callback, void * pData );

    bool                                UpdateBlend             ( float fUnk );

protected:
    CAnimBlendAssociationSAInterface *  m_pInterface;
    CALLBACK_CAnimBlendAssoc            m_DeleteCallback;
    void *                              m_pDeleteCallbackData;
    CALLBACK_CAnimBlendAssoc            m_FinishCallback;
    void *                              m_pFinishCallbackData;
};

#endif
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

#define FUNC_CAnimBlendAssociation_SetDeleteCallback    0x4cebc0
#define FUNC_CAnimBlendAssociation_SetFinishCallback    0x4cebe0
#define FUNC_CAnimBlendAssociation_UpdateBlend          0x4d1490
#define FUNC_CAnimBlendAssociation_SetFlag              0x453e00
#define FUNC_CAnimBlendAssociation_ClearFlag            0x453e10
#define FUNC_CAnimBlendAssociation_GetBlendAmount       0x453e20
#define FUNC_CAnimBlendAssociation_SetBlendDelta        0x453e30
#define FUNC_CAnimBlendAssociation_SetBlendAmount       0x45b070
#define FUNC_CAnimBlendAssociation_SetSpeed             0x45b080
#define FUNC_CAnimBlendAssociation_GetSpeed             0x45b090
#define FUNC_CAnimBlendAssociation_GetCurrentTime       0x45b0a0
#define FUNC_CAnimBlendAssociation_GetAnimId            0x45b0b0
#define FUNC_CAnimBlendAssociation_GetAnimGrp           0x45b0c0
#define FUNC_CAnimBlendAssociation_ClearCallback        0x45b0d0
#define FUNC_CAnimBlendAssociation_GetTotalTime         0x470050
#define FUNC_CAnimBlendAssociation_GetBlendDelta        0x4b84f0
#define FUNC_CAnimBlendAssociation_IsFlagSet            0x4ce910
#define FUNC_CAnimBlendAssociation_SetCurrentTime       0x4cea80
#define FUNC_CAnimBlendAssociation_SyncAnimation        0x4ceb40
#define FUNC_CAnimBlendAssociation_GetNode              0x4ceb60
#define FUNC_CAnimBlendAssociation_Start                0x4ceb70
#define FUNC_CAnimBlendAssociation_SetBlendTo           0x4ceb80
#define FUNC_CAnimBlendAssociation_SetBlend             0x4ceba0
#define FUNC_CAnimBlendAssociation_Init                 0x4ced50
#define FUNC_CAnimBlendAssociation_GetTimeStep          0x4cfb60
#define FUNC_CAnimBlendAssociation_GetAnimHierarchy     0x4d1360
#define FUNC_CAnimBlendAssociation_UpdateTimeStep       0x4d13a0
#define FUNC_CAnimBlendAssociation_UpdateTime           0x4d13d0
#define FUNC_CAnimBlendAssociation_UpdateBlend          0x4d1490
#define FUNC_CAnimBlendAssociation_SetFlags             0x4d3880
#define FUNC_CAnimBlendAssociation_GetAnimHashKey       0x4d6200
#define FUNC_CAnimBlendAssociation_GetCallbackType      0x61a6a0
#define FUNC_CAnimBlendAssociation_HasCallbackDataPtr   0x61a6b0


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

    bool                                IsFlagSet               ( unsigned int uiFlag );   
    void                                SetFlag                 ( unsigned int uiFlag );
    void                                ClearFlag               ( unsigned int uiFlag );
    float                               GetSpeed                ( void );
    void                                SetSpeed                ( float fSpeed );
    float                               GetTime                 ( void );
    void                                SetTime                 ( float fTime );
    float                               GetTotalTime            ( void );
    float                               GetTimeStep             ( void );
    void                                UpdateTimeStep          ( float fUnk, float fUnk2 );

    bool                                UpdateBlend             ( float fUnk );
    void                                SyncAnimation           ( CAnimBlendAssociation * pAssoc );

protected:
    CAnimBlendAssociationSAInterface *  m_pInterface;
    CALLBACK_CAnimBlendAssoc            m_DeleteCallback;
    void *                              m_pDeleteCallbackData;
    CALLBACK_CAnimBlendAssoc            m_FinishCallback;
    void *                              m_pFinishCallbackData;
};

#endif
/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CAnimBlendAssociation.h
*  PURPOSE:		Animation blend association interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimBlendAssociation_H
#define __CAnimBlendAssociation_H

class CAnimBlendAssociationSAInterface;
typedef unsigned long AssocGroupId;
typedef unsigned long AnimationId;
class CAnimBlendHierarchy;
typedef void (*CALLBACK_CAnimBlendAssoc) ( CAnimBlendAssociation * pAssoc, void * pData );

class CAnimBlendAssociation
{
public:
    virtual CAnimBlendAssociationSAInterface *  GetInterface            ( void ) = 0;
    virtual AssocGroupId                        GetAnimGroup            ( void ) = 0;
    virtual AnimationId                         GetAnimID               ( void ) = 0;
    virtual CAnimBlendHierarchy *               GetAnimHierarchy        ( void ) = 0;

    virtual void                                SetDeleteCallback       ( CALLBACK_CAnimBlendAssoc Callback, void * pCallbackData ) = 0;
    virtual void                                SetFinishCallback       ( CALLBACK_CAnimBlendAssoc Callback, void * pCallbackData ) = 0;

    virtual bool                                IsFlagSet               ( unsigned int uiFlag ) = 0;
    virtual void                                SetFlag                 ( unsigned int uiFlag ) = 0;
    virtual void                                ClearFlag               ( unsigned int uiFlag ) = 0;
    virtual float                               GetSpeed                ( void ) = 0;
    virtual void                                SetSpeed                ( float fSpeed ) = 0;
    virtual float                               GetTime                 ( void ) = 0;
    virtual void                                SetTime                 ( float fTime ) = 0;
    virtual float                               GetTotalTime            ( void ) = 0;
    virtual float                               GetTimeStep             ( void ) = 0;
    virtual void                                UpdateTimeStep          ( float fUnk, float fUnk2 ) = 0;

    virtual bool                                UpdateBlend             ( float fUnk ) = 0;
    virtual void                                SyncAnimation           ( CAnimBlendAssociation * pAssoc ) = 0;
};

#endif
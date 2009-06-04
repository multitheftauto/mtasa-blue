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

    virtual bool                                UpdateBlend             ( float fUnk ) = 0;
};

#endif
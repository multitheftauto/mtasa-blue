/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlock.h
 *  PURPOSE:     Animation block interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum EModelRequestType
{
    BLOCKING,
    NON_BLOCKING
};

class CAnimBlendHierarchySAInterface;
class CAnimBlockSAInterface;

class CAnimBlock
{
public:
    virtual CAnimBlockSAInterface*          GetInterface() = 0;
    virtual char*                           GetName() = 0;
    virtual int                             GetIndex() = 0;
    virtual void                            AddRef() = 0;
    virtual unsigned short                  GetRefs() = 0;
    virtual bool                            IsLoaded() = 0;
    virtual int                             GetIDOffset() = 0;
    virtual size_t                          GetAnimationCount() = 0;
    virtual void                            Request(EModelRequestType requestType, bool bAllowBlockingFail = false) = 0;
    virtual CAnimBlendHierarchySAInterface* GetAnimationHierarchyInterface(size_t iAnimation) = 0;
};

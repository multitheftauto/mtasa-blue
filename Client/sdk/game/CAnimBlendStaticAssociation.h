/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlendStaticAssocation.h
 *  PURPOSE:     Animation blend static association interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CAnimBlendStaticAssociation_H
#define __CAnimBlendStaticAssociation_H

struct RpClump;
class CAnimBlendHierarchySAInterface;

class CAnimBlendStaticAssociation
{
public:
    virtual void Initialize(RpClump* pClump, CAnimBlendHierarchySAInterface* pAnimBlendHierarchyInterface) = 0;
    virtual void SetNumBlendNodes(unsigned short nNumBlendNodes) = 0;
    virtual void SetAnimID(short sAnimID) = 0;
    virtual void SetAnimGroup(short sAnimGroup) = 0;
    virtual void SetFlags(short sFlags) = 0;

    virtual unsigned short                  GetNumBlendNodes(void) = 0;
    virtual short                           GetAnimID(void) = 0;
    virtual short                           GetAnimGroup(void) = 0;
    virtual short                           GetFlags(void) = 0;
    virtual CAnimBlendHierarchySAInterface* GetAnimHierachyInterface(void) = 0;
};

#endif

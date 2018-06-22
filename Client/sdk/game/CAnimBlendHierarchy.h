/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAnimBlendHierarchy.h
 *  PURPOSE:     Animation blend hierarchy interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CAnimBlendHierarchy_H
#define __CAnimBlendHierarchy_H

class CAnimBlendHierarchySAInterface;

class CAnimBlendHierarchy
{
public:
    virtual void                            Initialize(void) = 0;
    virtual void                            SetName(const char* szName) = 0;
    virtual void                            SetSequences(CAnimBlendSequenceSAInterface* pSequences) = 0;
    virtual void                            SetNumSequences(unsigned short uNumSequences) = 0;
    virtual void                            SetRunningCompressed(bool bCompressed) = 0;
    virtual void                            SetAnimationBlockID(int iBlockID) = 0;
    virtual void                            RemoveAnimSequences(void) = 0;
    virtual void                            RemoveFromUncompressedCache(void) = 0;
    virtual void                            RemoveQuaternionFlips(void) = 0;
    virtual void                            CalculateTotalTime(void) = 0;
    virtual CAnimBlendSequenceSAInterface*  GetSequence(DWORD dwIndex) = 0;
    virtual CAnimBlendSequenceSAInterface*  GetSequences(void) = 0;
    virtual unsigned short                  GetNumSequences(void) = 0;
    virtual bool                            isRunningCompressed(void) = 0;
    virtual int                             GetAnimBlockID(void) = 0;
    virtual CAnimBlendHierarchySAInterface* GetInterface(void) = 0;
};

#endif

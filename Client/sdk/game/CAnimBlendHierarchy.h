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

#pragma once

class CAnimBlendHierarchySAInterface;
class CAnimBlendSequenceSAInterface;

class CAnimBlendHierarchy
{
public:
    virtual void                            Initialize() = 0;
    virtual void                            SetName(const char* szName) = 0;
    virtual void                            SetSequences(CAnimBlendSequenceSAInterface* pSequences) = 0;
    virtual void                            SetNumSequences(unsigned short uNumSequences) = 0;
    virtual void                            SetRunningCompressed(bool bCompressed) = 0;
    virtual void                            SetAnimationBlockID(int iBlockID) = 0;
    virtual void                            RemoveAnimSequences() = 0;
    virtual void                            RemoveFromUncompressedCache() = 0;
    virtual void                            RemoveQuaternionFlips() = 0;
    virtual void                            CalculateTotalTime() = 0;
    virtual float                           GetTotalTime() const noexcept = 0;
    virtual CAnimBlendSequenceSAInterface*  GetSequence(DWORD dwIndex) = 0;
    virtual CAnimBlendSequenceSAInterface*  GetSequences() = 0;
    virtual unsigned short                  GetNumSequences() = 0;
    virtual bool                            IsRunningCompressed() = 0;
    virtual bool                            IsCustom() = 0;
    virtual int                             GetAnimBlockID() = 0;
    virtual CAnimBlendHierarchySAInterface* GetInterface() = 0;
    virtual unsigned int                    GetNameHashKey() = 0;
};

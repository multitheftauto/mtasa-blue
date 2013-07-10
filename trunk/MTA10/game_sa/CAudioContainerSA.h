/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioContainerSA.h
*  PURPOSE:     Audio container reader
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*  NOTE:        Big thanks to pdescobar for making SAAT. Because of him I saved a lot of time with reversing
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAUDIOCONTAINERSA_H
#define __CAUDIOCONTAINERSA_H

#include <game/CAudioContainer.h>
#include "CAudioContainerLookupTableSA.h"
#include <fstream>

class CAudioContainerSA : public CAudioContainer
{
public:
    CAudioContainerSA(void);
    ~CAudioContainerSA(void);
    
    bool GetAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length );

private:
    CAudioContainerLookupTableSA* m_pLookupTable;

protected:
    bool GetRawAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, uint8*& dataOut, unsigned int& lengthOut, int& iSampleRateOut );
    const SString GetAudioArchiveName ( eAudioLookupIndex );

};

struct SAudioEntrySA
{
    uint32 offset;
    uint32 unknown1;
    uint16 sampleRate;
    uint16 unknown2;
};

struct SAudioBankHeaderSA
{
    int numSounds;
    SAudioEntrySA sounds[400]; // Todo: optimize this (dynamic allocation)
};

struct SRiffWavePCMHeader
{
    uint32 chunkId; // big-endian // 0
    uint32 chunkSize; // 4
    uint32 format; // big-endian // 8

    uint32 subchunk1Id; // big-endian // 12
    uint32 subchunk1Size; // 16
    uint16 audioFormat; // 20
    uint16 numChannels; // 22
    uint32 sampleRate; // 24
    uint32 byteRate; // 28
    uint16 blockAlign; // 32
    uint16 bitsPerSample; // 34

    uint32 subchunk2Id; // big-endian // 36
    uint32 subchunk2Size; // 40
}; // size = 44 = 0x2C

#endif

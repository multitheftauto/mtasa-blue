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

#define VALIDATE_BUFFER_SIZE 4096

class CAudioContainerSA : public CAudioContainer
{
public:
    CAudioContainerSA(void);
    ~CAudioContainerSA(void);
    
    bool GetAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length );
    bool ValidateContainer ( eAudioLookupIndex lookupIndex );

    bool GetRadioAudioData(eRadioStreamIndex streamIndex, int trackIndex, void*& pMemory, unsigned int& length);

private:
    CAudioContainerLookupTableSA* m_pLookupTable;

protected:
    bool GetRawAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, uint8*& dataOut, unsigned int& lengthOut, int& iSampleRateOut );
    const SString GetAudioArchiveName ( eAudioLookupIndex );

    const SString GetRadioStreamArchiveName(eRadioStreamIndex streamIndex);
    
    template<typename T> void ReadRadioArchive(std::ifstream& stream, T& value, std::size_t len = 1)
    {
        static uint8_t xor[] = { 0xEA, 0x3A, 0xC4, 0xA1, 0x9A, 0xA8, 0x14, 0xF3, 0x48, 0xB0, 0xD7, 0x23, 0x9D, 0xE8, 0xFF, 0xF1 };
        uint8_t xorPosition = stream.tellg() % sizeof(xor);

        stream.read((char*) &value, sizeof(T) * len);

        // for some reason R* decided to xor the radio streams
        // see gta_sa.exe @ 0x4F17D0
        for (unsigned int i = 0; i < sizeof(T) * len; ++i)
        {
            ((char*) &value)[i] ^= xor[xorPosition];

            xorPosition++;
            if (xorPosition >= sizeof(xor))
                xorPosition = 0;

        }
    }
};

struct SAudioEntrySA
{
    uint32 offset;
    uint32 unknown1;
    uint16 sampleRate;
    uint16 unknown2;
};
C_ASSERT ( sizeof ( SAudioEntrySA ) == 0xC );

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
C_ASSERT ( sizeof ( SRiffWavePCMHeader ) == 0x2C );

struct SRadioTrackHeader
{
    char unk[8000]; // 0
    int32_t length; // 8000 (length of the ogg stream)
    int32_t unk2;   // 8004 (sample rate?)
    char unk3[60];  // 8008
};
C_ASSERT(sizeof(SRadioTrackHeader) == 8068);

#endif

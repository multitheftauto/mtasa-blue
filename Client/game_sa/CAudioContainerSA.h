/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAudioContainerSA.h
 *  PURPOSE:     Audio container reader
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <fstream>
#include <game/CAudioContainer.h>

class CAudioContainerLookupTableSA;

#define VALIDATE_BUFFER_SIZE 4096
#define NUM_BEAT_ENTRIES 1000
#define NUM_LENGTH_ENTRIES 8

struct SAudioEntrySA
{
    uint32 offset;
    uint32 unknown1;
    uint16 sampleRate;
    uint16 unknown2;
};
static_assert(sizeof(SAudioEntrySA) == 0xC, "Invalid size for SAudioLookupEntrySA");

struct SAudioBankHeaderSA
{
    int           numSounds;
    SAudioEntrySA sounds[400];            // Todo: optimize this (dynamic allocation)
};

struct SRiffWavePCMHeader
{
    uint32 chunkId;              // big-endian // 0
    uint32 chunkSize;            // 4
    uint32 format;               // big-endian // 8

    uint32 subchunk1Id;              // big-endian // 12
    uint32 subchunk1Size;            // 16
    uint16 audioFormat;              // 20
    uint16 numChannels;              // 22
    uint32 sampleRate;               // 24
    uint32 byteRate;                 // 28
    uint16 blockAlign;               // 32
    uint16 bitsPerSample;            // 34

    uint32 subchunk2Id;              // big-endian // 36
    uint32 subchunk2Size;            // 40
};                                   // size = 44 = 0x2C
static_assert(sizeof(SRiffWavePCMHeader) == 0x2C, "Invalid size for SRiffWavePCMHeader");

// Documentation by SAAT //
// https://pdescobar.home.comcast.net/~pdescobar/gta/saat/ //
struct SBeatEntry
{
    int32 timing;
    int32 control;
};
static_assert(sizeof(SBeatEntry) == 0x8, "Invalid size for SBeatEntry");

struct SLengthEntry
{
    uint32 length;
    uint32 extra;
};
static_assert(sizeof(SLengthEntry) == 0x8, "Invalid size for SLengthEntry");

struct SRadioTrackHeader
{
    SBeatEntry   beats[NUM_BEAT_ENTRIES];
    SLengthEntry lengths[NUM_LENGTH_ENTRIES];
    uint32       trailer;
};
static_assert(sizeof(SRadioTrackHeader) == 8068, "Invalid size for SRadioTrackHeader");

// End of documentation by SAAT //
// https://pdescobar.home.comcast.net/~pdescobar/gta/saat/ //

class CAudioContainerSA : public CAudioContainer
{
public:
    CAudioContainerSA();
    ~CAudioContainerSA();

    bool GetAudioData(eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length);
    bool ValidateContainer(eAudioLookupIndex lookupIndex);

    bool GetRadioAudioData(eRadioStreamIndex streamIndex, int trackIndex, void*& pMemory, unsigned int& length);

private:
    CAudioContainerLookupTableSA* m_pLookupTable;

protected:
    bool          GetRawAudioData(eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, uint8*& dataOut, unsigned int& lengthOut, int& iSampleRateOut);
    const SString GetAudioArchiveName(eAudioLookupIndex);

    const SString GetRadioStreamArchiveName(eRadioStreamIndex streamIndex);

    bool GetAudioSizeFromHeader(const SRadioTrackHeader& header, int& iSize);

    template <typename T>
    void ReadRadioArchive(std::ifstream& stream, T& value, std::size_t len = 1)
    {
        static uint8 xorkey[] = {0xEA, 0x3A, 0xC4, 0xA1, 0x9A, 0xA8, 0x14, 0xF3, 0x48, 0xB0, 0xD7, 0x23, 0x9D, 0xE8, 0xFF, 0xF1};
        uint8        xorPosition = stream.tellg() % sizeof(xorkey);

        stream.read((char*)&value, sizeof(T) * len);

        // for some reason R* decided to xor the radio streams
        // see gta_sa.exe @ 0x4F17D0
        for (unsigned int i = 0; i < sizeof(T) * len; ++i)
        {
            ((char*)&value)[i] ^= xorkey[xorPosition];

            xorPosition++;
            if (xorPosition >= sizeof(xorkey))
                xorPosition = 0;
        }
    }
};

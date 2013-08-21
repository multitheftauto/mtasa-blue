/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioContainerSA.cpp
*  PURPOSE:     Audio container reader
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CAudioContainerSA.h"

unsigned long EndianSwap32(unsigned long x)
{
    unsigned long y=0;
    y += (x & 0x000000FF)<<24;
    y += (x & 0xFF000000)>>24;
    y += (x & 0x0000FF00)<<8;
    y += (x & 0x00FF0000)>>8;

    return y;
}

CAudioContainerSA::CAudioContainerSA ( void )
{
    m_pLookupTable = new CAudioContainerLookupTableSA ( "AUDIO\\CONFIG\\BankLkup.dat" );
}

CAudioContainerSA::~CAudioContainerSA ( void )
{
    if ( m_pLookupTable != NULL )
    {
        delete m_pLookupTable;
        m_pLookupTable = NULL;
    }
}

bool CAudioContainerSA::GetAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, void*& pMemory, unsigned int& length )
{
    uint8* rawAudioData = NULL;
    unsigned int rawAudioLength;
    int iSampleRate;

    if ( !GetRawAudioData ( lookupIndex, bankIndex, audioIndex, rawAudioData, rawAudioLength, iSampleRate ) )
    {
        if ( rawAudioData != NULL )
            delete[] rawAudioData;

        return false;
    }

    // Add the RIFF Wave header since BASS does not support raw data
    SRiffWavePCMHeader waveHeader;
    waveHeader.chunkId = EndianSwap32(0x52494646); // "RIFF" in ASCII, big-Endian
    waveHeader.chunkSize = 36 + rawAudioLength;
    waveHeader.format = EndianSwap32(0x57415645); // "WAVE" in ASCII, big-Endian

    waveHeader.subchunk1Id = EndianSwap32(0x666d7420); // "fmt " in ASCII, big-Endian
    waveHeader.subchunk1Size = 16; // 16 for PCM
    waveHeader.audioFormat = 0x1; // PCM
    waveHeader.numChannels = 1; // Mono
    waveHeader.sampleRate = iSampleRate;
    waveHeader.bitsPerSample = 16; // 16-Bit PCM
    waveHeader.byteRate = waveHeader.sampleRate * waveHeader.numChannels * waveHeader.bitsPerSample/8;
    waveHeader.blockAlign = waveHeader.numChannels * waveHeader.bitsPerSample/8;

    waveHeader.subchunk2Id = EndianSwap32(0x64617461); // "data" in ASCII, big-endian
    waveHeader.subchunk2Size = rawAudioLength;

    // Allocate a second buffer as we've to insert the wave pcm header at the beginning
    uint8* buffer = new uint8[sizeof(SRiffWavePCMHeader) + rawAudioLength];

    // Copy header and first buffer into the new buffer
    memcpy ( buffer, &waveHeader, sizeof(SRiffWavePCMHeader) );
    memcpy ( buffer + sizeof(SRiffWavePCMHeader), rawAudioData, rawAudioLength );

    // Free previously allocated memory (not needed anymore since we copied the memory to the new buffer)
    delete[] rawAudioData;
    
    // Return via call by reference
    pMemory = buffer;
    length = sizeof(SRiffWavePCMHeader) + rawAudioLength;

    return true;
}

bool CAudioContainerSA::GetRawAudioData ( eAudioLookupIndex lookupIndex, int bankIndex, int audioIndex, uint8*& dataOut, unsigned int& lengthOut, int& iSampleRateOut )
{
    int numBanks = m_pLookupTable->CountIndex ( lookupIndex );
    if ( numBanks == 0 )
        return false;

    // Get archive file size
    std::ifstream archive ( GetAudioArchiveName ( lookupIndex ), std::ios::binary | std::ios::ate ); // ate = seek to end of file
    int archiveSize = archive.tellg (); // get file pointer position

    SAudioLookupEntrySA* lookupEntry = m_pLookupTable->GetEntry ( lookupIndex, bankIndex );
    if ( !lookupEntry )
        return false;

    // Read audio archieve bank header (an audio archieve has multiple bank headers)
    SAudioBankHeaderSA bankHeader;

    // Seek to the bank offset and read the header
    archive.seekg ( lookupEntry->offset );
    archive.read ( reinterpret_cast<char*> ( &bankHeader ), sizeof ( SAudioBankHeaderSA ) );

    // Get offsets to calculate the length
    SAudioEntrySA* audioEntry = &bankHeader.sounds[audioIndex];
    if ( !audioEntry )
        return false;

    unsigned int rawLength;
    if ( audioIndex+1 < bankHeader.numSounds ) // hacky fix: audioIndex starts at 0
    {
        SAudioEntrySA* nextAudioEntry = &bankHeader.sounds[audioIndex+1];
        if ( !nextAudioEntry )
            return false;

        rawLength = nextAudioEntry->offset - audioEntry->offset;
    }
    else if ( audioIndex+1 == bankHeader.numSounds )
    {
        rawLength = lookupEntry->length - audioEntry->offset;
    }
    else
        return false;

    // Now we are ready to read the audio data :)
    uint8* buffer = new uint8[rawLength];

    dataOut = buffer;
    lengthOut = rawLength;
    iSampleRateOut = audioEntry->sampleRate;

    // Seek to the correct offset and read
    archive.seekg ( lookupEntry->offset + sizeof(SAudioBankHeaderSA) + audioEntry->offset ); // Or just archive.seekg ( archive.tellg() + audioEntry->offset )
    return archive.read(reinterpret_cast<char*> ( buffer ), rawLength ) != NULL;
}

const SString CAudioContainerSA::GetAudioArchiveName ( eAudioLookupIndex lookupIndex )
{
    SString archiveName = "AUDIO\\SFX\\";
    switch ( lookupIndex )
    {
        case AUDIO_LOOKUP_FEET: archiveName += "FEET"; break;
        case AUDIO_LOOKUP_GENRL: archiveName += "GENRL"; break;
        case AUDIO_LOOKUP_PAIN_A: archiveName += "PAIN_A"; break;
        case AUDIO_LOOKUP_SCRIPT: archiveName += "SCRIPT"; break;
        case AUDIO_LOOKUP_SPC_EA: archiveName += "SPC_EA"; break;
        case AUDIO_LOOKUP_SPC_FA: archiveName += "SPC_FA"; break;
        case AUDIO_LOOKUP_SPC_GA: archiveName += "SPC_GA"; break;
        case AUDIO_LOOKUP_SPC_NA: archiveName += "SPC_NA"; break;
        case AUDIO_LOOKUP_SPC_PA: archiveName += "SPC_PA"; break;
    }
    return archiveName;
}

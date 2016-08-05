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
    std::ifstream archive ( FromUTF8( GetAudioArchiveName ( lookupIndex ) ), std::ios::binary );

    SAudioLookupEntrySA* lookupEntry = m_pLookupTable->GetEntry ( lookupIndex, bankIndex );
    if ( !lookupEntry )
        return false;

    // Read audio archive bank header (an audio archive has multiple bank headers)
    SAudioBankHeaderSA bankHeader;

    // Seek to the bank offset and read the header
    archive.seekg ( lookupEntry->offset );
    if ( !archive.read ( reinterpret_cast<char*> ( &bankHeader ), sizeof ( SAudioBankHeaderSA ) ) )
        return false;

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

    // 2MB check in case of user modification errors (Max length of standard audio files is 560KB)
    if ( rawLength > 2 * 1024 * 1024 )
        return false;

    // Now we are ready to read the audio data :)
    uint8* buffer = new uint8[rawLength];

    dataOut = buffer;
    lengthOut = rawLength;
    iSampleRateOut = audioEntry->sampleRate;

    // Seek to the correct offset and read
    archive.seekg ( lookupEntry->offset + sizeof(SAudioBankHeaderSA) + audioEntry->offset ); // Or just archive.seekg ( archive.tellg() + audioEntry->offset )
    archive.read(reinterpret_cast<char*> ( buffer ), rawLength );

    return !archive.fail();
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

bool CAudioContainerSA::ValidateContainer ( eAudioLookupIndex lookupIndex )
{
#ifdef MTA_DEBUG
    return true;
#endif

    // Open archive and place file pointer at the end
    std::ifstream archive ( FromUTF8( GetAudioArchiveName ( lookupIndex ) ), std::ios::binary );

    // Check if it does not exist
    if ( !archive )
        return false;

    // Get the archive size divided by 10 and seek to the beginning
    archive.seekg ( 0, std::ios::end );
    std::streampos stepSize = archive.tellg () / 10;
    archive.seekg ( 0 );

    // Count the zeros -> if more than 80% we assume that it has been cut (read 4KB blocks at once)
    uint8 buffer[VALIDATE_BUFFER_SIZE];
    unsigned long long numZeros = 0;
    while ( archive.read ( reinterpret_cast<char*>(buffer), VALIDATE_BUFFER_SIZE ) )
    {
        for ( unsigned int i = 0; i < VALIDATE_BUFFER_SIZE; i++ )
        {
            if ( buffer[i] == 0 )
                numZeros++;
        }

        archive.seekg ( stepSize, std::ios::cur );
    }

    // Close the archive
    archive.close ();

    if ( static_cast < float > ( numZeros ) / (10*VALIDATE_BUFFER_SIZE) >= 0.8f )
        return false;

    return true;
}
bool CAudioContainerSA::GetAudioSizeFromHeader ( const SRadioTrackHeader &header, int &iSize )
{
    // find our track length
    // each header contains 8 lengths
    for ( int i = 0; i < NUM_LENGTH_ENTRIES; i++ )
    {
        // find the length that isn't "default"
        // default is 0xCDCDCDCD (ignoring this will result in a crash.)
        if ( header.lengths[i].length != 0xCDCDCDCD )
        {
            // that's our size
            iSize = header.lengths[i].length;
        }
    }
    // if we haven't found a size we can't continue
    if ( iSize == 0 )
        return false;

    return true;
}

bool CAudioContainerSA::GetRadioAudioData(eRadioStreamIndex streamIndex, int trackIndex, void*& pMemory, unsigned int& length)
{
    std::ifstream archive(FromUTF8(GetRadioStreamArchiveName(streamIndex)), std::ios::binary);

    // Check if it does not exist
    if (!archive)
        return false;

    // The tracks are stored consecutive in the stream files
    // A track consist of a header and is followed by the actual ogg stream

    // Since there is no index, we need to move through the tracks until we reach the desired one

    // header
    SRadioTrackHeader header;
    int trackid = 1;
    
    int iSize = 0;

    while (trackid < trackIndex)
    {
        // Read our header
        ReadRadioArchive ( archive, header );

        // if we haven't found a size we can't continue
        if ( GetAudioSizeFromHeader ( header, iSize ) == false )
            return false;

        // seek forward to the next header
        archive.seekg ( iSize, std::ios_base::cur );

        ++trackid;

        if (archive.fail())
            return false;
    }

    // We now have reached the right track
    ReadRadioArchive ( archive, header );


    // if we haven't found a size we can't continue
    if ( GetAudioSizeFromHeader ( header, iSize ) == false )
        return false;

    // if we haven't found a size we can't continue
    if ( iSize == 0 )
        return false;

    if (archive.fail())
        return false;

    uint8 *pData = new uint8[iSize];
    ReadRadioArchive ( archive, *pData, iSize );

    if (archive.fail())
    {
        delete[] pData;
        return false;
    }

    // Test the header signature of the ogg stream
    if (pData[0] != 'O' ||
        pData[1] != 'g' ||
        pData[2] != 'g' ||
        pData[3] != 'S')
    {
        delete[] pData;
        return false;
    }

    // handle steam deleted files cleanly by not playing.
    if ( iSize <= 4500 )
    {
        delete[] pData;
        return false;
    }

    pMemory = pData;
    length = iSize;

    return true;
}


const SString CAudioContainerSA::GetRadioStreamArchiveName(eRadioStreamIndex streamIndex)
{
    switch (streamIndex)
    {
        case RADIO_STREAM_POLICE:           return "AUDIO\\STREAMS\\AA";
        case RADIO_STREAM_ADVERTS:          return "AUDIO\\STREAMS\\ADVERTS";
        case RADIO_STREAM_AMBIENCE:         return "AUDIO\\STREAMS\\AMBIENCE";
        case RADIO_STREAM_BEATS:            return "AUDIO\\STREAMS\\BEATS";
        case RADIO_STREAM_PLAYBACK:         return "AUDIO\\STREAMS\\CH";
        case RADIO_STREAM_KROSE:            return "AUDIO\\STREAMS\\CO";
        case RADIO_STREAM_KDST:             return "AUDIO\\STREAMS\\CR";
        case RADIO_STREAM_CUTSCENE:         return "AUDIO\\STREAMS\\CUTSCENE";
        case RADIO_STREAM_BOUNCE:           return "AUDIO\\STREAMS\\DS";
        case RADIO_STREAM_SFUR:             return "AUDIO\\STREAMS\\HC";
        case RADIO_STREAM_RADIO_LOS_SANTOS: return "AUDIO\\STREAMS\\MH";
        case RADIO_STREAM_RADIO_X:          return "AUDIO\\STREAMS\\MR";
        case RADIO_STREAM_CSR:              return "AUDIO\\STREAMS\\NJ";
        case RADIO_STREAM_KJAH:             return "AUDIO\\STREAMS\\RE";
        case RADIO_STREAM_MASTER_SOUNDS:    return "AUDIO\\STREAMS\\RG";
        case RADIO_STREAM_WCTR:             return "AUDIO\\STREAMS\\TK";
    }

    // Bad Index
    return "";
}
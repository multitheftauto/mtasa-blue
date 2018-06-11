#include <StdInc.h>

CFileReader::CFileReader ( void )
{
    pFilePath = nullptr;
    pFilePointer = nullptr;
    pFileDataBuffer = nullptr;
    u32FileLength = 0;
    u32BytesReadFromBuffer = 0;
}

CFileReader::~CFileReader ( void )
{
    UnloadFile ();
}

BOOL CFileReader::CreateLoader ( const char * pFileToRead )
{
    SetLoaderFilePath ( pFileToRead );
    if ( !OpenFile ( ) || !GetFileLength ( ) )
        return FALSE;

    AllocateBufferMemory ( );
    pFileDataBuffer [ u32FileLength ] = '\0';
    return TRUE;
}

BOOL CFileReader::LoadFile ( void )
{
    if ( !LoadToMemory ( ) )
        return FALSE;

    //std::cout << "Length: " << u32FileLength << std::endl;
    return TRUE;
}

void CFileReader::UnloadFile ( void )
{
    if ( pFileDataBuffer != nullptr )
    {
        delete[] pFileDataBuffer;
        pFileDataBuffer = nullptr;
    } 
    if ( pFilePointer != nullptr )
    {
        fclose ( pFilePointer );
    }
}

void CFileReader::ReadBytes ( void * pDestination, const UINT BytesToRead )
{
    const UINT ReadOffset = u32BytesReadFromBuffer;
    u32BytesReadFromBuffer += BytesToRead;
    memcpy ( pDestination, pFileDataBuffer + ReadOffset, BytesToRead );
}

std::string CFileReader::ReadString ( UINT u32SizeInBytes )
{
    std::string String;
    String.resize ( u32SizeInBytes );

    for ( UINT i = 0; i < u32SizeInBytes; i++ )
    {
        const UINT ReadOffset = u32BytesReadFromBuffer;
        String [ i ] = pFileDataBuffer [ ReadOffset ];
        u32BytesReadFromBuffer++;
    }
    return String;
}

void CFileReader::ReadCString ( char * pDestination, const UINT u32BytesToRead )
{
    const UINT u32ReadOffset = u32BytesReadFromBuffer;
    u32BytesReadFromBuffer += u32BytesToRead;
    memcpy ( pDestination, pFileDataBuffer + u32ReadOffset, u32BytesToRead );
    *( pDestination + (u32BytesToRead- 1) ) = '\0';
}

void CFileReader::SkipBytes ( UINT u32BytesToSkip )
{
    u32BytesReadFromBuffer += u32BytesToSkip;
}

inline void CFileReader::SetLoaderFilePath ( const char * szFileToRead )
{
    pFilePath = szFileToRead;
}

BOOL CFileReader::OpenFile ( void )
{
    errno_t FileOpen = fopen_s ( &pFilePointer, pFilePath, "rb" );
    if ( FileOpen != 0 )
    {
        std::cout << "Failed to open file";
        return FALSE;
    }
    return TRUE;
}

BOOL CFileReader::GetFileLength ( void )
{
    if ( !pFilePointer )
        return FALSE;

    if ( fseek ( pFilePointer, 0, SEEK_END ) != 0 )
    {
        std::cout << "Failed to set cursor to end of file." << std::endl;
        return FALSE;
    }

    u32FileLength = ftell ( pFilePointer );
    if ( u32FileLength == -1L )
    {
        std::cout << "Failed to get length of file (ftell)" << std::endl;
        return FALSE;
    }

    rewind ( pFilePointer );
    return TRUE;
}

void CFileReader::AllocateBufferMemory ( void )
{
    if ( !pFilePointer )
        return;

    pFileDataBuffer = new char [ u32FileLength + 1 ];
}

BOOL CFileReader::LoadToMemory ( void )
{
    if ( !pFilePointer )
        return FALSE;

    UINT u32ReadSize = fread ( pFileDataBuffer, sizeof ( char ), u32FileLength, pFilePointer );
    if ( u32ReadSize != u32FileLength )
    {
        std::cout << "loadToMemory Failed. ReadSize != u32FileLength";
        return FALSE;
    }
    return TRUE;
}

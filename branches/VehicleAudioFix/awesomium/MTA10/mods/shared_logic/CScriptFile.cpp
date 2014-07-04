/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CScriptFile.cpp
*  PURPOSE:     Script file class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

CScriptFile::CScriptFile ( const char* szFilename, unsigned long ulMaxSize ) : ClassInit ( this ), CClientEntity ( INVALID_ELEMENT_ID )
{
    // Init
    SetTypeName ( "file" );
    m_pFile = NULL;
    m_strFilename = szFilename ? szFilename : "";
    m_ulMaxSize = ulMaxSize;
}


CScriptFile::~CScriptFile ( void )
{
    // Close the file
    Unload ();
}


bool CScriptFile::Load ( eMode Mode )
{
    // If we haven't already got a file
    if ( !m_pFile )
    {
        m_pFile = g_pNet->AllocateBinaryFile ();
        bool bOk = false;
        switch ( Mode )
        {
            // Open file in read only binary mode
            case MODE_READ:
                bOk = m_pFile->FOpen ( m_strFilename.c_str (), "rb", true );
                break;

            // Open file in read write binary mode.
            case MODE_READWRITE:
                // Try to load the file in rw mode. Use existing content.
                bOk = m_pFile->FOpen ( m_strFilename.c_str (), "rb+", true );
                break;

            // Open file in read write binary mode. Truncate size to 0.
            case MODE_CREATE:
                bOk = m_pFile->FOpen ( m_strFilename.c_str (), "wb+", true );
                break;
        }

        if ( !bOk )
        {
            SAFE_DELETE( m_pFile );
        }

        // Return whether we successfully opened it or not
        return m_pFile != NULL;
    }

    // Failed
    return false;
}


void CScriptFile::Unload ( void )
{
    // Close the file if required
    SAFE_DELETE( m_pFile );
}


bool CScriptFile::IsEOF ( void )
{
    if ( !m_pFile )
        return true;

    // Reached end of file?
    return m_pFile->FEof ();
}


long CScriptFile::GetPointer ( void )
{
    if ( !m_pFile )
        return -1;

    return m_pFile->FTell ();
}


long CScriptFile::GetSize ( void )
{
    if ( !m_pFile )
        return -1;

    // Remember current position and seek to the end
    long lCurrentPos = m_pFile->FTell ();
    m_pFile->FSeek ( 0, SEEK_END );

    // Retrieve size of file
    long lSize = m_pFile->FTell ();

    // Seek back to where the pointer was
    m_pFile->FSeek ( lCurrentPos, SEEK_SET );

    // Return the size
    return lSize;
}


long CScriptFile::SetPointer ( unsigned long ulPosition )
{
    if ( !m_pFile )
        return -1;

    // Is the new position bigger than the file?
    if ( GetSize () < static_cast < long > ( ulPosition ) )
    {
        // Don't make it bigger than our limit
        if ( ulPosition > m_ulMaxSize )
        {
            ulPosition = m_ulMaxSize;
        }
    }

    // Move the pointer
    m_pFile->FSeek ( ulPosition, SEEK_SET );

    // Bigger than file size? Tell the script how far we were able to move it
    long lSize = GetSize ();
    if ( ulPosition > static_cast < unsigned long > ( lSize ) )
    {
        ulPosition = static_cast < unsigned long > ( lSize );
    }

    // Return the new position
    return ulPosition;
}


void CScriptFile::Flush ( void )
{
    if ( !m_pFile )
        return;

    m_pFile->FFlush ();
}


long CScriptFile::Read ( unsigned long ulSize, char* pData )
{
    if ( !m_pFile )
        return -1;

    // Try to read data into the given block. Return number of bytes we read.
    return m_pFile->FRead ( pData, ulSize );
}


long CScriptFile::Write ( unsigned long ulSize, const char* pData )
{
    if ( !m_pFile )
        return -1;

    // Write the data into the given block. Return number of bytes we wrote.
    return m_pFile->FWrite ( pData, ulSize );
}

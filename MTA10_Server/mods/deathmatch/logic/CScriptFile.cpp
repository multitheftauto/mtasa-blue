/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptFile.cpp
*  PURPOSE:     Script file element class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CScriptFile::CScriptFile ( const char* szFilename, unsigned long ulMaxSize ) : CElement ( NULL )
{
    // Init
    m_iType = CElement::SCRIPTFILE;
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
        switch ( Mode )
        {
            // Open file in read only binary mode
            case MODE_READ:
                m_pFile = fopen ( m_strFilename.c_str (), "rb" );
                break;

            // Open file in read write binary mode.
            case MODE_READWRITE:
                // Try to load the file in rw mode. Use existing content.
                m_pFile = fopen ( m_strFilename.c_str (), "rb+" );
                break;

            // Open file in read write binary mode. Truncate size to 0.
            case MODE_CREATE:
                m_pFile = fopen ( m_strFilename.c_str (), "wb+" );
                break;
        }

        // Return whether we successfully opened it or not
        return m_pFile != NULL;
    }

    // Failed
    return false;
}


void CScriptFile::Unload ( void )
{
    // Loaded?
    if ( m_pFile )
    {
        // Close the file
        fclose ( m_pFile );
        m_pFile = NULL;
    }
}


bool CScriptFile::IsEOF ( void )
{
    if ( !m_pFile )
        return true;

    // Reached end of file?
    return feof ( m_pFile ) != 0;
}


long CScriptFile::GetPointer ( void )
{
    if ( !m_pFile )
        return -1;

    return ftell ( m_pFile );
}


long CScriptFile::GetSize ( void )
{
    if ( !m_pFile )
        return -1;

    // Remember current position and seek to the end
    long lCurrentPos = ftell ( m_pFile );
    fseek ( m_pFile, 0, SEEK_END );

    // Retrieve size of file
    long lSize = ftell ( m_pFile );

    // Seek back to where the pointer was
    fseek ( m_pFile, lCurrentPos, SEEK_SET );

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
    fseek ( m_pFile, ulPosition, SEEK_SET );

    // Bigger than file size? Tell the script how far we were able to move it
    long lSize = GetSize ();
    if ( ulPosition > static_cast < unsigned long > ( lSize ) )
    {
        ulPosition = static_cast < unsigned long > ( lSize );
    }

    // Return the new position
    return ulPosition;
}


void CScriptFile::SetSize ( unsigned long ulNewSize )
{
    // TODO: A way to truncate a file
    if ( !m_pFile )
        return;

}


void CScriptFile::Flush ( void )
{
    if ( !m_pFile )
        return;

    fflush ( m_pFile );
}


long CScriptFile::Read ( unsigned long ulSize, char* pData )
{
    if ( !m_pFile )
        return -1;

    // Try to read data into the given block. Return number of bytes we read.
    return fread ( pData, 1, ulSize, m_pFile );
}


long CScriptFile::Write ( unsigned long ulSize, const char* pData )
{
    if ( !m_pFile )
        return -1;

    // Write the data into the given block. Return number of bytes we wrote.
    return fwrite ( pData, 1, ulSize, m_pFile );
}

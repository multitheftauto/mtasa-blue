/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapFile.cpp
*  PURPOSE:     Map file class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMapFile::CMapFile ( unsigned int uiID, const char* szFilename )
{
    m_uiID = uiID;

    if ( szFilename )
    {
        m_szFilename = new char [ strlen ( szFilename ) + 1 ];
        strcpy ( m_szFilename, szFilename );
    }
    else
    {
        m_szFilename = NULL;
    }
}


CMapFile::~CMapFile ( void )
{
    if ( m_szFilename )
    {
        delete [] m_szFilename;
        m_szFilename = NULL;
    }
}


char* CMapFile::GetFilename ( char* pBuffer, size_t sizeBuffer )
{
    if ( sizeBuffer > 0 )
    {
        if ( m_szFilename )
        {
            strncpy ( pBuffer, m_szFilename, sizeBuffer - 1 );
            pBuffer [ sizeBuffer - 1 ] = 0;
        }
        else
        {
            strcpy ( pBuffer, "" );
        }

        return pBuffer;
    }
    
    return NULL;
}


const char* CMapFile::GetFilenamePointer ( void )
{
    return m_szFilename;
}


unsigned int CMapFile::GetID ( void )
{
    return m_uiID;
}


void CMapFile::SetFilename ( const char* szFilename )
{
    if ( m_szFilename )
    {
        delete [] m_szFilename;
        m_szFilename = NULL;
    }

    if ( szFilename )
    {
        m_szFilename = new char [ strlen ( szFilename ) + 1 ];
        strcpy ( m_szFilename, szFilename );
    }
}


void CMapFile::SetID ( unsigned int uiID )
{
    m_uiID = uiID;
}

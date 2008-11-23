/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapFile.h
*  PURPOSE:     Map file class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMAPFILE_H
#define __CMAPFILE_H

#include "CCommon.h"

class CMapFile
{
public:
                    CMapFile            ( unsigned int uiID, const char* szFilename );
                    ~CMapFile           ( void );

    char*           GetFilename         ( char* pBuffer, size_t sizeBuffer );
    const char*     GetFilenamePointer  ( void );
    unsigned int    GetID               ( void );

    void            SetFilename         ( const char* szFilename );
    void            SetID               ( unsigned int uiID );

private:
    char*           m_szFilename;
    unsigned int    m_uiID;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapFiles.h
*  PURPOSE:     Map file manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMAPFILES_H
#define __CMAPFILES_H

#include <list>
#include "../Config.h"
#include "CMapFile.h"

class CMapFiles
{
public:
                                    CMapFiles           ( void );
                                    ~CMapFiles          ( void );

    bool                            Refresh             ( void );
    void                            Reset               ( void );

    unsigned int                    Count               ( void );

    bool                            Exists              ( const char* szFilename );
    CMapFile*                       Get                 ( const char* szFilename );
    CMapFile*                       Get                 ( unsigned int uiID );
    CMapFile*                       GetRandom           ( void );

    void                            Remove              ( CMapFile* pFile );
    void                            Remove              ( const char* szFilename );
    void                            Remove              ( unsigned int uiID );
    void                            RemoveAll           ( void );

    list < CMapFile* > ::iterator   IterBegin           ( void );
    list < CMapFile* > ::iterator   IterEnd             ( void );

private:
    CMapFile*                       Add                 ( const char* szFilename );

    list < CMapFile* >              m_Files;
    unsigned int                    m_uiLastCount;

	long long                       m_llNewestTime;

    unsigned int                    m_uiLastRandomIndex;
};

#endif

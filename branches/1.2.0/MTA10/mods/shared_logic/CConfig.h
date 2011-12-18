/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CConfig.h
*  PURPOSE:     Config class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCONFIG_H
#define __CCONFIG_H

#include <string.h>
#include <stdio.h>

class CConfig
{
public:
                        CConfig                     ( const char* szFileName );
                        ~CConfig                    ( void );

    bool                GetEntry                    ( char* szEntry, char* szReturnText, int iInstance );
    void                GetPreviousEntry            ( char* szEntry, char* szReturnTest, int iInstance );
    int                 GetNumberOfLines            ( void );
    int                 GetNumberOfEntries          ( void );
    int                 GetNumberOfSpecificEntries  ( char* szEntry );
    int                 GetNumberOfEntryProperties  ( char* szEntry, int iInstance );

    void                GetLine                     ( int iLine, char* szReturnText );

    int                 WriteToConfig               ( char* szEntry );

    void                SetFileName                 ( char* szFileName );
    void                GetFileName                 ( char* szFileName );
    bool                DoesFileExist               ( void );

private:
    char*               m_szFileName;
    FILE*               m_fp;

};

#endif

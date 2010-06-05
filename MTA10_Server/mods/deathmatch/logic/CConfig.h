/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConfig.h
*  PURPOSE:     Plain-text configuration file parser
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONFIG_H
#define __CCONFIG_H

#include <string.h>
#include <stdio.h>

#include "CCommon.h"
#include "../Config.h"

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

    int                 WriteToConfig               ( const char* szEntry );

    void                SetFileName                 ( const char* szFileName );
    void                GetFileName                 ( char* szFileName );
    bool                DoesFileExist               ( void );

private:
    std::string         m_strFileName;
    FILE*               m_fp;

};

#endif

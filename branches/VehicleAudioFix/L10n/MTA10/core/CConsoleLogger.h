/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleLogger.h
*  PURPOSE:     Header file for console logger class
*  DEVELOPERS:  Derek Abdine <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLELOGGER_H
#define __CCONSOLELOGGER_H

#include "CSingleton.h"
#include <fstream>
#include <string>


class CConsoleLogger : public CSingleton < CConsoleLogger >
{
    public:
            CConsoleLogger ( );
           ~CConsoleLogger ( );

	void    LinePrintf          ( const char* szFormat, ... );
    void    WriteLine          ( const std::string& strLine );

    private:
    
    std::string             m_strFilename;
    FILE*                   m_pFile;
    std::fstream File;

};

#endif
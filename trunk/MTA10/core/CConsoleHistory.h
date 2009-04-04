/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CConsoleHistory.h
*  PURPOSE:		Header file for the console history class
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLEHISTORY_H
#define __CCONSOLEHISTORY_H

#include <list>

class CConsoleHistory
{
public:
                    CConsoleHistory             ( unsigned int uiHistoryLength );
                    ~CConsoleHistory            ( void );

    bool            LoadFromFile                ( const char* szFilename, bool bAutosave );

    void            Add                         ( const char* szLine );
    void            Clear                       ( void );

    const char*     Get                         ( unsigned int uiIndex );

private:
    void                    DeleteLastEntry             ( void );

    std::list < char* >     m_History;
    unsigned int            m_uiHistoryLength;

    std::string             m_strFilename;
    FILE*                   m_pFile;
};

#endif

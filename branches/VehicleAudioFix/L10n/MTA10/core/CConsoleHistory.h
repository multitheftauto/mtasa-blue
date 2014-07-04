/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleHistory.h
*  PURPOSE:     Header file for the console history class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CConsoleHistory
{
public:
                    CConsoleHistory             ( unsigned int uiHistoryLength );
    void            LoadFromFile                ( const char* szFilename );
    void            Add                         ( const char* szLine );
    const char*     Get                         ( unsigned int uiIndex );

protected:
    CMappedList < SString > m_History;
    unsigned int            m_uiHistoryLength;
    SString                 m_strFilename;
};

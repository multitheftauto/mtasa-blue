/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CDOMConfig.h
*  PURPOSE:     DOM config class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CDOMCONFIG_H
#define __CDOMCONFIG_H

#include <list>
#include <string>

enum
{
    DOMCONFIG_EMPTY,
    DOMCONFIG_COMMENT,
    DOMCONFIG_DATA,
};

struct SDOMConfigLine
{
    int                 iType;
    std::string         Key;
    std::string         Entry;
};

class CDOMConfig
{
public:
    inline                  ~CDOMConfig             ( void )                        { Clear (); };

    bool                    LoadFromFile            ( const char* szFilename );
    bool                    SaveToFile              ( const char* szFilename );
    void                    Clear                   ( void );

    SDOMConfigLine*         FindEntry               ( const char* szKey, unsigned int uiInstance );
    bool                    GetEntry                ( const char* szKey, char* szEntry, unsigned int uiBufferSize, unsigned int uiInstance );
    unsigned int            GetEntryCount           ( const char* szKey );

    void                    SetEntry                ( const char* szKey, const char* szEntry, unsigned int uiInstance );

    inline unsigned int     CountLines              ( void )                        { return static_cast < unsigned int > ( m_Lines.size () ); };

    inline std::list < SDOMConfigLine* > ::const_iterator
                            IterBegin               ( void )        { return m_Lines.begin (); };
    inline std::list < SDOMConfigLine* > ::const_iterator
                            IterEnd                 ( void )        { return m_Lines.end (); };

private:
    char*                   SkipWhitespace          ( char* szString );

    std::list < SDOMConfigLine* >  m_Lines;
};

#endif

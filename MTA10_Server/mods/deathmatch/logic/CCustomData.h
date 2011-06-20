/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomData.h
*  PURPOSE:     Custom entity data class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCUSTOMDATA_H
#define __CCUSTOMDATA_H

#include <core/CServerInterface.h>
#include "lua/CLuaArgument.h"
#include <map>
#include <string>

#define MAX_CUSTOMDATA_NAME_LENGTH 128

struct SCustomData
{
    CLuaArgument        Variable;
    class CLuaMain*     pLuaMain;
    bool                bSynchronized;
};

class CCustomData
{
public:

    void                    Copy                    ( CCustomData* pCustomData );

    SCustomData*            Get                     ( const char* szName );
    void                    Set                     ( const char* szName, const CLuaArgument& Variable, class CLuaMain* pLuaMain, bool bSynchronized = true );

    bool                    Delete                  ( const char* szName );
    void                    DeleteAll               ( class CLuaMain* pLuaMain );
    void                    DeleteAll               ( void );

    inline unsigned int     Count                   ( void )                           { return static_cast < unsigned int > ( m_Data.size () ); }
    unsigned short          CountOnlySynchronized   ( void );

    CXMLNode *              OutputToXML             ( CXMLNode * pNode );

    std::map < std::string, SCustomData > :: const_iterator IterBegin   ( void )   { return m_Data.begin (); }
    std::map < std::string, SCustomData > :: const_iterator IterEnd     ( void )   { return m_Data.end (); }

private:
    std::map < std::string, SCustomData >       m_Data;
};

#endif

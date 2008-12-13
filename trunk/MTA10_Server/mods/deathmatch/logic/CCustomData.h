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
#include <list>

#define MAX_CUSTOMDATA_ID_LENGTH 64
#define MAX_CUSTOMDATA_NAME_LENGTH 32

struct SCustomData
{
    char                szName [MAX_CUSTOMDATA_NAME_LENGTH + 1];
    CLuaArgument        Variable;
    class CLuaMain*     pLuaMain;
};

class CCustomData
{
public:
    inline                  ~CCustomData        ( void )                    { DeleteAll (); };

    void                    Copy                ( CCustomData* pCustomData );

    SCustomData*            Get                 ( const char* szName );
    void                    Set                 ( const char* szName, const CLuaArgument& Variable, class CLuaMain* pLuaMain );

    bool                    Delete              ( const char* szName );
    void                    DeleteAll           ( class CLuaMain* pLuaMain );
    void                    DeleteAll           ( void );

    CXMLNode *              OutputToXML         ( CXMLNode * pNode );

    inline unsigned int     Count               ( void )                    { return static_cast < unsigned int > ( m_List.size () ); }

    std::list < SCustomData* > ::const_iterator IterBegin    ( void )             { return m_List.begin (); }
    std::list < SCustomData* > ::const_iterator IterEnd      ( void )             { return m_List.end (); }

private:
    std::list < SCustomData* >   m_List;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CCustomData.h
*  PURPOSE:     Custom data storage class header
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCUSTOMDATA_H
#define __CCUSTOMDATA_H

#include "lua/CLuaArgument.h"
#include <list>

using namespace std;

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
                            ~CCustomData        ( void );

    void                    Copy                ( CCustomData* pCustomData );

    SCustomData*            Get                 ( const char* szName );
    void                    Set                 ( const char* szName, const CLuaArgument& Variable, class CLuaMain* pLuaMain );

    bool                    Delete              ( const char* szName );
    void                    DeleteAll           ( class CLuaMain* pLuaMain );
    void                    DeleteAll           ( void );

    inline unsigned int     Count               ( void )                    { return static_cast < unsigned int > ( m_List.size () ); }

    list < SCustomData* > ::const_iterator IterBegin    ( void )             { return m_List.begin (); }
    list < SCustomData* > ::const_iterator IterEnd      ( void )             { return m_List.end (); }

private:
    list < SCustomData* >   m_List;
};

#endif

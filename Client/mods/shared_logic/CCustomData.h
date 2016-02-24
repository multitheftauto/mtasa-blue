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

#define MAX_CUSTOMDATA_NAME_LENGTH 128

struct SCustomData
{
    CLuaArgument        Variable;
};

class CCustomData
{
public:

    void                    Copy                ( CCustomData* pCustomData );

    SCustomData*            Get                 ( const char* szName );
    void                    Set                 ( const char* szName, const CLuaArgument& Variable);

    bool                    Delete              ( const char* szName );

    std::map < std::string, SCustomData > :: const_iterator IterBegin   ( void )   { return m_Data.begin (); }
    std::map < std::string, SCustomData > :: const_iterator IterEnd     ( void )   { return m_Data.end (); }

private:
    std::map < std::string, SCustomData >       m_Data;
};

#endif

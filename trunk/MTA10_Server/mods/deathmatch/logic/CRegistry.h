/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRegistry.h
*  PURPOSE:     SQLite registry abstraction class
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct CRegistryResult;

#ifndef __CREGISTRY_H
#define __CREGISTRY_H

#include "CLogger.h"
#include "lua/CLuaArguments.h"
#include <list>
#include <sqlite3.h>

using namespace std;

class CRegistry
{
public:
                                CRegistry               ( std::string strFileName );
                                ~CRegistry              ( void );

    void						Load					( std::string strFileName );

    void						CreateTable				( std::string strTable, std::string strDefinition );
	void						DropTable				( std::string strTable );

    bool						Delete					( std::string strTable, std::string strWhere );
	bool						Insert					( std::string strTable, std::string strValues, std::string strColumns );
	bool						Select					( std::string strColumns, std::string strTable, std::string strWhere, unsigned int uiLimit, CRegistryResult* pResult );
	bool						Update					( std::string strTable, std::string strSet, std::string strWhere );

	bool						Query					( std::string strQuery, CLuaArguments *pArgs, CRegistryResult* pResult );

    std::string&                GetLastError            ( void ) { return m_strLastError; }

protected:

	sqlite3						*m_db;
	bool						m_bOpened;
	bool						m_bMutexLocked;

private:
    std::string                 m_strLastError;

};

struct CRegistryResult {
                                CRegistryResult ( void )
                                {
                                    pResult = NULL;
                                };
								~CRegistryResult ( void )
                                {
                                    if ( pResult ) sqlite3_free_table ( pResult );
                                };
	char						**pResult;
	int							nRows;
	int							nColumns;
};

#endif

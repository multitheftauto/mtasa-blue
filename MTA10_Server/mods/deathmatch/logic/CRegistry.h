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
#include <vector>
#include <string>
#include <sqlite3.h>

class CRegistry
{
public:
                                CRegistry               ( const std::string& strFileName );
                                ~CRegistry              ( void );

    void                        Load                    ( const std::string& strFileName );

    void                        CreateTable             ( const std::string& strTable, const std::string& strDefinition );
    void                        DropTable               ( const std::string& strTable );

    bool                        Delete                  ( const std::string& strTable, const std::string& strWhere );
    bool                        Insert                  ( const std::string& strTable, const std::string& strValues, const std::string& strColumns );
    bool                        Select                  ( const std::string& strColumns, const std::string& strTable, const std::string& strWhere, unsigned int uiLimit, CRegistryResult* pResult );
    bool                        Update                  ( const std::string& strTable, const std::string& strSet, const std::string& strWhere );

    bool                        Query                   ( const std::string& strQuery, CLuaArguments *pArgs, CRegistryResult* pResult );

    const std::string&          GetLastError            ( void ) { return m_strLastError; }

protected:

    bool                        QueryInternal           ( const char* szQuery, CRegistryResult* pResult );

    sqlite3                     *m_db;
    bool                        m_bOpened;
    bool                        m_bMutexLocked;

private:
    std::string                 m_strLastError;

};

struct CRegistryResultCell
{
                                CRegistryResultCell ( void )
                                {
                                    nType = SQLITE_NULL;
                                    nLength = 0;
                                    pVal = NULL;
                                }
                                CRegistryResultCell ( const CRegistryResultCell& cell )
                                {
                                    nType = cell.nType;
                                    nLength = cell.nLength;
                                    nVal = cell.nVal;
                                    fVal = cell.fVal;
                                    pVal = NULL;
                                    if ( (nType == SQLITE_BLOB || nType == SQLITE_TEXT) && cell.pVal && nLength > 0 )
                                    {
                                        pVal = new unsigned char [ nLength ];
                                        memcpy ( pVal, cell.pVal, nLength );
                                    }
                                };
                                ~CRegistryResultCell ( void )
                                {
                                    if ( pVal )
                                        delete pVal;
                                }

    int                         nType;      // Type identifier, SQLITE_*
    int                         nLength;    // Length in bytes if nType == SQLITE_BLOB or SQLITE_TEXT
                                            //    (includes zero terminator if TEXT)
    int                         nVal;
    float                       fVal;
    unsigned char*              pVal;
};

struct CRegistryResult
{
                                             CRegistryResult ( void )
                                             {
                                                nRows = 0;
                                                nColumns = 0;
                                             }
    vector < string >                        ColNames;
    vector < vector<CRegistryResultCell> >   Data;
    int                                      nRows;
    int                                      nColumns;
};

#endif

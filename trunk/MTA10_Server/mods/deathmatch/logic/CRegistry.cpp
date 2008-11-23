/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRegistry.cpp
*  PURPOSE:     SQLite registry abstraction class
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CRegistry::CRegistry ( std::string strFileName )
{
    m_strLastError = "";

	m_bMutexLocked = false;

    Load ( strFileName );
}


CRegistry::~CRegistry ( void )
{
	CLogger::LogPrint ( "Closing SQLite3 database\n" );
	if ( m_bOpened )
		sqlite3_close ( m_db );
}


void CRegistry::Load ( std::string strFileName )
{
	m_bOpened = false;

	if ( !strFileName.empty () ) {
		if ( sqlite3_open ( strFileName.c_str (), &m_db ) )
		{
			CLogger::ErrorPrintf ( "Could not open SQLite3 database! (%s)\n", sqlite3_errmsg ( m_db ) );
		} else {
			m_bOpened = true;
		}
	}
}


bool CRegistry::Update ( std::string strTable, std::string strSet, std::string strWhere )
{
	char *szErrorMsg = NULL;
    std::string strQuery;

	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot get value!";
		return false;
	}

    if ( strSet.empty () || strTable.empty () ) {
        m_strLastError = "Invalid arguments!";
        return false;
    }

    strQuery = "UPDATE " + strTable + " SET " + strSet;
    if ( !strWhere.empty () )
        strQuery += " WHERE " + strWhere;

	// Execute SQL
	if ( sqlite3_exec ( m_db, strQuery.c_str (), NULL, NULL, &szErrorMsg ) != SQLITE_OK )
    {
        m_strLastError = std::string ( szErrorMsg );
        sqlite3_free ( szErrorMsg );
		return false;
    }
    return true;
}


void CRegistry::CreateTable ( std::string strTable, std::string strDefinition )
{
	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot create table!";
		return;
	}

    std::string strQuery = "CREATE TABLE IF NOT EXISTS " + strTable + " ( " + strDefinition + " )";

	CLogger::LogPrintf ( "Creating new DB table %s\n", strTable.c_str () );
	sqlite3_exec ( m_db, strQuery.c_str (), NULL, NULL, NULL );
}


void CRegistry::DropTable ( std::string strTable )
{
	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot drop table!";
		return;
	}

    std::string strQuery = "DROP IF EXISTS " + strTable;

	CLogger::LogPrintf ( "Dropping DB table %s\n", strTable.c_str () );
	sqlite3_exec ( m_db, strQuery.c_str (), NULL, NULL, NULL );
}


bool CRegistry::Insert ( std::string strTable, std::string strValues, std::string strColumns )
{
	char *szErrorMsg = NULL;
    std::string strQuery;

	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot add value!";
		return false;
	}

	if ( strColumns.empty () ) {
        strQuery = "INSERT INTO " + strTable + " VALUES ( " + strValues + " )";
	} else {
        strQuery = "INSERT INTO " + strTable + " ( " + strColumns + " ) VALUES ( " + strValues + " )";
	}

	if ( sqlite3_exec ( m_db, strQuery.c_str (), NULL, NULL, &szErrorMsg ) != SQLITE_OK ) {
        m_strLastError = std::string ( szErrorMsg );
        sqlite3_free ( szErrorMsg );
		return false;
	}
	return true;
}


bool CRegistry::Delete ( std::string strTable, std::string strWhere )
{
	char *szErrorMsg = NULL;

	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot delete table!";
		return false;
	}

    std::string strQuery = "DELETE FROM " + strTable + " WHERE " + strWhere;

	if ( sqlite3_exec ( m_db, strQuery.c_str (), NULL, NULL, &szErrorMsg ) != SQLITE_OK ) {
        m_strLastError = std::string ( szErrorMsg );
        sqlite3_free ( szErrorMsg );
		return false;
	}
	return true;
}


bool CRegistry::Query ( std::string strQuery, CLuaArguments *pArgs, CRegistryResult* pResult )
{
	char *szErrorMsg = NULL;
    std::string strParsedQuery = "";

	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot perform query!";
		return false;
	}

	// Walk through the query and replace the variable placeholders with the actual variables
	unsigned int uiLen = strQuery.length ();
	unsigned int j = 0, a = 0, type = 0;
	const char *szContent = NULL;
	char szBuffer[32] = {0};
	for ( unsigned int i = 0; i < uiLen; i++ )
	{
		if ( strQuery.at(i) == SQL_VARIABLE_PLACEHOLDER ) {
			// If the placeholder is found, replace it with the variable
			CLuaArgument *pArgument = (*pArgs)[a++];
			
			// Check the type of the argument and convert it to a string we can process
			if ( pArgument ) {
				type = pArgument->GetType ();
                if ( type == LUA_TBOOLEAN ) {
                    szContent = ( pArgument->GetBoolean() ) ? "true" : "false";
                } else if ( type == LUA_TNUMBER ) {
					_snprintf ( szBuffer, 31, "%f", pArgument->GetNumber () );
					szContent = szBuffer;
                } else if ( type == LUA_TSTRING ) {
					szContent = pArgument->GetString ().c_str ();

					// If we have a string, add a quote at the beginning too
					strParsedQuery += '\'';
				}
			}

			// Copy the string into the query, and escape the quotes and slashes as well
			if ( szContent ) {
				for ( unsigned int k = 0; k < strlen ( szContent ); k++ ) {
					if ( szContent[k] == '\'' || szContent[k] == '\\' )
                        strParsedQuery += '\\';
                    strParsedQuery += szContent[k];
				}
				// If we have a string, add a quote at the end too
				if ( type == LUA_TSTRING ) strParsedQuery += '\'';
			} else {
				// If we don't have any content, put just output 2 quotes to indicate an empty variable
                strParsedQuery += "\'\'";
			}

		} else {
			// If we found a normal character, copy it into the destination buffer
			strParsedQuery += strQuery[i];
		}
	}

	// Execute the query and get the result table
    if ( sqlite3_get_table ( m_db, strParsedQuery.c_str (), &pResult->pResult, &pResult->nRows, &pResult->nColumns, &szErrorMsg ) != SQLITE_OK ) {
        m_strLastError = std::string ( szErrorMsg );
        sqlite3_free ( szErrorMsg );
		return false;
	}
	return true;
}


bool CRegistry::Select ( std::string strColumns, std::string strTable, std::string strWhere, unsigned int uiLimit, CRegistryResult* pResult )
{
	char *szErrorMsg = NULL;
    char szBuffer[32] = {0};

	if ( m_bOpened == false ) {
		m_strLastError = "SQLite3 was not opened, cannot get value!";
		return false;
	}

    std::string strQuery = "SELECT " + strColumns + " FROM " + strTable;
    if ( !strWhere.empty () )
        strQuery += " WHERE " + strWhere;
    if ( uiLimit > 0 )
        strQuery += " LIMIT " + std::string ( itoa ( uiLimit, szBuffer, 10 ) );

	// Execute the query and get the result table
    if ( sqlite3_get_table ( m_db, strQuery.c_str (), &pResult->pResult, &pResult->nRows, &pResult->nColumns, &szErrorMsg ) != SQLITE_OK ) {
		m_strLastError = std::string ( szErrorMsg );
        sqlite3_free ( szErrorMsg );
		return false;
	}
	return true;
}

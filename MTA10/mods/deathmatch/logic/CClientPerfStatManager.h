/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientPerfStatManager.h
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTPERFSTATMANAGER_H
#define __CCLIENTPERFSTATMANAGER_H

typedef unsigned long TIMEUS;
TIMEUS GetTimeUs ( void );

//
// CClientPerfStatResult
//
// Result of GetStats
//
class CClientPerfStatResult
{
    std::vector < SString > colNames;
    std::vector < SString > cellList;
    int iNumColumns;
    int iNumRows;
public:

    CClientPerfStatResult ()
    {
        iNumColumns = 0;
        iNumRows = 0;
    }

    const SString& ColumnName( unsigned long c ) const
    {
        unsigned long idx = c;
        if ( idx < colNames.size () )
            return colNames[idx];
        static SString dummy;
        return dummy;
    }

    int ColumnCount() const
    {
        return iNumColumns;
    }

    int RowCount() const
    {
        return iNumRows;
    }

    void AddColumn ( const SString& strColumnName )
    {
        colNames.push_back ( strColumnName );
        iNumColumns++;
    }

    SString* AddRow( void )
    {
        iNumRows++;
        cellList.insert( cellList.end (), ColumnCount(), SString() );
        return &cellList[ cellList.size () - ColumnCount() ];
    }

    SString& Data( unsigned long c, unsigned long r )
    {
        unsigned long idx = c + r * ColumnCount();
        if ( idx < cellList.size () )
            return cellList[idx];
        static SString cellDummy;
        return cellDummy;
    }

    void Clear ()
    {
        colNames.clear ();
        cellList.clear ();
        iNumColumns = 0;
        iNumRows = 0;
    }
};


//
// CClientPerfStatManager
//
class CClientPerfStatManager
{
public:
    virtual             ~CClientPerfStatManager   ( void ) {}

    virtual void        DoPulse             ( void ) = 0;
    virtual void        OnLuaMainCreate     ( CLuaMain* pLuaMain ) = 0;
    virtual void        OnLuaMainDestroy    ( CLuaMain* pLuaMain ) = 0;
    virtual void        GetStats            ( CClientPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter ) = 0;
    virtual void        UpdateLuaMemory     ( CLuaMain* pLuaMain, int iMemUsed ) = 0;
    virtual void        UpdateLuaTiming     ( CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs ) = 0;
    virtual void        UpdateLibMemory     ( const SString& strLibName, int iMemUsed, int iMemUsedMax ) = 0;
};

CClientPerfStatManager* GetClientPerfStatManager ();


#endif

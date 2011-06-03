/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerfStatManager.cpp
*  PURPOSE:     Performance stats manager class
*  DEVELOPERS:  Mr OCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl
//
//
///////////////////////////////////////////////////////////////
class CPerfStatManagerImpl : public CPerfStatManager
{
public:
                                CPerfStatManagerImpl       ( void );
    virtual                     ~CPerfStatManagerImpl      ( void );

    // CPerfStatManager
    virtual void                DoPulse                     ( void );
    virtual void                GetStats                    ( CPerfStatResult* pOutResult, const SString& strCategory, const SString& strOptions, const SString& strFilter );

    // CPerfStatManagerImpl
    void                        AddModule                   ( CPerfStatModule* pModule );
    void                        RemoveModule                ( CPerfStatModule* pModule );
    uint                        GetModuleCount              ( void );
    CPerfStatModule*            GetModuleByIndex            ( uint uiIndex );
    CPerfStatModule*            GetModuleByCategoryName     ( const SString& strCategory );

    std::vector < CPerfStatModule* >    m_ModuleList;
};


///////////////////////////////////////////////////////////////
//
// Temporary home for global object
//
//
//
///////////////////////////////////////////////////////////////
static CPerfStatManagerImpl* g_pPerfStatManagerImp = NULL;

CPerfStatManager* CPerfStatManager::GetSingleton ( void )
{
    if ( !g_pPerfStatManagerImp )
        g_pPerfStatManagerImp = new CPerfStatManagerImpl ();
    return g_pPerfStatManagerImp;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::CPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatManagerImpl::CPerfStatManagerImpl ( void )
{
    AddModule ( CPerfStatLuaTiming::GetSingleton () );
    AddModule ( CPerfStatLuaMemory::GetSingleton () );
    AddModule ( CPerfStatLibMemory::GetSingleton () );
    AddModule ( CPerfStatPacketUsage::GetSingleton () );
    AddModule ( CPerfStatSqliteTiming::GetSingleton () );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::CPerfStatManagerImpl
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatManagerImpl::~CPerfStatManagerImpl ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::AddModule
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::AddModule ( CPerfStatModule* pModule )
{
    if ( !ListContains ( m_ModuleList, pModule ) )
    {
        m_ModuleList.push_back ( pModule );
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::RemoveModule
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::RemoveModule ( CPerfStatModule* pModule )
{
    ListRemove ( m_ModuleList, pModule );
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetModuleCount
//
//
//
///////////////////////////////////////////////////////////////
uint CPerfStatManagerImpl::GetModuleCount ( void )
{
    return m_ModuleList.size ();
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetModuleByIndex
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatModule* CPerfStatManagerImpl::GetModuleByIndex ( uint uiIndex )
{
    if ( uiIndex < m_ModuleList.size () )
        return m_ModuleList[ uiIndex ];
    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetModuleByCategoryName
//
//
//
///////////////////////////////////////////////////////////////
CPerfStatModule* CPerfStatManagerImpl::GetModuleByCategoryName ( const SString& strCategory )
{
    for ( uint i = 0 ; i < GetModuleCount () ; i++ )
    {
        CPerfStatModule* pModule = GetModuleByIndex ( i );
        if ( pModule->GetCategoryName () == strCategory )
            return pModule;
    }
    return NULL;
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::DoPulse ( void )
{
    for ( uint i = 0 ; i < GetModuleCount () ; i++ )
    {
        CPerfStatModule* pModule = GetModuleByIndex ( i );
        pModule->DoPulse ();
    }
}


///////////////////////////////////////////////////////////////
//
// CPerfStatManagerImpl::GetStats
//
//
//
///////////////////////////////////////////////////////////////
void CPerfStatManagerImpl::GetStats ( CPerfStatResult* pResult, const SString& strCategory, const SString& strOptions, const SString& strFilter )
{
    pResult->Clear ();

    if ( strCategory == "" )
    {
        // List all modules
        pResult->AddColumn ( "Categories" );
        for ( uint i = 0 ; i < GetModuleCount () ; i++ )
        {
            CPerfStatModule* pModule = GetModuleByIndex ( i );
            pResult->AddRow ()[0] = pModule->GetCategoryName ();
        }
        pResult->AddRow ()[0] ="Help";
        return;
    }

    // Handle help
    if ( strCategory == "Help" )
    {
        pResult->AddColumn ( "Help" );
        pResult->AddRow ()[0] ="Comma separate multiple options";
        pResult->AddRow ()[0] ="Type h in options and select a category to see help for that category";
        return;
    }

    // Put options in a map
    std::map < SString, int > strOptionMap;
    {
        std::vector < SString > strParts;
        strOptions.Split ( ",", strParts );
        for ( unsigned int i = 0 ; i < strParts.size (); i++ )
            MapSet ( strOptionMap, strParts[i], 1 );
    }

    // Find module
    CPerfStatModule* pModule = GetModuleByCategoryName ( strCategory );
    if ( !pModule )
    {
        pResult->AddColumn ( "Error" );
        pResult->AddRow ()[0] = "Error: Unknown category '" + strCategory + "'";
        return;
    }

    // Get stats from module
    pModule->GetStats ( pResult, strOptionMap, strFilter );
}

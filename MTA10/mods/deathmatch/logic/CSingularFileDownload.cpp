/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSingularFileDownloadManager.cpp
*  PURPOSE:     Singular file download manager interface
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CSingularFileDownload::CSingularFileDownload ( CResource* pResource, const char *szName, const char *szNameShort, SString strHTTPURL, CChecksum checksum )
{
    // Store the name
    m_strName = szName;

    // Store the name (short)
    m_strNameShort = szNameShort;

    // store the resource
    m_pResource = pResource;

    // Store the provided checksum
    m_ProvidedChecksum = checksum;

    GenerateClientChecksum();

    if ( !DoesClientAndServerChecksumMatch () )
    {
        CNetHTTPDownloadManagerInterface* pHTTP = g_pCore->GetNetwork ()->GetHTTPDownloadManager ( EDownloadMode::RESOURCE_SINGULAR_FILES );
        pHTTP->QueueFile ( strHTTPURL.c_str(), szName, 0, NULL, 0, false, this, ProgressCallBack, false, 10, 10000, true );
        m_bComplete = false;
        g_pClientGame->SetTransferringSingularFiles ( true );
    }
    else
    {
        CallFinished ( true );
    }

}


CSingularFileDownload::~CSingularFileDownload ( void )
{
}


bool CSingularFileDownload::ProgressCallBack ( double sizeJustDownloaded, double totalDownloaded, char *data, size_t dataLength, void *obj, bool complete, int error )
{
    if ( complete )
    {
        CSingularFileDownload * pFile = (CSingularFileDownload*)obj;
        pFile->CallFinished ( true );
    }
    else if ( error )
    {
        CSingularFileDownload * pFile = (CSingularFileDownload*)obj;
        pFile->CallFinished ( false );
    }
    return true;
}


void CSingularFileDownload::CallFinished ( bool bSuccess )
{
    if ( m_pResource )
    {
        // Call the onClientbFileDownloadComplete event
        CLuaArguments Arguments;
        Arguments.PushString ( GetShortName() );        // file name
        Arguments.PushBoolean ( bSuccess );     // Completed successfully?
        m_pResource->GetResourceEntity()->CallEvent ( "onClientFileDownloadComplete", Arguments, false );
    }
    SetComplete();
}


bool CSingularFileDownload::DoesClientAndServerChecksumMatch ( void )
{
    return ( m_LastClientChecksum.CompareWithLegacy ( m_ProvidedChecksum ) );
}

CChecksum CSingularFileDownload::GenerateClientChecksum ( void )
{
    m_LastClientChecksum = CChecksum::GenerateChecksumFromFile ( m_strName );
    return m_LastClientChecksum;
}

CChecksum CSingularFileDownload::GetLastClientChecksum ( void )
{
    return m_LastClientChecksum;
}

CChecksum CSingularFileDownload::GetProvidedChecksum ( void )
{
    return m_ProvidedChecksum;
}

void CSingularFileDownload::SetProvidedChecksum ( CChecksum providedChecksum )
{
    m_ProvidedChecksum = providedChecksum;
}

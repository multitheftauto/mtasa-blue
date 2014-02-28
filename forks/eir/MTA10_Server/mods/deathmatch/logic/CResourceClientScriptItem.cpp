/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceClientScriptItem.cpp
*  PURPOSE:     Resource client-side script item class
*  DEVELOPERS:  Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single resource script item, being a .lua file
// It's task is to load and unload the script client side

#include "StdInc.h"

extern CGame* g_pGame;

CResourceClientScriptItem::CResourceClientScriptItem ( CResource * resource, const char * szShortName, const char * szResourceFileName, CXMLAttributes * xmlAttributes )
: CResourceFile ( resource, szShortName, szResourceFileName, xmlAttributes )
{
    m_type = RESOURCE_FILE_TYPE_CLIENT_SCRIPT;

    // Check if this file should be cached by the client
    if ( MapGet(m_attributeMap, "protected") == "true" ||
         MapGet(m_attributeMap, "cache") == "false" )
        m_bIsNoClientCache = true;
    else
        m_bIsNoClientCache = false;
}

CResourceClientScriptItem::~CResourceClientScriptItem ( void )
{

}

ResponseCode CResourceClientScriptItem::Request ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
    if ( IsNoClientCache() == true )
    {
        const char* errmsg = "This script is not client cacheable";
        ipoHttpResponse->SetBody ( errmsg, strlen(errmsg) );
        return HTTPRESPONSECODE_403_FORBIDDEN;
    }
    else
        return CResourceFile::Request ( ipoHttpRequest, ipoHttpResponse );
}

bool CResourceClientScriptItem::Start ( void )
{
    // Pre-load the script
    if ( IsNoClientCache() == true )
    {
        // HACK - Use http-client-files if possible as the resources directory may have been changed since the resource was loaded.
        SString strDstFilePath = GetCachedPathFilename ();
        if ( !FileLoad( strDstFilePath, m_sourceCode ) )
            return false;

        // Compress the source
        unsigned int originalLength = m_sourceCode.length ();
        unsigned long bufferLength = m_sourceCode.length() + 12 + (unsigned int)(m_sourceCode.length() * 0.001f); // Refer to the compress2() function documentation.
        char* compressedBuffer = new char [ bufferLength ];
        if ( compress2 ( (Bytef*)compressedBuffer, (uLongf *)&bufferLength, (const Bytef *)m_sourceCode.c_str(), m_sourceCode.length(), Z_BEST_COMPRESSION ) != Z_OK )
        {
            g_pGame->GetScriptDebugging ()->LogWarning ( 0, "Failed to compress the client-side script '%s' of resource '%s'\n", GetName(), m_resource->GetName ().c_str() );
            return false;
        }
        char lengthData [ 4 ];
        lengthData[0] = ( originalLength >> 24 ) & 0xFF;
        lengthData[1] = ( originalLength >> 16 ) & 0xFF;
        lengthData[2] = ( originalLength >> 8  ) & 0xFF;
        lengthData[3] =   originalLength         & 0xFF;
        m_sourceCode.assign ( lengthData, 4 );
        m_sourceCode.append ( compressedBuffer, bufferLength );
        delete [] compressedBuffer;
    }

    return true;
}

bool CResourceClientScriptItem::Stop ( void )
{
    return true;
}

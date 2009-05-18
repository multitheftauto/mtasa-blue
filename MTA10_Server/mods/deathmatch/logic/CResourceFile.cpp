/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceFile.cpp
*  PURPOSE:     Resource server-side file item class
*  DEVELOPERS:  Ed Lyons <>
*               Jax <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class controls a single resource file. This could be
// any item contained within the resource, mainly being a
// map or script.

#include "StdInc.h"

CResourceFile::CResourceFile ( CResource * resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes * xmlAttributes ) 
{ 
    m_strResourceFileName = szResourceFileName; 

    // Stupid hack to automatically change all forward slashes to back slashes to get around internal http sub dir issue
    m_strShortName = szShortName;
    for ( size_t i = 0; i < m_strShortName.size (); i++ )
    {
        if ( m_strShortName[ i ] == '\\' )
        {
            m_strShortName[ i ] = '/';
        }
    }

    m_strWindowsName = m_strShortName;
    for ( size_t i = 0; i < m_strWindowsName.size (); i++ )
    {
        if ( m_strWindowsName[ i ] == '/' )
        {
            m_strWindowsName[ i ] = '\\';
        }
    }


    m_resource = resource;
    m_pVM = NULL;
    m_ulCRC = 0;

    // Create a map of the attributes for later use
    if ( xmlAttributes )
	    for ( list < CXMLAttribute * > ::iterator iter = xmlAttributes->ListBegin () ; iter != xmlAttributes->ListEnd () ; iter++ )
            m_attributeMap[ ( *iter )->GetName () ] = ( *iter )->GetValue ();
}

CResourceFile::~CResourceFile ( void )
{
    
}


double CResourceFile::GetSize ( void )
{
    double dSize = 0.0;

    FILE * file = fopen ( m_strResourceFileName.c_str (), "rb" );
    if ( file )
    {
        fseek ( file, 0, SEEK_END );
        dSize = ftell ( file );
        fclose ( file );
    }

    return dSize;
}


ResponseCode CResourceFile::Request ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
    // its a raw page
    FILE * file = fopen ( m_strResourceFileName.c_str (), "rb" );
    if ( file )
    {
        // Grab the filesize. Don't use the above method because it doesn't account for a changing
        // filesize incase of for example an included resource (causing bug #2676)
        fseek ( file, 0, SEEK_END );
        long lBufferLength = ftell ( file );
        rewind ( file );

        // Allocate and read the entire file
        // TODO: This is inefficient.
        char * szBuffer = new char [ lBufferLength + 1 ];
        fread ( szBuffer, 1, lBufferLength, file );
        fclose ( file );

        // 
        ipoHttpResponse->oResponseHeaders [ "content-type" ] = "application/octet-stream"; // not really the right mime-type
        ipoHttpResponse->SetBody ( szBuffer, lBufferLength );
        delete[] szBuffer;
        return HTTPRESPONSECODE_200_OK;
    }
    else
    {
        ipoHttpResponse->SetBody ( "Can't read file!", strlen("Can't read file!") );
        return HTTPRESPONSECODE_500_INTERNALSERVERERROR;
    }
}


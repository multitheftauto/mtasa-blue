/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceFile.cpp
 *  PURPOSE:     Resource server-side file item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

// This class controls a single resource file. This could be
// any item contained within the resource, mainly being a
// map or script.

#include "StdInc.h"
#include "CResourceFile.h"
#include "CResource.h"
#include <core/CServerInterface.h>

extern CServerInterface* g_pServerInterface;

CResourceFile::CResourceFile(CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes)
{
    m_strResourceFileName = szResourceFileName;

    // Stupid hack to automatically change all forward slashes to back slashes to get around internal http sub dir issue
    m_strShortName = szShortName;
    for (size_t i = 0; i < m_strShortName.size(); i++)
    {
        if (m_strShortName[i] == '\\')
        {
            m_strShortName[i] = '/';
        }
    }

    m_strWindowsName = m_strShortName;
    for (size_t i = 0; i < m_strWindowsName.size(); i++)
    {
        if (m_strWindowsName[i] == '/')
        {
            m_strWindowsName[i] = '\\';
        }
    }

    m_resource = resource;
    m_pVM = NULL;

    // Create a map of the attributes for later use
    if (xmlAttributes)
        for (list<CXMLAttribute*>::iterator iter = xmlAttributes->ListBegin(); iter != xmlAttributes->ListEnd(); iter++)
            m_attributeMap[(*iter)->GetName()] = (*iter)->GetValue();
}

CResourceFile::~CResourceFile()
{
}

HttpStatusCode CResourceFile::Request(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse)
{
    // HACK - Use http-client-files if possible as the resources directory may have been changed since the resource was loaded.
    SString strDstFilePath = GetCachedPathFilename();

    FILE* file = File::Fopen(strDstFilePath.c_str(), "rb");
    if (!file)
        file = File::Fopen(m_strResourceFileName.c_str(), "rb");

    // its a raw page
    if (file)
    {
        // Grab the filesize. Don't use the above method because it doesn't account for a changing
        // filesize incase of for example an included resource (causing bug #2676)
        fseek(file, 0, SEEK_END);
        long lBufferLength = ftell(file);
        rewind(file);

        if (lBufferLength < 0)
        {
            fclose(file);
            ipoHttpResponse->SetBody("Failed to determine file size", strlen("Failed to determine file size"));
            return HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
        }

        // Allocate and read the entire file
        // TODO: This is inefficient.
        char* szBuffer = nullptr;
        try
        {
            szBuffer = new char[lBufferLength + 1];
            size_t bytesRead = fread(szBuffer, 1, lBufferLength, file);
            fclose(file);
            file = nullptr;

            ipoHttpResponse->oResponseHeaders["content-type"] = "application/octet-stream";
            ipoHttpResponse->SetBody(szBuffer, static_cast<int>(bytesRead));
            delete[] szBuffer;
            return HTTP_STATUS_CODE_200_OK;
        }
        catch (const std::bad_alloc&)
        {
            delete[] szBuffer;
            if (file)
                fclose(file);

            ipoHttpResponse->SetBody("Server out of memory", strlen("Server out of memory"));
            return HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
        }
    }
    else
    {
        ipoHttpResponse->SetBody("Can't read file!", strlen("Can't read file!"));
        return HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR;
    }
}

SString CResourceFile::GetCachedPathFilename(bool bForceClientCachePath)
{
    if (IsNoClientCache() == false || bForceClientCachePath)
        return PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "http-client-files", m_resource->GetName(), GetName());
    else
        return PathJoin(g_pServerInterface->GetServerModPath(), "resource-cache", "http-client-files-no-client-cache", m_resource->GetName(), GetName());
}

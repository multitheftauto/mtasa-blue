/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CResourceClientScriptItem.h
 *  PURPOSE:     Resource client-side script item class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CResourceFile.h"

class CResourceClientScriptItem : public CResourceFile
{
public:
    CResourceClientScriptItem(class CResource* resource, const char* szShortName, const char* szResourceFileName, CXMLAttributes* xmlAttributes);
    ~CResourceClientScriptItem();

    bool Start();
    bool Stop();

    bool           IsNoClientCache() const { return m_bIsNoClientCache; }
    const SString& GetSourceCode() const { return m_sourceCode; }

    HttpStatusCode Request(HttpRequest* ipoHttpRequest, HttpResponse* ipoHttpResponse);

private:
    bool    m_bIsNoClientCache;
    SString m_sourceCode;
};

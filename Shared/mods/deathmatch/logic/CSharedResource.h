/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CSharedResource.h
 *  PURPOSE:     Shared resource class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define MAX_RESOURCE_NAME_LENGTH 255

class CSharedResource
{
public:
    CSharedResource(const char* szResourceName);

    const SString& GetName() const noexcept { return m_strResourceName; }

protected:
    SString m_strResourceName;
};

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

#include "StdInc.h"

#include "CSharedResource.h"

CSharedResource::CSharedResource(const char* szResourceName)
{
    if (szResourceName)
        m_strResourceName.AssignLeft(szResourceName, MAX_RESOURCE_NAME_LENGTH);
}

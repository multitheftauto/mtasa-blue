/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.h
*  PURPOSE:     Header for resource file
*  DEVELOPERS:  Chris McArthur <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCEFILE_H
#define CRESOURCEFILE_H

#include "CDownloadableResource.h"

class CResourceFile :
    public CDownloadableResource
{
public:

    CResourceFile ( eResourceType resourceType, const char * szShortName, const char * szResourceFileName, CChecksum serverChecksum ) :
        CDownloadableResource ( resourceType, szResourceFileName, szShortName, serverChecksum, true )
    {
    };
};

#endif
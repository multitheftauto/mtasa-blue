/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceConfigItem.h
*  PURPOSE:     Header for resource config item class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCECONFIGITEM_H
#define CRESOURCECONFIGITEM_H

#include "CResource.h"
#include "CDownloadableResource.h"
#include <list>

#ifndef MAX_PATH
#define MAX_PATH        260
#endif

class CResourceConfigItem :
    public CDownloadableResource
{
public:
                                        CResourceConfigItem       ( class CResource * resource, const char * szShortName, const char * szResourceFileName, CChecksum serverChecksum );
                                        ~CResourceConfigItem      ( void );

    bool                                Start                           ( void );
    bool                                Stop                            ( void );
    inline class CXMLFile *             GetFile                         ( void ) { return m_pXMLFile; }
    inline class CXMLNode *             GetRoot                         ( void ) { return m_pXMLRootNode; }
    
private:
    class CXMLFile*                     m_pXMLFile;
    CXMLNode*                           m_pXMLRootNode;
    CResource*                          m_pResource;
};

#endif
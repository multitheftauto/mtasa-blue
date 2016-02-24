/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.h
*  PURPOSE:     Header for resource manager class
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CResourceManager;

#ifndef __CRESOURCEMANAGER_H
#define __CRESOURCEMANAGER_H

#include <list>

class CClientEntity;
class CResource;
class CDownloadableResource;

enum eAccessType
{
    ACCESS_PUBLIC,
    ACCESS_PRIVATE,
};

class CResourceManager
{  

public:
                                  CResourceManager            ( void );
                                  ~CResourceManager           ( void );

    CResource*                    Add                         ( unsigned short usNetID, const char* szResourceName, CClientEntity* pResourceEntity, CClientEntity* pResourceDynamicEntity, const SString& strMinServerReq, const SString& strMinClientReq, bool bEnableOOP );
    CResource*                    GetResource                 ( const char* szResourceName );
    CResource*                    GetResourceFromNetID        ( unsigned short usNetID );
    CResource*                    GetResourceFromScriptID     ( uint uiScriptID );
    CResource*                    GetResourceFromLuaState     ( struct lua_State* luaVM );
    SString                       GetResourceName             ( struct lua_State* luaVM );
    
    bool                          RemoveResource              ( unsigned short usID );
    void                          Remove                      ( CResource* pResource );
    bool                          Exists                      ( CResource* pResource );
    void                          StopAll                     ( void );

    void                          OnDownloadGroupFinished     ( void );

    void                          OnAddResourceFile           ( CDownloadableResource* pResourceFile );
    void                          OnRemoveResourceFile        ( CDownloadableResource* pResourceFile );
    void                          OnDownloadedResourceFile    ( const SString& strFilename );
    bool                          IsResourceFile              ( const SString& strFilename );
    void                          OnFileModifedByScript       ( const SString& strFilename, const SString& strReason );
    void                          ValidateResourceFile        ( const SString& strFilename, const CBuffer& fileData );
    inline CDownloadableResource* GetDownloadableResourceFile ( const SString& strFilename ) { return MapFindRef ( m_ResourceFileMap, strFilename ); }

    static bool                   ParseResourcePathInput      ( std::string strInput, CResource* &pResource, std::string &strPath, std::string &strMetaPath );
    static bool                   ParseResourcePathInput      ( std::string strInput, CResource* &pResource, std::string &strPath );

private:

    CMappedList < CResource* >          m_resources;
    std::map < ushort, CResource* >     m_NetIdResourceMap;
    std::map < SString, CDownloadableResource* > m_ResourceFileMap;
};

#endif

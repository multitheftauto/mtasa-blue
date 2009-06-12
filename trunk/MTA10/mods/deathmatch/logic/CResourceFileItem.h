/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceConfigItem.h
*  PURPOSE:     Header for resource config item class
*  DEVELOPERS:  Talidan
*
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#ifndef CRESOURCEFILEITEM_H
#define CRESOURCEFILEITEM_H


#include "CResource.h"

class CResourceFileItem
{
public:
                                        CResourceFileItem       ( class CResource * resource, std::string strFilePath );
    bool                                Exists                  ( void );

    std::string                         GetFilePath                     ( void ) { return m_strFilePath; }
    CResource*                          GetResource                     ( void ) { return m_pResource; }
    
private:
    std::string                         m_strFilePath;
    CResource*                          m_pResource;
};

#endif
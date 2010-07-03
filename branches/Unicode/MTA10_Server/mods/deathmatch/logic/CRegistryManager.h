/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRegistryManager.h
*  PURPOSE:     Track registry objects
*  DEVELOPERS:  Mr Knobbly
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CREGISTRYMANAGER_H
#define __CREGISTRYMANAGER_H

class CRegistry;

class CRegistryManager
{
public:
                                ~CRegistryManager       ( void );
    void                        DoPulse                 ( void );
    CRegistry*                  OpenRegistry            ( const std::string& strFileName );
    bool                        CloseRegistry           ( CRegistry* pRegistry );

private:
    std::vector < CRegistry* >  m_RegistryList;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CModManager.h
*  PURPOSE:     Server mod manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMODMANAGER_H
#define __CMODMANAGER_H

class CModManager
{
public:
    virtual bool            RequestLoad         ( const char* szModName ) = 0;

    virtual const char*     GetModPath          ( void ) = 0;
    virtual SString         GetAbsolutePath     ( const char* szRelative ) = 0;
};

#endif

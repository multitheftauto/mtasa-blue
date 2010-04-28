/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CCommunityInterface.h
*  PURPOSE:     Multi Theft Auto Community connector interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOMMUNITYINTERFACE_H
#define __CCOMMUNITYINTERFACE_H

#include <string>

class CCommunityInterface
{
public:
    virtual bool                    IsLoggedIn      ( void ) = 0;

    virtual void                    GetUsername     ( std::string &strUsername ) = 0;
    virtual void                    GetPassword     ( std::string &strPassword ) = 0;
};

#endif

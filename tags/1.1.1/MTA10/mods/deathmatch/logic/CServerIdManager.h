/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CServerIdManager.h
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


class CServerIdManager
{
public:
    virtual                     ~CServerIdManager               ( void ) {}
    virtual SString             GetConnectionPrivateDirectory   ( void ) = 0;

    static CServerIdManager*    GetSingleton  ( void );
};

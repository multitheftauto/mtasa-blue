/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CServerIdManager.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CServerIdManager
{
public:
    virtual ~CServerIdManager() {}
    virtual SString GetConnectionPrivateDirectory(bool bPreviousVer = false) = 0;

    static CServerIdManager* GetSingleton();
};

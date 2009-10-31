/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientModelRequestManager.h
*  PURPOSE:     Entity model streaming manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

class CClientModelRequestManager;

#ifndef __CCLIENTMODELREQUESTMANAGER_H
#define __CCLIENTMODELREQUESTMANAGER_H

#include "CClientCommon.h"
#include "CClientEntity.h"
#include <list>

struct SClientModelRequest
{
    CModelInfo*         pModel;
    CClientEntity*      pEntity;
    DWORD               dwTimeRequested;
};

class CClientModelRequestManager
{
    friend class CClientManager;

public:
                                    CClientModelRequestManager      ( void );
                                    ~CClientModelRequestManager     ( void );

    bool                            IsLoaded                        ( unsigned short usModelID );
    bool                            IsRequested                     ( CModelInfo* pModelInfo );
    bool                            HasRequested                    ( CClientEntity* pRequester );
    CModelInfo*                     GetRequestedModelInfo           ( CClientEntity* pRequester );

    bool                            RequestBlocking                 ( unsigned short usModelID );

    bool                            Request                         ( unsigned short usModelID, CClientEntity* pRequester );
    bool                            RequestUpgrade                  ( unsigned short usModelID, CClientEntity* pRequester );
    void                            Cancel                          ( CClientEntity* pRequester, bool bAllowQueue );

private:
    void                                DoPulse                     ( void );
    bool                                GetRequestEntry             ( CClientEntity* pRequester, std::list < SClientModelRequest* > ::iterator& iter );

    bool                                m_bDoingPulse;
    std::list < SClientModelRequest* >  m_Requests;
    std::list < CClientEntity* >        m_CancelQueue;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientAnimationManager.h
*  PURPOSE:     Animation block streaming class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTANIMATIONMANAGER_H
#define __CCLIENTANIMATIONMANAGER_H

#include <vector>

class CAnimBlock;

class CAnimBlockRequest
{
public:
    inline                              CAnimBlockRequest   ( CAnimBlock * pBlock ) { this->pBlock = pBlock; }
    
    bool                                IsRequester         ( CClientAnimation * pRequester );

    CAnimBlock *                        pBlock;
    std::vector < CClientAnimation * >  Requesters;
};

struct SExistingBlock
{
    CAnimBlock *    pBlock;
    unsigned long   ulIdleTime;
};

class CClientAnimationManager
{
public:
                                            CClientAnimationManager         ( CClientManager * pManager );
                                            ~CClientAnimationManager        ( void );

    void                                    Request                         ( CAnimBlock * pBlock, CClientAnimation * pRequester );
    void                                    DoPulse                         ( void );

    void                                    Cleanup                         ( CClientAnimation * pRequester );

private:
    CAnimBlockRequest *                     GetRequest                      ( CAnimBlock * pBlock );
    bool                                    IsExistingBlock                 ( CAnimBlock * pBlock );

    CClientManager *                        m_pManager;
    std::vector < CAnimBlockRequest * >     m_Requests;
    std::vector < SExistingBlock * >        m_ExistingBlocks;
};

#endif
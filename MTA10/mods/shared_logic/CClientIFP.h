/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientIFP.h
*  PURPOSE:     Animation manager class header
*  DEVELOPERS:  Cazomino05 <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*****************************************************************************/

#ifndef __CCLIENTIFP_H
#define __CCLIENTIFP_H

#include "CClientEntity.h"

class CClientIFP: public CClientEntity
{
public:
                                    CClientIFP              ( class CClientManager* pManager, ElementID ID );
                                    ~CClientIFP             ( void );

    virtual eClientEntityType       GetType                 ( void ) const              { return CCLIENTIFP; }

    bool                            LoadIFP                 ( const char* szFile );
    void                            UnloadIFP               ( void );

    
    void                            Remove                  ( void );


    // Sorta a hack that these are required by CClientEntity...
    void                            Unlink                  ( void ) {};
    void                            GetPosition             ( CVector& vecPosition ) const {};
    void                            SetPosition             ( const CVector& vecPosition ) {};

private:

    CAnimBlock *                    m_pAnimBlock;
    int                             m_iAnimBlock;
};

#endif

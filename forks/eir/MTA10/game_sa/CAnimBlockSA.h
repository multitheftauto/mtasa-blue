/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimBlockSA.h
*  PURPOSE:     Header file for animation block class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimBlockSA_H
#define __CAnimBlockSA_H

#include <game/CAnimBlock.h>
#include "Common.h"

class CAnimBlendAssocGroupSA;

#define ARRAY_AnimBlock             0x00B5D4A0

class CAnimBlockSAInterface // 32 bytes
{
public:
    char                szName [ 16 ];
    bool                bLoaded;    // ?
    BYTE                pad [ 1 ];
    unsigned short      usRefs;
    unsigned int        animationIndex;   // 20
    unsigned int        count;            // 24
    unsigned int        animInfoIndex;    // 28

    void                                Reference           ( void )                    { usRefs++; }
    void                                Dereference         ( void )                    { usRefs--; }
    unsigned short                      GetRefCount         ( void ) const              { return usRefs; }

    const char*                         GetName             ( void ) const              { return szName; }

    bool                                IsLoaded            ( void ) const              { return bLoaded; }
    unsigned int                        GetAnimIndex        ( void ) const              { return animationIndex; }
    unsigned int                        GetCount            ( void ) const              { return count; }
    unsigned int                        GetAnimInfoIndex    ( void ) const              { return animInfoIndex; }

    int                                 GetIndex            ( void );

    CAnimBlendHierarchySAInterface*     GetAnimation        ( unsigned int hash );
};

class CAnimBlockSA : public CAnimBlock
{
    friend class CAnimBlendAssocGroupSA;
public:
                                        CAnimBlockSA            ( CAnimBlockSAInterface * pInterface )     { m_pInterface = pInterface; }

    CAnimBlockSAInterface *             GetInterface            ( void )    { return m_pInterface; }
    char *                              GetName                 ( void )    { return m_pInterface->szName; }
    int                                 GetIndex                ( void )    { return m_pInterface->GetIndex (); }
    void                                AddRef                  ( void )    { m_pInterface->usRefs++; }
    unsigned short                      GetRefs                 ( void )    { return m_pInterface->usRefs; }
    void                                Request                 ( EModelRequestType requestType, bool bAllowBlockingFail = false );
    bool                                IsLoaded                ( void )    { return m_pInterface->bLoaded; }

protected:
    CAnimBlockSAInterface *             m_pInterface;
};

#endif

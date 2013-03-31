/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAnimBlockSA.cpp
*  PURPOSE:     Animation block
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CAnimBlockSAInterface::GetIndex( void )
{
    // GTA:SA does this all the time; it ain't hacky.
    return this - (CAnimBlockSAInterface*)ARRAY_CAnimManager_AnimBlocks;
}

CAnimBlendHierarchySAInterface* CAnimBlockSAInterface::GetAnimation( unsigned int hash )
{
    for ( unsigned int n = 0; n < GetCount(); n++ )
    {
        CAnimBlendHierarchySAInterface *anim = (CAnimBlendHierarchySAInterface*)ARRAY_CAnimManager_Animations + GetAnimIndex() + n;

        if ( anim->iHashKey == hash )
            return anim;
    }

    return NULL;
}

void CAnimBlockSA::Request ( EModelRequestType requestType, bool bAllowBlockingFail )
{
    if ( IsLoaded () )
        return;

    DWORD dwModelID = GetIndex() + 25575;

    if ( requestType == BLOCKING )
    {
        CModelInfo* pModelInfo = pGame->GetModelInfo ( dwModelID );
        pGame->GetStreaming()->RequestModel ( dwModelID, 0x14 );
        AddRef ();
        pGame->GetStreaming()->LoadAllRequestedModels ( true );

        // bAllowBlockingFail is for debugging only (Blocking load should never fail)
        if ( !bAllowBlockingFail )
        {
            assert ( IsLoaded() );
            assert ( pGame->GetStreaming()->HasModelLoaded ( dwModelID ) );
        }
    }
    else
    {
        pGame->GetStreaming()->RequestModel ( dwModelID, 0x04 );
        AddRef ();
    }
}

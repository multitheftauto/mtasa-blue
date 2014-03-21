/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.ClothesReplacing.cpp
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

namespace ClothesReplacing
{
    std::map < ushort, char* > ms_ReplacementClothesFileDataMap;
    bool bClothesReplacementChanged = false;

    struct SPlayerImgItem
    {
        uint uiBlockOffset;
        uint uiBlockCount;
        const char szName[24];
    };

    struct SPlayerImgItemArray
    {
        SPlayerImgItem* pItems;
        uint uiArrayMax;
        uint uiArraySize;
    };

    int     iReturnFileId;
    char*   pReturnBuffer;
}

using namespace ClothesReplacing;

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ClothesAddReplacementTxd
//
// Add replacement txd for a clothing component
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ClothesAddReplacementTxd( char* pFileData, ushort usFileId )
{
    if ( !pFileData )
        return;
    if ( pFileData != MapFindRef( ms_ReplacementClothesFileDataMap, usFileId ) )
    {
        MapSet( ms_ReplacementClothesFileDataMap, usFileId, pFileData );
        bClothesReplacementChanged = true;
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::ClothesRemoveReplacementTxd
//
// Remove replacement txd for a clothing component
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::ClothesRemoveReplacementTxd( char* pFileData )
{
    if ( !pFileData )
        return;
    for( std::map < ushort, char* >::iterator iter = ms_ReplacementClothesFileDataMap.begin() ; iter != ms_ReplacementClothesFileDataMap.end() ; )
    {
        if ( iter->second == pFileData )
        {
            ms_ReplacementClothesFileDataMap.erase( iter++ );
            bClothesReplacementChanged = true;
        }
        else
            ++iter;
    }
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::HasClothesReplacementChanged
//
// Returns true (once) if clothes textures need regenerating
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::HasClothesReplacementChanged( void )
{
    bool bResult = bClothesReplacementChanged;
    bClothesReplacementChanged = false;
    return bResult;
}


////////////////////////////////////////////////////////////////
//
// CStreaming_RequestModel_Mid
//
// If request is for a file inside player.img (imgId 5)
// then maybe switch to replacement txd file data
//
////////////////////////////////////////////////////////////////
bool _cdecl OnCStreaming_RequestModel_Mid ( int flags, ClothesReplacing::SImgGTAItemInfo* pImgGTAInfo )
{
    // Check is player.img 
    if ( pImgGTAInfo->ucImgId != 5 )
        return false;

    // Early out if no clothes textures to replace with
    if ( ms_ReplacementClothesFileDataMap.empty() )
        return false;

    // Initialze lookup map if needed
    static std::map < uint, int > blockOffsetToFileIdMap;
    if ( blockOffsetToFileIdMap.empty() )
    {
        // Check is player.img dir has been loaded by GTA
        SPlayerImgItemArray* pItemArray = (SPlayerImgItemArray*)0x00BC12C0;
        if ( !pItemArray->pItems || pItemArray->uiArraySize != 542 )
            return false;

        for ( uint i = 0 ; i < pItemArray->uiArraySize ; i++ )
        {
            SPlayerImgItem* pImgItem = &pItemArray->pItems[i];
            MapSet( blockOffsetToFileIdMap, pImgItem->uiBlockOffset, i );
        }
    }

    // Get player.img fileId by comparing the supplied BlockOffset with entries in the player.img dir
    int* piPlayerImgFileId = MapFind( blockOffsetToFileIdMap, pImgGTAInfo->iBlockOffset );
    if ( !piPlayerImgFileId )
        return false;

    int iPlayerImgFileId = *piPlayerImgFileId;

    // Do we have a replacement for this clothes texture?
    char* pReplacementFileData = MapFindRef( ms_ReplacementClothesFileDataMap, iPlayerImgFileId );
    if ( !pReplacementFileData )
        return false;

    // If bLoadingBigModel is set, try to get it unset
    if ( Streaming::isLoadingBigModel )
    {
        pGame->GetStreaming()->LoadAllRequestedModels ( true );
        if ( Streaming::isLoadingBigModel )
            pGame->GetStreaming()->LoadAllRequestedModels ( false );
        assert ( !Streaming::isLoadingBigModel );
    }

    // Set results
    iReturnFileId = ((int)pImgGTAInfo - 0x08E4CC0) / 20;
    pReturnBuffer = pReplacementFileData;

    // Update flags
    pImgGTAInfo->uiLoadflag = 3;

    // Remove priorty flag, as not counted in ms_numPriorityRequests
    pImgGTAInfo->uiUnknown2 &= ~ 0x10;

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticSetClothesReplacingHooks( void )
{
}

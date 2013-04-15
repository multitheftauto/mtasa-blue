/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.ClothesReplacing.cpp
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

namespace
{
    struct SImgGTAItemInfo
    {
        ushort    usNext;
        ushort    usPrev;

        ushort  uiUnknown1;         // Parent ?
        uchar   uiUnknown2;         // 0x12 when loading, 0x02 when finished loading
        uchar   ucImgId;

        int     iBlockOffset;
        int     iBlockCount;
        uint    uiLoadflag;         // 0-not loaded  2-requested  3-loaded  1-processed
    };

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

    DWORD FUNC_CStreamingConvertBufferToObject      = 0x40C6B0;
    int     iReturnFileId;
    char*   pReturnBuffer;
}


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
bool _cdecl OnCStreaming_RequestModel_Mid ( int flags, SImgGTAItemInfo* pImgGTAInfo )
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
    #define VAR_CStreaming_bLoadingBigModel     0x08E4A58
    BYTE& bLoadingBigModel = *(BYTE*)VAR_CStreaming_bLoadingBigModel;
    if ( bLoadingBigModel )
    {
        pGame->GetStreaming()->LoadAllRequestedModels ( true );
        if ( bLoadingBigModel )
            pGame->GetStreaming()->LoadAllRequestedModels ( false );
        assert ( !bLoadingBigModel );
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

// Hook info
#define HOOKPOS_CStreaming_RequestModel_Mid             0x040895A
#define HOOKSIZE_CStreaming_RequestModel_Mid            5
DWORD RETURN_CStreaming_RequestModel_MidA               = 0x0408960;
DWORD RETURN_CStreaming_RequestModel_MidB               = 0x0408990;
void _declspec(naked) HOOK_CStreaming_RequestModel_Mid()
{
    _asm
    {
        pushad
        push    esi
        push    ebx
        call    OnCStreaming_RequestModel_Mid
        add     esp, 4*2
        cmp     al, 0
        jnz     skip

        // Continue with standard code
        popad
        mov     eax, ds:0x08E4C58
        push    eax
        jmp     RETURN_CStreaming_RequestModel_MidA


        // Handle load here
skip:
        popad
        pushad

        mov     eax, 0
        push    eax
        mov     eax, iReturnFileId
        push    eax
        mov     eax, pReturnBuffer
        push    eax
        call    FUNC_CStreamingConvertBufferToObject
        add     esp, 4*3

        popad
        jmp     RETURN_CStreaming_RequestModel_MidB
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CRenderWareSA::StaticSetClothesReplacingHooks( void )
{
    EZHookInstall( CStreaming_RequestModel_Mid );
}

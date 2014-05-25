/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.cpp
*  PURPOSE:     Entity model information handler
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA * pGame;

CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**) ARRAY_ModelInfo;

std::map < unsigned short, int > CModelInfoSA::ms_RestreamTxdIDMap;
std::map < DWORD, float > CModelInfoSA::ms_ModelDefaultLodDistanceMap;
std::set < uint > CModelInfoSA::ms_ReplacedColModels;
std::map < DWORD, BYTE > CModelInfoSA::ms_ModelDefaultAlphaTransparencyMap;

CModelInfoSA::CModelInfoSA ( void )
{
    m_pInterface = NULL;
    this->m_dwModelID = 0xFFFFFFFF;
    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;
    m_pOriginalColModelInterface = NULL;
    m_pCustomClump = NULL;
    m_pCustomColModel = NULL;
    m_bAddedRefForCollision = false;
}


CModelInfoSA::CModelInfoSA ( DWORD dwModelID )
{    
    this->m_dwModelID = dwModelID;
    m_pInterface = ppModelInfo [ m_dwModelID ];
    m_dwReferences = 0;
    m_dwPendingInterfaceRef = 0;
    m_pOriginalColModelInterface = NULL;
    m_pCustomClump = NULL;
    m_pCustomColModel = NULL;
    m_bAddedRefForCollision = false;
}


CBaseModelInfoSAInterface * CModelInfoSA::GetInterface ( void )
{
    return m_pInterface = ppModelInfo [ m_dwModelID ];
}


BOOL CModelInfoSA::IsBoat ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsBoat ( )");
    DWORD dwFunction = FUNC_IsBoatModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsCar ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsCar ( )");
    DWORD dwFunction = FUNC_IsCarModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsTrain ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsTrain ( )");
    DWORD dwFunction = FUNC_IsTrainModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsHeli ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsHeli ( )");
    DWORD dwFunction = FUNC_IsHeliModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsPlane ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsPlane ( )");
    DWORD dwFunction = FUNC_IsPlaneModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsBike ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsBike ( )");
    DWORD dwFunction = FUNC_IsBikeModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsFakePlane ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsFakePlane ( )");
    DWORD dwFunction = FUNC_IsFakePlaneModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsMonsterTruck ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsMonsterTruck ( )");
    DWORD dwFunction = FUNC_IsMonsterTruckModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}

BOOL CModelInfoSA::IsQuadBike ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsQuadBike ( )");
    DWORD dwFunction = FUNC_IsQuadBikeModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsBmx ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsBmx ( )");
    DWORD dwFunction = FUNC_IsBmxModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsTrailer ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsTrailer ( )");
    DWORD dwFunction = FUNC_IsTrailerModel;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
}   

BOOL CModelInfoSA::IsVehicle ( )
{
    /*
    DEBUG_TRACE("BOOL CModelInfoSA::IsVehicle ( )");
    DWORD dwFunction = FUNC_IsVehicleModelType;
    DWORD ModelID = m_dwModelID;
    BYTE bReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunction
        mov     bReturn, al
        add     esp, 4
    }
    return (BOOL)bReturn;
    */

    // Above doesn't seem to work
    return m_dwModelID >= 400 && m_dwModelID <= 611;
}   

bool CModelInfoSA::IsPlayerModel ( )
{
    return ( m_dwModelID == 0 ||
             m_dwModelID == 1 ||
             m_dwModelID == 2 ||
             m_dwModelID == 7 ||
             (m_dwModelID >= 9 &&
             m_dwModelID != 208 &&
             m_dwModelID != 149 &&
             m_dwModelID != 119 &&
             m_dwModelID != 86 &&
             m_dwModelID != 74 &&
             m_dwModelID != 65 &&
             m_dwModelID != 42 &&
             m_dwModelID <= 272) ||
             (m_dwModelID >= 274 &&
             m_dwModelID <= 288) ||
             (m_dwModelID >= 290 &&
             m_dwModelID <= 312 ) );
}

BOOL CModelInfoSA::IsUpgrade ( void )
{
    return m_dwModelID >= 1000 && m_dwModelID <= 1193;
}

char * CModelInfoSA::GetNameIfVehicle ( )
{
    DEBUG_TRACE("char * CModelInfoSA::GetNameIfVehicle ( )");
//  if(this->IsVehicle())
//  {
        DWORD dwModelInfo = ARRAY_ModelInfo;
        DWORD dwFunc = FUNC_CText_Get;
        DWORD ModelID = m_dwModelID;
        DWORD dwReturn = 0;

        _asm
        {       
            push    eax
            push    ebx
            push    ecx

            mov     ebx, ModelID
            lea     ebx, [ebx*4]
            add     ebx, ARRAY_ModelInfo
            mov     eax, [ebx]
            add     eax, 50

            push    eax
            mov     ecx, CLASS_CText
            call    dwFunc

            mov     dwReturn, eax

            pop     ecx
            pop     ebx
            pop     eax
        }
        return (char *)dwReturn;
//  }
//  return NULL;
}

uint CModelInfoSA::GetAnimFileIndex ( void )
{
    DWORD dwFunc = m_pInterface->VFTBL->GetAnimFileIndex;
    DWORD dwThis = (DWORD) m_pInterface;
    uint uiReturn = 0;
    if ( dwFunc )
    {
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
            mov     uiReturn, eax
        }
    }
    return uiReturn;
}


VOID CModelInfoSA::Request( EModelRequestType requestType, const char* szTag )
{
    DEBUG_TRACE("VOID CModelInfoSA::Request( BOOL bAndLoad, BOOL bWaitForLoad )");
    // don't bother loading it if it already is
    if ( IsLoaded () )
        return;

    if ( m_dwModelID <= 288 && m_dwModelID != 7 && !pGame->GetModelInfo ( 7 )->IsLoaded () )
    {
        // Skin 7 must be loaded in order for other skins to work. No, really. (#4010)
        pGame->GetModelInfo ( 7 )->Request ( requestType, "Model 7" );
    }

    // Bikes can sometimes get stuck when loading unless the anim file is handled like what is does here
    // Don't change the code below unless you can test it (by recreating the problem it solves)
    if ( IsVehicle () )
    {
        uint uiAnimFileIndex = GetAnimFileIndex ();
        if ( uiAnimFileIndex != 0xffffffff )
        {
            uint uiAnimId = uiAnimFileIndex + 25575;
            CModelInfoSA* pAnim = static_cast < CModelInfoSA* > ( pGame->GetModelInfo ( uiAnimId ) );
            if ( !pAnim )
            {
                if ( uiAnimId != 25714 )
                    LogEvent ( 505, "Model no anim", "", SString ( "%d (%d)", m_dwModelID, uiAnimId ) );
            }
            else
            if ( !pAnim->IsLoaded() )
            {
                OutputDebugLine ( SString ( "[Models] Requesting anim file %d for model %d", uiAnimId, m_dwModelID ) );
                pAnim->Request ( requestType, szTag );
            }
        }
    }

    if ( requestType == BLOCKING )
    {
        pGame->GetStreaming()->RequestModel ( m_dwModelID, 0x16 );
        pGame->GetStreaming()->LoadAllRequestedModels ( true, szTag );
        if ( !IsLoaded() )
        {
            // Try 3 more times, final time without high priority flag
            int iCount = 0;
            while ( iCount++ < 10 && !IsLoaded() )
            {
                bool bOnlyPriorityModels = ( iCount < 3 || iCount & 1 );
                pGame->GetStreaming()->LoadAllRequestedModels ( bOnlyPriorityModels, szTag );
            }
            if ( !IsLoaded() )
            {
                AddReportLog ( 6641, SString ( "Blocking load fail: %d (%s)", m_dwModelID, szTag ) );
                LogEvent ( 641, "Blocking load fail", "", SString ( "%d (%s)", m_dwModelID, szTag ) );
                dassert ( 0 );
            }
            else
            {
                AddReportLog ( 6642, SString ( "Blocking load: %d (%s) (Took %d attempts)", m_dwModelID, szTag, iCount ) );
                LogEvent ( 642, "Blocking load", "", SString ( "%d (%s) (Took %d attempts)", m_dwModelID, szTag, iCount ) );
            }
        }
    }
    else
    {
        pGame->GetStreaming()->RequestModel ( m_dwModelID, 0x06 );
    }
}

VOID CModelInfoSA::Remove ( )
{
    DEBUG_TRACE("VOID CModelInfoSA::Remove ( )");

    // Don't remove if GTA refers to it somehow.
    // Or we'll screw up SA's map for example.

    m_pInterface = ppModelInfo [ m_dwModelID ];

    // Remove ref added for collision
    if ( m_bAddedRefForCollision )
    {
        m_bAddedRefForCollision = false;
        if ( m_pInterface->usNumberOfRefs > 0 )
            m_pInterface->usNumberOfRefs--;
    }

    // Remove our reference
    if ( m_pInterface->usNumberOfRefs > 0 ) m_pInterface->usNumberOfRefs--;

    // No references left?
    if ( m_pInterface->usNumberOfRefs == 0 )
    {  
        // We have a custom model?
        if ( m_pCustomClump )
        {            
            // Mark us as unloaded. We manage the clump unloading.
            //BYTE *ModelLoaded = (BYTE*)ARRAY_ModelLoaded;
            //ModelLoaded[(m_dwModelID+m_dwModelID*4)<<2] = 0;
        }
        else
        {
            // Make our collision model original again before we unload.
            RestoreColModel ();

            // Remove the model.
            DWORD dwFunction = FUNC_RemoveModel;
            DWORD ModelID = m_dwModelID;
            _asm
            {
                push    ModelID
                call    dwFunction
                add     esp, 4
            }
        }
    }
}

BYTE CModelInfoSA::GetLevelFromPosition ( CVector * vecPosition )
{
    DEBUG_TRACE("BYTE CModelInfoSA::GetLevelFromPosition ( CVector * vecPosition )");
    DWORD dwFunction = FUNC_GetLevelFromPosition;
    BYTE bReturn = 0;
    _asm
    {
        push    vecPosition
        call    dwFunction
        add     esp, 4
        mov     bReturn, al
    }
    return bReturn;
}

BOOL CModelInfoSA::IsLoaded ( )
{
    if ( DoIsLoaded () )
    {
        if ( m_dwPendingInterfaceRef )
        {
            assert ( m_dwReferences > 0 );
            m_pInterface = ppModelInfo [ m_dwModelID ];
            m_pInterface->usNumberOfRefs++;
            m_dwPendingInterfaceRef = 0;
        }
        return true;
    }
    return false;
}

BOOL CModelInfoSA::DoIsLoaded ( )
{
    DEBUG_TRACE("BOOL CModelInfoSA::IsLoaded ( )");

    //return (BOOL)*(BYTE *)(ARRAY_ModelLoaded + 20*dwModelID);
    BOOL bLoaded = pGame->GetStreaming()->HasModelLoaded(m_dwModelID);
    m_pInterface = ppModelInfo [ m_dwModelID ];

    if ( bLoaded && m_dwModelID < 20000 )
    {
        // Check rw object is there
        if ( !m_pInterface || !m_pInterface->pRwObject )
            return false;
    }
    return bLoaded;
}

BYTE CModelInfoSA::GetFlags ( )
{
    DWORD dwFunc = FUNC_GetModelFlags;
    DWORD ModelID = m_dwModelID;
    BYTE bFlags = 0;
    _asm
    {
        push    ModelID
        call    dwFunc
        add     esp, 4
        mov     bFlags, al
    }
    return bFlags;
}

CBoundingBox * CModelInfoSA::GetBoundingBox ( )
{
    DWORD dwFunc = FUNC_GetBoundingBox;
    DWORD ModelID = m_dwModelID;
    CBoundingBox * dwReturn = 0;
    _asm
    {
        push    ModelID
        call    dwFunc
        add     esp, 4
        mov     dwReturn, eax
    }
    return dwReturn;
}

bool CModelInfoSA::IsValid ( )
{
    return ppModelInfo [ m_dwModelID ] != 0;
}

float CModelInfoSA::GetDistanceFromCentreOfMassToBaseOfModel ( )
{

    DWORD dwModelInfo = 0;
    DWORD ModelID = m_dwModelID;
    FLOAT fReturn = 0;
    _asm
    {
        mov     eax, ModelID
        mov     eax, ARRAY_ModelInfo[eax*4]
        mov     eax, [eax+20]
        cmp     eax, 0
        jz      skip
        fld     [eax + 8]
        fchs
        fstp    fReturn
skip:
    }
    return fReturn;
}

unsigned short CModelInfoSA::GetTextureDictionaryID ()
{
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if ( m_pInterface )
        return m_pInterface->usTextureDictionary;

    return 0;
}

void CModelInfoSA::SetTextureDictionaryID ( unsigned short usID )
{
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if ( m_pInterface )
        m_pInterface->usTextureDictionary = usID;
}

float CModelInfoSA::GetLODDistance ()
{
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if ( m_pInterface )
        return m_pInterface->fLodDistanceUnscaled;

    return 0.0f;
}

void CModelInfoSA::SetLODDistance ( float fDistance )
{
#if 0
    // fLodDistanceUnscaled values:
    //
    // With the draw distance setting in GTA SP options menu set to maximum:
    //      0 - 170     roughly correlates to a LOD distance of 0 - 300 game units
    //      170 - 480   sets the LOD distance to 300 game units and has a negative effect on the alpha fade-in
    //      490 - 1e7   sets the LOD distance to 300 game units and removes the alpha fade-in completely
    //
    // With the draw distance setting in GTA SP options menu set to minimum:
    //      0 - 325     roughly correlates to a LOD distance of 0 - 300 game units
    //      340 - 960   sets the LOD distance to 300 game units and has a negative effect on the alpha fade-in
    //      1000 - 1e7  sets the LOD distance to 300 game units and removes the alpha fade-in completely
    //
    // So, to ensure the maximum draw distance with a working alpha fade-in, fLodDistanceUnscaled has to be
    // no more than: 325 - (325-170) * draw_distance_setting
    //

    // Change GTA draw distance value from 0.925 to 1.8 into 0 to 1
    float fDrawDistanceSetting = UnlerpClamped ( 0.925f, CSettingsSA ().GetDrawDistance (), 1.8f );

    // Calc max setting allowed for fLodDistanceUnscaled to preserve alpha fade-in
    float fMaximumValue = Lerp ( 325.f, fDrawDistanceSetting, 170.f );

    // Ensure fDistance is in range
    fDistance = Min ( fDistance, fMaximumValue );
#endif
    // Limit to 325.f as it goes horrible after that
    fDistance = Min ( fDistance, 325.f );
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if ( m_pInterface )
    {
        // Save default value if not done yet
        if ( !MapContains ( ms_ModelDefaultLodDistanceMap, m_dwModelID ) )
            MapSet ( ms_ModelDefaultLodDistanceMap, m_dwModelID, m_pInterface->fLodDistanceUnscaled );
        m_pInterface->fLodDistanceUnscaled = fDistance;
    }
}

void CModelInfoSA::StaticResetLodDistances ()
{
    // Restore default values
    for ( std::map < DWORD, float >::const_iterator iter = ms_ModelDefaultLodDistanceMap.begin () ; iter != ms_ModelDefaultLodDistanceMap.end () ; ++iter )
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo [ iter->first ];
        if ( pInterface )
            pInterface->fLodDistanceUnscaled = iter->second;
    }

    ms_ModelDefaultLodDistanceMap.clear ();
}

void CModelInfoSA::RestreamIPL ()
{
    // IPLs should not contain peds, weapons, vehicles and vehicle upgrades
    if ( m_dwModelID > 611 && ( m_dwModelID < 1000 || m_dwModelID > 1193 ) )
        MapSet ( ms_RestreamTxdIDMap, GetTextureDictionaryID (), 0 );
}

void CModelInfoSA::StaticFlushPendingRestreamIPL ( void )
{
    if ( ms_RestreamTxdIDMap.empty () )
        return;
    // This function restreams all instances of the model *that are from the default SA world (ipl)*.
    // In other words, it does not affect elements created by MTA.
    // It's mostly a reimplementation of SA's DeleteAllRwObjects, except that it filters by model ID.

    ( (void (*)())FUNC_FlushRequestList )();

    std::set < unsigned short > removedModels;
    
    for ( int i = 0; i < 2*NUM_StreamSectorRows*NUM_StreamSectorCols; i++ )
    {
        DWORD* pSectorEntry = ((DWORD **)ARRAY_StreamSectors) [ i ];
        while ( pSectorEntry )
        {
            CEntitySAInterface* pEntity = (CEntitySAInterface *)pSectorEntry [ 0 ];

            // Possible bug - pEntity seems to be invalid here occasionally
            if ( pEntity->vtbl->DeleteRwObject != 0x00534030 )
            {
                // Log info
                OutputDebugString ( SString ( "Entity 0x%08x (with model %d) at ARRAY_StreamSectors[%d,%d] is invalid\n", pEntity, pEntity->m_nModelIndex, i / 2 % NUM_StreamSectorRows, i / 2 / NUM_StreamSectorCols ) );
                // Assert in debug
                #if _DEBUG
                    assert ( pEntity->vtbl->DeleteRwObject == 0x00534030 );
                #endif
                pSectorEntry = (DWORD *)pSectorEntry [ 1 ];
                continue;
            }

            if ( MapContains ( ms_RestreamTxdIDMap, pGame->GetModelInfo ( pEntity->m_nModelIndex )->GetTextureDictionaryID () ) )
            {
                if ( !pEntity->bStreamingDontDelete && !pEntity->bImBeingRendered )
                {
                    __asm
                    {
                        mov ecx, pEntity
                        mov eax, [ecx]
                        call dword ptr [eax+20h]
                    }
                    removedModels.insert ( pEntity->m_nModelIndex );
                }
            }
            
            pSectorEntry = (DWORD *)pSectorEntry [ 1 ];
        }
    }

    for ( int i = 0; i < NUM_StreamRepeatSectorRows*NUM_StreamRepeatSectorCols; i++ )
    {
        DWORD* pSectorEntry = ((DWORD **)ARRAY_StreamRepeatSectors) [ 3*i + 2 ];
        while ( pSectorEntry )
        {
            CEntitySAInterface* pEntity = (CEntitySAInterface *)pSectorEntry [ 0 ];
            if ( MapContains ( ms_RestreamTxdIDMap, pGame->GetModelInfo ( pEntity->m_nModelIndex )->GetTextureDictionaryID () ) )
            {
                if ( !pEntity->bStreamingDontDelete && !pEntity->bImBeingRendered )
                {
                    __asm
                    {
                        mov ecx, pEntity
                        mov eax, [ecx]
                        call dword ptr [eax+20h]
                    }
                    removedModels.insert ( pEntity->m_nModelIndex );
                }
            }
            pSectorEntry = (DWORD *)pSectorEntry [ 1 ];
        }
    }

    ms_RestreamTxdIDMap.clear ();

    std::set < unsigned short >::iterator it;
    for ( it = removedModels.begin (); it != removedModels.end (); it++ )
    {
        ( (void (__cdecl *)(unsigned short))FUNC_RemoveModel )( *it );
        MemPut < BYTE > ( ARRAY_ModelLoaded + 20*(*it), 0 );
    }
}

void CModelInfoSA::ModelAddRef ( EModelRequestType requestType, const char* szTag )
{
    // Are we not loaded?
    if ( !IsLoaded () )
    {
        // Request it. Wait for it to load if we're asked to.
        if ( pGame && pGame->IsASyncLoadingEnabled () )
            Request ( requestType, szTag );
        else
            Request ( BLOCKING, szTag );
    }

    // Increment the references.
    if ( m_dwReferences == 0 )
    {
        assert ( !m_dwPendingInterfaceRef );
        if ( IsLoaded () )
        {
            m_pInterface = ppModelInfo [ m_dwModelID ];
            m_pInterface->usNumberOfRefs++;
        }
        else
            m_dwPendingInterfaceRef = 1;
    }

    m_dwReferences++;
}

int CModelInfoSA::GetRefCount ()
{
    return static_cast < int > ( m_dwReferences );
}

void CModelInfoSA::RemoveRef ( bool bRemoveExtraGTARef )
{
    assert ( m_dwReferences > 0 );

    // Decrement the references
    if ( m_dwReferences > 0 )
        m_dwReferences--;

    if ( m_dwReferences == 0 && m_dwPendingInterfaceRef )
    {
        m_dwPendingInterfaceRef = 0;
        return;
    }

    // Handle extra ref if requested
    if ( bRemoveExtraGTARef )
    {
        // Remove ref added by GTA.
        if ( m_pInterface->usNumberOfRefs > 1 )
        {
            DWORD dwFunction = FUNC_RemoveRef;
            CBaseModelInfoSAInterface* pInterface = m_pInterface;
            _asm
            {
                mov     ecx, pInterface
                call    dwFunction
            }
        }
    }

    // Unload it if 0 references left and we're not CJ model.
    // And if we're loaded.
    if ( m_dwReferences == 0 &&
         m_dwModelID != 0 &&
         IsLoaded () )
    {
        Remove ();
    }
}

void CModelInfoSA::SetAlphaTransparencyEnabled ( BOOL bEnabled )
{
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if(m_pInterface)
    {
        if ( !MapContains ( ms_ModelDefaultAlphaTransparencyMap, m_dwModelID ) )
        {
            MapSet ( ms_ModelDefaultAlphaTransparencyMap, m_dwModelID, (BYTE)(m_pInterface->bAlphaTransparency) );
        }
        m_pInterface->bAlphaTransparency = bEnabled;
    }
}

bool CModelInfoSA::IsAlphaTransparencyEnabled ()
{
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if(m_pInterface)
    {
        return m_pInterface->bAlphaTransparency;
    }
    return false;
}

void CModelInfoSA::StaticResetAlphaTransparencies ()
{
    for ( std::map < DWORD, BYTE >::const_iterator iter = ms_ModelDefaultAlphaTransparencyMap.begin (); iter != ms_ModelDefaultAlphaTransparencyMap.end (); iter++ )
    {
        CBaseModelInfoSAInterface* pInterface = ppModelInfo [ iter->first ];
        if ( pInterface )
        {
            pInterface->bAlphaTransparency = iter->second;
        }
    }

    ms_ModelDefaultAlphaTransparencyMap.clear ();
}

void CModelInfoSA::ResetAlphaTransparency ()
{
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if(m_pInterface)
    {
        BYTE* pbEnabled = MapFind ( ms_ModelDefaultAlphaTransparencyMap, m_dwModelID );
        if ( pbEnabled )
        {
            m_pInterface->bAlphaTransparency = *pbEnabled;
            MapRemove ( ms_ModelDefaultAlphaTransparencyMap, m_dwModelID );
        }
    }
}

short CModelInfoSA::GetAvailableVehicleMod ( unsigned short usUpgrade )
{
    short sreturn = -1;
    if ( usUpgrade >= 1000 && usUpgrade <= 1193 )
    {
        DWORD ModelID = m_dwModelID;
        _asm
        {
            mov     eax, ModelID
            movsx   edx, usUpgrade
            mov     eax, ARRAY_ModelInfo[eax*4]
            mov     ax, [eax+edx*2+0x2D6]
            mov     sreturn, ax
        }
    }
    return sreturn;
}

bool CModelInfoSA::IsUpgradeAvailable ( eVehicleUpgradePosn posn )
{
    bool bRet = false;
    DWORD ModelID = m_dwModelID;
    _asm
    {
        mov     eax, ModelID
        lea     ecx, ARRAY_ModelInfo[eax*4]

        mov     eax, posn
        mov     ecx, [ecx+0x5C]
        shl     eax, 5
        push    esi
        mov     esi, [ecx+eax+0D0h]
        xor     edx, edx
        test    esi, esi
        setnl   dl
        mov     al, dl
        pop     esi

        mov     bRet, al
    }
    return bRet;
}

void CModelInfoSA::SetCustomCarPlateText ( const char * szText )
{
    char * szStoredText;
    DWORD ModelID = m_dwModelID;
    _asm
    {
        push    ecx
        mov     ecx, ModelID
        mov     ecx, ARRAY_ModelInfo[ecx*4]
        add     ecx, 40
        mov     szStoredText, ecx
        pop     ecx
    }

    if ( szText ) 
        strncpy ( szStoredText, szText, 8 );
    else
        szStoredText[0] = 0;
}

unsigned int CModelInfoSA::GetNumRemaps ( void )
{
    DWORD dwFunc = FUNC_CVehicleModelInfo__GetNumRemaps;
    DWORD ModelID = m_dwModelID;
    unsigned int uiReturn = 0;
    _asm
    {
        mov     ecx, ModelID
        mov     ecx, ARRAY_ModelInfo[ecx*4]
        call    dwFunc
        mov     uiReturn, eax
    }
    return uiReturn;
}

void* CModelInfoSA::GetVehicleSuspensionData ( void )
{
    return GetInterface ()->pColModel->pColData->pSuspensionLines;
}

void* CModelInfoSA::SetVehicleSuspensionData ( void* pSuspensionLines )
{
    CColDataSA* pColData = GetInterface ()->pColModel->pColData;
    void* pOrigSuspensionLines = pColData->pSuspensionLines;
    pColData->pSuspensionLines = pSuspensionLines;
    return pOrigSuspensionLines;
}

void CModelInfoSA::SetCustomModel ( RpClump* pClump )
{
    // Error
    if ( pClump == NULL )
        return;

    // Store the custom clump
    m_pCustomClump = pClump;

    // Replace the model if we're loaded.
    if ( IsLoaded () )
    {
        switch (GetModelType())
        {
            case MODEL_INFO_TYPE_PED:
                return pGame->GetRenderWare ()->ReplacePedModel ( pClump, static_cast < unsigned short > ( m_dwModelID ) );
            case MODEL_INFO_TYPE_WEAPON:
                return pGame->GetRenderWare ()->ReplaceWeaponModel ( pClump, static_cast < unsigned short > ( m_dwModelID ) );
            case MODEL_INFO_TYPE_VEHICLE:
                return pGame->GetRenderWare ()->ReplaceVehicleModel ( pClump, static_cast < unsigned short > ( m_dwModelID ) );
            case MODEL_INFO_TYPE_ATOMIC:
            case MODEL_INFO_TYPE_LOD_ATOMIC:
            case MODEL_INFO_TYPE_TIME:
                return pGame->GetRenderWare()->ReplaceAllAtomicsInModel(pClump, static_cast < unsigned short > (m_dwModelID));
        }
    }
}

void CModelInfoSA::RestoreOriginalModel ( void )
{
    // Are we loaded?
    if ( IsLoaded () )
    {
        ( (void (__cdecl *)(unsigned short))FUNC_RemoveModel )( static_cast < unsigned short > ( m_dwModelID ) );
    }

    // Reset the stored custom vehicle clump
    m_pCustomClump = NULL;
}

void CModelInfoSA::SetColModel ( CColModel* pColModel )
{
    // Grab the interfaces
    CColModelSAInterface* pColModelInterface = pColModel->GetInterface ();

    if ( !m_bAddedRefForCollision )
    {
        // Prevent this model from unloading while we have custom collision
        ModelAddRef ( BLOCKING, "for collision" );
        m_bAddedRefForCollision = true;
    }

    // Should always be loaded at this point

    // Skip setting if already done
    if ( m_pCustomColModel == pColModel )
        return;

    // Remember model so we can skip GTA trying to reload the original
    MapInsert ( ms_ReplacedColModels, m_dwModelID );

    // Store the col model we set
    m_pCustomColModel = pColModel;

    // Do the following only if we're loaded
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if ( m_pInterface )
    {
        // If no collision model has been set before, store the original in case we want to restore it
        if ( !m_pOriginalColModelInterface )
            m_pOriginalColModelInterface = m_pInterface->pColModel;

        // Apply some low-level hacks
        pColModelInterface->level = 0xA9;

        // Call SetColModel
        DWORD dwFunc = FUNC_SetColModel;
        DWORD ModelID = m_dwModelID;
        _asm
        {
            mov     ecx, ModelID
            mov     ecx, ARRAY_ModelInfo[ecx*4]
            push    1
            push    pColModelInterface
            call    dwFunc
        }

        // FUNC_SetColModel resets bDoWeOwnTheColModel
        m_pInterface->bDoWeOwnTheColModel = false;
        m_pInterface->bCollisionWasStreamedWithModel = false;

        // public: static void __cdecl CColAccel::addCacheCol(int, class CColModel const &)
        DWORD func = 0x5B2C20;
        __asm {
            push    pColModelInterface
            push    ModelID
            call    func
            add     esp, 8
        }

        // Set some lighting for this collision if not already present
        CColDataSA* pColData = pColModelInterface->pColData;
        if ( pColData )
        {
            for ( uint i = 0 ; i < pColData->numColTriangles ; i++ )
            {
                CColTriangleSA* pTriangle = pColData->pColTriangles + i;
                if ( pTriangle->lighting.night == 0 && pTriangle->lighting.day == 0 )
                {
                    pTriangle->lighting.night = 1;
                    pTriangle->lighting.day = 12;
                }
            }
        }
    }
}

void CModelInfoSA::RestoreColModel ( void )
{
    MapRemove ( ms_ReplacedColModels, m_dwModelID );

    // Are we loaded?
    m_pInterface = ppModelInfo [ m_dwModelID ];
    if ( m_pInterface )
    {
        // We only have to store if the collision model was set
        // Also only if we have a col model set
        if ( m_pOriginalColModelInterface && m_pCustomColModel )
        {
            DWORD dwFunc = FUNC_SetColModel;
            DWORD dwOriginalColModelInterface = (DWORD)m_pOriginalColModelInterface;
            DWORD ModelID = m_dwModelID;
            _asm
            {
                mov     ecx, ModelID
                mov     ecx, ARRAY_ModelInfo[ecx*4]
                push    1
                push    dwOriginalColModelInterface
                call    dwFunc
            }

            // public: static void __cdecl CColAccel::addCacheCol(int, class CColModel const &)
            DWORD func = 0x5B2C20;
            __asm {
                push    dwOriginalColModelInterface
                push    ModelID
                call    func
                add     esp, 8
            }
            #pragma message(__LOC__ "(IJs) Document this function some time.")
        }
    }

    // We currently have no custom model loaded
    m_pCustomColModel = NULL;

    // Remove ref added for collision
    if ( m_bAddedRefForCollision )
    {
        m_bAddedRefForCollision = false;
        RemoveRef ();
    }
}


void CModelInfoSA::MakeCustomModel ( void )
{
    // We have a custom model?
    if ( m_pCustomClump )
    {
        SetCustomModel ( m_pCustomClump );
    }

    // Custom collision model is not NULL and it's different from the original?
    if ( m_pCustomColModel )
    {
        SetColModel ( m_pCustomColModel );
    }
}


void CModelInfoSA::GetVoice ( short* psVoiceType, short* psVoiceID )
{
    if ( psVoiceType )
        *psVoiceType = GetPedModelInfoInterface ()->sVoiceType;
    if ( psVoiceID )
        *psVoiceID = GetPedModelInfoInterface ()->sFirstVoice;
}

void CModelInfoSA::GetVoice ( const char** pszVoiceType, const char** pszVoice )
{
    short sVoiceType, sVoiceID;
    GetVoice ( &sVoiceType, &sVoiceID );
    if ( pszVoiceType )
        *pszVoiceType = CPedSoundSA::GetVoiceTypeNameFromID ( sVoiceType );
    if ( pszVoice )
        *pszVoice = CPedSoundSA::GetVoiceNameFromID ( sVoiceType, sVoiceID );
}

void CModelInfoSA::SetVoice ( short sVoiceType, short sVoiceID )
{
    GetPedModelInfoInterface ()->sVoiceType = sVoiceType;
    GetPedModelInfoInterface ()->sFirstVoice = sVoiceID;
    GetPedModelInfoInterface ()->sLastVoice = sVoiceID;
    GetPedModelInfoInterface ()->sNextVoice = sVoiceID;
}

void CModelInfoSA::SetVoice ( const char* szVoiceType, const char* szVoice )
{
    short sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName ( szVoiceType );
    if ( sVoiceType < 0 )
        return;
    short sVoiceID = CPedSoundSA::GetVoiceIDFromName ( sVoiceType, szVoice );
    if ( sVoiceID < 0 )
        return;
    SetVoice ( sVoiceType, sVoiceID );
}

void CModelInfoSA::MakePedModel ( char * szTexture )
{
    // Create a new CPedModelInfo
    CPedModelInfoSA pedModelInfo;
    ppModelInfo [ m_dwModelID ] = ( CBaseModelInfoSAInterface * ) pedModelInfo.GetPedModelInfoInterface ();

    // Load our texture
    pGame->GetStreaming ()->RequestSpecialModel ( m_dwModelID, szTexture, 0 );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Hook for CFileLoader_LoadCollisionFile_Mid
//
// Skip loading GTA collision model if we have replaced it
//
//////////////////////////////////////////////////////////////////////////////////////////
bool OnMY_CFileLoader_LoadCollisionFile_Mid ( int iModelId )
{
    if ( MapContains ( CModelInfoSA::ms_ReplacedColModels, iModelId ) )
        return false;

    return true;
}

// Hook info
#define HOOKPOS_CFileLoader_LoadCollisionFile_Mid                         0x5384EE
#define HOOKSIZE_CFileLoader_LoadCollisionFile_Mid                        6
DWORD RETURN_CFileLoader_LoadCollisionFile_Mid =                          0x5384F4;
DWORD RETURN_CFileLoader_LoadCollisionFile_Mid_Skip =                     0x53863B;
void _declspec(naked) HOOK_CFileLoader_LoadCollisionFile_Mid()
{
    _asm
    {
        pushad
        push    eax
        call    OnMY_CFileLoader_LoadCollisionFile_Mid
        add     esp, 4*1

        cmp     al,0
        jz      skip

        popad
        sub     edx,18h 
        add     ebp,2 
        jmp     RETURN_CFileLoader_LoadCollisionFile_Mid

skip:
        popad
        sub     edx,18h 
        add     ebp,2 
        mov     dword ptr [esp+4Ch],edx 
        jmp     RETURN_CFileLoader_LoadCollisionFile_Mid_Skip
    }
}


void CModelInfoSA::StaticSetHooks ( void )
{
    HookInstall( HOOKPOS_CFileLoader_LoadCollisionFile_Mid, (DWORD)HOOK_CFileLoader_LoadCollisionFile_Mid, HOOKSIZE_CFileLoader_LoadCollisionFile_Mid );
}

// Recursive RwFrame children searching function
void CModelInfoSA::RwSetSupportedUpgrades ( RwFrame * parent, DWORD dwModel ) 
{
    for( RwFrame* ret = parent->child ; ret != NULL ; ret = ret->next )
    {
        // recurse into the child
        if ( ret->child != NULL ) {
            RwSetSupportedUpgrades ( ret, dwModel );
        }
        SString strName = ret->szName;
        // Spoiler
        if ( strName == "ug_bonnet" )
        {
            m_ModelSupportedUpgrades.m_bBonnet = true;
        }
        else if ( strName == "ug_bonnet_left" )
        {
            m_ModelSupportedUpgrades.m_bBonnet_Left = true;
        }
        else if ( strName == "ug_bonnet_left_dam" )
        {
            m_ModelSupportedUpgrades.m_bBonnet_Left_dam = true;
        }
        else if ( strName == "ug_bonnet_right" )
        {
            m_ModelSupportedUpgrades.m_bBonnet_Right = true;
        }
        else if ( strName == "ug_bonnet_right_dam" )
        {
            m_ModelSupportedUpgrades.m_bBonnet_Right_dam = true;
        }
        // Spoiler
        else if ( strName == "ug_spoiler" )
        {
            m_ModelSupportedUpgrades.m_bSpoiler = true;
        }
        else if ( strName == "ug_spoiler_dam" )
        {
            m_ModelSupportedUpgrades.m_bSpoiler_dam = true;
        }
        // Bonnet
        else if ( strName == "ug_lights" )
        {
            m_ModelSupportedUpgrades.m_bLamps = true;
        }
        else if ( strName == "ug_lights_dam" )
        {
            m_ModelSupportedUpgrades.m_bLamps_dam = true;
        }
        // Roof
        else if ( strName == "ug_roof" )
        {
            m_ModelSupportedUpgrades.m_bRoof = true;
        }
        // Side Skirt
        else if ( strName == "ug_wing_right" )
        {
            m_ModelSupportedUpgrades.m_bSideSkirt_Right = true;
        }
        // Side Skirt
        else if ( strName == "ug_wing_left" )
        {
            m_ModelSupportedUpgrades.m_bSideSkirt_Left = true;
        }
        // Exhaust
        else if ( strName == "exhaust_ok" )
        {
            m_ModelSupportedUpgrades.m_bExhaust = true;
        }
        // Front bullbars
        else if ( strName == "ug_frontbullbar" )
        {
            m_ModelSupportedUpgrades.m_bFrontBullbars = true;
        }
        // rear bullbars
        else if ( strName == "ug_backbullbar" )
        {
            m_ModelSupportedUpgrades.m_bRearBullbars = true;
        }
        // Front bumper
        else if ( strName == "bump_front_dummy" )
        {
            m_ModelSupportedUpgrades.m_bFrontBumper = true;
        }
        // Rear bumper
        else if ( strName == "bump_rear_dummy" )
        {   
            m_ModelSupportedUpgrades.m_bRearBumper = true;
        }
        // Rear bumper
        else if ( strName == "misc_c" )
        {   
            m_ModelSupportedUpgrades.m_bMisc = true;
        }
    }
}

void CModelInfoSA::InitialiseSupportedUpgrades ( RpClump * pClump )
{
    m_ModelSupportedUpgrades.Reset ( );
    RwFrame * pFrame = RpGetFrame ( pClump );
    RwSetSupportedUpgrades ( pFrame, m_dwModelID );
    m_ModelSupportedUpgrades.m_bInitialised = true;
}

void CModelInfoSA::ResetSupportedUpgrades ( void )
{
    m_ModelSupportedUpgrades.Reset ( );
}

eModelInfoType CModelInfoSA::GetModelType(void)
{
    return ((eModelInfoType(*)(void))m_pInterface->VFTBL->GetModelType)();
}
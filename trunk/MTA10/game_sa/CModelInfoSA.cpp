/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CModelInfoSA.cpp
*  PURPOSE:		Entity model information handler
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
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

CModelInfoSA::CModelInfoSA ( void )
{
    m_pInterface = NULL;
    this->m_dwModelID = 0xFFFFFFFF;
    m_dwReferences = 0;
    m_pOriginalColModel = NULL;
    m_pCustomClump = NULL;
    m_pColModelInterface = NULL;
    m_pColModel = NULL;
}


CModelInfoSA::CModelInfoSA ( DWORD dwModelID )
{    
    this->m_dwModelID = dwModelID;
    m_pInterface = ppModelInfo [ m_dwModelID ];
    m_dwReferences = 0;
    m_pOriginalColModel = NULL;
    m_pCustomClump = NULL;
    m_pColModelInterface = NULL;
    m_pColModel = NULL;
}


BOOL CModelInfoSA::IsBoat ( )
{
	DEBUG_TRACE("BOOL CModelInfoSA::IsBoat ( )");
	DWORD dwFunction = FUNC_IsBoatModel;
	DWORD ModelID = m_dwModelID;
	BYTE bReturn = 0;
	_asm
	{
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
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
		push	ModelID
		call	dwFunction
		mov		bReturn, al
		add		esp, 4
	}
	return (BOOL)bReturn;
    */

    // Above doesn't seem to work
    return m_dwModelID >= 400 && m_dwModelID <= 611;
}	

char * CModelInfoSA::GetNameIfVehicle ( )
{
	DEBUG_TRACE("char * CModelInfoSA::GetNameIfVehicle ( )");
//	if(this->IsVehicle())
//	{
		DWORD dwModelInfo = ARRAY_ModelInfo;
		DWORD dwFunc = FUNC_CText_Get;
		DWORD ModelID = m_dwModelID;
		DWORD dwReturn = 0;

		_asm
		{		
			push	eax
			push	ebx
			push	ecx

			mov		ebx, ModelID
			lea		ebx, [ebx*4]
			add		ebx, ARRAY_ModelInfo
			mov		eax, [ebx]
			add		eax, 50

			push	eax
			mov		ecx, CLASS_CText
			call	dwFunc

			mov		dwReturn, eax

			pop		ecx
			pop		ebx
			pop		eax
		}
		return (char *)dwReturn;
//	}
//	return NULL;
}

VOID CModelInfoSA::Request( bool bAndLoad, bool bWaitForLoad )
{
	DEBUG_TRACE("VOID CModelInfoSA::Request( BOOL bAndLoad, BOOL bWaitForLoad )");
	// don't bother loading it if it already is
    if ( IsLoaded () )
        return;

    if ( m_dwModelID <= 288 && m_dwModelID != 7 && !pGame->GetModelInfo ( 7 )->IsLoaded () )
    {
        // Skin 7 must be loaded in order for other skins to work. No, really. (#4010)
        pGame->GetModelInfo ( 7 )->Request ( bAndLoad, false );
    }

	DWORD dwFunction = FUNC_RequestModel;
	DWORD ModelID = m_dwModelID;
	//DWORD dwChannel = ( m_dwModelID < 400 ) ? 0 : 6;
    DWORD dwChannel = 6;
	_asm
	{
		push	dwChannel
		push	ModelID
		call	dwFunction
		add		esp, 8
	}

	int iTimeToWait = 50;

	if(bAndLoad)
	{
		LoadAllRequestedModels();
		
		if(bWaitForLoad)
		{
			while(!this->IsLoaded() && iTimeToWait != 0)
			{
				iTimeToWait--;
				Sleep(10);
			}
		}
	}
}

VOID CModelInfoSA::Remove ( )
{
	DEBUG_TRACE("VOID CModelInfoSA::Remove ( )");

	// Don't remove if GTA refers to it somehow.
    // Or we'll screw up SA's map for example.

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
	        if ( m_pOriginalColModel && m_pColModel )
	        {
                // SetColModel it back to original
                DWORD dwOriginalColModelInterface = (DWORD) m_pOriginalColModel;
                DWORD ModelID = m_dwModelID;
                DWORD dwFunc = FUNC_SetColModel;
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
		        __asm
                {
			        push	dwOriginalColModelInterface
			        push	ModelID
			        call	func
			        add		esp, 8
		        }
		        #pragma message(__LOC__ "(IJs) Document this function some time.")
	        }

            // Remove the model.
	        DWORD dwFunction = FUNC_RemoveModel;
	        DWORD ModelID = m_dwModelID;
	        _asm
	        {
		        push	ModelID
		        call	dwFunction
		        add		esp, 4
	        }
        }
    }
}

VOID CModelInfoSA::LoadAllRequestedModels ( )
{
	DEBUG_TRACE("VOID CModelInfoSA::LoadAllRequestedModels ( )");

	DWORD dwFunction = FUNC_LoadAllRequestedModels;
    DWORD dwSlot = 0;
    //if ( m_dwModelID >= 400 && m_dwModelID < 615 )
        //dwSlot = 1;
	_asm
	{
		push	dwSlot
		call	dwFunction
		add		esp, 4
	}
}

BYTE CModelInfoSA::GetLevelFromPosition ( CVector * vecPosition )
{
	DEBUG_TRACE("BYTE CModelInfoSA::GetLevelFromPosition ( CVector * vecPosition )");
	DWORD dwFunction = FUNC_GetLevelFromPosition;
	BYTE bReturn = 0;
	_asm
	{
		push	vecPosition
		call	dwFunction
		add		esp, 4
		mov		bReturn, al
	}
	return bReturn;
}

BOOL CModelInfoSA::IsLoaded ( )
{
	DEBUG_TRACE("BOOL CModelInfoSA::IsLoaded ( )");
	//return (BOOL)*(BYTE *)(ARRAY_ModelLoaded + ((dwModelID + m_dwModelID * 4)*4));
	DWORD dwFunc = FUNC_CStreaming__HasModelLoaded;
	DWORD ModelID = m_dwModelID;

	BOOL bReturn = 0;
	_asm
	{
		push	ModelID
		call	dwFunc
		movzx	eax, al
		mov		bReturn, eax
		pop		eax
	}

    m_pInterface = ( bReturn ) ? m_pInterface = ppModelInfo [ m_dwModelID ] : NULL;
	return bReturn;
}

BYTE CModelInfoSA::GetFlags ( )
{
	DWORD dwFunc = FUNC_GetModelFlags;
	DWORD ModelID = m_dwModelID;
	BYTE bFlags = 0;
	_asm
	{
		push	ModelID
		call	dwFunc
		add		esp, 4
		mov		bFlags, al
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
		push	ModelID
		call	dwFunc
		add		esp, 4
		mov		dwReturn, eax
	}
	return dwReturn;
}

bool CModelInfoSA::IsValid ( )
{
	DWORD dwModelInfo = 0;
	DWORD ModelID = m_dwModelID;
    _asm
    {
        mov     eax, ModelID
        mov     eax, ARRAY_ModelInfo[eax*4]
        mov     dwModelInfo, eax
    }

	if ( dwModelInfo )
		return true;
	else
		return false;
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
		mov		eax, [eax+20]
        cmp     eax, 0
        jz      skip
		fld		[eax + 8]
		fchs
		fstp	fReturn
skip:
    }
	return fReturn;
}

unsigned short CModelInfoSA::GetTextureDictionaryID ()
{
    return ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[m_dwModelID]->usTextureDictionary;
}

void CModelInfoSA::AddRef ( bool bWaitForLoad )
{
	// Are we not loaded?
	if ( !IsLoaded () )
    {
        // Request it. Wait for it to load if we're asked to.
		Request ( TRUE, bWaitForLoad );
    }

    // Increment the references.
	m_dwReferences++;
}

int CModelInfoSA::GetRefCount ()
{
    return static_cast < int > ( m_dwReferences );
}

void CModelInfoSA::RemoveRef ()
{
    // Decrement the references
    if ( m_dwReferences > 0 )
	    m_dwReferences--;

    // Unload it if 0 references left and we're not CJ model.
    // And if we're loaded.
	if ( m_dwReferences == 0 &&
         m_dwModelID != 0 &&
         IsLoaded () )
    {
		Remove ();
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

void CModelInfoSA::RequestVehicleUpgrade ( void )
{
    DWORD dwFunc = FUNC_RequestVehicleUpgrade;
    DWORD ModelID = m_dwModelID;
    _asm
    {
        push    10
        push    ModelID
        call    dwFunc
        add     esp, 8
    }
}

void CModelInfoSA::SetCustomModel ( RpClump* pClump )
{
    // Error
	if ( pClump == NULL )
        return;

    // Store the custom clump
	m_pCustomClump = pClump;

    // Replace the vehicle model if we're loaded.
    if ( IsLoaded () )
    {
        // Are we a vehicle?
        if ( IsVehicle () )
        {
	        pGame->GetRenderWare ()->ReplaceVehicleModel ( pClump, static_cast < unsigned short > ( m_dwModelID ) );
        }
        else
        {
            // We are an object.
            pGame->GetRenderWare ()->ReplaceAllAtomicsInModel ( pClump, static_cast < unsigned short > ( m_dwModelID ) );
        }
    }
}

void CModelInfoSA::RestoreOriginalModel ( void )
{
    // Are we loaded?
    if ( IsLoaded () )
    {
        // Enable the unloaded state for this model so the original gets reloaded on request
		// and our custom clump isnt destroyed
		BYTE *ModelLoaded = (BYTE*)ARRAY_ModelLoaded;
		ModelLoaded [ 20*m_dwModelID ] = 0;

        // Load the original model
		//Request ( true, true );
    }

	// Reset the stored custom vehicle clump
	m_pCustomClump = NULL;
}

void CModelInfoSA::SetColModel ( CColModel* pColModel )
{
    // Grab the interfaces
	DWORD *pPool = ( DWORD* ) ARRAY_ModelInfo;
	CColModelSAInterface* pInterface = ((CColModelSA*)pColModel)->GetColModel ();

    // Store the col model we set
    m_pColModel = pColModel;
    m_pColModelInterface = pInterface;

    // Do the following only if we're loaded
    if ( IsLoaded () )
    {
        // Grab the current collision interface
        CColModelSAInterface* pCurrentInterface = m_pInterface->pColModel;

	    // If no collision model has been set before, store the original in case we want to restore it
	    if ( m_pOriginalColModel == NULL ) m_pOriginalColModel = pCurrentInterface;

        // Are we setting a different model than we have now?
        if ( pCurrentInterface != pInterface )
        {
	        // Apply some low-level hacks (copies the old col area and sets a flag)
	        DWORD pColModelInterface = (DWORD)pInterface;
	        DWORD pOldColModelInterface = (DWORD) m_pOriginalColModel;
	        *((BYTE *)( pPool [m_dwModelID ] + 0x13 )) |= 8;
	        *((BYTE *)( pColModelInterface + 40 )) = *((BYTE *)( pOldColModelInterface + 40 ));

			// Extra flags (3064) -- needs to be tested
			m_pInterface->bDoWeOwnTheColModel = false;
			m_pInterface->bCollisionWasStreamedWithModel = false;

            // Call SetColModel
            DWORD dwFunc = FUNC_SetColModel;
	        DWORD ModelID = m_dwModelID;
            _asm
            {
                mov     ecx, ModelID
                mov     ecx, ARRAY_ModelInfo[ecx*4]
                push    1
                push    pInterface
                call    dwFunc
            }

	        // public: static void __cdecl CColAccel::addCacheCol(int, class CColModel const &)
	        DWORD func = 0x5B2C20;
	        __asm {
		        push	pInterface
		        push	ModelID
		        call	func
		        add		esp, 8
	        }
	        #pragma message(__LOC__ "(IJs) Document this function some time.")
        }
    }
}

void CModelInfoSA::RestoreColModel ( void )
{
    // Remember that the current col model is the original
    m_pColModelInterface = m_pOriginalColModel;

    // Are we loaded?
    if ( IsLoaded () )
    {
	    // We only have to store if the collision model was set
        // Also only if we have a col model set
	    if ( m_pOriginalColModel && m_pColModel )
	    {
            DWORD dwFunc = FUNC_SetColModel;
            DWORD dwOriginalColModelInterface = (DWORD) m_pOriginalColModel;
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
			    push	dwOriginalColModelInterface
			    push	ModelID
			    call	func
			    add		esp, 8
		    }
		    #pragma message(__LOC__ "(IJs) Document this function some time.")
	    }
    }

    // We currently have no custom model loaded
    m_pColModel = NULL;
}


void CModelInfoSA::MakeCustomModel ( void )
{
    // We have a custom model?
    if ( m_pCustomClump )
    {
        SetCustomModel ( m_pCustomClump );
    }

    // Custom collision model is not NULL and it's different from the original?
    if ( m_pColModel != NULL &&
         m_pColModelInterface != NULL )
    {
        SetColModel ( m_pColModel );
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

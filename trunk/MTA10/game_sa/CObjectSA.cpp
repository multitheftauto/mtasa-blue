/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectSA.cpp
*  PURPOSE:     Object entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

//#define MTA_USE_BUILDINGS_AS_OBJECTS

// GTA uses this to pass to CFileLoader::LoadObjectInstance the info it wants to load
struct CFileObjectInstance
{
    float x;
    float y;
    float z;
    float rx;
    float ry;
    float rz;
    float rr; // = 1
    DWORD modelId;
    DWORD areaNumber;
    long flags; // = -1
};

CObjectSA::CObjectSA(CObjectSAInterface * objectInterface)
{
    DEBUG_TRACE("CObjectSA::CObjectSA(CObjectSAInterface * objectInterface)");
    this->SetInterface(objectInterface);
    m_ucAlpha = 255;
}

CObjectSA::CObjectSA( DWORD dwModel )
{
    DEBUG_TRACE("CObjectSA::CObjectSA( DWORD dwModel )");

    CWorldSA * world = (CWorldSA *)pGame->GetWorld();

    DWORD dwThis = 0;
    
#ifdef MTA_USE_BUILDINGS_AS_OBJECTS

    DWORD dwFunc = 0x538090; // CFileLoader__LoadObjectInstance
    CFileObjectInstance fileLoader;
    memset(&fileLoader, 0, sizeof(CFileObjectInstance));
    fileLoader.modelId = dwModel;
    fileLoader.rr = 1;
    fileLoader.areaNumber = 0;
    fileLoader.flags = -1;

    _asm
    {
        push    0
        lea     ecx, fileLoader
        push    ecx
        call    dwFunc
        add     esp, 8
        mov     dwThis, eax
    }

    this->SetInterface((CEntitySAInterface*)dwThis);
    
    *(DWORD *)(0xBCC0E0) = dwThis; // dw_buildings_created_this_scene
    *(DWORD *)0xBCC0D8 = 1; // dw_current_ipl_line

    dwFunc = 0x404DE0; // CIplStore__SetupRelatedIpls
    DWORD dwTemp = 0;
    char szTemp[255];
    strcpy(szTemp, "moo");

    _asm
    {
        push    0xBCC0E0
        push    -1
        lea     eax, szTemp
        push    eax
        call    dwFunc
        add     esp, 0xC
        mov     dwTemp, eax
    }

//    _asm int 3
    dwFunc = 0x5B51E0; // AddBuildingInstancesToWorld
    _asm
    {
        push    dwTemp
        call    dwFunc
        add     esp, 4
    }

    dwFunc = 0x405110; // CIplStore__RemoveRelatedIpls
    _asm
    {
        push    -1
        call    dwFunc
        add     esp, 4
    }

    // VITAL to get colmodels to appear
    // this gets the level for a colmodel (colmodel+40)
    dwFunc = 0x4107A0;
    _asm
    {
        mov     eax, dwModel
        mov     eax, 0xA9B0C8[eax*4]
        mov     eax, [eax+20]
        movzx   eax, byte ptr [eax+40]
        push    eax
        call    dwFunc
        add     esp, 4
    }

#else


    DWORD CObjectCreate = FUNC_CObject_Create;  
    DWORD dwObjectPtr = 0;
    _asm
    {
        push    1
        push    dwModel
        call    CObjectCreate
        add     esp, 8
        mov     dwObjectPtr, eax
    }
    if ( dwObjectPtr )
    {
        this->SetInterface((CEntitySAInterface *)dwObjectPtr);

        world->Add( m_pInterface );

        // Setup some flags
        this->BeingDeleted = FALSE;
        this->DoNotRemoveFromGame = FALSE;
        *(BYTE *)(dwObjectPtr + 316) = 6;   // Related to moving stuff (eg: fire hydrants, default is 2)
        m_pInterface->bStreamingDontDelete = true;
    }
    else
    {
        // The exception handler doesn't work for some reason, so do this
        this->SetInterface ( NULL );
    }
#endif

    this->internalID = pGame->GetPools ()->GetObjectRef ( (DWORD *)this->GetInterface () );

    m_ucAlpha = 255;
}

CObjectSA::~CObjectSA( )
{
    DEBUG_TRACE("CObjectSA::~CObjectSA( )");
    //OutputDebugString("Attempting to destroy Object\n");
    if(!this->BeingDeleted && DoNotRemoveFromGame == false)
    {
        DWORD dwInterface = (DWORD)this->GetInterface();
        if ( dwInterface )
        {       
            CWorldSA * world = (CWorldSA *)pGame->GetWorld();
            world->Remove(this->GetInterface());
        
            DWORD dwThis = (DWORD)this->GetInterface();
            DWORD dwFunc = this->GetInterface()->vtbl->Remove;
            _asm
            {
                mov     ecx, dwThis
                call    dwFunc
            }

            dwFunc = this->GetInterface()->vtbl->SCALAR_DELETING_DESTRUCTOR; // we use the vtbl so we can be type independent
            _asm    
            {
                mov     ecx, dwThis
                push    1           //delete too
                call    dwFunc
            }
        
#ifdef MTA_USE_BUILDINGS_AS_OBJECTS
            DWORD dwModelID = this->internalInterface->m_nModelIndex;
            // REMOVE ref to colstore thingy
            dwFunc = 0x4107D0;
            _asm
            {
                mov     eax, dwModelID
                mov     eax, 0xA9B0C8[eax*4]
                mov     eax, [eax+20]
                movzx   eax, byte ptr [eax+40]
                push    eax
                call    dwFunc
                add     esp, 4
            }
#endif
        }

        this->BeingDeleted = true;
        ((CPoolsSA *)pGame->GetPools())->RemoveObject((CObject *)(CObjectSA *)this);

        //OutputDebugString("Destroying Object\n");
    }
}

void CObjectSA::Explode()
{
    DWORD dwFunc = FUNC_CObject_Explode;
    DWORD dwThis = (DWORD)this->GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CObjectSA::SetScale( float faScale )
{
    DWORD dwFunc = 0x4745E0;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm
    {
        push    faScale
        mov     ecx, dwThis
        call    dwFunc
    }

//  *(FLOAT *)(this->GetInterface() + 348) = fScale;
}

void CObjectSA::SetHealth ( float fHealth )
{
    *(float *)( (DWORD)this->GetInterface () + 340 ) = fHealth;
}

float CObjectSA::GetHealth ( void )
{
    return *(float *)( (DWORD)this->GetInterface () + 340 );
}

void CObjectSA::SetModelIndex ( unsigned long ulModel )
{
    // Jax: I'm not sure if using the vtbl is right (as ped and vehicle dont), but it works
    DWORD dwFunc = this->GetInterface()->vtbl->SetModelIndex;
    DWORD dwThis = (DWORD)this->GetInterface();
    _asm    
    {
        mov     ecx, dwThis
        push    ulModel
        call    dwFunc
    }
}

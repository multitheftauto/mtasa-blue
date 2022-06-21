/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingSA.cpp
 *  PURPOSE:     Building physical entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CBuildingSA::CBuildingSA(CBuildingSAInterface* objectInterface)
{
    DEBUG_TRACE("CBuildingSA::CBuildingSA(CBuildingSAInterface * objectInterface)");
    this->SetInterface(objectInterface);
}

CBuildingSA::CBuildingSA(DWORD dwModel)
{
    DEBUG_TRACE("CBuildingSA::CBuildingSA( DWORD dwModel )");
    /*
    DWORD CBuilding_operator_new = FUNC_CBuilding_operator_new;
    DWORD CBuilding_constructor = FUNC_CBuilding_Constructor;

    DWORD dwBuildingPtr = 0;
    _asm
    {
        push    SIZEOF_CBuilding
        call    CBuilding_operator_new
        add     esp, 4
        mov     dwBuildingPtr, eax
    }

    if ( dwBuildingPtr )
    {
        _asm
        {
            mov     ecx, dwBuildingPtr
            call    CBuilding_constructor
        }
    }

    this->SetInterface((CEntitySAInterface *)dwBuildingPtr);

    DWORD dwThis = (DWORD)this->GetInterface();
    DWORD dwFunc = this->GetInterface()->vtbl->SetModelIndexNoCreate;
    _asm
    {
        mov     ecx, dwThis
        push    dwModel
        call    dwFunc
    }*/

    /*dwFunc = 0x54F4C0; // CPlaceable::AllocateStaticMatrix
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }*/

    /*dwFunc = 0x533150; // setup big building
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }*/

    CWorldSA* world = (CWorldSA*)pGame->GetWorld();
    // world->Add(this->GetInterface());
    // this->GetInterface()->bIsStatic = true;

    // CBuildingSAInterface * building = (CBuildingSAInterface*)this->GetInterface();
    // building->bBackfaceCulled = true;
    // building->m_areaCode = *(DWORD *)0xB72914;
    /*dwFunc = 0x533D30; // createrwobject - fails if the game hasn't already loaded the object, and doesn't seem to show anyway
    _asm
    {
        mov     ecx, building
        call    dwFunc
    }*/

    /*building->m_pLod = (CBuildingSAInterface*)-1;
    MemPutFast < DWORD > ( 0xBCC0D8, 1 );
    MemPutFast < DWORD > ( 0xBCC0E0, (DWORD)building );
    dwFunc = 0x5B51E0;
    _asm
    {
        push    1
        call    dwFunc
        add     esp, 4
    }*/

    //    EAX=00B71848 (gta_sa.00B71848), ASCII "3999  LODtwintjail1_LAn  13  1785.976563  -1564.859375  25.25  0  0  0.04361938313  0.999048233  -1  "
    char szLoadString[255];

    sprintf(szLoadString, "%d NoName 0  0.00  0.00  0.00  0  0  0  1  -1", dwModel);

    // CFileLoader::LoadObjectInstance
    CEntitySAInterface* pEntity = ((CEntitySAInterface * (__cdecl*)(const char*))0x538690)(szLoadString);

    this->SetInterface(pEntity);

    // DWORD * Index = (DWORD *)0xBCC0D8;
    MemPutFast<CEntitySAInterface*>(0xBCC0E0, pEntity);
    MemPutFast<DWORD>(0xBCC0D8, 1);

    // CIplStore::SetupRelatedIpls
    int ipl = ((int(__cdecl*)(const char*, short, CEntity**))0x538690)("moo", -1, (CEntity**)0xBCC0E0);

    // AddBuildingInstancesToWorld
    ((void(__cdecl*)(int))0x5B51E0)(ipl);

    // CIplStore::RemoveRelatedIpls
    ((void(__cdecl*)(int))0x405110)(-1);

    // world->Add(this->GetInterface());

    // this->BeingDeleted = FALSE;
    // this->DoNotRemoveFromGame = FALSE;

    // this->internalInterface->bStreamingDontDelete = true;
    // this->internalInterface->bDontStream = true;
}

CBuildingSA::~CBuildingSA()
{
    DEBUG_TRACE("CBuildingSA::~CBuildingSA( )");
    if (!this->BeingDeleted && DoNotRemoveFromGame == false)
    {
        DWORD dwInterface = (DWORD)this->GetInterface();

        CWorldSA* world = (CWorldSA*)pGame->GetWorld();
        world->Remove(this->GetInterface(), CBuilding_Destructor);

        ((void(__thiscall*)(void*, bool))GetInterface()->vtbl->SCALAR_DELETING_DESTRUCTOR)(GetInterface(), true);

        this->BeingDeleted = true;
        //((CPoolsSA *)pGame->GetPools())->RemoveBuilding((CBuilding *)(CBuildingSA *)this);
    }
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/fx_manager.cpp
*  PURPOSE:     fx manager modification
*  DEVELOPERS:  eAi <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define HOOKPOS_FxManager_CreateFxSystem                    0x4A9BE0
#define HOOKPOS_FxManager_DestroyFxSystem                   0x4A9810
DWORD RETURN_FxManager_CreateFxSystem =                     0x4A9BE8;
DWORD RETURN_FxManager_DestroyFxSystem =                    0x4A9817;

void HOOK_FxManager_CreateFxSystem ();
void HOOK_FxManager_DestroyFxSystem ();

void fx_manager_init ( void )
{
    HookInstall(HOOKPOS_FxManager_CreateFxSystem, (DWORD)HOOK_FxManager_CreateFxSystem, 8);
    HookInstall(HOOKPOS_FxManager_DestroyFxSystem, (DWORD)HOOK_FxManager_DestroyFxSystem, 7);

    // Disable call to FxSystem_c__GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters 
    // that was causing a crash - spent ages debugging, the crash happens if you create 40 or 
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    memset((void*)0x4DCF87,0x90,6);
}

// Used to store copied pointers for explosions in the FxSystem

std::list < DWORD* > Pointers_FxSystem;

void AddFxSystemPointer ( DWORD* pPointer )
{
    Pointers_FxSystem.push_front ( pPointer );
}


void RemoveFxSystemPointer ( DWORD* pPointer )
{
    // Look through our list for the pointer
    std::list < DWORD* > ::iterator iter = Pointers_FxSystem.begin ();
    for ( ; iter != Pointers_FxSystem.end (); iter++ )
    {
        // It exists in our list?
        if ( *iter == pPointer )
        {
            // Remove it from the list over our copied matrices
            Pointers_FxSystem.erase ( iter );

            // Delete the pointer itself
            free ( pPointer );
            return;
        }
    }
}


char* szCreateFxSystem_ExplosionType = 0;
DWORD* pCreateFxSystem_Matrix = 0;
DWORD* pNewCreateFxSystem_Matrix = 0;


void _declspec(naked) HOOK_FxManager_CreateFxSystem ()
{
    _asm
    {
        // Store the explosion type
        mov eax, [esp+4]
        mov szCreateFxSystem_ExplosionType, eax

        // Store the vector
        mov eax, [esp+12]
        mov pCreateFxSystem_Matrix, eax

        // Store all the registers on the stack
        pushad
    }

    // If we got a matrix and it is an explosion type?
    if ( pCreateFxSystem_Matrix != 0 &&
         strncmp ( szCreateFxSystem_ExplosionType, "explosion", 9 ) == 0 )
    {
        // Copy the matrix so we don't crash if the owner of this matrix is deleted
        pNewCreateFxSystem_Matrix = (DWORD*) malloc ( 64 );
        memcpy ( pNewCreateFxSystem_Matrix, pCreateFxSystem_Matrix, 64 );

        // Add it to the list over FxSystem matrices we've copied
        AddFxSystemPointer ( pNewCreateFxSystem_Matrix );
    }
    else
    {
        // Use the same pointer. This is not an explosion or it is 0.
        pNewCreateFxSystem_Matrix = pCreateFxSystem_Matrix;
    }

    _asm 
    {
        // Restore the registers
        popad

        // Put the new vector back onto the stack
        mov         eax, pNewCreateFxSystem_Matrix
        mov         [esp+12], eax

        // The original code we replaced
        mov         eax, [esp+10]
        mov         edx, [esp+8]

		// Jump back to the rest of the function we hooked
        jmp         RETURN_FxManager_CreateFxSystem
    }
}


DWORD dwDestroyFxSystem_Pointer = 0;
DWORD* pDestroyFxSystem_Matrix = 0;

void _declspec(naked) HOOK_FxManager_DestroyFxSystem ()
{
    _asm
    {
        // Grab the FxSystem that's being destroyed
        mov eax, [esp+4]
        mov dwDestroyFxSystem_Pointer, eax

        // Store all the registers on the stack
        pushad
    }

    // Grab the matrix pointer in it
    pDestroyFxSystem_Matrix = *( (DWORD**) ( dwDestroyFxSystem_Pointer + 12 ) );

    // Delete it if it's in our list
    RemoveFxSystemPointer ( pDestroyFxSystem_Matrix );

    _asm 
    {
        // Restore the registers
        popad

        // The original code we replaced
        push        ecx  
        push        ebx  
        push        edi  
        mov         edi, [esp+10h] 

		// Jump back to the rest of the function we hooked
        jmp         RETURN_FxManager_DestroyFxSystem
    }
}
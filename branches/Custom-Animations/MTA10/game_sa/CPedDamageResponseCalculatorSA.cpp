/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedDamageResponseSA.cpp
*  PURPOSE:     ped damage response
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPedDamageResponseCalculatorSA::CPedDamageResponseCalculatorSA ( CEntity * pEntity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool b_1 )
{
    m_pInterface = new CPedDamageResponseCalculatorSAInterface;
    m_bDestroyInterface = true;

    DWORD dwEntityInterface = (DWORD)pEntity->GetInterface ();
    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CPedDamageResponseCalculator_Constructor;
    _asm
    {
        mov     ecx, dwThis
        push    b_1
        push    bodyPart
        push    weaponType
        push    fDamage
        push    dwEntityInterface
        call    dwFunc
    }
}


CPedDamageResponseCalculatorSA::CPedDamageResponseCalculatorSA ( CPedDamageResponseCalculatorSAInterface * pInterface )
{
    m_pInterface = pInterface;
    m_bDestroyInterface = false;
}


CPedDamageResponseCalculatorSA::~CPedDamageResponseCalculatorSA ( void )
{
    if ( m_bDestroyInterface )
    {
        DWORD dwThis = (DWORD)m_pInterface;
        DWORD dwFunc = FUNC_CPedDamageResponseCalculator_Destructor;
        _asm
        {
            mov     ecx, dwThis
            call    dwFunc
        }
        delete m_pInterface;
    }
}


void CPedDamageResponseCalculatorSA::ComputeDamageResponse ( CPed * pPed, CPedDamageResponse * pDamageResponse, bool bSpeak )
{
    DWORD dwPedInterface = ( DWORD ) pPed->GetInterface ();
    DWORD dwResponseInterface = ( DWORD ) pDamageResponse->GetInterface ();
    DWORD dwThis = ( DWORD ) m_pInterface;
    DWORD dwFunc = FUNC_CPedDamageResponseCalculator_ComputeDamageResponse;
    _asm
    {
        mov     ecx, dwThis
        push    bSpeak
        push    dwResponseInterface
        push    dwPedInterface
        call    dwFunc
    }
}
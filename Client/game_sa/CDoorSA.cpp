/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDoorSA.cpp
 *  PURPOSE:     Vehicle door entity
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDoorSA.h"

/**
 * \todo Find out what GetAngleOpenRatio actually does
 * @return FLOAT Not sure...
 */
float CDoorSA::GetAngleOpenRatio()
{
    DWORD dwFunction = FUNC_GetAngleOpenRatio;
    DWORD dwPointer = (DWORD)GetInterface();
    float fReturn = 0.0f;

    if (dwPointer != 0)
    {
        _asm
        {
            mov     ecx, dwPointer
            call    dwFunction
            fstp    fReturn
        }
    }

    return fReturn;
}

/**
 * Checks if the door is closed
 * @return BOOL TRUE if the door is closed, FALSE otherwise
 */
bool CDoorSA::IsClosed()
{
    DWORD dwFunction = FUNC_IsClosed;
    DWORD dwPointer = (DWORD)GetInterface();
    BYTE  bReturn = 0;

    if (dwPointer != 0)
    {
        _asm
        {
            mov     ecx, dwPointer
            call    dwFunction
            mov     bReturn, al
        }
    }

    return bReturn;
}

/**
 * Checks if the door is completely open
 * @return BOOL TRUE if the door is fully opened, FALSE if it is opening, is closed or is missing.
 * \todo Check what this returns if the door has been ripped off
 */
bool CDoorSA::IsFullyOpen()
{
    DWORD dwFunction = FUNC_IsFullyOpen;
    DWORD dwPointer = (DWORD)GetInterface();
    BYTE  bReturn = 0;

    if (dwPointer != 0)
    {
        _asm
        {
            mov     ecx, dwPointer
            call    dwFunction
            mov     bReturn, al
        }
    }

    return bReturn;
}

/**
 * Opens the door
 * @param fUnknown Not sure...
 * \todo Check what the parameter for Open does
 */
void CDoorSA::Open(float fOpenRatio)
{
    DWORD dwFunction = FUNC_Open;
    DWORD dwPointer = (DWORD)GetInterface();

    if (dwPointer != 0)
    {
        _asm
        {
            mov     ecx, dwPointer
            push    fOpenRatio
            call    dwFunction
        }
    }
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirectInputEvents8.h
*  PURPOSE:     Header file for DirectInput 8 events class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECTINPUTEVENTS8_H
#define __CDIRECTINPUTEVENTS8_H

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

class CDirectInputEvents8
{
    public:

    static void __stdcall   OnDirectInputDeviceCreate  ( IDirectInputDevice8 *pDevice );
    static void __stdcall   OnDirectInputDeviceDestroy ( IDirectInputDevice8 *pDevice );
};

#endif
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirectInputHookManager.h
*  PURPOSE:     Header file for DirectInput hook manager class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECTINPUTHOOKMANAGER_H
#define __CDIRECTINPUTHOOKMANAGER_H

#include "CDirectInputHook8.h"

class CDirectInputHookManager 
{
    public:
    
                        CDirectInputHookManager    ( );
                       ~CDirectInputHookManager    ( );
    
    void                ApplyHook               ( );
    void                RemoveHook              ( );

    private:

    CDirectInputHook8 *     m_pDirectInputHook8;
};

#endif
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DHookManager.h
*  PURPOSE:     Header file for Direct3D hook manager class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDIRECT3DHOOKMANAGER_H
#define __CDIRECT3DHOOKMANAGER_H

#include "CDirect3DHook9.h"
class CDirect3DHookManager
{
    public:
                    CDirect3DHookManager    ( );
                   ~CDirect3DHookManager    ( );
    
    void            ApplyHook               ( );
    void            RemoveHook              ( );

    private:

    CDirect3DHook9 *    m_pDirect3DHook9;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDebugView.h
*  PURPOSE:     Header file for debug view class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDEBUGVIEW_H
#define __CDEBUGVIEW_H

#include "CGUI.h"
#include <gui/CGUI.h>
#include "CChat.h"

class CDebugView : public CChat
{
public:
                CDebugView              ( CGUI* pManager, CVector2D & vecPosition );

    void        Draw                    ( void );
    void        Output                  ( char* szText, bool bColorCoded );
    void        Outputf                 ( bool bColorCoded, char* szText, ... );
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Videocard.h
*  PURPOSE:     Shared stuff which is videocard oriented
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include <d3d9.h>

namespace SharedUtil
{
    int             GetTotalVideoMemory         ( IDirect3DDevice9* pDevice );
    unsigned int    GetMinStreamingMemory       ( IDirect3DDevice9* pDevice );
    unsigned int    GetMaxStreamingMemory       ( IDirect3DDevice9* pDevice );
}

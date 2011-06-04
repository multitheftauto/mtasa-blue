/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/VideoCard.cpp
*  PURPOSE:     Shared stuff which is videocard oriented
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <core/VideoCard.h>

int GetTotalVideoMemory ( IDirect3DDevice9* pDevice )
{
    return pDevice->GetAvailableTextureMem () / 1024 / 1024;
}

unsigned int GetMinStreamingMemory ( IDirect3DDevice9* pDevice )
{
#ifdef MTA_DEBUG
    return 1U;
#else
    return 50U;
#endif
}

unsigned int GetMaxStreamingMemory ( IDirect3DDevice9* pDevice )
{
    return static_cast < unsigned int > ( GetTotalVideoMemory(pDevice) * 0.9f );
}

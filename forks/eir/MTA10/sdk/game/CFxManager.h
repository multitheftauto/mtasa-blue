/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFxManager.h
*  PURPOSE:     Game effects interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFxManager
#define __CFxManager

// do not include RenderWare.h into virtual logic.

class CFxSystem;

class CFxManager
{
public:
    virtual CFxSystem* CreateFxSystem ( const char * szBlueprint, const CVector & vecPosition, RwMatrix * pRwMatrixTag, unsigned char ucFlag) = 0;
    virtual void       DestroyFxSystem ( CFxSystem* pFxSystem ) = 0;
};

#endif

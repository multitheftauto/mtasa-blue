/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFxSystem.h
*  PURPOSE:     Game effects interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFxSystem
#define __CFxSystem

#include "RenderWare.h"
class CFxSystem
{
public:
    virtual void  PlayAndKill ( ) = 0;
    virtual void  SetEffectSpeed ( float fSpeed ) = 0;
    virtual float GetEffectSpeed ( ) = 0;

    virtual void  SetEffectDensity ( float fDensity ) = 0;
    virtual float GetEffectDensity ( ) = 0;

    virtual void  GetPosition ( CVector & vecPos ) = 0;
    virtual void  SetPosition ( const CVector & vecPos ) = 0;

    virtual void  GetMatrix ( CMatrix & matrix ) = 0;
    virtual void  SetMatrix ( const CMatrix & matrix ) = 0;

    virtual void* GetInterface( void ) = 0;
};

#endif

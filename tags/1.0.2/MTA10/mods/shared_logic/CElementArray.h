/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CElementArray.h
*  PURPOSE:     Element array class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CELEMENTARRAY_H
#define __CELEMENTARRAY_H

#include "CStack.h"

class CClientEntity;

class CElementIDs
{
public:
    static void             Initialize              ( void );

    static CClientEntity*   GetElement              ( ElementID ID );
    static void             SetElement              ( ElementID ID, CClientEntity* pEntity );

    static ElementID        PopClientID             ( void );
    static void             PushClientID            ( ElementID ID );

private:
    static CClientEntity*       m_Elements [MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS];
    static CStack < ElementID > m_ClientStack;
};

#endif

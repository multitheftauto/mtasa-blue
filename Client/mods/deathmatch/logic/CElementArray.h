/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CElementArray.h
 *  PURPOSE:     Element array class header
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CClientEntity;

class CElementIDs
{
public:
    static void Initialize();

    static CClientEntity* GetElement(ElementID ID);
    static void           SetElement(ElementID ID, CClientEntity* pEntity);

    static ElementID PopClientID();
    static void      PushClientID(ElementID ID);

private:
    static SFixedArray<CClientEntity*, MAX_SERVER_ELEMENTS + MAX_CLIENT_ELEMENTS> m_Elements;
    static CStack<ElementID, MAX_CLIENT_ELEMENTS - 2>                             m_ClientStack;
};

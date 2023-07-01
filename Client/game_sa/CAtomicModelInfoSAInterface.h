/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAtomicModelInfoSAInterface.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAtomicModelInfo_SA_VTBL : public CBaseModelInfo_SA_VTBL
{
public:
    DWORD SetAtomic;            // (RpAtomic*)
};

class CAtomicModelInfoSAInterface : public CBaseModelInfoSAInterface
{
};

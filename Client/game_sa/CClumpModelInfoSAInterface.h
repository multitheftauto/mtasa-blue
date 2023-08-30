/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CClumpModelInfoSAInterface.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CClumpModelInfo_SA_VTBL : public CBaseModelInfo_SA_VTBL
{
public:
    DWORD GetBoundingBox;
    DWORD SetClump;            // (RpClump*)
};

class CClumpModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    union
    {
        char*  m_animFileName;
        uint32 m_nAnimFileIndex;
    };
};

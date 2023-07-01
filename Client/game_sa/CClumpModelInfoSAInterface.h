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

class CClumpModelInfoSAInterface : public CBaseModelInfoSAInterface
{
public:
    union
    {
        char*  m_animFileName;
        uint32 m_nAnimFileIndex;
    };
};

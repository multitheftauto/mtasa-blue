/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendNodeSA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAnimBlendNodeSAInterface
{
public:
    float                                   m_theta0;
    float                                   m_theta1;
    short                                   m_endKeyFrameId;
    short                                   m_startKeyFrameId;
    float                                   m_timeDelta;
    class CAnimBlendSequenceSAInterface*    pAnimSequence;
    class CAnimBlendAssociationSAInterface* pAnimBlendAssociation;
};
static_assert(sizeof(CAnimBlendNodeSAInterface) == 0x18, "Incorrect class size: CAnimBlendNodeSAInterface");

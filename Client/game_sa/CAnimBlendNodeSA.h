/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAnimBlendNodeSA.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAnimBlendNodeSAInterface
{
public:
    float                                   field_0;
    float                                   field_4;
    short                                   field_8;
    short                                   field_A;
    float                                   field_C;
    class CAnimBlendSequenceSAInterface*    pAnimSequence;
    class CAnimBlendAssociationSAInterface* pAnimBlendAssociation;
};
static_assert(sizeof(CAnimBlendNodeSAInterface) == 0x18, "Incorrect class size: CAnimBlendNodeSAInterface");

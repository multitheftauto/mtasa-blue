/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAnimBlendSequenceSA.cpp
*  PURPOSE:		Header file for animation blend sequence class
*  DEVELOPERS:	Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CAnimBlendSequenceSA_H
#define __CAnimBlendSequenceSA_H

#include <game/CAnimBlendSequence.h>

class CAnimBlendSequenceSAInterface
{
public:
    unsigned int                    iHashKey;       // Get/SetBoneTag accesses the first? 2 bytes here
    short                           sFlags; // 4    // or16=bHasBoneTag
    short                           sNumKeyFrames; // 6
    class CAnimBlendKeyFrame *      pKeyFrames; // ?
};

class CAnimBlendSequenceSA : public CAnimBlendSequence
{
public:
                                            CAnimBlendSequenceSA   ( CAnimBlendSequenceSAInterface * pInterface )     { m_pInterface = pInterface; }

protected:
    CAnimBlendSequenceSAInterface *        m_pInterface;
};

#endif
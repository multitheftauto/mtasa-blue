/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CIFPAnimations.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include <game/CAnimBlendHierarchy.h>
#include <game/CAnimBlendSequence.h>
#include <game/CAnimManager.h>

CIFPAnimations::~CIFPAnimations()
{
    DeleteAnimations();
}

void CIFPAnimations::DeleteAnimations()
{
    CAnimManager* pAnimManager = g_pGame->GetAnimManager();
    for (auto& Animation : vecAnimations)
    {
        for (unsigned short SequenceIndex = 0; SequenceIndex < Animation.pHierarchy->GetNumSequences(); SequenceIndex++)
        {
            pAnimManager->RemoveFromUncompressedCache(Animation.pHierarchy->GetInterface());
            auto  pAnimationSequence = pAnimManager->GetCustomAnimBlendSequence(Animation.pHierarchy->GetSequence(SequenceIndex));
            void* pKeyFrames = pAnimationSequence->GetKeyFrames();
            if (!pAnimationSequence->IsBigChunkForAllSequences())
            {
                pAnimManager->FreeKeyFramesMemory(pKeyFrames);
            }
            else
            {
                if (SequenceIndex == 0)
                {
                    // All frames of all sequences are allocated on one memory block, so free that one
                    // and break the loop
                    pAnimManager->FreeKeyFramesMemory(pKeyFrames);
                    break;
                }
            }
        }
        delete Animation.pSequencesMemory;
        pAnimManager->DeleteCustomAnimHierarchyInterface(Animation.pHierarchy->GetInterface());
    }
}

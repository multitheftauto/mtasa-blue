#include <StdInc.h>
#include "game/CAnimBlendSequence.h"
#include "game/CAnimBlendHierarchy.h"

CIFPAnimations::~CIFPAnimations(void)
{
    DeleteAnimations();
}

void CIFPAnimations::DeleteAnimations(void)
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
        delete Animation.pHierarchy->GetInterface();
    }
}

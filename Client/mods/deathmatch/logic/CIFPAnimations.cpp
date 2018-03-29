#include <StdInc.h>
#include "../game_sa/CAnimBlendSequenceSA.h"

CIFPAnimations::CIFPAnimations ( void )
{

}

CIFPAnimations::~CIFPAnimations ( void )
{
    printf("~CIFPAnimations(): CIFPAnimations destoryed\n");
    DeleteAnimations ( );
}

void CIFPAnimations::DeleteAnimations ( void )
{
    CAnimManager * pAnimManager = g_pGame->GetAnimManager ( );
    for ( size_t i = 0; i < vecAnimations.size(); i++ )
    {
        IFP_Animation * ifpAnimation = &vecAnimations[i];
        
        pAnimManager->RemoveFromUncompressedCache ( &ifpAnimation->Hierarchy );

        for (unsigned short SequenceIndex = 0; SequenceIndex < ifpAnimation->Hierarchy.usNumSequences; SequenceIndex++)
        {
            auto pSequence = (CAnimBlendSequenceSAInterface*)((BYTE*)ifpAnimation->Hierarchy.pSequences + (sizeof(CAnimBlendSequenceSAInterface) * SequenceIndex));

            if ( !( (pSequence->sFlags >> 3) & 1 ) ) // If ( !OneBigChunkForAllSequences )
            {
                pAnimManager->FreeKeyFramesMemory ( pSequence->pKeyFrames ); //*(void **)(pThis + 8)); //pSequence->m_pFrames );
            }
            else
            {
                if ( SequenceIndex == 0 )
                { 
                    // All frames of all sequences are allocated on one memory block, so free that one
                    // and break the loop 
                    pAnimManager->FreeKeyFramesMemory ( pSequence->pKeyFrames );
                    break;
                }
            }
  
        }
        delete ifpAnimation->pSequencesMemory;  
    }

    printf("CIFPAnimations::DeleteAnimations: IFP Animations have been unloaded successfully!\n");

}

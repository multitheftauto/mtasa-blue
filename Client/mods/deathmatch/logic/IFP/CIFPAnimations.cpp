#include <StdInc.h>

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
    hCMemoryMgr_Free OLD_CMemoryMgr_Free = (hCMemoryMgr_Free)0x0072F430;
    auto OLD_CAnimBlendHierarchy_RemoveFromUncompressedCache = (hCAnimBlendHierarchy_RemoveFromUncompressedCache)0x004D42A0;

    for ( size_t i = 0; i < vecAnimations.size(); i++ )
    {
        IFP_Animation * ifpAnimation = &vecAnimations[i];
        
        OLD_CAnimBlendHierarchy_RemoveFromUncompressedCache ( (int)&ifpAnimation->Hierarchy );
            
        for (unsigned short SequenceIndex = 0; SequenceIndex < ifpAnimation->Hierarchy.usNumSequences; SequenceIndex++)
        {
            _CAnimBlendSequence * pSequence = (_CAnimBlendSequence*)((BYTE*)ifpAnimation->Hierarchy.pSequences + (sizeof(_CAnimBlendSequence) * SequenceIndex));

            if ( !( (pSequence->m_nFlags >> 3) & 1 ) ) // If ( !OneBigChunkForAllSequences )
            {
                OLD_CMemoryMgr_Free ( pSequence->m_pFrames ); //*(void **)(pThis + 8)); //pSequence->m_pFrames );
            }
            else
            {
                if ( SequenceIndex == 0 )
                { 
                    // All frames of all sequences are allocated on one memory block, so free that one
                    // and break the loop 
                    OLD_CMemoryMgr_Free ( pSequence->m_pFrames );
                    break;
                }
            }
  
        }
        delete ifpAnimation->pSequencesMemory;  
    }

    printf("CIFPAnimations::DeleteAnimations: IFP Animations have been unloaded successfully!\n");

}

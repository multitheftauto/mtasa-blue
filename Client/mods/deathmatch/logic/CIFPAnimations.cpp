#include <StdInc.h>
#include "../game_sa/CAnimBlendSequenceSA.h"

CIFPAnimations::CIFPAnimations ( void )
{

}

CIFPAnimations::~CIFPAnimations ( void )
{
    printf("~CIFPAnimations(): Going to call DeleteAnimations\n");
    DeleteAnimations ( );
}

void CIFPAnimations::DeleteAnimations ( void )
{
    CAnimManager * pAnimManager = g_pGame->GetAnimManager ( );
    for ( auto it = vecAnimations.begin(); it != vecAnimations.end(); ++it ) 
    {    
        auto pAnimationHierarchy = pAnimManager->GetCustomAnimBlendHierarchy ( &it->Hierarchy );
        for (unsigned short SequenceIndex = 0; SequenceIndex < pAnimationHierarchy->GetNumSequences ( ); SequenceIndex++)
        {
            pAnimManager->RemoveFromUncompressedCache ( pAnimationHierarchy->GetInterface ( ) );
            auto pAnimationSequence = pAnimManager->GetCustomAnimBlendSequence ( pAnimationHierarchy->GetSequence ( SequenceIndex ) );
            void * pKeyFrames = pAnimationSequence->GetKeyFrames ( );
            if ( !pAnimationSequence->IsBigChunkForAllSequences ( ) )
            {
               pAnimManager->FreeKeyFramesMemory ( pKeyFrames ); 
            }
            else
            {
                if ( SequenceIndex == 0 )
                { 
                    // All frames of all sequences are allocated on one memory block, so free that one
                    // and break the loop 
                    pAnimManager->FreeKeyFramesMemory ( pKeyFrames ); 
                    break;
                }
            }
        }
        delete it->pSequencesMemory;  
    }
    printf("CIFPAnimations::DeleteAnimations: IFP Animations have been unloaded successfully!\n");
}

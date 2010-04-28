/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointsSA.cpp
*  PURPOSE:     Checkpoint entity manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// constructor
CCheckpointsSA::CCheckpointsSA()
{
    DEBUG_TRACE("CCheckpointsSA::CCheckpointsSA()");
    for(int i = 0;i< MAX_CHECKPOINTS;i++)
        this->Checkpoints[i] = new CCheckpointSA((CCheckpointSAInterface *)(ARRAY_CHECKPOINTS + i * sizeof(CCheckpointSAInterface)));
}

CCheckpointsSA::~CCheckpointsSA ( void )
{
    for ( int i = 0; i < MAX_CHECKPOINTS; i++ )
    {
        delete Checkpoints [i];
    }
}

/**
 * \todo Update default color to SA's orange instead of VC's pink
 */
CCheckpoint * CCheckpointsSA::CreateCheckpoint(DWORD Identifier, WORD wType, CVector * vecPosition, CVector * vecPointDir, FLOAT fSize, FLOAT fPulseFraction, const SColor color)
{
    DEBUG_TRACE("CCheckpoint * CCheckpointsSA::CreateCheckpoint(DWORD Identifier, DWORD wType, CVector * vecPosition, CVector * vecPointDir, FLOAT fSize, FLOAT fPulseFraction, RGBA Color)");
    /*
    static CCheckpoint  *PlaceMarker(unsigned int nIdentifier, unsigned short nType, CVector &vecPosition, CVector &pointDir,
    float fSize, unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned short nPeriod, float fPulseFrac, short nRotRate);
    */

    CCheckpoint * Checkpoint = FindFreeMarker();
    if(Checkpoint)
    {
        ((CCheckpointSA *)(Checkpoint))->SetIdentifier ( Identifier );
        ((CCheckpointSA *)(Checkpoint))->Activate();
        ((CCheckpointSA *)(Checkpoint))->SetType(wType);
        Checkpoint->SetPosition(vecPosition);
        Checkpoint->SetPointDirection(vecPointDir);
        Checkpoint->SetSize(fSize);
        Checkpoint->SetColor(color);
        Checkpoint->SetPulsePeriod(1024);
        ((CCheckpointSA *)(Checkpoint))->SetPulseFraction(fPulseFraction);
        Checkpoint->SetRotateRate(1);

        return Checkpoint;
    }

/*  DWORD dwFunc = FUNC_CCheckpoints__PlaceMarker;
    DWORD dwReturn = 0;
    _asm
    {
        push    1               // rotate rate
        push    fPulseFraction  // pulse
        push    1024            // period
        push    255             // alpha
        push    0               // blue
        push    0               // green
        push    255             // red
        push    fSize           // size
        push    vecPointDir     // point direction
        push    vecPosition     // position
        push    dwType          // type
        push    Identifier      // identifier
        call    dwFunc
        mov     dwReturn, eax
        add     esp, 0x30
    }
    
    if(dwReturn)
    {
        for(int i = 0; i < MAX_CHECKPOINTS; i++)
        {
            if(Checkpoints[i]->GetInterface() == (CCheckpointSAInterface *)dwReturn)
                return Checkpoints[i];
        }
    }*/

    return NULL;
}

CCheckpoint * CCheckpointsSA::FindFreeMarker()
{
    DEBUG_TRACE("CCheckpoint * CCheckpointsSA::FindFreeMarker()");
    for(int i = 0; i<MAX_CHECKPOINTS;i++)
    {
        if(!Checkpoints[i]->IsActive()) 
            return Checkpoints[i];
    }
    return NULL;
}

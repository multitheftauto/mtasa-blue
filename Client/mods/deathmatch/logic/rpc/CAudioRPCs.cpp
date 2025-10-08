/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CAudioRPCs.cpp
 *  PURPOSE:     Audio remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CAudioRPCs.h"
#include "net/SyncStructures.h"

void CAudioRPCs::LoadFunctions()
{
    AddHandler(PLAY_SOUND, PlaySound, "PlaySound");
}

void CAudioRPCs::PlaySound(NetBitStreamInterface& bitStream)
{
    // Read out sound type
    //  unsigned char ucType;
    //  unsigned short usSound, usSlot;
    //  CVector vecPosition;

    //  if ( bitStream.Read ( ucType ) )
    //  {
    //      switch ( ucType )
    //      {
    //          case AUDIO_FRONTEND:
    SIntegerSync<unsigned char, 7> sound;
    if (bitStream.Read(&sound))
    {
        g_pGame->GetAudioEngine()->PlayFrontEndSound(sound);
    }
    //              break;
    //          case AUDIO_MISSION_PRELOAD:
    //              if ( bitStream.Read ( usSound ) && bitStream.Read ( usSlot ) )
    //              {
    //                  g_pCore->ChatPrintf ( "Preload %u into slot %u", false, usSound, usSlot );
    //                  g_pGame->GetAudio ()->PreloadMissionAudio ( usSound, usSlot );
    //              }
    //              break;
    //          case AUDIO_MISSION_PLAY:
    //              if ( bitStream.Read ( usSlot ) )
    //              {
    // g_pCore->ChatPrintf ( "Playing slot %u", usSlot );
    //                  if ( bitStream.Read ( vecPosition.fX ) && bitStream.Read ( vecPosition.fY ) && bitStream.Read ( vecPosition.fZ ) )
    //                      g_pGame->GetAudio ()->SetMissionAudioPosition ( &vecPosition, usSlot );

    // Play the sound if it's loaded
    //                  if ( g_pGame->GetAudio ()->GetMissionAudioLoadingStatus ( usSlot ) == 1 )
    //                      g_pGame->GetAudio ()->PlayLoadedMissionAudio ( usSlot );
    //              }
    //              break;
    //          default:
    //              break;
    //      }
    //      if ( ucType == 0 ) // Frontend sound (0)
    //      {
    //      }
    //      else // 3D sound (1)
    //      {
    //      }
    //  }
}

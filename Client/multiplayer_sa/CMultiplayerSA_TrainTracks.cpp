/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

////////////////////////////////////////////////////////////////////////
// Custom train tracks used to be driven by a hook that replaced CTrain::ProcessControl outright,
// because the track ID indexes the game's three fixed 4-entry track arrays with no bounds check.
//
// That is now solved further down, in CTrainTrackManagerSA: the arrays themselves are relocated to
// larger ones, so the original ProcessControl runs untouched for a custom track and keeps all the
// behaviour a reimplementation would have had to redo by hand - engine and brake audio, throttle
// response, camera shake and derailing.
//
// Nothing is hooked here anymore. The file stays as the place to put train track hooks if the
// relocation ever needs supplementing.
////////////////////////////////////////////////////////////////////////

void CMultiplayerSA::InitHooks_TrainTracks()
{
}

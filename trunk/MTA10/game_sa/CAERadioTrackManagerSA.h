/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CAERadioTrackManagerSA.cpp
*  PURPOSE:		Header file for audio entity radio track manager class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_AERADIOTRACKMANAGER
#define __CGAMESA_AERADIOTRACKMANAGER

#include "Common.h"
#include <game/CAERadioTrackManager.h>

#define FUNC_CAERadioTrackManager_GetCurrentRadioStationID	    0x4E83F0
#define FUNC_CAERadioTrackManager_IsVehicleRadioActive		    0x4E9800
#define FUNC_CAERadioTrackManager_GetRadioStationName		    0x4E9E10
#define FUNC_CAERadioTrackManager_IsRadioOn					    0x4E8350
#define FUNC_CAERadioTrackManager_SetBassSetting				0x4E82F0
#define FUNC_CAERadioTrackManager_Reset						    0x4E7F80
#define FUNC_CAERadioTrackManager_StartRadio					0x4EB3C0

#define CLASS_CAERadioTrackManager		                        0x8CB6F8

class CAERadioTrackManagerSAInterface
{
public:
};

class CAERadioTrackManagerSA : public CAERadioTrackManager
{
public:
                CAERadioTrackManagerSA          ( CAERadioTrackManagerSAInterface * pInterface )    { m_pInterface = pInterface; }

	BYTE	    GetCurrentRadioStationID        ( void );
	BYTE	    IsVehicleRadioActive            ( void );
	char *      GetRadioStationName             ( BYTE bStationID );
	BOOL	    IsRadioOn                       ( void );
	VOID	    SetBassSetting                  ( DWORD dwBass );
	VOID	    Reset                           ( void );
	VOID	    StartRadio                      ( BYTE bStationID, BYTE bUnknown );

private:
    CAERadioTrackManagerSAInterface *   m_pInterface;
};

#endif
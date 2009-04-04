/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CAERadioTrackManager.h
*  PURPOSE:		Radio track audio entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CAERADIOTRACKMANAGER
#define __CGAME_CAERADIOTRACKMANAGER

#include <windows.h>

class CAERadioTrackManager
{
public:
	virtual BYTE	GetCurrentRadioStationID()=0;
	virtual BYTE	IsVehicleRadioActive()=0;
	virtual char	* GetRadioStationName(BYTE bStationID)=0;
	virtual BOOL	IsRadioOn()=0;
	virtual VOID	SetBassSetting(DWORD dwBass)=0;
	virtual VOID	Reset()=0;
	virtual VOID	StartRadio(BYTE bStationID, BYTE bUnknown)=0;
};

#endif
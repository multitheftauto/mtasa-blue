/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CTrackTrackManager.h
*  PURPOSE:     Train Track Manager Interface

*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CTRAINTRACKMANAGER_H
#define __CTRAINTRACKMANAGER_H

class CTrainTrackManager
{
public:
    virtual CTrainTrack *       GetTrainTrack                   ( unsigned char ucTrack ) = 0;
    virtual void                Initialise                      ( void ) = 0;
    virtual void                ResetTracks                     ( void ) = 0;

    virtual CTrainTrack *       CreateTrainTrack                ( unsigned int uiNodes, unsigned char ucTrackID, bool bLinkedLastNodes ) = 0;
    virtual bool                DestroyTrainTrack               ( DWORD dwTrackID ) = 0;

    virtual float               GetRailLength                   ( unsigned char ucTrack ) = 0;
    virtual DWORD               GetNumberOfRailNodes            ( unsigned char ucTrack ) = 0;

    virtual bool                SetRailLength                   ( unsigned char ucTrack, float fLength ) = 0;
    virtual DWORD               SetNumberOfRailNodes            ( unsigned char ucTrack, DWORD dwTrackNodes ) = 0;

    virtual bool                IsValid                         ( unsigned char ucTrack ) = 0;
};

#endif

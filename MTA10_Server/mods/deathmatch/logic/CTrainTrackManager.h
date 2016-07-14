/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainTrackManager.h
*  PURPOSE:     Header file for train node manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "CTrainTrack.h"

#define NUM_RAILTRACKS   4
#define MAX_TOTAL_TRACKS 50 + NUM_RAILTRACKS   // decent amount.

class CTrainTrackManager
{
public:
                        CTrainTrackManager              ( );
                        ~CTrainTrackManager             ( );
    CTrainTrack *       GetTrainTrack                   ( unsigned char ucTrack );
    void                Initialise                      ( void );
    bool                GetOriginalRailNode             ( unsigned char ucTrack, unsigned short usNode, SRailNode * pNode );
    void                ResetTracks                     ( void );
    float               GetRailLength                   ( unsigned char ucTrack )                           { return m_fRailTrackLengths[ucTrack]; };
    DWORD               GetNumberOfRailNodes            ( unsigned char ucTrack )                           { return m_dwNumberOfTrackNodes[ucTrack]; };

    bool                SetRailLength                   ( unsigned char ucTrack, float fLength )            { m_fRailTrackLengths[ucTrack] = fLength; return true; };
    DWORD               SetNumberOfRailNodes            ( unsigned char ucTrack, DWORD dwTrackNodes )       { return m_dwNumberOfTrackNodes[ucTrack] = dwTrackNodes; };

    CTrainTrack *       CreateTrainTrack                ( const std::vector<CVector>& vecNodeList, CElement * pParent, CXMLNode * pNode );


    bool                UnreferenceTrainTrack           ( DWORD dwTrackID );

private:
    bool                Reallocate                      ( CTrainTrack * pTrainTrack, unsigned int uiNodes );


    CTrainTrack *                                       m_pTrainTracks[MAX_TOTAL_TRACKS];
    static std::map< unsigned int, SRailNode >          m_OriginalTrainTrackData[4];
    float                                               m_fRailTrackLengths[MAX_TOTAL_TRACKS];
    DWORD                                               m_dwNumberOfTrackNodes[MAX_TOTAL_TRACKS];
    unsigned char                                       m_dwNumberOfTracks;
    bool                                                m_bInitialised;

   
};

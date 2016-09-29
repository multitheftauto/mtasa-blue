/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CTrainTrack.h
*  PURPOSE:     Train Track Interface

*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once;

class CTrainTrack
{
public:
    virtual bool                            SetRailNodePosition             ( unsigned int uiNode, CVector vecPosition ) = 0;
    virtual bool                            GetRailNodePosition             ( unsigned int uiNode, CVector& vecPosition ) = 0;
    virtual bool                            GetRailNodeDistance             ( unsigned int uiNode, float& fDistance ) = 0;

    virtual bool                            SetTrackLength                  ( float fLength ) = 0;
    virtual float                           GetTrackLength                  ( void ) = 0;

    virtual bool                            SetNumberOfNodes                ( unsigned int uiNumberOfNodes ) = 0;
    virtual unsigned int                    GetNumberOfNodes                ( void ) = 0;

    virtual unsigned char                   GetTrackID                      ( void ) = 0;

    virtual void                            SetLastNodesLinked              ( bool bLinked ) = 0;

};

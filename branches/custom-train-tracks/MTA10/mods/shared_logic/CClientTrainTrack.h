/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTrainTrack.h
*  PURPOSE:     Track Track class header
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#ifndef __CCLIENTTRAINTRACK_H
#define __CCLIENTTRAINTRACK_H

#include "CClientEntity.h"

class CClientTrainTrack: public CClientEntity
{
public:
                                    CClientTrainTrack               ( class CClientManager* pManager, ElementID ID, unsigned int uiNodes, unsigned char ucTrackID, bool bLinkedLastNode );
                                    ~CClientTrainTrack              ( void );

    virtual eClientEntityType       GetType                         ( void ) const              { return CCLIENTTRAINTRACK; }

    bool                            SetNodePosition                 ( unsigned int uiNode, CVector vecPosition );
    bool                            GetNodePosition                 ( unsigned int uiNode, CVector& vecPosition );
    bool                            SetTrackLength                  ( float fLength );
    float                           GetTrackLength                  ( void );
    bool                            SetNumberOfNodes                ( unsigned int uiNumberOfNodes );
    unsigned int                    GetNumberOfNodes                ( void );
    unsigned char                   GetTrackID                      ( void );

    bool                            SetLastNodesLinked              ( bool bLinked );

    // Sorta a hack that these are required by CClientEntity
    void                            Unlink                          ( void ) {};
    void                            GetPosition                     ( CVector& vecPosition ) const {};
    void                            SetPosition                     ( const CVector& vecPosition ) {};


private:
    CTrainTrack *                   m_pTrainTrack;
};

#endif

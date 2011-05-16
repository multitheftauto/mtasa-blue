/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamElement.h
*  PURPOSE:     Streamed entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#ifndef __CClientStreamElement_H
#define __CClientStreamElement_H

#include "CClientEntity.h"
class CClientStreamer;
class CClientStreamSector;
class CClientStreamSectorRow;

class CClientStreamElement : public CClientEntity
{
    friend class CClientStreamer;
public:
                                CClientStreamElement        ( CClientStreamer * pStreamer, ElementID ID );
                                ~CClientStreamElement       ( void );

    void                        UpdateStreamPosition        ( const CVector & vecPosition );
    CVector                     GetStreamPosition           ( void )                            { return m_vecStreamPosition; }
    bool                        IsStreamedIn                ( void )                            { return m_bStreamedIn; }
    void                        InternalStreamIn            ( bool bInstantly );
    void                        InternalStreamOut           ( void );
    virtual void                StreamIn                    ( bool bInstantly ) = 0;
    virtual void                StreamOut                   ( void ) = 0;
    virtual void                NotifyCreate                ( void );
    void                        NotifyUnableToCreate        ( void );
    void                        AddStreamReference          ( bool bScript = false );
    void                        RemoveStreamReference       ( bool bScript = false );
    unsigned short              GetStreamReferences         ( bool bScript = false );
    unsigned long               GetTotalStreamReferences    ( void )                            { return m_usStreamReferences + m_usStreamReferencesScript; }
    void                        StreamOutForABit            ( void );
    void                        SetDimension                ( unsigned short usDimension );
    virtual CSphere             GetWorldBoundingSphere      ( void );
    float                       GetDistanceToBoundingBoxSquared ( const CVector& vecPosition );

    bool                        IsStreamingCompatibleClass  ( void )                            { return true; };

private:
    
    CClientStreamer *           m_pStreamer;
    CVector                     m_vecStreamPosition;    
    bool                        m_bAttemptingToStreamIn;
    bool                        m_bStreamedIn;
    unsigned short              m_usStreamReferences, m_usStreamReferencesScript;
public:
    float                       m_fCachedRadius;
    int                         m_iCachedRadiusCounter;
    CVector                     m_vecCachedBoundingBox[2];
    int                         m_iCachedBoundingBoxCounter;
};

#endif

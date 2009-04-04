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
    friend CClientStreamer;
public:
                                CClientStreamElement        ( CClientStreamer * pStreamer, ElementID ID );
                                ~CClientStreamElement       ( void );

    void                        UpdateStreamPosition        ( const CVector & vecPosition );
    CVector                     GetStreamPosition           ( void )                            { return m_vecStreamPosition; }
    CClientStreamSectorRow *    GetStreamRow                ( void )                            { return m_pStreamRow; }
    CClientStreamSector *       GetStreamSector             ( void )                            { return m_pStreamSector; }
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
    float                       GetExpDistance              ( void )                            { return m_fExpDistance; }
    
	bool						IsStreamingCompatibleClass  ( void )							{ return true; };

private:
    void                        SetStreamRow                ( CClientStreamSectorRow * pRow )   { m_pStreamRow = pRow; }
    void                        SetStreamSector             ( CClientStreamSector * pSector )   { m_pStreamSector = pSector; }
    void                        SetExpDistance              ( float fDistance )                 { m_fExpDistance = fDistance; }
    
    CClientStreamer *           m_pStreamer;
    CClientStreamSectorRow *    m_pStreamRow;
    CClientStreamSector *       m_pStreamSector;
    CVector                     m_vecStreamPosition;    
    float                       m_fExpDistance;
    bool                        m_bAttemptingToStreamIn;
    bool                        m_bStreamedIn;
    unsigned short              m_usStreamReferences, m_usStreamReferencesScript;
};

#endif

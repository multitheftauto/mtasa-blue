/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientStreamElement.h
 *  PURPOSE:     Streamed entity class header
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"
class CClientStreamer;
class CClientStreamSector;
class CClientStreamSectorRow;

class CClientStreamElement : public CClientEntity
{
    DECLARE_CLASS(CClientStreamElement, CClientEntity)
    friend class CClientStreamer;

public:
    CClientStreamElement(CClientStreamer* pStreamer, ElementID ID);
    ~CClientStreamElement();

    void                    UpdateStreamPosition(const CVector& vecPosition);
    CVector                 GetStreamPosition() { return m_vecStreamPosition; }
    CClientStreamSectorRow* GetStreamRow() { return m_pStreamRow; }
    CClientStreamSector*    GetStreamSector() { return m_pStreamSector; }
    bool                    IsStreamedIn() { return m_bStreamedIn; }
    void                    InternalStreamIn(bool bInstantly);
    void                    InternalStreamOut();
    virtual void            StreamIn(bool bInstantly) = 0;
    virtual void            StreamOut() = 0;
    virtual void            NotifyCreate();
    void                    NotifyUnableToCreate();
    void                    AddStreamReference(bool bScript = false);
    void                    RemoveStreamReference(bool bScript = false);
    unsigned short          GetStreamReferences(bool bScript = false);
    unsigned long           GetTotalStreamReferences() { return m_usStreamReferences + m_usStreamReferencesScript; }
    void                    StreamOutForABit();
    void                    SetDimension(unsigned short usDimension) override;
    float                   GetExpDistance() { return m_fExpDistance; }
    virtual CSphere         GetWorldBoundingSphere();
    float                   GetDistanceToBoundingBoxSquared(const CVector& vecPosition);

    bool IsStreamingCompatibleClass() { return true; };

    virtual bool IsVisibleInAllDimensions() { return false; };

private:
    void SetStreamRow(CClientStreamSectorRow* pRow) { m_pStreamRow = pRow; }
    void SetStreamSector(CClientStreamSector* pSector) { m_pStreamSector = pSector; }
    void SetExpDistance(float fDistance) { m_fExpDistance = fDistance; }
    
    CClientStreamSectorRow* m_pStreamRow;
    CClientStreamSector*    m_pStreamSector;
    CVector                 m_vecStreamPosition;
    float                   m_fExpDistance;
    unsigned short          m_usStreamReferences, m_usStreamReferencesScript;

protected:
    CClientStreamer*        m_pStreamer;
    bool                    m_bStreamedIn;
    bool                    m_bAttemptingToStreamIn;

public:
    float                   m_fCachedRadius;
    int                     m_iCachedRadiusCounter;
    SFixedArray<CVector, 2> m_vecCachedBoundingBox;
    int                     m_iCachedBoundingBoxCounter;
};

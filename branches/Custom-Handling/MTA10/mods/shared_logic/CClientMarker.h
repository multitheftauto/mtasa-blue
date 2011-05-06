/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientMarker.h
*  PURPOSE:     Marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTMARKER_H
#define __CCLIENTMARKER_H

#include "CClientStreamElement.h"
#include "CClientMarkerCommon.h"
#include "CClient3DMarker.h"
#include "CClientCheckpoint.h"
#include "CClientCorona.h"
#include "CClientColShape.h"
#include "CClientColCallback.h"

class CClientMarkerManager;

class CClientMarker: public CClientStreamElement, private CClientColCallback
{
    friend class CClientMarkerManager;
    friend class CClientColShape;

public:
    enum eMarkerType
    {
        MARKER_CHECKPOINT,
        MARKER_RING,
        MARKER_CYLINDER,
        MARKER_ARROW,
        MARKER_CORONA,
        MARKER_INVALID = 0xFF,
    };

                                CClientMarker           ( class CClientManager* pManager, ElementID ID, int iMarkerType );
                                ~CClientMarker          ( void );

    void                        Unlink                  ( void );

    void                        GetPosition             ( CVector& vecPosition ) const;
    void                        SetPosition             ( const CVector& vecPosition );
    bool                        SetMatrix               ( const CMatrix & matrix );

    void                        DoPulse                 ( void );

    inline eClientEntityType    GetType                 ( void ) const                          { return CCLIENTMARKER; }

    CClientMarker::eMarkerType  GetMarkerType           ( void ) const;
    void                        SetMarkerType           ( CClientMarker::eMarkerType eType );

    class CClient3DMarker*      Get3DMarker             ( void );
    class CClientCheckpoint*    GetCheckpoint           ( void );
    class CClientCorona*        GetCorona               ( void );

    bool                        IsHit                   ( const CVector& vecPosition ) const;
    bool                        IsHit                   ( CClientEntity* pEntity ) const;

    bool                        IsVisible               ( void ) const;
    void                        SetVisible              ( bool bVisible );

    SColor                      GetColor                ( void ) const;
    void                        SetColor                ( const SColor color );

    float                       GetSize                 ( void ) const;
    void                        SetSize                 ( float fSize );

    static int                  StringToType            ( const char* szString );
    static bool                 TypeToString            ( unsigned int uiType, char* szString );

    static bool                 IsLimitReached          ( void );

    inline CClientColShape *    GetColShape             ( void ) { return m_pCollision; }

    void                        Callback_OnCollision    ( CClientColShape& Shape, CClientEntity& Entity );
    void                        Callback_OnLeave        ( CClientColShape& Shape, CClientEntity& Entity );

    virtual CSphere             GetWorldBoundingSphere  ( void );

protected:
    void                        StreamIn                ( bool bInstantly );
    void                        StreamOut               ( void );

private:
    void                        CreateOfType            ( int iType );

    CClientMarkerManager*       m_pMarkerManager;
    CClientMarkerCommon*        m_pMarker;

    static unsigned int         m_uiStreamedInMarkers;

    CClientColShape*            m_pCollision;
};

#endif

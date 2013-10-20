/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientMarkerCommon.h
*  PURPOSE:     Marker entity base class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTMARKERCOMMON_H
#define __CCLIENTMARKERCOMMON_H

#include "CClientEntity.h"

class CClientMarkerCommon
{
public:
    enum
    {
        CLASS_CHECKPOINT,
        CLASS_3DMARKER,
        CLASS_CORONA,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
        ICON_INVALID,
    };

                                CClientMarkerCommon     ( void );
    virtual                     ~CClientMarkerCommon    ( void );

    virtual void                DoPulse                 ( void );
    virtual unsigned int        GetMarkerType           ( void ) const = 0;

    virtual bool                IsHit                   ( const CVector& vecPosition ) const = 0;
    bool                        IsHit                   ( CClientEntity* pEntity ) const;

    virtual bool                IsVisible               ( void ) const = 0;
    virtual void                SetVisible              ( bool bVisible ) = 0;

    virtual SColor              GetColor                ( void ) const = 0;
    virtual void                SetColor                ( const SColor& color ) = 0;

    virtual void                GetPosition             ( CVector& vecPosition ) const = 0;
    virtual void                SetPosition             ( const CVector& vecPosition ) = 0;

    virtual void                GetMatrix               ( CMatrix & matrix ) = 0;
    virtual void                SetMatrix               ( CMatrix & matrix ) = 0;

    virtual float               GetSize                 ( void ) const = 0;
    virtual void                SetSize                 ( float fSize ) = 0;

    virtual void                StreamIn                ( void ) = 0;
    virtual void                StreamOut               ( void ) = 0;
};

#endif

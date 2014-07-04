/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CObject.h
*  PURPOSE:     Object entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_OBJECT
#define __CGAME_OBJECT

#include "CPhysical.h"

class CVehicle;
class CPed;
class CObjectSAInterface;

class CObject : public virtual CPhysical
{
public:
    virtual                         ~CObject            ( void ) {};

    virtual CObjectSAInterface *    GetObjectInterface  ( void ) = 0;

    virtual void                    Explode             ( void ) = 0;
    virtual void                    Break               ( void ) = 0;
    virtual void                    SetHealth           ( float fHealth ) = 0;
    virtual float                   GetHealth           ( void ) = 0;
    virtual void                    SetModelIndex       ( unsigned long ulModel ) = 0;

    virtual void                    SetAlpha            ( unsigned char ucAlpha ) = 0;
    virtual unsigned char           GetAlpha            ( ) = 0;

    virtual bool                    IsAGangTag          ( ) const = 0;
    virtual bool                    IsGlass             ( ) = 0;

    virtual void                    SetScale            ( float fX, float fY, float fZ ) = 0;
    virtual CVector*                GetScale            ( ) = 0;
    virtual void                    ResetScale          ( ) = 0;
};

#endif

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

#pragma once

#include "CPhysical.h"

class CObjectSAInterface;
class CPed;
class CVehicle;

class CObject : public virtual CPhysical
{
public:
    virtual ~CObject(){};

    virtual CObjectSAInterface* GetObjectInterface() = 0;

    virtual void  Explode() = 0;
    virtual void  Break() = 0;
    virtual void  SetHealth(float fHealth) = 0;
    virtual float GetHealth() = 0;
    virtual void  SetModelIndex(unsigned long ulModel) = 0;

    virtual void          SetPreRenderRequired(bool required) = 0;
    virtual bool          GetPreRenderRequired() = 0;
    virtual void          SetAlpha(unsigned char ucAlpha) = 0;
    virtual unsigned char GetAlpha() = 0;

    virtual bool IsAGangTag() const = 0;
    virtual bool IsGlass() = 0;

    virtual void     SetScale(float fX, float fY, float fZ) = 0;
    virtual CVector* GetScale() = 0;
    virtual void     ResetScale() = 0;
};

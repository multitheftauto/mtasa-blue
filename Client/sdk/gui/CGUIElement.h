/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <../Shared/sdk/CVector2D.h>

class CGUIElement
{
public:
    virtual ~CGUIElement(){};

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual void SetPosition(CVector2D pos) = 0;
    virtual void SetSize(CVector2D size) = 0;

    virtual CVector2D GetInitialPosition() const = 0;
    virtual CVector2D GetInitialSize() const = 0;

    virtual void ProcessPosition() = 0;
    virtual void ProcessSize() = 0;

    virtual bool IsDeleted() = 0;
};

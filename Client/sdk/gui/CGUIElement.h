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
#include "CGUITypes.h"

class CGUIElement
{
public:
    virtual ~CGUIElement(){};

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual std::string GetID() = 0;

    virtual void SetPosition(CVector2D pos, bool relative = false) = 0;
    virtual void SetSize(CVector2D size, bool relative = false) = 0;

    virtual CVector2D GetPosition() = 0;
    virtual CVector2D GetSize() = 0;

    virtual void         SetParent(CGUIElement* element) = 0;
    virtual CGUIElement* GetParent() = 0;

    virtual void AddChild(CGUIElement* child) = 0;
    virtual void RemoveChild(CGUIElement* child) = 0;

    virtual std::list<CGUIElement*> GetChildren() = 0;

    virtual CGUIType GetType() = 0;

    virtual void ProcessPosition() = 0;
    virtual void ProcessSize() = 0;

    virtual void SetFrameEnabled(bool state) = 0;
    virtual bool GetFrameEnabled() = 0;

    virtual void SetDynamicPositionEnabled(bool state) = 0;
    virtual bool GetDynamicPositionEnabled() = 0;

    virtual bool IsDeleted() = 0;
};

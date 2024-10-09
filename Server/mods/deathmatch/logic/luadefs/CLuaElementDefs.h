/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaElementDefs.h
 *  PURPOSE:     Lua element definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLuaDefs.h"
#include "CSpatialDatabase.h"

class CLuaElementDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Create/destroy
    LUA_DECLARE(createElement);
    LUA_DECLARE(destroyElement);
    LUA_DECLARE(cloneElement);

    // Is/get
    LUA_DECLARE(isElement);
    LUA_DECLARE(isElementWithinColShape);
    LUA_DECLARE(isElementWithinMarker);

    LUA_DECLARE(getElementChildren);
    LUA_DECLARE(getElementChild);
    LUA_DECLARE(getElementChildrenCount);
    LUA_DECLARE(getElementID);
    LUA_DECLARE(getElementByID);
    LUA_DECLARE(getElementByIndex);
    LUA_DECLARE(getElementParent);
    LUA_DECLARE_OOP(getElementMatrix);
    LUA_DECLARE_OOP(getElementPosition);
    LUA_DECLARE_OOP(getElementRotation);
    LUA_DECLARE_OOP(getElementVelocity);
    LUA_DECLARE_OOP(getElementTurnVelocity);
    LUA_DECLARE(getElementType);
    LUA_DECLARE(getElementsByType);
    LUA_DECLARE(getElementInterior);
    LUA_DECLARE(getElementsWithinColShape);
    static CElementResult getElementsWithinRange(CVector pos, float radius, std::optional<std::string> type, std::optional<unsigned short> interior,
                                                 std::optional<unsigned short> dimension);
    LUA_DECLARE(getElementDimension);
    LUA_DECLARE(getElementZoneName);
    LUA_DECLARE(getElementColShape);
    LUA_DECLARE(getElementAlpha);
    LUA_DECLARE(isElementDoubleSided);
    LUA_DECLARE(getElementHealth);
    LUA_DECLARE(getElementModel);
    LUA_DECLARE(isElementInWater);
    LUA_DECLARE(getElementSyncer);
    LUA_DECLARE(getElementCollisionsEnabled);
    LUA_DECLARE(isElementFrozen);
    LUA_DECLARE(getLowLODElement);
    LUA_DECLARE(isElementLowLOD);
    LUA_DECLARE(isElementCallPropagationEnabled);

    // Visible to
    LUA_DECLARE(clearElementVisibleTo);
    LUA_DECLARE(isElementVisibleTo);
    LUA_DECLARE(setElementVisibleTo);

    // Element data
    LUA_DECLARE(GetElementData);
    static CLuaArguments GetAllElementData(CElement* pElement);
    LUA_DECLARE(HasElementData);
    LUA_DECLARE(setElementData);
    LUA_DECLARE(removeElementData);
    LUA_DECLARE(addElementDataSubscriber);
    LUA_DECLARE(removeElementDataSubscriber);
    LUA_DECLARE(hasElementDataSubscriber);
    static bool IsElementOnFire(CElement* element) noexcept;

    // Attachement
    LUA_DECLARE(attachElements);
    LUA_DECLARE(detachElements);
    LUA_DECLARE(isElementAttached);
    LUA_DECLARE(getAttachedElements);
    LUA_DECLARE(getElementAttachedTo);
    LUA_DECLARE(setElementAttachedOffsets);
    LUA_DECLARE(getElementAttachedOffsets);

    // Set
    LUA_DECLARE(setElementID);
    LUA_DECLARE(setElementParent);
    LUA_DECLARE(setElementMatrix);
    LUA_DECLARE(setElementPosition);
    LUA_DECLARE_OOP(setElementRotation);
    LUA_DECLARE(setElementVelocity);
    LUA_DECLARE(setElementTurnVelocity);
    LUA_DECLARE(setElementInterior);
    LUA_DECLARE(setElementDimension);
    LUA_DECLARE(setElementAlpha);
    LUA_DECLARE(setElementDoubleSided);
    LUA_DECLARE(setElementHealth);
    LUA_DECLARE(setElementModel);
    LUA_DECLARE(setElementSyncer);
    LUA_DECLARE(setElementCollisionsEnabled);
    LUA_DECLARE(setElementFrozen);
    LUA_DECLARE(setLowLODElement);
    LUA_DECLARE(setElementCallPropagationEnabled);
    static bool SetElementOnFire(CElement* element, bool onFire) noexcept;
};

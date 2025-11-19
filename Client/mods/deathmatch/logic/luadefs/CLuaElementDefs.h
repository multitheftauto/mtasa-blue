/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaElementDefs.h
 *  PURPOSE:     Lua element definitions class header
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaElementDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Element get functions
    LUA_DECLARE(GetRootElement);
    LUA_DECLARE(IsElement);
    LUA_DECLARE(GetElementChildren);
    LUA_DECLARE(GetElementChild);
    LUA_DECLARE(GetElementChildrenCount);
    LUA_DECLARE(GetElementID);
    LUA_DECLARE(GetElementByID);
    LUA_DECLARE(GetElementByIndex);
    LUA_DECLARE(GetElementData);
    static CLuaArguments GetAllElementData(CClientEntity* pElement);
    LUA_DECLARE(HasElementData);
    LUA_DECLARE(GetElementParent);
    LUA_DECLARE_OOP(GetElementMatrix);
    LUA_DECLARE_OOP(GetElementPosition);
    LUA_DECLARE_OOP(GetElementRotation);
    LUA_DECLARE_OOP(GetElementVelocity);
    LUA_DECLARE_OOP(GetElementTurnVelocity);
    LUA_DECLARE(GetElementType);
    LUA_DECLARE(GetElementsByType);
    LUA_DECLARE(GetElementInterior);
    LUA_DECLARE(IsElementWithinColShape);
    LUA_DECLARE(IsElementWithinMarker);
    LUA_DECLARE(GetElementsWithinColShape);
    static CClientEntityResult GetElementsWithinRange(CVector pos, float radius, std::optional<std::string> type, std::optional<unsigned short> interior,
                                                      std::optional<unsigned short> dimension);
    LUA_DECLARE(GetElementDimension);
    LUA_DECLARE(GetElementZoneName);
    LUA_DECLARE_OOP(GetElementBoundingBox);
    LUA_DECLARE(GetElementRadius);
    LUA_DECLARE(IsElementAttached);
    LUA_DECLARE(GetElementAttachedTo);
    LUA_DECLARE(GetAttachedElements);
    LUA_DECLARE(GetElementDistanceFromCentreOfMassToBaseOfModel);
    LUA_DECLARE(IsElementLocal);
    LUA_DECLARE(GetElementAttachedOffsets);
    LUA_DECLARE(GetElementAlpha);
    static std::variant<bool, float> GetElementLighting(CClientEntity* entity);
    LUA_DECLARE(IsElementOnScreen);
    LUA_DECLARE(GetElementHealth);
    LUA_DECLARE(IsElementStreamedIn);
    LUA_DECLARE(IsElementStreamable);
    LUA_DECLARE(GetElementModel);
    LUA_DECLARE(GetElementColShape);
    LUA_DECLARE(IsElementInWater);
    LUA_DECLARE(IsElementSyncer);
    LUA_DECLARE(IsElementCollidableWith);
    LUA_DECLARE(IsElementDoubleSided);
    LUA_DECLARE(GetElementCollisionsEnabled);
    LUA_DECLARE(IsElementFrozen);
    LUA_DECLARE(GetLowLodElement);
    LUA_DECLARE(IsElementLowLod);
    LUA_DECLARE(IsElementCallPropagationEnabled);
    LUA_DECLARE(IsElementWaitingForGroundToLoad);
    static bool IsElementOnFire(CClientEntity* entity) noexcept;

    // Element set funcs
    LUA_DECLARE(CreateElement);
    LUA_DECLARE(DestroyElement);
    LUA_DECLARE(SetElementID);
    LUA_DECLARE(SetElementData);
    LUA_DECLARE(RemoveElementData);
    LUA_DECLARE(SetElementMatrix);
    LUA_DECLARE(SetElementPosition);
    LUA_DECLARE_OOP(SetElementRotation);
    LUA_DECLARE(SetElementVelocity);
    LUA_DECLARE(SetElementAngularVelocity);
    LUA_DECLARE(SetElementParent);
    LUA_DECLARE(SetElementInterior);
    LUA_DECLARE(SetElementDimension);
    LUA_DECLARE(AttachElements);
    LUA_DECLARE(DetachElements);
    LUA_DECLARE(SetElementAttachedOffsets);
    LUA_DECLARE(SetElementCollisionsEnabled);
    LUA_DECLARE(SetElementAlpha);
    LUA_DECLARE(SetElementHealth);
    LUA_DECLARE(SetElementStreamable);
    LUA_DECLARE(SetElementModel);
    LUA_DECLARE(SetElementCollidableWith);
    LUA_DECLARE(SetElementDoubleSided);
    LUA_DECLARE(SetElementFrozen);
    static bool SetLowLodElement(lua_State* luaVM, CClientEntity* pEntity, std::optional<CClientEntity*> pLowLodEntity);
    LUA_DECLARE(SetElementCallPropagationEnabled);
    static bool SetElementLighting(CClientEntity* entity, float lighting);
    static bool SetElementOnFire(CClientEntity* entity, bool onFire) noexcept;

    static bool SetElementAnimation(CClientEntity* entity, std::optional<std::variant<CClientIFP*, bool>> ifpOrNil, std::optional<std::string> animationName, std::optional<std::uint16_t> flags);
    static bool SetElementAnimationSpeed(CClientEntity* entity, float speed);
};

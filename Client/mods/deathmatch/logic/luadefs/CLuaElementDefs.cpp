/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaElementDefs.cpp
 *  PURPOSE:     Lua element definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>
using std::list;

#define MIN_CLIENT_REQ_LOD_FOR_BUILDING "1.6.0-9.22470"

void CLuaElementDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Element get funcs
        {"getRootElement", GetRootElement},
        {"isElement", IsElement},
        {"getElementByID", GetElementByID},
        {"getElementByIndex", GetElementByIndex},
        {"getElementData", GetElementData},
        {"getAllElementData", ArgumentParserWarn<false, GetAllElementData>},
        {"getElementMatrix", GetElementMatrix},
        {"getElementPosition", GetElementPosition},
        {"getElementRotation", GetElementRotation},
        {"getElementVelocity", GetElementVelocity},
        {"getElementAngularVelocity", GetElementTurnVelocity},
        {"getElementType", GetElementType},
        {"getElementChildren", GetElementChildren},
        {"getElementChild", GetElementChild},
        {"getElementChildrenCount", GetElementChildrenCount},
        {"getElementID", GetElementID},
        {"getElementParent", GetElementParent},
        {"getElementsByType", GetElementsByType},
        {"getElementInterior", GetElementInterior},
        {"isElementWithinColShape", IsElementWithinColShape},
        {"isElementWithinMarker", IsElementWithinMarker},
        {"getElementsWithinColShape", GetElementsWithinColShape},
        {"getElementsWithinRange", ArgumentParserWarn<false, GetElementsWithinRange>},
        {"getElementDimension", GetElementDimension},
        {"getElementBoundingBox", GetElementBoundingBox},
        {"getElementRadius", GetElementRadius},
        {"isElementAttached", IsElementAttached},
        {"getElementAttachedTo", GetElementAttachedTo},
        {"getAttachedElements", GetAttachedElements},
        {"getElementDistanceFromCentreOfMassToBaseOfModel", GetElementDistanceFromCentreOfMassToBaseOfModel},
        {"isElementLocal", IsElementLocal},
        {"hasElementData", HasElementData},
        {"getElementAttachedOffsets", GetElementAttachedOffsets},
        {"getElementAlpha", GetElementAlpha},
        {"getElementLighting", ArgumentParser<GetElementLighting>},
        {"isElementOnScreen", IsElementOnScreen},
        {"getElementHealth", GetElementHealth},
        {"getElementModel", GetElementModel},
        {"isElementStreamedIn", IsElementStreamedIn},
        {"isElementStreamable", IsElementStreamable},
        {"getElementColShape", GetElementColShape},
        {"isElementInWater", IsElementInWater},
        {"isElementSyncer", IsElementSyncer},
        {"isElementCollidableWith", IsElementCollidableWith},
        {"isElementDoubleSided", IsElementDoubleSided},
        {"getElementCollisionsEnabled", GetElementCollisionsEnabled},
        {"isElementFrozen", IsElementFrozen},
        {"getLowLODElement", GetLowLodElement},
        {"isElementLowLOD", IsElementLowLod},
        {"isElementCallPropagationEnabled", IsElementCallPropagationEnabled},
        {"isElementWaitingForGroundToLoad", IsElementWaitingForGroundToLoad},
        {"isElementOnFire", ArgumentParser<IsElementOnFire>},

        // Element set funcs
        {"createElement", CreateElement},
        {"destroyElement", DestroyElement},
        {"setElementID", SetElementID},
        {"setElementParent", SetElementParent},
        {"setElementData", SetElementData},
        // {"removeElementData", RemoveElementData}, TODO Clientside
        {"setElementMatrix", SetElementMatrix},
        {"setElementPosition", SetElementPosition},
        {"setElementRotation", SetElementRotation},
        {"setElementVelocity", SetElementVelocity},
        {"setElementAngularVelocity", SetElementAngularVelocity},
        {"setElementInterior", SetElementInterior},
        {"setElementDimension", SetElementDimension},
        {"attachElements", AttachElements},
        {"detachElements", DetachElements},
        {"setElementAttachedOffsets", SetElementAttachedOffsets},
        {"setElementAlpha", SetElementAlpha},
        {"setElementHealth", SetElementHealth},
        {"setElementModel", SetElementModel},
        {"setElementStreamable", SetElementStreamable},
        {"setElementCollisionsEnabled", SetElementCollisionsEnabled},
        {"setElementCollidableWith", SetElementCollidableWith},
        {"setElementDoubleSided", SetElementDoubleSided},
        {"setElementFrozen", SetElementFrozen},
        {"setLowLODElement", ArgumentParser<SetLowLodElement>},
        {"setElementCallPropagationEnabled", SetElementCallPropagationEnabled},
        {"setElementLighting", ArgumentParser<SetElementLighting>},
        {"setElementOnFire", ArgumentParser<SetElementOnFire>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaElementDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // Class functions
    lua_classfunction(luaVM, "getByID", "getElementByID");
    lua_classfunction(luaVM, "getAllByType", "getElementsByType");

    // Action functions
    lua_classfunction(luaVM, "create", "createElement");

    lua_classfunction(luaVM, "attach", "attachElements");
    lua_classfunction(luaVM, "detach", "detachElements");
    lua_classfunction(luaVM, "destroy", "destroyElement");

    // Get functions
    lua_classfunction(luaVM, "getCollisionsEnabled", "getElementCollisionsEnabled");
    lua_classfunction(luaVM, "isWithinColShape", "isElementWithinColShape");
    lua_classfunction(luaVM, "isWithinMarker", "isElementWithinMarker");
    lua_classfunction(luaVM, "isInWater", "isElementInWater");
    lua_classfunction(luaVM, "isFrozen", "isElementFrozen");
    lua_classfunction(luaVM, "isLowLOD", "isElementLowLOD");
    lua_classfunction(luaVM, "isDoubleSided", "isElementDoubleSided");
    lua_classfunction(luaVM, "isCollidableWith", "isElementCollidableWith");
    lua_classfunction(luaVM, "isAttached", "isElementAttached");
    lua_classfunction(luaVM, "isCallPropagationEnabled", "isElementCallPropagationEnabled");
    lua_classfunction(luaVM, "isWaitingForGroundToLoad", "isElementWaitingForGroundToLoad");
    lua_classfunction(luaVM, "isOnScreen", "isElementOnScreen");
    lua_classfunction(luaVM, "isStreamedIn", "isElementStreamedIn");
    lua_classfunction(luaVM, "isStreamable", "isElementStreamable");
    lua_classfunction(luaVM, "isLocal", "isElementLocal");
    lua_classfunction(luaVM, "hasData", "hasElementData");
    lua_classfunction(luaVM, "isSyncer", "isElementSyncer");
    lua_classfunction(luaVM, "getChildren", "getElementChildren");
    lua_classfunction(luaVM, "getChild", "getElementChild");
    lua_classfunction(luaVM, "getChildrenCount", "getElementChildrenCount");
    lua_classfunction(luaVM, "getID", "getElementID");
    lua_classfunction(luaVM, "getParent", "getElementParent");
    lua_classfunction(luaVM, "getBoundingBox", OOP_GetElementBoundingBox);
    lua_classfunction(luaVM, "getPosition", OOP_GetElementPosition);
    lua_classfunction(luaVM, "getRotation", OOP_GetElementRotation);
    lua_classfunction(luaVM, "getMatrix", OOP_GetElementMatrix);
    lua_classfunction(luaVM, "getVelocity", OOP_GetElementVelocity);
    lua_classfunction(luaVM, "getAngularVelocity", OOP_GetElementTurnVelocity);
    lua_classfunction(luaVM, "getByType", "getElementsByType");
    lua_classfunction(luaVM, "getDistanceFromCentreOfMassToBaseOfModel", "getElementDistanceFromCentreOfMassToBaseOfModel");
    lua_classfunction(luaVM, "getRadius", "getElementRadius");
    lua_classfunction(luaVM, "getType", "getElementType");
    lua_classfunction(luaVM, "getInterior", "getElementInterior");
    lua_classfunction(luaVM, "getWithinColShape", "getElementsWithinColShape");
    lua_classfunction(luaVM, "getWithinRange", "getElementsWithinRange");
    lua_classfunction(luaVM, "getDimension", "getElementDimension");
    lua_classfunction(luaVM, "getColShape", "getElementColShape");
    lua_classfunction(luaVM, "getAlpha", "getElementAlpha");
    lua_classfunction(luaVM, "getLighting", "getElementLighting");
    lua_classfunction(luaVM, "getHealth", "getElementHealth");
    lua_classfunction(luaVM, "getModel", "getElementModel");
    lua_classfunction(luaVM, "getLowLOD", "getLowLODElement");
    lua_classfunction(luaVM, "getAttachedElements", "getAttachedElements");
    lua_classfunction(luaVM, "getAttachedTo", "getElementAttachedTo");
    lua_classfunction(luaVM, "getAttachedOffsets", "getElementAttachedOffsets");
    lua_classfunction(luaVM, "getData", "getElementData");
    lua_classfunction(luaVM, "getAllData", "getAllElementData");
    lua_classfunction(luaVM, "isOnFire", "isElementOnFire");

    lua_classfunction(luaVM, "setAttachedOffsets", "setElementAttachedOffsets");
    lua_classfunction(luaVM, "setData", "setElementData");
    lua_classfunction(luaVM, "setID", "setElementID");
    lua_classfunction(luaVM, "setParent", "setElementParent");
    lua_classfunction(luaVM, "setPosition", "setElementPosition");
    lua_classfunction(luaVM, "setRotation", OOP_SetElementRotation);
    lua_classfunction(luaVM, "setMatrix", "setElementMatrix");
    lua_classfunction(luaVM, "setVelocity", "setElementVelocity");
    lua_classfunction(luaVM, "setAngularVelocity", "setElementAngularVelocity");
    lua_classfunction(luaVM, "setInterior", "setElementInterior");
    lua_classfunction(luaVM, "setDimension", "setElementDimension");
    lua_classfunction(luaVM, "setAlpha", "setElementAlpha");
    lua_classfunction(luaVM, "setDoubleSided", "setElementDoubleSided");
    lua_classfunction(luaVM, "setHealth", "setElementHealth");
    lua_classfunction(luaVM, "setModel", "setElementModel");
    lua_classfunction(luaVM, "setCollisionsEnabled", "setElementCollisionsEnabled");
    lua_classfunction(luaVM, "setCollidableWith", "setElementCollidableWith");
    lua_classfunction(luaVM, "setFrozen", "setElementFrozen");
    lua_classfunction(luaVM, "setLowLOD", "setLowLODElement");
    lua_classfunction(luaVM, "setCallPropagationEnabled", "setElementCallPropagationEnabled");
    lua_classfunction(luaVM, "setStreamable", "setElementStreamable");
    lua_classfunction(luaVM, "setLighting", "setElementLighting");
    lua_classfunction(luaVM, "setOnFire", "setElementOnFire");

    lua_classvariable(luaVM, "callPropagationEnabled", "setElementCallPropagationEnabled", "isElementCallPropagationEnabled");
    lua_classvariable(luaVM, "waitingForGroundToLoad", NULL, "isElementWaitingForGroundToLoad");
    lua_classvariable(luaVM, "onScreen", NULL, "isElementOnScreen");
    lua_classvariable(luaVM, "streamedIn", NULL, "isElementStreamedIn");
    lua_classvariable(luaVM, "streamable", "setElementStreamable", "isElementStreamable");
    lua_classvariable(luaVM, "localElement", NULL, "isElementLocal");
    lua_classvariable(luaVM, "id", "setElementID", "getElementID");
    lua_classvariable(luaVM, "type", NULL, "getElementType");
    lua_classvariable(luaVM, "colShape", NULL, "getElementColShape");
    lua_classvariable(luaVM, "parent", "setElementParent", "getElementParent");
    lua_classvariable(luaVM, "attachedTo", "attachElements", "getElementAttachedTo");
    lua_classvariable(luaVM, "attached", NULL, "isElementAttached");
    lua_classvariable(luaVM, "health", "setElementHealth", "getElementHealth");
    lua_classvariable(luaVM, "alpha", "setElementAlpha", "getElementAlpha");
    lua_classvariable(luaVM, "doubleSided", "setElementDoubleSided", "isElementDoubleSided");
    lua_classvariable(luaVM, "model", "setElementModel", "getElementModel");
    lua_classvariable(luaVM, "syncedByLocalPlayer", NULL, "isElementSyncer");
    lua_classvariable(luaVM, "collisions", "setElementCollisionsEnabled", "getElementCollisionsEnabled");
    lua_classvariable(luaVM, "frozen", "setElementFrozen", "isElementFrozen");
    lua_classvariable(luaVM, "inWater", NULL, "isElementInWater");
    lua_classvariable(luaVM, "lowLOD", "setLowLODElement", "getLowLODElement");
    lua_classvariable(luaVM, "dimension", "setElementDimension", "getElementDimension");
    lua_classvariable(luaVM, "interior", "setElementInterior", "getElementInterior");
    lua_classvariable(luaVM, "distanceFromCentreOfMassToBaseOfModel", NULL, "getElementDistanceFromCentreOfMassToBaseOfModel");
    lua_classvariable(luaVM, "radius", NULL, "getElementRadius");
    lua_classvariable(luaVM, "childrenCount", NULL, "getElementChildrenCount");
    lua_classvariable(luaVM, "boundingBox", NULL, OOP_GetElementBoundingBox);
    lua_classvariable(luaVM, "position", SetElementPosition, OOP_GetElementPosition);
    lua_classvariable(luaVM, "rotation", OOP_SetElementRotation, OOP_GetElementRotation);
    lua_classvariable(luaVM, "matrix", SetElementMatrix, OOP_GetElementMatrix);
    lua_classvariable(luaVM, "velocity", SetElementVelocity, OOP_GetElementVelocity);
    lua_classvariable(luaVM, "angularVelocity", SetElementAngularVelocity, OOP_GetElementTurnVelocity);
    lua_classvariable(luaVM, "isElement", NULL, "isElement");
    lua_classvariable(luaVM, "lighting", "setElementLighting", "getElementLighting");
    lua_classvariable(luaVM, "onFire", "setElementOnFire", "isElementOnFire");
    // TODO: Support element data: player.data["age"] = 1337; <=> setElementData(player, "age", 1337)

    lua_registerclass(luaVM, "Element");
}

int CLuaElementDefs::GetRootElement(lua_State* luaVM)
{
    CClientEntity* pRoot = CStaticFunctionDefinitions::GetRootElement();

    // Return the root element
    lua_pushelement(luaVM, pRoot);
    return 1;
}

int CLuaElementDefs::IsElement(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementID(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Get its ID and return it
        const SString& strName = pEntity->GetName();
        if (strName.length() > 0)
        {
            lua_pushstring(luaVM, strName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementByID(lua_State* luaVM)
{
    SString          strID = "";
    unsigned int     uiIndex = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strID);
    argStream.ReadNumber(uiIndex, 0);

    if (!argStream.HasErrors())
    {
        // Try to find the element with that ID. Return it
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByID(strID.c_str(), uiIndex);
        if (pEntity)
        {
            lua_pushelement(luaVM, pEntity);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementByIndex(lua_State* luaVM)
{
    SString          strType = "";
    unsigned int     uiIndex = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strType);
    argStream.ReadNumber(uiIndex, 0);

    if (!argStream.HasErrors())
    {
        // Try to find it. Return it if we did.
        CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByIndex(strType.c_str(), uiIndex);
        if (pEntity)
        {
            lua_pushelement(luaVM, pEntity);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementMatrix(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;
    bool           bBadSyntax;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bBadSyntax, true);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Grab the position
        CMatrix matrix;
        if (CStaticFunctionDefinitions::GetElementMatrix(*pEntity, matrix))
        {
            // Apparently some scripts like the dirty syntax... should be 0.0f but was 1.0f post 1.3.2
            float fData = bBadSyntax == true ? 1.0f : 0.0f;

            // Return it
            lua_createtable(luaVM, 4, 0);

            // First row
            lua_createtable(luaVM, 4, 0);
            lua_pushnumber(luaVM, matrix.vRight.fX);
            lua_rawseti(luaVM, -2, 1);
            lua_pushnumber(luaVM, matrix.vRight.fY);
            lua_rawseti(luaVM, -2, 2);
            lua_pushnumber(luaVM, matrix.vRight.fZ);
            lua_rawseti(luaVM, -2, 3);
            lua_pushnumber(luaVM, fData);
            lua_rawseti(luaVM, -2, 4);
            lua_rawseti(luaVM, -2, 1);

            // Second row
            lua_createtable(luaVM, 4, 0);
            lua_pushnumber(luaVM, matrix.vFront.fX);
            lua_rawseti(luaVM, -2, 1);
            lua_pushnumber(luaVM, matrix.vFront.fY);
            lua_rawseti(luaVM, -2, 2);
            lua_pushnumber(luaVM, matrix.vFront.fZ);
            lua_rawseti(luaVM, -2, 3);
            lua_pushnumber(luaVM, fData);
            lua_rawseti(luaVM, -2, 4);
            lua_rawseti(luaVM, -2, 2);

            // Third row
            lua_createtable(luaVM, 4, 0);
            lua_pushnumber(luaVM, matrix.vUp.fX);
            lua_rawseti(luaVM, -2, 1);
            lua_pushnumber(luaVM, matrix.vUp.fY);
            lua_rawseti(luaVM, -2, 2);
            lua_pushnumber(luaVM, matrix.vUp.fZ);
            lua_rawseti(luaVM, -2, 3);
            lua_pushnumber(luaVM, fData);
            lua_rawseti(luaVM, -2, 4);
            lua_rawseti(luaVM, -2, 3);

            // Fourth row
            lua_createtable(luaVM, 4, 0);
            lua_pushnumber(luaVM, matrix.vPos.fX);
            lua_rawseti(luaVM, -2, 1);
            lua_pushnumber(luaVM, matrix.vPos.fY);
            lua_rawseti(luaVM, -2, 2);
            lua_pushnumber(luaVM, matrix.vPos.fZ);
            lua_rawseti(luaVM, -2, 3);
            lua_pushnumber(luaVM, 1.0f);
            lua_rawseti(luaVM, -2, 4);
            lua_rawseti(luaVM, -2, 4);

            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_GetElementMatrix(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CMatrix matrix;
        pEntity->GetMatrix(matrix);

        lua_pushmatrix(luaVM, matrix);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementPosition(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the position
        CVector vecPosition;
        if (CStaticFunctionDefinitions::GetElementPosition(*pEntity, vecPosition))
        {
            // Return it
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_GetElementPosition(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CVector vecPosition;
        pEntity->GetPosition(vecPosition);

        lua_pushvector(luaVM, vecPosition);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementRotation(lua_State* luaVM)
{
    //  float float float getElementRotation ( element theElement [, string rotOrder = "default" ] )
    CClientEntity*      pEntity = NULL;
    eEulerRotationOrder rotationOrder;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadEnumString(rotationOrder, EULER_DEFAULT);

    if (!argStream.HasErrors())
    {
        // Grab the rotation
        CVector vecRotation;
        if (CStaticFunctionDefinitions::GetElementRotation(*pEntity, vecRotation, rotationOrder))
        {
            // Return it
            lua_pushnumber(luaVM, vecRotation.fX);
            lua_pushnumber(luaVM, vecRotation.fY);
            lua_pushnumber(luaVM, vecRotation.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_GetElementRotation(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CMatrix matrix;
        CVector vecRotation;
        pEntity->GetMatrix(matrix);

        vecRotation = matrix.GetRotation();
        ConvertRadiansToDegrees(vecRotation);

        lua_pushvector(luaVM, vecRotation);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementVelocity(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the velocity
        CVector vecVelocity;
        if (CStaticFunctionDefinitions::GetElementVelocity(*pEntity, vecVelocity))
        {
            // Return it
            lua_pushnumber(luaVM, vecVelocity.fX);
            lua_pushnumber(luaVM, vecVelocity.fY);
            lua_pushnumber(luaVM, vecVelocity.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_GetElementVelocity(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CVector vecVelocity;
        CStaticFunctionDefinitions::GetElementVelocity(*pEntity, vecVelocity);

        lua_pushvector(luaVM, vecVelocity);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementTurnVelocity(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the turn velocity
        CVector vecTurnVelocity;
        if (CStaticFunctionDefinitions::GetElementTurnVelocity(*pEntity, vecTurnVelocity))
        {
            // Return it
            lua_pushnumber(luaVM, vecTurnVelocity.fX);
            lua_pushnumber(luaVM, vecTurnVelocity.fY);
            lua_pushnumber(luaVM, vecTurnVelocity.fZ);
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_GetElementTurnVelocity(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CVector vecTurnVelocity;
        CStaticFunctionDefinitions::GetElementTurnVelocity(*pEntity, vecTurnVelocity);

        lua_pushvector(luaVM, vecTurnVelocity);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementType(lua_State* luaVM)
{
    // Check the arg type
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Get its typename
        const char* szTypeName = pEntity->GetTypeName();
        if (szTypeName)
        {
            lua_pushstring(luaVM, szTypeName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementChildren(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Find our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (!argStream.NextIsString())
            {
                // Create a new table
                lua_newtable(luaVM);

                // Add all the elements with a matching type to it
                pEntity->GetChildren(luaVM);
                return 1;
            }
            else if (argStream.NextIsString())
            {
                SString strType = "";
                argStream.ReadString(strType);
                // Create a new table
                lua_newtable(luaVM);

                // Add all the elements with a matching type to it
                pEntity->GetChildrenByType(strType.c_str(), luaVM);
                return 1;
            }
            else
                m_pScriptDebugging->LogBadType(luaVM);
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementChild(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    unsigned int     uiIndex = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(uiIndex);

    if (!argStream.HasErrors())
    {
        // Grab the child
        CClientEntity* pChild = CStaticFunctionDefinitions::GetElementChild(*pEntity, uiIndex);
        if (pChild)
        {
            lua_pushelement(luaVM, pChild);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementChildrenCount(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the count
        unsigned int uiCount = pEntity->CountChildren();
        lua_pushnumber(luaVM, static_cast<lua_Number>(uiCount));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementParent(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the parent and return it
        CClientEntity* pParent = pEntity->GetParent();
        if (pParent)
        {
            lua_pushelement(luaVM, pParent);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementsByType(lua_State* luaVM)
{
    // Verify the argument
    SString          strType = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strType);

    if (!argStream.HasErrors())
    {
        // Find our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // see if a root argument has been specified
            CClientEntity* pStartAt = m_pRootEntity;
            CClientEntity* pEntity = NULL;
            bool           bStreamedIn = false;

            if (argStream.NextIsUserData())
            {
                // if its valid, use that, otherwise, produce an error
                argStream.ReadUserData(pEntity);
                if (pEntity)
                    pStartAt = pEntity;
                else
                {
                    m_pScriptDebugging->LogBadPointer(luaVM, "element", 2);
                    lua_pushboolean(luaVM, false);
                    return 1;
                }
                if (argStream.NextIsBool())
                {
                    argStream.ReadBool(bStreamedIn);
                }
            }

            // Create a new table
            lua_newtable(luaVM);

            // Add all the elements with a matching type to it
            pStartAt->FindAllChildrenByType(strType.c_str(), luaVM, bStreamedIn);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementInterior(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the interior and return it
        unsigned char ucInterior;
        if (CStaticFunctionDefinitions::GetElementInterior(*pEntity, ucInterior))
        {
            lua_pushnumber(luaVM, ucInterior);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementWithinColShape(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CClientColShape* pColShape = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pColShape);

    if (!argStream.HasErrors())
    {
        // Check if it's within
        bool bWithin = pEntity->CollisionExists(pColShape);
        lua_pushboolean(luaVM, bWithin);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::IsElementWithinMarker(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CClientMarker*   pMarker = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        // Check if it's within
        bool bWithin = pEntity->CollisionExists(pMarker->GetColShape());
        lua_pushboolean(luaVM, bWithin);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::GetElementsWithinColShape(lua_State* luaVM)
{
    // Verify the arguments
    CClientColShape* pColShape = NULL;
    SString          strType = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadString(strType, "");

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab optional type arg
            const char* szType = strType == "" ? NULL : strType.c_str();

            // Create a new table
            lua_newtable(luaVM);

            // Add all the elements within the shape to it
            unsigned int                        uiIndex = 0;
            CFastList<CClientEntity*>::iterator iter = pColShape->CollidersBegin();
            for (; iter != pColShape->CollidersEnd(); iter++)
            {
                if (szType == NULL || strcmp((*iter)->GetTypeName(), szType) == 0)
                {
                    if (!(*iter)->IsBeingDeleted())
                    {
                        lua_pushnumber(luaVM, ++uiIndex);
                        lua_pushelement(luaVM, *iter);
                        lua_settable(luaVM, -3);
                    }
                }
            }

            // We're returning 1 table
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

CClientEntityResult CLuaElementDefs::GetElementsWithinRange(CVector pos, float radius, std::optional<std::string> type, std::optional<unsigned short> interior,
                                                            std::optional<unsigned short> dimension)
{
    const auto typeHash = (type.has_value() && !type.value().empty()) ? CClientEntity::GetTypeHashFromString(type.value()) : 0;

    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery(result, CSphere{pos, radius});

    // Remove elements that do not match the criterias
    if (interior || dimension || typeHash)
    {
        result.erase(std::remove_if(result.begin(), result.end(),
                                    [&, radiusSq = radius * radius](CElement* pElement) {
                                        if (typeHash && typeHash != pElement->GetTypeHash())
                                            return true;

                                        if (interior.has_value() && interior != pElement->GetInterior())
                                            return true;

                                        if (dimension.has_value() && dimension != pElement->GetDimension())
                                            return true;

                                        // Check if element is within the sphere, because the spatial database is 2D
                                        CVector elementPos;
                                        pElement->GetPosition(elementPos);
                                        if ((elementPos - pos).LengthSquared() > radiusSq)
                                            return true;

                                        return pElement->IsBeingDeleted();
                                    }),
                     result.end());
    }

    return result;
}

int CLuaElementDefs::GetElementDimension(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::IsObjectVisibleInAllDimensions(*pEntity))
        {
            // The entity is visible in all dimensions, just return -1
            lua_pushnumber(luaVM, -1);
            return 1;
        }
        else
        {
            // Grab the dimension and return it
            unsigned short usDimension = pEntity->GetDimension();
            lua_pushnumber(luaVM, usDimension);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_GetElementBoundingBox(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the bounding box and return it
        CVector vecMin, vecMax;
        if (CStaticFunctionDefinitions::GetElementBoundingBox(*pEntity, vecMin, vecMax))
        {
            // If the caller expects six results, return six floats, otherwise two vectors
            int iExpected = lua_ncallresult(luaVM);
            if (iExpected == 6)
            {
                lua_pushnumber(luaVM, vecMin.fX);
                lua_pushnumber(luaVM, vecMin.fY);
                lua_pushnumber(luaVM, vecMin.fZ);
                lua_pushnumber(luaVM, vecMax.fX);
                lua_pushnumber(luaVM, vecMax.fY);
                lua_pushnumber(luaVM, vecMax.fZ);
                return 6;
            }
            else
            {
                lua_pushvector(luaVM, vecMin);
                lua_pushvector(luaVM, vecMax);
                return 2;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementBoundingBox(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the bounding box and return it
        CVector vecMin, vecMax;
        if (CStaticFunctionDefinitions::GetElementBoundingBox(*pEntity, vecMin, vecMax))
        {
            lua_pushnumber(luaVM, vecMin.fX);
            lua_pushnumber(luaVM, vecMin.fY);
            lua_pushnumber(luaVM, vecMin.fZ);
            lua_pushnumber(luaVM, vecMax.fX);
            lua_pushnumber(luaVM, vecMax.fY);
            lua_pushnumber(luaVM, vecMax.fZ);
            return 6;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementRadius(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab its radius and return it
        float fRadius = 0.0f;
        if (CStaticFunctionDefinitions::GetElementRadius(*pEntity, fRadius))
        {
            lua_pushnumber(luaVM, fRadius);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementAttached(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CClientEntity* pEntityAttachedTo = pEntity->GetAttachedTo();
        if (pEntityAttachedTo)
        {
            assert(pEntityAttachedTo->IsEntityAttached(pEntity));
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::GetElementAttachedTo(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the enity attached to it
        CClientEntity* pEntityAttachedTo = CStaticFunctionDefinitions::GetElementAttachedTo(*pEntity);
        if (pEntityAttachedTo)
        {
            lua_pushelement(luaVM, pEntityAttachedTo);
            return 1;
        }
        else
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::GetAttachedElements(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Create a new table
            lua_newtable(luaVM);

            // Add All Attached Elements
            for (uint i = 0; i < pEntity->GetAttachedEntityCount(); i++)
            {
                CClientEntity* pAttached = pEntity->GetAttachedEntity(i);
                assert(pAttached->GetAttachedTo() == pEntity);
                if (!pAttached->IsBeingDeleted())
                {
                    lua_pushnumber(luaVM, i + 1);
                    lua_pushelement(luaVM, pAttached);
                    lua_settable(luaVM, -3);
                }
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementDistanceFromCentreOfMassToBaseOfModel(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the distance and return it
        float fDistance;
        if (CStaticFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel(*pEntity, fDistance))
        {
            lua_pushnumber(luaVM, fDistance);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementLocal(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Return whether it's local or not
        bool bLocal = pEntity->IsLocalEntity();
        lua_pushboolean(luaVM, bLocal);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::GetElementAttachedOffsets(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CVector vecPosition, vecRotation;

        if (CStaticFunctionDefinitions::GetElementAttachedOffsets(*pEntity, vecPosition, vecRotation))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            lua_pushnumber(luaVM, vecRotation.fX);
            lua_pushnumber(luaVM, vecRotation.fY);
            lua_pushnumber(luaVM, vecRotation.fZ);
            return 6;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementAlpha(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab its alpha level and return it
        unsigned char ucAlpha;
        if (CStaticFunctionDefinitions::GetElementAlpha(*pEntity, ucAlpha))
        {
            lua_pushnumber(luaVM, ucAlpha);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<bool, float> CLuaElementDefs::GetElementLighting(CClientEntity* entity)
{
    switch (entity->GetType())
    {
        case CCLIENTPED:
        case CCLIENTPLAYER:
        {
            CPlayerPed* ped = static_cast<CClientPed*>(entity)->GetGamePlayer();
            if (ped)
                return ped->GetLighting();
            break;
        }
        case CCLIENTVEHICLE:
        {
            CVehicle* vehicle = static_cast<CClientVehicle*>(entity)->GetGameVehicle();
            if (vehicle)
                return vehicle->GetLighting();
            break;
        }
        case CCLIENTOBJECT:
        case CCLIENTWEAPON:
        {
            CObject* object = static_cast<CClientObject*>(entity)->GetGameObject();
            if (object)
                return object->GetLighting();
            break;
        }
        default:
            break;
    }
    return false;
}

int CLuaElementDefs::GetElementHealth(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Grab the health and return it
        float fHealth;
        if (CStaticFunctionDefinitions::GetElementHealth(*pEntity, fHealth))
        {
            lua_pushnumber(luaVM, fHealth);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementModel(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        unsigned short usModel;
        if (CStaticFunctionDefinitions::GetElementModel(*pEntity, usModel))
        {
            lua_pushnumber(luaVM, usModel);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementColShape(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CClientEntity* pColShape = static_cast<CClientEntity*>(CStaticFunctionDefinitions::GetElementColShape(pEntity));
        if (pColShape)
        {
            lua_pushelement(luaVM, pColShape);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementInWater(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bInWater = false;
        if (CStaticFunctionDefinitions::IsElementInWater(*pEntity, bInWater))
        {
            lua_pushboolean(luaVM, bInWater);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementSyncer(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bIsSyncer = false;
        if (CStaticFunctionDefinitions::IsElementSyncer(*pEntity, bIsSyncer))
        {
            lua_pushboolean(luaVM, bIsSyncer);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementCollidableWith(lua_State* luaVM)
{
    // Verify the arguments
    CClientEntity*   pEntity = NULL;
    CClientEntity*   pWithEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pWithEntity);

    if (!argStream.HasErrors())
    {
        bool bCanCollide;
        if (CStaticFunctionDefinitions::IsElementCollidableWith(*pEntity, *pWithEntity, bCanCollide))
        {
            lua_pushboolean(luaVM, bCanCollide);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementDoubleSided(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, pEntity->IsDoubleSided());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetElementCollisionsEnabled(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::GetElementCollisionsEnabled(*pEntity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementFrozen(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bFrozen;
        if (CStaticFunctionDefinitions::IsElementFrozen(*pEntity, bFrozen))
        {
            lua_pushboolean(luaVM, bFrozen);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementStreamedIn(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Is this a streaming compatible class?
        if (pEntity->IsStreamingCompatibleClass())
        {
            CClientStreamElement* pStreamElement = static_cast<CClientStreamElement*>(pEntity);

            // Return whether or not this class is streamed in
            lua_pushboolean(luaVM, pStreamElement->IsStreamedIn());
            return 1;
        }
        else if (pEntity->GetType() == CCLIENTSOUND)
        {
            CClientSound* pSound = static_cast<CClientSound*>(pEntity);
            lua_pushboolean(luaVM, pSound->IsSoundStopped() ? false : true);
            return 1;
        }
        else
            m_pScriptDebugging->LogWarning(luaVM, "%s; element is not streaming compatible\n", lua_tostring(luaVM, lua_upvalueindex(1)));
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::IsElementStreamable(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Is this a streaming compatible class?
        if (pEntity->IsStreamingCompatibleClass())
        {
            CClientStreamElement* pStreamElement = static_cast<CClientStreamElement*>(pEntity);

            bool bStreamable = (pStreamElement->GetTotalStreamReferences() == 0);
            // Return whether or not this element is set to never be streamed out
            lua_pushboolean(luaVM, bStreamable);
            return 1;
        }
        else
            m_pScriptDebugging->LogWarning(luaVM, "%s; element is not streaming compatible\n", lua_tostring(luaVM, lua_upvalueindex(1)));

        // Return false, we're not streamable
        lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::IsElementOnScreen(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Return whether we're on the screen or not
        bool bOnScreen;
        if (CStaticFunctionDefinitions::IsElementOnScreen(*pEntity, bOnScreen))
        {
            lua_pushboolean(luaVM, bOnScreen);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::CreateElement(lua_State* luaVM)
{
    // Verify the argument
    SString          strTypeName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTypeName);

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Grab its resource
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Grab the optional name argument
                SString strID = "";
                if (argStream.NextIsString())
                {
                    argStream.ReadString(strID);
                }

                // Try to create
                CClientDummy* pDummy = CStaticFunctionDefinitions::CreateElement(*pResource, strTypeName.c_str(), strID.c_str());
                if (pDummy)
                {
                    // Add it to the element group
                    // TODO: Get rid of element groups
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pDummy);
                    }

                    // Return it
                    lua_pushelement(luaVM, pDummy);
                    return 1;
                }
                else
                    m_pScriptDebugging->LogError(luaVM, "createElement; unable to create more elements\n");
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::DestroyElement(lua_State* luaVM)
{
    // Verify the argument
    CClientEntity*   pEntity = NULL;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        // Destroy it
        if (CStaticFunctionDefinitions::DestroyElement(*pEntity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementID(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    SString          strKey = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        // It returns false if we tried to change ID of server-created element
        if (CStaticFunctionDefinitions::SetElementID(*pEntity, strKey.c_str()))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementData(lua_State* luaVM)
{
    //  bool setElementData ( element theElement, string key, var value, [bool synchronize = true] )
    CClientEntity* pEntity;
    SString        strKey;
    CLuaArgument   value;
    bool           bSynchronize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strKey);
    argStream.ReadLuaArgument(value);
    argStream.ReadBool(bSynchronize, true);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                                                             *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
                strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
            }

            if (CStaticFunctionDefinitions::SetElementData(*pEntity, strKey, value, bSynchronize))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::RemoveElementData(lua_State* luaVM)
{
    //  bool removeElementData ( element theElement, string key )
    CClientEntity* pEntity;
    SString        strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                                                             *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
                strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
            }

            if (CStaticFunctionDefinitions::RemoveElementData(*pEntity, strKey))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementMatrix(lua_State* luaVM)
{
    //  setElementMatrix ( element theElement, table matrix )
    CClientEntity* pEntity;
    CMatrix        matrix;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (argStream.NextIsTable())
    {
        if (!ReadMatrix(luaVM, argStream.m_iIndex, matrix))
        {
            argStream.SetCustomError("Matrix is not 4 x 4");
        }
    }
    else
    {
        argStream.ReadMatrix(matrix);
    }

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementMatrix(*pEntity, matrix))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementPosition(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    CVector          vecPosition;
    bool             bWarp = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadBool(bWarp, true);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Try to set the position
        if (CStaticFunctionDefinitions::SetElementPosition(*pEntity, vecPosition, bWarp))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementRotation(lua_State* luaVM)
{
    //  bool setElementRotation ( element theElement, float rotX, float rotY, float rotZ [, string rotOrder = "default", bool fixPedRotation = false ] )
    CClientEntity*      pEntity;
    CVector             vecRotation;
    eEulerRotationOrder rotationOrder;
    bool                bNewWay;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecRotation);
    argStream.ReadEnumString(rotationOrder, EULER_DEFAULT);
    argStream.ReadBool(bNewWay, false);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementRotation(*pEntity, vecRotation, rotationOrder, bNewWay))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

// See 6ccf48f65df77caf1e907e49d62f4b5ac3a7a265 for reasoning behind this special code
int CLuaElementDefs::OOP_SetElementRotation(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;
    CVector        vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecRotation);

    if (!argStream.HasErrors())
    {
        CMatrix matrix;
        pEntity->GetMatrix(matrix);

        ConvertDegreesToRadians(vecRotation);

        matrix.SetRotation(vecRotation);

        pEntity->SetMatrix(matrix);

        ConvertRadiansToDegrees(vecRotation);

        eEulerRotationOrder rotationOrder = EULER_DEFAULT;
        if (pEntity->GetType() == CCLIENTOBJECT)
            rotationOrder = EULER_ZYX;

        if (CStaticFunctionDefinitions::SetElementRotation(*pEntity, vecRotation, rotationOrder, true))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementVelocity(lua_State* luaVM)
{
    CClientEntity* pEntity;
    CVector        vecVelocity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecVelocity);
    // previous code did this for some reason.
    if (pEntity && pEntity->GetType() == CCLIENTRADARAREA)
        vecVelocity.fZ = 0.0f;

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Set the velocity
        if (CStaticFunctionDefinitions::SetElementVelocity(*pEntity, vecVelocity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementAngularVelocity(lua_State* luaVM)
{
    CClientEntity* pEntity;
    CVector        vecTurnVelocity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecTurnVelocity);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Set the turn velocity
        if (CStaticFunctionDefinitions::SetElementAngularVelocity(*pEntity, vecTurnVelocity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementParent(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;
    CClientEntity* pParent = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pParent);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        // Change the parent
        if (CStaticFunctionDefinitions::SetElementParent(*pEntity, *pParent, pLuaMain))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementInterior(lua_State* luaVM)
{
    CClientEntity* pEntity;
    unsigned int   uiInterior = 0;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(uiInterior);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (uiInterior <= 0xFF)
        {
            // Grab position if supplied
            bool    bSetPosition = false;
            CVector vecPosition;

            if (argStream.NextIsVector3D())
            {
                argStream.ReadVector3D(vecPosition);
                if (!argStream.HasErrors())
                {
                    bSetPosition = true;
                }
                else
                {
                    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                    // Failed
                    lua_pushboolean(luaVM, false);
                    return 1;
                }
            }

            // Set the interior
            if (CStaticFunctionDefinitions::SetElementInterior(*pEntity, uiInterior, bSetPosition, vecPosition))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType(luaVM);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementDimension(lua_State* luaVM)
{
    CClientEntity* pEntity;
    int            iDimension = 0;
    bool           bMakeVisibleInAllDimensions = false;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(iDimension);

    if (iDimension == -1)
    {
        if (IS_OBJECT(pEntity))
            bMakeVisibleInAllDimensions = true;
        else
            argStream.SetCustomError("The -1 value can be used only in objects!");
    }
    else if (iDimension < 0 || iDimension > 65535)
        argStream.SetCustomError("Invalid dimension range specified!");

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (bMakeVisibleInAllDimensions)
        {
            // Set the object visible in all dimensions
            if (CStaticFunctionDefinitions::SetObjectVisibleInAllDimensions(*pEntity, true))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
        {
            // If it's an object and is visible in all dimensions
            if (IS_OBJECT(pEntity) && CStaticFunctionDefinitions::IsObjectVisibleInAllDimensions(*pEntity))
            {
                // Make it not visible in all dimensions and set its dimension
                if (CStaticFunctionDefinitions::SetObjectVisibleInAllDimensions(*pEntity, false, (unsigned short)iDimension))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
            else
            {
                // Just change the dimension
                if (CStaticFunctionDefinitions::SetElementDimension(*pEntity, (unsigned short)iDimension))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::AttachElements(lua_State* luaVM)
{
    CClientEntity* pEntity = NULL;
    CClientEntity* pAttachedToEntity = NULL;
    CVector        vecPosition, vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pAttachedToEntity);
    argStream.ReadVector3D(vecPosition, vecPosition);
    argStream.ReadVector3D(vecRotation, vecRotation);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Try to attach them
        if (CStaticFunctionDefinitions::AttachElements(*pEntity, *pAttachedToEntity, vecPosition, vecRotation))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::DetachElements(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    CClientEntity*   pAttachedToEntity = NULL;
    unsigned short   usDimension = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pAttachedToEntity, NULL);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Try to detach
        if (CStaticFunctionDefinitions::DetachElements(*pEntity, pAttachedToEntity))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failure
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementAttachedOffsets(lua_State* luaVM)
{
    CClientEntity* pEntity;
    CClientEntity* pAttachedToEntity = NULL;
    unsigned short usDimension = 0;
    CVector        vecPosition, vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadVector3D(vecPosition, vecPosition);
    argStream.ReadVector3D(vecRotation, vecRotation);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementAttachedOffsets(*pEntity, vecPosition, vecRotation))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementCollisionsEnabled(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    bool             bEnabled = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bEnabled);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementCollisionsEnabled(*pEntity, bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementCollidableWith(lua_State* luaVM)
{
    CClientEntity*   pEntity = NULL;
    CClientEntity*   pWithEntity = NULL;
    bool             bCanCollide = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pWithEntity);
    argStream.ReadBool(bCanCollide);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementCollidableWith(*pEntity, *pWithEntity, bCanCollide))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementDoubleSided(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    bool             bDoubleSided = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bDoubleSided);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Grab the chosen value and set it
        pEntity->SetDoubleSided(bDoubleSided);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failure
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementFrozen(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    bool             bFrozen = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bFrozen);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementFrozen(*pEntity, bFrozen))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failure
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementAlpha(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    unsigned char    ucAlpha = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(ucAlpha);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementAlpha(*pEntity, ucAlpha))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failure
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementHealth(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    float            fHealth = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(fHealth);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementHealth(*pEntity, fHealth))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementModel(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    unsigned short   usModel = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadNumber(usModel);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementModel(*pEntity, usModel))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementStreamable(lua_State* luaVM)
{
    CClientEntity*   pEntity;
    bool             bStreamable = true;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bStreamable);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Is this a streaming compatible class?
        if (pEntity->IsStreamingCompatibleClass())
        {
            CClientStreamElement* pStreamElement = static_cast<CClientStreamElement*>(pEntity);

            // TODO: maybe use a better VM-based reference system (rather than a boolean one)
            if (bStreamable && (pStreamElement->GetStreamReferences(true) > 0))
            {
                pStreamElement->RemoveStreamReference(true);
            }
            else if (!bStreamable && (pStreamElement->GetStreamReferences(true) == 0))
            {
                pStreamElement->AddStreamReference(true);
            }
            else
            {
                lua_pushboolean(luaVM, false);
                return 1;
            }

            // Return success
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // We failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::GetLowLodElement(lua_State* luaVM)
{
    //  element getLowLODElement ( element theElement )
    CClientEntity* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CClientEntity* pLowLodEntity;
        if (CStaticFunctionDefinitions::GetLowLodElement(*pEntity, pLowLodEntity))
        {
            lua_pushelement(luaVM, pLowLodEntity);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaElementDefs::SetLowLodElement(lua_State* luaVM, CClientEntity* pEntity, std::optional<CClientEntity*> pLowLodEntity)
{
    //  bool setLowLODElement ( element theElement [, element lowLowElement ] )

    if (pEntity->GetType() == CCLIENTBUILDING)
        MinClientReqCheck(luaVM, MIN_CLIENT_REQ_LOD_FOR_BUILDING, "target is building");

    return CStaticFunctionDefinitions::SetLowLodElement(*pEntity, pLowLodEntity.value_or(nullptr));
}

bool CLuaElementDefs::SetElementOnFire(CClientEntity* entity, bool onFire) noexcept
{
    if (!entity->IsLocalEntity() && entity != CStaticFunctionDefinitions::GetLocalPlayer())
        return false;

    return entity->SetOnFire(onFire);
}

int CLuaElementDefs::IsElementLowLod(lua_State* luaVM)
{
    //  bool isElementLowLOD ( element theElement )
    CClientEntity* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bLowLod;
        if (CStaticFunctionDefinitions::IsElementLowLod(*pEntity, bLowLod))
        {
            lua_pushboolean(luaVM, bLowLod);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::SetElementCallPropagationEnabled(lua_State* luaVM)
{
    //  bool setElementCallPropagationEnabled ( element theElement, bool enabled )
    CClientEntity* pEntity;
    bool           bEnabled;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementCallPropagationEnabled(*pEntity, bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementCallPropagationEnabled(lua_State* luaVM)
{
    //  bool isElementCallPropagationEnabled ( element theElement )
    CClientEntity* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bEnabled;
        if (CStaticFunctionDefinitions::IsElementCallPropagationEnabled(*pEntity, bEnabled))
        {
            lua_pushboolean(luaVM, bEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::IsElementWaitingForGroundToLoad(lua_State* luaVM)
{
    CClientEntity* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bEnabled;
        if (CStaticFunctionDefinitions::IsElementFrozenWaitingForGroundToLoad(*pEntity, bEnabled))
        {
            lua_pushboolean(luaVM, bEnabled);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaElementDefs::SetElementLighting(CClientEntity* entity, float lighting)
{
    switch (entity->GetType())
    {
        case CCLIENTPLAYER:
        case CCLIENTPED:
        {
            auto* ped = static_cast<CClientPed*>(entity)->GetGamePlayer();
            if (!ped)
                return false;

            ped->SetLighting(lighting);
            return true;
        }
        case CCLIENTVEHICLE:
        {
            auto* vehicle = static_cast<CClientVehicle*>(entity)->GetGameVehicle();
            if (!vehicle)
                return false;

            vehicle->SetLighting(lighting);
            return true;
        }
        case CCLIENTOBJECT:
        case CCLIENTWEAPON:
        {
            auto* object = static_cast<CClientObject*>(entity)->GetGameObject();
            if (!object)
                return false;

            object->SetLighting(lighting);
            return true;
        }
    }

    return false;
}

bool CLuaElementDefs::IsElementOnFire(CClientEntity* entity) noexcept
{
    return entity->IsOnFire();
}

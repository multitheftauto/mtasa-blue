/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaElementDefs.cpp
 *  PURPOSE:     Lua element definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaElementDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"
#include "CDummy.h"
#include "Utils.h"

void CLuaElementDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Create/destroy
        {"createElement", createElement},
        {"destroyElement", destroyElement},
        {"cloneElement", cloneElement},

        // Get
        {"isElement", isElement},
        {"isElementWithinColShape", isElementWithinColShape},
        {"isElementWithinMarker", isElementWithinMarker},
        {"isElementInWater", isElementInWater},
        {"isElementFrozen", isElementFrozen},
        {"isElementLowLOD", isElementLowLOD},
        {"setElementCallPropagationEnabled", setElementCallPropagationEnabled},
        {"isElementCallPropagationEnabled", isElementCallPropagationEnabled},

        {"getElementByID", getElementByID},
        {"getElementByIndex", getElementByIndex},
        {"getElementChildren", getElementChildren},
        {"getElementChild", getElementChild},
        {"getElementChildrenCount", getElementChildrenCount},
        {"getElementID", getElementID},
        {"getElementParent", getElementParent},
        {"getElementMatrix", getElementMatrix},
        {"getElementPosition", getElementPosition},
        {"getElementRotation", getElementRotation},
        {"getElementVelocity", getElementVelocity},
        {"getElementAngularVelocity", getElementTurnVelocity},
        {"getElementsByType", getElementsByType},
        {"getElementType", getElementType},
        {"getElementInterior", getElementInterior},
        {"getElementsWithinColShape", getElementsWithinColShape},
        {"getElementsWithinRange", ArgumentParserWarn<false, getElementsWithinRange>},
        {"getElementDimension", getElementDimension},
        {"getElementZoneName", getElementZoneName},
        {"getElementColShape", getElementColShape},
        {"getElementAlpha", getElementAlpha},
        {"isElementDoubleSided", isElementDoubleSided},
        {"getElementHealth", getElementHealth},
        {"getElementModel", getElementModel},
        {"getElementSyncer", getElementSyncer},
        {"getElementCollisionsEnabled", getElementCollisionsEnabled},
        {"getLowLODElement", getLowLODElement},
        {"isElementOnFire", ArgumentParser<IsElementOnFire>},

        // Attachement
        {"attachElements", attachElements},
        {"detachElements", detachElements},
        {"isElementAttached", isElementAttached},
        {"getAttachedElements", getAttachedElements},
        {"getElementAttachedTo", getElementAttachedTo},
        {"setElementAttachedOffsets", setElementAttachedOffsets},
        {"getElementAttachedOffsets", getElementAttachedOffsets},

        // Element data
        {"getElementData", GetElementData},
        {"getAllElementData", ArgumentParserWarn<false, GetAllElementData>},
        {"hasElementData", HasElementData},
        {"setElementData", setElementData},
        {"removeElementData", removeElementData},
        {"addElementDataSubscriber", addElementDataSubscriber},
        {"removeElementDataSubscriber", removeElementDataSubscriber},
        {"hasElementDataSubscriber", hasElementDataSubscriber},

        // Set
        {"setElementID", setElementID},
        {"setElementParent", setElementParent},
        {"setElementMatrix", setElementMatrix},
        {"setElementPosition", setElementPosition},
        {"setElementRotation", setElementRotation},
        {"setElementVelocity", setElementVelocity},
        {"setElementAngularVelocity", setElementTurnVelocity},
        {"setElementVisibleTo", setElementVisibleTo},
        {"clearElementVisibleTo", clearElementVisibleTo},
        {"isElementVisibleTo", isElementVisibleTo},
        {"setElementInterior", setElementInterior},
        {"setElementDimension", setElementDimension},
        {"setElementAlpha", setElementAlpha},
        {"setElementDoubleSided", setElementDoubleSided},
        {"setElementHealth", setElementHealth},
        {"setElementModel", setElementModel},
        {"setElementSyncer", setElementSyncer},
        {"setElementCollisionsEnabled", setElementCollisionsEnabled},
        {"setElementFrozen", setElementFrozen},
        {"setLowLODElement", setLowLODElement},
        {"setElementOnFire", ArgumentParser<SetElementOnFire>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

// TODO: specials
void CLuaElementDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "getByID", "getElementByID");
    lua_classfunction(luaVM, "getAllByType", "getElementsByType");
    lua_classfunction(luaVM, "getByIndex", "getElementByIndex");
    lua_classfunction(luaVM, "getWithinRange", "getElementsWithinRange");

    lua_classfunction(luaVM, "create", "createElement");
    lua_classfunction(luaVM, "clone", "cloneElement");
    lua_classfunction(luaVM, "destroy", "destroyElement");
    lua_classfunction(luaVM, "clearVisibility", "clearElementVisibleTo");
    lua_classfunction(luaVM, "attach", "attachElements");
    lua_classfunction(luaVM, "detach", "detachElements");
    lua_classfunction(luaVM, "removeData", "removeElementData");
    lua_classfunction(luaVM, "addDataSubscriber", "addElementDataSubscriber");
    lua_classfunction(luaVM, "removeDataSubscriber", "removeElementDataSubscriber");
    lua_classfunction(luaVM, "hasDataSubscriber", "hasElementDataSubscriber");

    lua_classfunction(luaVM, "setParent", "setElementParent");
    lua_classfunction(luaVM, "setFrozen", "setElementFrozen");
    lua_classfunction(luaVM, "setHealth", "setElementHealth");
    lua_classfunction(luaVM, "setModel", "setElementModel");
    lua_classfunction(luaVM, "setSyncer", "setElementSyncer");
    lua_classfunction(luaVM, "setAlpha", "setElementAlpha");
    lua_classfunction(luaVM, "setDoubleSided", "setElementDoubleSided");
    lua_classfunction(luaVM, "setCollisionsEnabled", "setElementCollisionsEnabled");
    lua_classfunction(luaVM, "setData", "setElementData");
    lua_classfunction(luaVM, "setPosition", "setElementPosition");
    lua_classfunction(luaVM, "setRotation", "setElementRotation");
    lua_classfunction(luaVM, "setVelocity", "setElementVelocity");
    lua_classfunction(luaVM, "setAngularVelocity", "setElementAngularVelocity");
    lua_classfunction(luaVM, "setVisibleTo", "setElementVisibleTo");
    lua_classfunction(luaVM, "setMatrix", "setElementMatrix");
    lua_classfunction(luaVM, "setID", "setElementID");
    lua_classfunction(luaVM, "setInterior", "setElementInterior");
    lua_classfunction(luaVM, "setDimension", "setElementDimension");
    lua_classfunction(luaVM, "setLowLOD", "setLowLODElement");
    lua_classfunction(luaVM, "setAttachedOffsets", "setElementAttachedOffsets");
    lua_classfunction(luaVM, "setCallPropagationEnabled", "setElementCallPropagationEnabled");
    lua_classfunction(luaVM, "setOnFire", "setElementOnFire");

    lua_classfunction(luaVM, "getAttachedOffsets", "getElementAttachedOffsets");
    lua_classfunction(luaVM, "getChild", "getElementChild");
    lua_classfunction(luaVM, "getChildren", "getElementChildren");
    lua_classfunction(luaVM, "getParent", "getElementParent");
    lua_classfunction(luaVM, "getAttachedElements", "getAttachedElements");
    lua_classfunction(luaVM, "getAttachedTo", "getElementAttachedTo");
    lua_classfunction(luaVM, "getVelocity", "getElementVelocity", OOP_getElementVelocity);
    lua_classfunction(luaVM, "getAngularVelocity", "getElementAngularVelocity", OOP_getElementTurnVelocity);
    lua_classfunction(luaVM, "getID", "getElementID");
    lua_classfunction(luaVM, "getZoneName", "getElementZoneName");
    lua_classfunction(luaVM, "getAlpha", "getElementAlpha");
    lua_classfunction(luaVM, "getHealth", "getElementHealth");
    lua_classfunction(luaVM, "getModel", "getElementModel");
    lua_classfunction(luaVM, "getChildrenCount", "getElementChildrenCount");
    lua_classfunction(luaVM, "getSyncer", "getElementSyncer");
    lua_classfunction(luaVM, "getAllData", "getAllElementData");
    lua_classfunction(luaVM, "getColShape", "getElementColShape");
    lua_classfunction(luaVM, "getData", "getElementData");
    lua_classfunction(luaVM, "hasData", "hasElementData");
    lua_classfunction(luaVM, "getPosition", "getElementPosition", OOP_getElementPosition);
    lua_classfunction(luaVM, "getRotation", "getElementRotation", OOP_getElementRotation);
    lua_classfunction(luaVM, "getMatrix", "getElementMatrix", OOP_getElementMatrix);
    lua_classfunction(luaVM, "getType", "getElementType");
    lua_classfunction(luaVM, "getInterior", "getElementInterior");
    lua_classfunction(luaVM, "getDimension", "getElementDimension");
    lua_classfunction(luaVM, "getLowLOD", "getLowLODElement");

    lua_classfunction(luaVM, "getCollisionsEnabled", "getElementCollisionsEnabled");
    lua_classfunction(luaVM, "isCallPropagationEnabled", "isElementCallPropagationEnabled");
    lua_classfunction(luaVM, "isWithinMarker", "isElementWithinMarker");
    lua_classfunction(luaVM, "isWithinColShape", "isElementWithinColShape");
    lua_classfunction(luaVM, "isFrozen", "isElementFrozen");
    lua_classfunction(luaVM, "isInWater", "isElementInWater");
    lua_classfunction(luaVM, "isDoubleSided", "isElementDoubleSided");
    lua_classfunction(luaVM, "isVisibleTo", "isElementVisibleTo");
    lua_classfunction(luaVM, "isLowLOD", "isElementLowLOD");
    lua_classfunction(luaVM, "isAttached", "isElementAttached");
    lua_classfunction(luaVM, "isOnFire", "isElementOnFire");

    lua_classvariable(luaVM, "id", "setElementID", "getElementID");
    lua_classvariable(luaVM, "callPropagationEnabled", "setElementCallPropagationEnabled", "isElementCallPropagationEnabled");
    lua_classvariable(luaVM, "parent", "setElementParent", "getElementParent");
    lua_classvariable(luaVM, "zoneName", NULL, "getElementZoneName");
    lua_classvariable(luaVM, "attachedTo", "attachElements", "getElementAttachedTo");
    lua_classvariable(luaVM, "children", NULL, "getElementChildren");
    lua_classvariable(luaVM, "frozen", "setElementFrozen", "isElementFrozen");
    lua_classvariable(luaVM, "attachedElements", NULL, "getAttachedElements");
    lua_classvariable(luaVM, "inWater", NULL, "isElementInWater");
    lua_classvariable(luaVM, "health", "setElementHealth", "getElementHealth");
    lua_classvariable(luaVM, "alpha", "setElementAlpha", "getElementAlpha");
    lua_classvariable(luaVM, "type", NULL, "getElementType");
    lua_classvariable(luaVM, "dimension", "setElementDimension", "getElementDimension");
    lua_classvariable(luaVM, "doubleSided", "setElementDoubleSided", "isElementDoubleSided");
    lua_classvariable(luaVM, "model", "setElementModel", "getElementModel");
    lua_classvariable(luaVM, "lowLOD", "setLowLODElement", "getLowLODElement");
    lua_classvariable(luaVM, "syncer", NULL, "getElementSyncer");
    lua_classvariable(luaVM, "childrenCount", NULL, "getElementChildrenCount");
    lua_classvariable(luaVM, "interior", "setElementInterior", "getElementInterior");
    lua_classvariable(luaVM, "colShape", NULL, "getElementColShape");
    lua_classvariable(luaVM, "collisions", "setElementCollisionsEnabled", "getElementCollisionsEnabled");
    lua_classvariable(luaVM, "position", "setElementPosition", "getElementPosition", setElementPosition, OOP_getElementPosition);
    lua_classvariable(luaVM, "rotation", "setElementRotation", "getElementRotation", OOP_setElementRotation, OOP_getElementRotation);
    lua_classvariable(luaVM, "matrix", "setElementMatrix", "getElementMatrix", setElementMatrix, OOP_getElementMatrix);
    lua_classvariable(luaVM, "velocity", "setElementVelocity", "getElementVelocity", setElementVelocity, OOP_getElementVelocity);
    lua_classvariable(luaVM, "angularVelocity", "setElementAngularVelocity", "getElementAngularVelocity", setElementTurnVelocity, OOP_getElementTurnVelocity);
    lua_classvariable(luaVM, "isElement", NULL, "isElement");
    lua_classvariable(luaVM, "onFire", "setElementOnFire", "isElementOnFire");

    lua_registerclass(luaVM, "Element");
}

int CLuaElementDefs::createElement(lua_State* luaVM)
{
    //  element createElement ( string elementType, [ string elementID ] )
    SString strTypeName;
    SString strId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strTypeName);
    argStream.ReadString(strId, "");

    if (!argStream.HasErrors())
    {
        // Get the virtual machine
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Get the resource
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Try to create
                CDummy* pDummy = CStaticFunctionDefinitions::CreateElement(pResource, strTypeName, strId);
                if (pDummy)
                {
                    // Get the group
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pDummy);
                    }
                    lua_pushelement(luaVM, pDummy);
                    return 1;
                }
                argStream.SetCustomError(SString("element type '%s' cannot be used", *strTypeName));
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::destroyElement(lua_State* luaVM)
{
    //  bool destroyElement ( element elementToDestroy )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::DestroyElement(pElement))
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

int CLuaElementDefs::cloneElement(lua_State* luaVM)
{
    //  element cloneElement ( element theElement, [ float xPos = 0, float yPos = 0, float zPos = 0, bool cloneChildren = false ] )
    CElement* pElement;
    CVector   vecPosition;
    bool      bCloneChildren = false;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    if (!argStream.HasErrors())
    {
        argStream.ReadVector3D(vecPosition, pElement->GetPosition());
        argStream.ReadBool(bCloneChildren, false);
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CElement* pNewElement = CStaticFunctionDefinitions::CloneElement(pResource, pElement, vecPosition, bCloneChildren);

                if (pNewElement)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pNewElement);
                    }
                    lua_pushelement(luaVM, pNewElement);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::isElement(lua_State* luaVM)
{
    //  bool isElement ( var theValue )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementChildren(lua_State* luaVM)
{
    //  table getElementChildren ( element parent, [ string theType = nil ] )
    CElement* pElement;
    SString   strType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strType, "");

    if (!argStream.HasErrors())
    {
        // Create a new table
        lua_newtable(luaVM);

        // Add all the elements with a matching type to it
        if (strType == "")
            pElement->GetChildren(luaVM);
        else
            pElement->GetChildrenByType(strType, luaVM);

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementChild(lua_State* luaVM)
{
    //  element getElementChild ( element parent, int index )
    CElement* pElement;
    uint      uiIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(uiIndex);

    if (!argStream.HasErrors())
    {
        CElement* pChild = CStaticFunctionDefinitions::GetElementChild(pElement, uiIndex);
        if (pChild)
        {
            lua_pushelement(luaVM, pChild);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementChildrenCount(lua_State* luaVM)
{
    //  int getElementChildrenCount ( element parent )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        unsigned int uiCount;
        if (CStaticFunctionDefinitions::GetElementChildrenCount(pElement, uiCount))
        {
            lua_pushnumber(luaVM, static_cast<lua_Number>(uiCount));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementID(lua_State* luaVM)
{
    //  string getElementID ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pElement->GetName().c_str());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementByID(lua_State* luaVM)
{
    //  element getElementByID ( string id, [ int index = 0 ] )
    SString strId;
    uint    uiIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strId);
    argStream.ReadNumber(uiIndex, 0);

    if (!argStream.HasErrors())
    {
        CElement* pElement = CStaticFunctionDefinitions::GetElementByID(strId, uiIndex);
        if (pElement)
        {
            lua_pushelement(luaVM, pElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementByIndex(lua_State* luaVM)
{
    //  element getElementByIndex ( string type, int index )
    SString strType;
    uint    uiIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strType);
    argStream.ReadNumber(uiIndex);

    if (!argStream.HasErrors())
    {
        CElement* pElement = CStaticFunctionDefinitions::GetElementByIndex(strType, uiIndex);
        if (pElement)
        {
            lua_pushelement(luaVM, pElement);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementParent(lua_State* luaVM)
{
    //  element getElementParent ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CElement* pParent = CStaticFunctionDefinitions::GetElementParent(pElement);
        if (pParent && !pParent->IsBeingDeleted())
        {
            lua_pushelement(luaVM, pParent);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementPosition(lua_State* luaVM)
{
    //  float, float, float getElementPosition ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        // Grab the position
        CVector vecPosition;
        if (CStaticFunctionDefinitions::GetElementPosition(pElement, vecPosition))
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

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_getElementPosition(lua_State* luaVM)
{
    CElement* pElement = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CVector vector = pElement->GetPosition();

        lua_pushvector(luaVM, vector);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementMatrix(lua_State* luaVM)
{
    CElement* pElement = NULL;
    bool      bBadSyntax;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bBadSyntax, true);

    // Verify the arguments
    if (!argStream.HasErrors())
    {
        // Grab the position
        CMatrix matrix;
        if (CStaticFunctionDefinitions::GetElementMatrix(pElement, matrix))
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

int CLuaElementDefs::OOP_getElementMatrix(lua_State* luaVM)
{
    CElement* pEntity = NULL;

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

int CLuaElementDefs::getElementRotation(lua_State* luaVM)
{
    //  float float float getElementRotation ( element theElement [, string rotOrder = "default" ] )
    CElement*           pElement;
    eEulerRotationOrder rotationOrder;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadEnumString(rotationOrder, EULER_DEFAULT);

    if (!argStream.HasErrors())
    {
        // Grab the rotation
        CVector vecRotation;
        if (CStaticFunctionDefinitions::GetElementRotation(pElement, vecRotation, rotationOrder))
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

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_getElementRotation(lua_State* luaVM)
{
    CElement* pElement = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CMatrix matrix;
        CVector vecRotation;
        pElement->GetMatrix(matrix);

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

int CLuaElementDefs::getElementVelocity(lua_State* luaVM)
{
    //  float float float getElementVelocity ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        // Grab the velocity
        CVector vecVelocity;
        if (CStaticFunctionDefinitions::GetElementVelocity(pElement, vecVelocity))
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

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_getElementVelocity(lua_State* luaVM)
{
    CElement* pElement = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CVector vecVelocity;
        CStaticFunctionDefinitions::GetElementVelocity(pElement, vecVelocity);

        lua_pushvector(luaVM, vecVelocity);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementTurnVelocity(lua_State* luaVM)
{
    //  float float float getElementAngularVelocity ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        // Grab the turn velocity
        CVector vecTurnVelocity;
        if (CStaticFunctionDefinitions::GetElementTurnVelocity(pElement, vecTurnVelocity))
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

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::OOP_getElementTurnVelocity(lua_State* luaVM)
{
    CElement* pElement = NULL;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CVector vecTurnVelocity;
        CStaticFunctionDefinitions::GetElementTurnVelocity(pElement, vecTurnVelocity);

        lua_pushvector(luaVM, vecTurnVelocity);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementType(lua_State* luaVM)
{
    //  string getElementType ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pElement->GetTypeName().c_str());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementsByType(lua_State* luaVM)
{
    //  table getElementsByType ( string theType, [ element startat=getRootElement() ] )
    SString   strType;
    CElement* startAt;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strType);
    argStream.ReadUserData(startAt, m_pRootElement);

    if (!argStream.HasErrors())
    {
        // Create a new table
        lua_newtable(luaVM);

        // Add all the elements with a matching type to it
        startAt->FindAllChildrenByType(strType, luaVM);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementInterior(lua_State* luaVM)
{
    //  int getElementInterior ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        unsigned char ucInterior;
        if (CStaticFunctionDefinitions::GetElementInterior(pElement, ucInterior))
        {
            lua_pushnumber(luaVM, ucInterior);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::isElementWithinMarker(lua_State* luaVM)
{
    //  bool isElementWithinMarker ( element theElement, marker theMarker )
    CElement* pElement;
    CMarker*  pMarker;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pMarker);

    if (!argStream.HasErrors())
    {
        bool bWithin;
        if (CStaticFunctionDefinitions::IsElementWithinMarker(pElement, pMarker, bWithin))
        {
            lua_pushboolean(luaVM, bWithin);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::isElementWithinColShape(lua_State* luaVM)
{
    //  bool isElementWithinColShape ( element theElement, colshape theShape )
    CElement*  pElement;
    CColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pColShape);

    if (!argStream.HasErrors())
    {
        bool bWithin;
        if (CStaticFunctionDefinitions::IsElementWithinColShape(pElement, pColShape, bWithin))
        {
            lua_pushboolean(luaVM, bWithin);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementsWithinColShape(lua_State* luaVM)
{
    //  table getElementsWithinColShape ( colshape shape, [ string elemType ] )
    CColShape* pColShape;
    SString    strType;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadString(strType, "");

    if (!argStream.HasErrors())
    {
        // Create a new table
        lua_newtable(luaVM);

        // Add all the elements within the shape to it
        unsigned int              uiIndex = 0;
        list<CElement*>::iterator iter = pColShape->CollidersBegin();
        for (; iter != pColShape->CollidersEnd(); ++iter)
        {
            if ((strType.empty() || strType == (*iter)->GetTypeName()) && !(*iter)->IsBeingDeleted())
            {
                lua_pushnumber(luaVM, ++uiIndex);
                lua_pushelement(luaVM, *iter);
                lua_settable(luaVM, -3);
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

CElementResult CLuaElementDefs::getElementsWithinRange(CVector pos, float radius, std::optional<std::string> type, std::optional<unsigned short> interior,
                                                       std::optional<unsigned short> dimension)
{
    const auto typeHash = (type.has_value() && !type.value().empty()) ? CElement::GetTypeHashFromString(type.value()) : 0;

    CElementResult result;
    GetSpatialDatabase()->SphereQuery(result, CSphere{pos, radius});

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
                                        if ((pElement->GetPosition() - pos).LengthSquared() > radiusSq)
                                            return true;

                                        return pElement->IsBeingDeleted();
                                    }),
                     result.end());
    }

    return result;
}

int CLuaElementDefs::getElementDimension(lua_State* luaVM)
{
    //  int getElementDimension ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (IS_OBJECT(pElement) && CStaticFunctionDefinitions::IsObjectVisibleInAllDimensions(pElement))
        {
            // The object is visible in all dimensions, just return -1
            lua_pushnumber(luaVM, -1);
            return 1;
        }
        else
        {
            unsigned short usDimension;
            if (CStaticFunctionDefinitions::GetElementDimension(pElement, usDimension))
            {
                lua_pushnumber(luaVM, usDimension);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementZoneName(lua_State* luaVM)
{
    //  string getElementZoneName ( element theElement, [bool citiesonly=false] )
    CElement* pElement;
    bool      bCitiesOnly;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bCitiesOnly, false);

    if (!argStream.HasErrors())
    {
        SString strZoneName;
        if (CStaticFunctionDefinitions::GetElementZoneName(pElement, strZoneName, bCitiesOnly))
        {
            lua_pushstring(luaVM, strZoneName);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::isElementAttached(lua_State* luaVM)
{
    //  bool isElementAttached ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::IsElementAttached(pElement))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushnil(luaVM);
    return 1;
}

int CLuaElementDefs::getElementAttachedTo(lua_State* luaVM)
{
    //  element getElementAttachedTo ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CElement* pElementAttachedTo = CStaticFunctionDefinitions::GetElementAttachedTo(pElement);
        if (pElementAttachedTo)
        {
            lua_pushelement(luaVM, pElementAttachedTo);
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

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getAttachedElements(lua_State* luaVM)
{
    //  table getAttachedElements ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        // Create a new table
        lua_newtable(luaVM);

        // Add All Attached Elements
        unsigned int                    uiIndex = 0;
        list<CElement*>::const_iterator iter = pElement->AttachedElementsBegin();
        for (; iter != pElement->AttachedElementsEnd(); ++iter)
        {
            if ((*iter)->GetAttachedToElement() == pElement)
            {
                lua_pushnumber(luaVM, ++uiIndex);
                lua_pushelement(luaVM, *iter);
                lua_settable(luaVM, -3);
            }
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::setElementAttachedOffsets(lua_State* luaVM)
{
    //  bool setElementAttachedOffsets ( element theElement, [ float xPosOffset, float yPosOffset, float zPosOffset, float xRotOffset, float yRotOffset, float
    //  zRotOffset ])
    CElement* pElement;
    CVector   vecPosition;
    CVector   vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector3D(vecPosition, vecPosition);
    argStream.ReadVector3D(vecRotation, vecRotation);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementAttachedOffsets(pElement, vecPosition, vecRotation))
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

int CLuaElementDefs::getElementAttachedOffsets(lua_State* luaVM)
{
    //  float, float, float, float, float, float getElementAttachedOffsets ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CVector vecPosition, vecRotation;
        if (CStaticFunctionDefinitions::GetElementAttachedOffsets(pElement, vecPosition, vecRotation))
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

int CLuaElementDefs::getElementColShape(lua_State* luaVM)
{
    //  colshape getElementColShape ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CColShape* pColShape = CStaticFunctionDefinitions::GetElementColShape(pElement);
        if (pColShape)
        {
            lua_pushelement(luaVM, pColShape);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementAlpha(lua_State* luaVM)
{
    //  int getElementAlpha ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        unsigned char ucAlpha;
        if (CStaticFunctionDefinitions::GetElementAlpha(pElement, ucAlpha))
        {
            lua_pushnumber(luaVM, ucAlpha);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::isElementDoubleSided(lua_State* luaVM)
{
    //  bool isElementDoubleSided ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        bool bDoubleSided;
        if (CStaticFunctionDefinitions::IsElementDoubleSided(pElement, bDoubleSided))
        {
            lua_pushboolean(luaVM, bDoubleSided);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementHealth(lua_State* luaVM)
{
    //  float getElementHealth ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        float fHealth;
        if (CStaticFunctionDefinitions::GetElementHealth(pElement, fHealth))
        {
            lua_pushnumber(luaVM, fHealth);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementModel(lua_State* luaVM)
{
    //  int getElementModel ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        unsigned short usModel;
        if (CStaticFunctionDefinitions::GetElementModel(pElement, usModel))
        {
            lua_pushnumber(luaVM, usModel);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementSyncer(lua_State* luaVM)
{
    //  element getElementSyncer ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        CElement* pSyncer = NULL;
        if ((pSyncer = CStaticFunctionDefinitions::GetElementSyncer(pElement)))
        {
            lua_pushelement(luaVM, pSyncer);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::getElementCollisionsEnabled(lua_State* luaVM)
{
    //  bool getElementCollisionsEnabled ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        lua_pushboolean(luaVM, CStaticFunctionDefinitions::GetElementCollisionsEnabled(pElement));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::isElementFrozen(lua_State* luaVM)
{
    //  bool isElementFrozen ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        bool bFrozen;
        if (CStaticFunctionDefinitions::IsElementFrozen(pElement, bFrozen))
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

int CLuaElementDefs::clearElementVisibleTo(lua_State* luaVM)
{
    //  bool clearElementVisibleTo ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::ClearElementVisibleTo(pElement))
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

int CLuaElementDefs::isElementVisibleTo(lua_State* luaVM)
{
    //  bool isElementVisibleTo ( element theElement, element visibleTo )
    CElement* pElement;
    CElement* pVisibleTo;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pVisibleTo);

    if (!argStream.HasErrors())
    {
        // Is it a per-player entity? If not return true as everything else is visible for everything.
        if (IS_PERPLAYER_ENTITY(pElement))
        {
            // Return true if we're visible for the given element
            if (static_cast<CPerPlayerEntity*>(pElement)->IsVisibleToReferenced(pVisibleTo))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
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

int CLuaElementDefs::isElementInWater(lua_State* luaVM)
{
    //  bool isElementInWater ( element theElement )
    CElement* pElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

    if (!argStream.HasErrors())
    {
        bool bInWater;
        if (CStaticFunctionDefinitions::IsElementInWater(pElement, bInWater))
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

int CLuaElementDefs::setElementID(lua_State* luaVM)
{
    //  bool setElementID ( element theElement, string name )
    CElement* pElement;
    SString   strId;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strId);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementID(pElement, strId))
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

int CLuaElementDefs::setElementData(lua_State* luaVM)
{
    //  bool setElementData ( element theElement, string key, var value, [var syncMode = true] )
    CElement*    pElement;
    SString      strKey;
    CLuaArgument value;
    ESyncType    syncType = ESyncType::BROADCAST;
    std::optional<eCustomDataClientTrust> clientTrust{};

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadLuaArgument(value);

    if (argStream.NextIsBool())
    {
        bool bSynchronize;
        argStream.ReadBool(bSynchronize, true);

        syncType = bSynchronize ? ESyncType::BROADCAST : ESyncType::LOCAL;
    }
    else
        argStream.ReadEnumString(syncType, ESyncType::BROADCAST);

    if (!argStream.NextIsNone())
    {
        eCustomDataClientTrust trustReaded;
        argStream.ReadEnumString(trustReaded);
        clientTrust = trustReaded;
    }

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
        {
            // Warn and truncate if key is too long
            m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                                                         *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
            strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
        }

        if (CStaticFunctionDefinitions::SetElementData(pElement, strKey, value, syncType, clientTrust))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::removeElementData(lua_State* luaVM)
{
    //  bool removeElementData ( element theElement, string key )
    CElement* pElement;
    SString   strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH)
        {
            // Warn and truncate if key is too long
            m_pScriptDebugging->LogCustom(luaVM, SString("Truncated argument @ '%s' [%s]", lua_tostring(luaVM, lua_upvalueindex(1)),
                                                         *SString("string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH)));
            strKey = strKey.Left(MAX_CUSTOMDATA_NAME_LENGTH);
        }

        if (CStaticFunctionDefinitions::RemoveElementData(pElement, strKey))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::addElementDataSubscriber(lua_State* luaVM)
{
    //  bool addElementDataSubscriber ( element theElement, string key, player thePlayer )
    CElement* pElement;
    SString   strKey;
    CPlayer*  pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::AddElementDataSubscriber(pElement, strKey, pPlayer))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::removeElementDataSubscriber(lua_State* luaVM)
{
    //  bool removeElementDataSubscriber ( element theElement, string key, player thePlayer )
    CElement* pElement;
    SString   strKey;
    CPlayer*  pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::RemoveElementDataSubscriber(pElement, strKey, pPlayer))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::hasElementDataSubscriber(lua_State* luaVM)
{
    //  bool hasElementDataSubscriber ( element theElement, string key, player thePlayer )
    CElement* pElement;
    SString   strKey;
    CPlayer*  pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadString(strKey);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        bool bResult = CStaticFunctionDefinitions::HasElementDataSubscriber(pElement, strKey, pPlayer);
        lua_pushboolean(luaVM, bResult);
        return 1;
    }

    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaElementDefs::setElementMatrix(lua_State* luaVM)
{
    //  setElementMatrix ( element theElement, table matrix )
    CElement* pElement;
    CMatrix   matrix;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);

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
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementMatrix(pElement, matrix))
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

int CLuaElementDefs::setElementParent(lua_State* luaVM)
{
    //  bool setElementParent ( element theElement, element parent )
    CElement* pElement;
    CElement* pParent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pParent);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementParent(pElement, pParent))
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

int CLuaElementDefs::setElementPosition(lua_State* luaVM)
{
    //  bool setElementPosition ( element theElement, float x, float y, float z [, bool warp = true ] )
    CElement* pElement;
    CVector   vecPosition;
    bool      bWarp;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    if (!argStream.HasErrors() && pElement->GetType() == CElement::RADAR_AREA)
    {
        // radar areas only take x and y
        CVector2D vecRadarPos;
        argStream.ReadVector2D(vecRadarPos);
        vecPosition.fX = vecRadarPos.fX;
        vecPosition.fY = vecRadarPos.fY;
    }
    else
    {
        argStream.ReadVector3D(vecPosition);
    }

    argStream.ReadBool(bWarp, true);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        // Set the position
        if (CStaticFunctionDefinitions::SetElementPosition(pElement, vecPosition, bWarp))
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

int CLuaElementDefs::setElementRotation(lua_State* luaVM)
{
    //  bool setElementRotation ( element theElement, float rotX, float rotY, float rotZ [, string rotOrder = "default", bool fixPedRotation = false ] )
    CElement*           pElement;
    CVector             vecRotation;
    eEulerRotationOrder rotationOrder;
    bool                bNewWay;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector3D(vecRotation);
    argStream.ReadEnumString(rotationOrder, EULER_DEFAULT);
    argStream.ReadBool(bNewWay, false);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        // Set the rotation
        if (CStaticFunctionDefinitions::SetElementRotation(pElement, vecRotation, rotationOrder, bNewWay))
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

int CLuaElementDefs::OOP_setElementRotation(lua_State* luaVM)
{
    // element.rotation = Vector3
    CElement* pElement = NULL;
    CVector   vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector3D(vecRotation);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        CMatrix matrix;

        // fill in our matrix
        pElement->GetMatrix(matrix);

        // degrees to radians
        ConvertDegreesToRadiansNoWrap(vecRotation);

        // set the matrix rotation (takes radians)
        matrix.SetRotation(vecRotation);

        // set the element matrix using the element specific SetMatrix function
        pElement->SetMatrix(matrix);

        // get the rotation (outputs radians)
        pElement->GetRotation(vecRotation);

        // convert radians to degrees
        ConvertRadiansToDegrees(vecRotation);

        eEulerRotationOrder rotationOrder = EULER_DEFAULT;
        if (pElement->GetType() == CElement::OBJECT)
            rotationOrder = EULER_ZYX;

        if (CStaticFunctionDefinitions::SetElementRotation(pElement, vecRotation, rotationOrder, true))
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

int CLuaElementDefs::setElementVelocity(lua_State* luaVM)
{
    //  bool setElementVelocity ( element theElement, float speedX, float speedY, float speedZ )
    CElement* pElement;
    CVector   vecVelocity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector3D(vecVelocity);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        // Set the velocity
        if (CStaticFunctionDefinitions::SetElementVelocity(pElement, vecVelocity))
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

int CLuaElementDefs::setElementTurnVelocity(lua_State* luaVM)
{
    //  bool setElementAngularVelocity ( element theElement, float spinX, float spinY, float spinZ )
    CElement* pElement;
    CVector   vecTurnVelocity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadVector3D(vecTurnVelocity);

    if (!argStream.HasErrors())
    {
        // Set the turn velocity
        if (CStaticFunctionDefinitions::SetElementAngularVelocity(pElement, vecTurnVelocity))
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

int CLuaElementDefs::setElementVisibleTo(lua_State* luaVM)
{
    //  bool setElementVisibleTo ( element theElement, element visibleTo, bool visible )
    CElement* pElement;
    CElement* pReference;
    bool      bVisible;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pReference);
    argStream.ReadBool(bVisible);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementVisibleTo(pElement, pReference, bVisible))
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

int CLuaElementDefs::setElementInterior(lua_State* luaVM)
{
    //  bool setElementInterior ( element theElement, int interior [, float x, float y, float z] )
    CElement* pElement;
    uint      uiInterior;
    CVector   vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(uiInterior);
    bool bSetPosition = argStream.NextIsVector3D();
    argStream.ReadVector3D(vecPosition, vecPosition);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        unsigned char ucInterior = static_cast<unsigned char>(uiInterior);
        if (CStaticFunctionDefinitions::SetElementInterior(pElement, ucInterior, bSetPosition, vecPosition))
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

int CLuaElementDefs::setElementDimension(lua_State* luaVM)
{
    //  bool setElementDimension ( element theElement, int dimension )
    CElement* pElement;
    int       iDimension = 0;
    bool      bMakeVisibleInAllDimensions = false;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(iDimension);

    if (iDimension == -1)
    {
        if (IS_OBJECT(pElement))
            bMakeVisibleInAllDimensions = true;
        else
            argStream.SetCustomError("The -1 value can be used only in objects!");
    }
    else if (iDimension < 0 || iDimension > 65535)
        argStream.SetCustomError("Invalid dimension range specified!");

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (bMakeVisibleInAllDimensions)
        {
            // Set the object visible in all dimensions
            if (CStaticFunctionDefinitions::SetObjectVisibleInAllDimensions(pElement, true))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
        {
            // If it's an object and is visible in all dimensions
            if (IS_OBJECT(pElement) && CStaticFunctionDefinitions::IsObjectVisibleInAllDimensions(pElement))
            {
                // Make it not visible in all dimensions and set its dimension
                if (CStaticFunctionDefinitions::SetObjectVisibleInAllDimensions(pElement, false, (unsigned short)iDimension))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
            else
            {
                if (CStaticFunctionDefinitions::SetElementDimension(pElement, (unsigned short)iDimension))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::attachElements(lua_State* luaVM)
{
    //  bool attachElements ( element theElement, element theAttachToElement, [ float xPosOffset, float yPosOffset, float zPosOffset, float xRotOffset, float
    //  yRotOffset, float zRotOffset ] )
    CElement* pElement;
    CElement* pAttachedToElement;
    CVector   vecPosition;
    CVector   vecRotation;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pAttachedToElement);
    argStream.ReadVector3D(vecPosition, vecPosition);
    argStream.ReadVector3D(vecRotation, vecRotation);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::AttachElements(pElement, pAttachedToElement, vecPosition, vecRotation))
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

int CLuaElementDefs::detachElements(lua_State* luaVM)
{
    //  bool detachElements ( element theElement, [ element theAttachToElement ] )
    CElement* pElement;
    CElement* pAttachedToElement;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadUserData(pAttachedToElement, NULL);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::DetachElements(pElement, pAttachedToElement))
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

int CLuaElementDefs::setElementAlpha(lua_State* luaVM)
{
    //  bool setElementAlpha ( element theElement, int alpha )
    CElement* pElement;
    uchar     ucAlpha;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucAlpha);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementAlpha(pElement, ucAlpha))
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

int CLuaElementDefs::setElementDoubleSided(lua_State* luaVM)
{
    //  bool setElementDoubleSided ( element theElement, bool enable )
    CElement* pElement;
    bool      bDoubleSided;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bDoubleSided);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetElementDoubleSided(pElement, bDoubleSided))
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

int CLuaElementDefs::setElementHealth(lua_State* luaVM)
{
    //  bool setElementHealth ( element theElement, float newHealth )
    CElement*        pElement = NULL;
    float            fHealth = 0.0f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(fHealth);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementHealth(pElement, fHealth))
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

int CLuaElementDefs::setElementModel(lua_State* luaVM)
{
    //  bool setElementModel ( element theElement, int model )
    CElement* pElement;
    ushort    usModel;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(usModel);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementModel(pElement, usModel))
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

int CLuaElementDefs::setElementSyncer(lua_State* luaVM)
{
    //  bool setElementSyncer ( element theElement, player thePlayer )
    CElement* pElement;
    CPlayer*  pPlayer = NULL;
    bool      bEnable = true;
    bool      bPersist = false;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    if (argStream.NextIsBool())
        argStream.ReadBool(bEnable);
    else
    {
        argStream.ReadUserData(pPlayer);
        argStream.ReadBool(bPersist, false);
    }

    if (!argStream.HasErrors())
    {
        bool bResult = CStaticFunctionDefinitions::SetElementSyncer(pElement, pPlayer, bEnable, bPersist);
        lua_pushboolean(luaVM, bResult);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::setElementCollisionsEnabled(lua_State* luaVM)
{
    //  bool setElementCollisionsEnabled ( element theElement, bool enabled )
    CElement* pElement;
    bool      bEnable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bEnable);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementCollisionsEnabled(pElement, bEnable))
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

int CLuaElementDefs::setElementFrozen(lua_State* luaVM)
{
    //  bool setElementFrozen ( element theElement, bool freezeStatus )
    CElement* pElement;
    bool      bFrozen;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadBool(bFrozen);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pElement);

        if (CStaticFunctionDefinitions::SetElementFrozen(pElement, bFrozen))
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

int CLuaElementDefs::getLowLODElement(lua_State* luaVM)
{
    //  element getLowLODElement ( element theElement )
    CElement* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        CElement* pLowLodEntity;
        if (CStaticFunctionDefinitions::GetLowLodElement(pEntity, pLowLodEntity))
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

int CLuaElementDefs::setLowLODElement(lua_State* luaVM)
{
    //  bool setLowLODElement ( element theElement )
    CElement* pEntity;
    CElement* pLowLodEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadUserData(pLowLodEntity, NULL);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetLowLodElement(pEntity, pLowLodEntity))
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

int CLuaElementDefs::isElementLowLOD(lua_State* luaVM)
{
    //  bool isElementLowLOD ( element theElement )
    CElement* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bIsLowLod;
        if (CStaticFunctionDefinitions::IsElementLowLod(pEntity, bIsLowLod))
        {
            lua_pushboolean(luaVM, bIsLowLod);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaElementDefs::setElementCallPropagationEnabled(lua_State* luaVM)
{
    //  bool setElementCallPropagationEnabled ( element theElement, bool enable )
    CElement* pEntity;
    bool      bEnable;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);
    argStream.ReadBool(bEnable);

    if (!argStream.HasErrors())
    {
        LogWarningIfPlayerHasNotJoinedYet(luaVM, pEntity);

        if (CStaticFunctionDefinitions::SetElementCallPropagationEnabled(pEntity, bEnable))
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

int CLuaElementDefs::isElementCallPropagationEnabled(lua_State* luaVM)
{
    //  bool isElementCallPropagationEnabled ( element theElement )
    CElement* pEntity;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pEntity);

    if (!argStream.HasErrors())
    {
        bool bEnabled;
        if (CStaticFunctionDefinitions::IsElementCallPropagationEnabled(pEntity, bEnabled))
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

bool CLuaElementDefs::IsElementOnFire(CElement* element) noexcept
{
    return element->IsOnFire();
}

bool CLuaElementDefs::SetElementOnFire(CElement* element, bool onFire) noexcept
{
    return CStaticFunctionDefinitions::SetElementOnFire(element, onFire);
}

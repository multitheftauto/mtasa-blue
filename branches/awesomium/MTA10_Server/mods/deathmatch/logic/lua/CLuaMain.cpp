/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaMain.cpp
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#include "CLuaFunctionDefinitions.h"
#include <clocale>

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000

extern CGame* g_pGame;
extern CNetServer* g_pRealNetServer;

// This script is loaded into all VM's created.
const char szPreloadedScript [] = ""\

    // Code for allowing this syntax:   exports.resourceName:exportedFunctionName (...)
    //                                  exports["resourceName"]:exportedFunctionName (...)
    //                                  exports[resourcePointer]:exportedFunctionName (...)
    // Aswell as the old:               call ( getResourceFromName ( "resourceName" ), "exportedFunctionName", ... )
    "local rescallMT = {}\n" \
    "function rescallMT:__index(k)\n" \
    "        if type(k) ~= 'string' then k = tostring(k) end\n" \
    "        self[k] = function(resExportTable, ...)\n" \
    "                if type(self.res) == 'userdata' and getResourceRootElement(self.res) then\n" \
    "                        return call(self.res, k, ...)\n" \
    "                else\n" \
    "                        return nil\n" \
    "                end\n" \
    "        end\n" \
    "        return self[k]\n" \
    "end\n" \
    "local exportsMT = {}\n" \
    "function exportsMT:__index(k)\n" \
    "        if type(k) == 'userdata' and getResourceRootElement(k) then\n" \
    "                return setmetatable({ res = k }, rescallMT)\n" \
    "        elseif type(k) ~= 'string' then\n" \
    "                k = tostring(k)\n" \
    "        end\n" \
    "        local res = getResourceFromName(k)\n" \
    "        if res and getResourceRootElement(res) then\n" \
    "                return setmetatable({ res = res }, rescallMT)\n" \
    "        else\n" \
    "                outputDebugString('exports: Call to non-running server resource (' .. k .. ')', 1)\n" \
    "                return setmetatable({}, rescallMT)\n" \
    "        end\n" \
    "end\n" \
    "exports = setmetatable({}, exportsMT)\n";

CLuaMain::CLuaMain ( CLuaManager* pLuaManager,
                     CObjectManager* pObjectManager,
                     CPlayerManager* pPlayerManager,
                     CVehicleManager* pVehicleManager,
                     CBlipManager* pBlipManager,
                     CRadarAreaManager* pRadarAreaManager,
                     CMapManager* pMapManager,
                     CResource* pResourceOwner, bool bEnableOOP  )
{
    // Initialise everything to be setup in the Start function
    m_pLuaManager = pLuaManager;
    m_luaVM = NULL;
    m_pResource = pResourceOwner;
    m_pResourceFile = NULL;
    m_bBeingDeleted = false;
    m_pLuaTimerManager = new CLuaTimerManager;
    m_FunctionEnterTimer.SetMaxIncrement ( 500 );
    m_WarningTimer.SetMaxIncrement ( 1000 );
    m_uiOpenFileCountWarnThresh = 10;
    m_uiOpenXMLFileCountWarnThresh = 20;

    m_pObjectManager = pObjectManager;
    m_pPlayerManager = pPlayerManager;
    m_pRadarAreaManager = pRadarAreaManager;
    m_pVehicleManager = pVehicleManager;
    m_pBlipManager = pBlipManager;
    m_pMapManager = pMapManager;

    m_bEnableOOP = bEnableOOP;


    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainCreate ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainCreate ( this );
}


CLuaMain::~CLuaMain ( void )
{
    // remove all current remote calls originating from this VM
    g_pGame->GetRemoteCalls()->Remove ( this );
    g_pGame->GetLuaCallbackManager ()->OnLuaMainDestroy ( this );
    g_pGame->GetLatentTransferManager ()->OnLuaMainDestroy ( this );
    g_pGame->GetDebugHookManager()->OnLuaMainDestroy ( this );
    g_pGame->GetScriptDebugging()->OnLuaMainDestroy ( this );

    // Unload the current script
    UnloadScript ();
    m_bBeingDeleted = true;

    // Delete the timer manager
    delete m_pLuaTimerManager;

    // Eventually delete the XML files the LUA script didn't
    list<CXMLFile *>::iterator iterXML = m_XMLFiles.begin ();
    for ( ; iterXML != m_XMLFiles.end (); ++iterXML )
    {
        delete *iterXML;
    }

    // Eventually delete the text displays the LUA script didn't
    list<CTextDisplay *>::iterator iterDisplays = m_Displays.begin ();
    for ( ; iterDisplays != m_Displays.end (); ++iterDisplays )
    {
        delete *iterDisplays;
    }

    // Eventually delete the text items the LUA script didn't
    list<CTextItem *>::iterator iterItems = m_TextItems.begin ();
    for ( ; iterItems != m_TextItems.end (); ++iterItems )
    {
        delete *iterItems;
    }

    CPerfStatLuaMemory::GetSingleton ()->OnLuaMainDestroy ( this );
    CPerfStatLuaTiming::GetSingleton ()->OnLuaMainDestroy ( this );
}

bool CLuaMain::BeingDeleted ( void )
{
    return m_bBeingDeleted;
}


void CLuaMain::ResetInstructionCount ( void )
{
    m_FunctionEnterTimer.Reset ();
}


void CLuaMain::InitSecurity ( void )
{
    lua_register ( m_luaVM, "dofile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadfile", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "require", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "loadlib", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "getfenv", CLuaFunctionDefinitions::DisabledFunction );
    lua_register ( m_luaVM, "newproxy", CLuaFunctionDefinitions::DisabledFunction );
}

void CLuaMain::AddVector4DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classmetamethod ( luaVM, "__tostring", CLuaVector4Defs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaVector4Defs::Destroy );

    lua_classmetamethod ( luaVM, "__add", CLuaVector4Defs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaVector4Defs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaVector4Defs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaVector4Defs::Div );
    lua_classmetamethod ( luaVM, "__pow", CLuaVector4Defs::Pow );
    lua_classmetamethod ( luaVM, "__unm", CLuaVector4Defs::Unm );
    lua_classmetamethod ( luaVM, "__eq", CLuaVector4Defs::Eq );

    lua_classfunction ( luaVM, "create", "", CLuaVector4Defs::Create );
    lua_classfunction ( luaVM, "normalize", "", CLuaVector4Defs::Normalize );
    lua_classfunction ( luaVM, "dot", "", CLuaVector4Defs::Dot );

    lua_classfunction ( luaVM, "getLength", "", CLuaVector4Defs::GetLength );
    lua_classfunction ( luaVM, "getSquaredLength", "", CLuaVector4Defs::GetLengthSquared );
    lua_classfunction ( luaVM, "getNormalized", "", CLuaVector4Defs::GetNormalized );
    lua_classfunction ( luaVM, "getX", "", CLuaVector4Defs::GetX );
    lua_classfunction ( luaVM, "getY", "", CLuaVector4Defs::GetY );
    lua_classfunction ( luaVM, "getZ", "", CLuaVector4Defs::GetZ );
    lua_classfunction ( luaVM, "getW", "", CLuaVector4Defs::GetW );

    lua_classfunction ( luaVM, "setX", "", CLuaVector4Defs::SetX );
    lua_classfunction ( luaVM, "setY", "", CLuaVector4Defs::SetY );
    lua_classfunction ( luaVM, "setZ", "", CLuaVector4Defs::SetZ );
    lua_classfunction ( luaVM, "setW", "", CLuaVector4Defs::SetW );

    lua_classvariable ( luaVM, "x", "", "", CLuaVector4Defs::SetX, CLuaVector4Defs::GetX );
    lua_classvariable ( luaVM, "y", "", "", CLuaVector4Defs::SetY, CLuaVector4Defs::GetY );
    lua_classvariable ( luaVM, "z", "", "", CLuaVector4Defs::SetZ, CLuaVector4Defs::GetZ );
    lua_classvariable ( luaVM, "w", "", "", CLuaVector4Defs::SetW, CLuaVector4Defs::GetW );

    lua_classvariable ( luaVM, "length", "", "", NULL, CLuaVector4Defs::GetLength );
    lua_classvariable ( luaVM, "squaredLength", "", "", NULL, CLuaVector4Defs::GetLengthSquared );
    lua_classvariable ( luaVM, "normalized", "", "", NULL, CLuaVector4Defs::GetNormalized );

    lua_registerclass ( luaVM, "Vector4" );
}


void CLuaMain::AddVector3DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classmetamethod ( luaVM, "__tostring", CLuaVector3Defs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaVector3Defs::Destroy );
    
    lua_classmetamethod ( luaVM, "__add", CLuaVector3Defs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaVector3Defs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaVector3Defs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaVector3Defs::Div );
    lua_classmetamethod ( luaVM, "__pow", CLuaVector3Defs::Pow );
    lua_classmetamethod ( luaVM, "__unm", CLuaVector3Defs::Unm );
    lua_classmetamethod ( luaVM, "__eq", CLuaVector3Defs::Eq );
    
    lua_classfunction ( luaVM, "create", "", CLuaVector3Defs::Create );
    lua_classfunction ( luaVM, "normalize", "", CLuaVector3Defs::Normalize );
    lua_classfunction ( luaVM, "cross", "", CLuaVector3Defs::Cross );
    lua_classfunction ( luaVM, "dot", "", CLuaVector3Defs::Dot );
    
    lua_classfunction ( luaVM, "getLength", "", CLuaVector3Defs::GetLength );
    lua_classfunction ( luaVM, "getSquaredLength", "", CLuaVector3Defs::GetLengthSquared );
    lua_classfunction ( luaVM, "getNormalized", "", CLuaVector3Defs::GetNormalized );
    lua_classfunction ( luaVM, "getX", "", CLuaVector3Defs::GetX );
    lua_classfunction ( luaVM, "getY", "", CLuaVector3Defs::GetY );
    lua_classfunction ( luaVM, "getZ", "", CLuaVector3Defs::GetZ );
    
    lua_classfunction ( luaVM, "setX", "", CLuaVector3Defs::SetX );
    lua_classfunction ( luaVM, "setY", "", CLuaVector3Defs::SetY );
    lua_classfunction ( luaVM, "setZ", "", CLuaVector3Defs::SetZ );
    
    lua_classvariable ( luaVM, "x", "", "", CLuaVector3Defs::SetX, CLuaVector3Defs::GetX );
    lua_classvariable ( luaVM, "y", "", "", CLuaVector3Defs::SetY, CLuaVector3Defs::GetY );
    lua_classvariable ( luaVM, "z", "", "", CLuaVector3Defs::SetZ, CLuaVector3Defs::GetZ );
    lua_classvariable ( luaVM, "length", "", "", NULL, CLuaVector3Defs::GetLength );
    lua_classvariable ( luaVM, "squaredLength", "", "", NULL, CLuaVector3Defs::GetLengthSquared );
    lua_classvariable ( luaVM, "normalized", "", "", NULL, CLuaVector3Defs::GetNormalized );
    
    lua_registerclass ( luaVM, "Vector3" );
}


void CLuaMain::AddVector2DClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classmetamethod ( luaVM, "__tostring", CLuaVector2Defs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaVector2Defs::Destroy );

    lua_classmetamethod ( luaVM, "__add", CLuaVector2Defs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaVector2Defs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaVector2Defs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaVector2Defs::Div );
    lua_classmetamethod ( luaVM, "__pow", CLuaVector2Defs::Pow );
    lua_classmetamethod ( luaVM, "__unm", CLuaVector2Defs::Unm );
    lua_classmetamethod ( luaVM, "__eq", CLuaVector2Defs::Eq );

    lua_classfunction ( luaVM, "create", "", CLuaVector2Defs::Create );
    lua_classfunction ( luaVM, "normalize", "", CLuaVector2Defs::Normalize );
    lua_classfunction ( luaVM, "dot", "", CLuaVector2Defs::Dot );

    lua_classfunction ( luaVM, "getLength", "", CLuaVector2Defs::GetLength );
    lua_classfunction ( luaVM, "getSquaredLength", "", CLuaVector2Defs::GetLengthSquared );
    lua_classfunction ( luaVM, "getNormalized", "", CLuaVector2Defs::GetNormalized );
    lua_classfunction ( luaVM, "getX", "", CLuaVector2Defs::GetX );
    lua_classfunction ( luaVM, "getY", "", CLuaVector2Defs::GetY );

    lua_classfunction ( luaVM, "setX", "", CLuaVector2Defs::SetX );
    lua_classfunction ( luaVM, "setY", "", CLuaVector2Defs::SetY );

    lua_classvariable ( luaVM, "x", "", "", CLuaVector2Defs::SetX, CLuaVector2Defs::GetX );
    lua_classvariable ( luaVM, "y", "", "", CLuaVector2Defs::SetY, CLuaVector2Defs::GetY );

    lua_classvariable ( luaVM, "length", "", "", NULL, CLuaVector2Defs::GetLength );
    lua_classvariable ( luaVM, "squaredLength", "", "", NULL, CLuaVector2Defs::GetLengthSquared );
    lua_classvariable ( luaVM, "normalized", "", "", NULL, CLuaVector2Defs::GetNormalized );

    lua_registerclass ( luaVM, "Vector2" );
}


void CLuaMain::AddMatrixClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classmetamethod ( luaVM, "__tostring", CLuaMatrixDefs::ToString );
    lua_classmetamethod ( luaVM, "__gc", CLuaMatrixDefs::Destroy );
    
    lua_classmetamethod ( luaVM, "__add", CLuaMatrixDefs::Add );
    lua_classmetamethod ( luaVM, "__sub", CLuaMatrixDefs::Sub );
    lua_classmetamethod ( luaVM, "__mul", CLuaMatrixDefs::Mul );
    lua_classmetamethod ( luaVM, "__div", CLuaMatrixDefs::Div );
    
    lua_classfunction ( luaVM, "create", "", CLuaMatrixDefs::Create );
    lua_classfunction ( luaVM, "transformPosition", "", CLuaMatrixDefs::TransformPosition );
    lua_classfunction ( luaVM, "transformDirection", "", CLuaMatrixDefs::TransformDirection );
    lua_classfunction ( luaVM, "inverse", "", CLuaMatrixDefs::Inverse );

    lua_classfunction ( luaVM, "getPosition", "", CLuaMatrixDefs::GetPosition );
    lua_classfunction ( luaVM, "getRotation", "", CLuaMatrixDefs::GetRotation );
    lua_classfunction ( luaVM, "getForward", "", CLuaMatrixDefs::GetForward );
    lua_classfunction ( luaVM, "getRight", "", CLuaMatrixDefs::GetRight );
    lua_classfunction ( luaVM, "getUp", "", CLuaMatrixDefs::GetUp );
    
    lua_classfunction ( luaVM, "setPosition", "", CLuaMatrixDefs::SetPosition );
    lua_classfunction ( luaVM, "setRotation", "", CLuaMatrixDefs::SetRotation );
    lua_classfunction ( luaVM, "setForward", "", CLuaMatrixDefs::SetForward );
    lua_classfunction ( luaVM, "setRight", "", CLuaMatrixDefs::SetRight );
    lua_classfunction ( luaVM, "setUp", "", CLuaMatrixDefs::SetUp );
    
    lua_classvariable ( luaVM, "position", "", "", CLuaMatrixDefs::SetPosition, CLuaMatrixDefs::GetPosition );
    lua_classvariable ( luaVM, "rotation", "", "", CLuaMatrixDefs::SetRotation, CLuaMatrixDefs::GetRotation );
    lua_classvariable ( luaVM, "forward", "", "", CLuaMatrixDefs::SetForward, CLuaMatrixDefs::GetForward );
    lua_classvariable ( luaVM, "right", "", "", CLuaMatrixDefs::SetRight, CLuaMatrixDefs::GetRight );
    lua_classvariable ( luaVM, "up", "", "", CLuaMatrixDefs::SetUp, CLuaMatrixDefs::GetUp );
    
    lua_registerclass ( luaVM, "Matrix" );
}


// TODO: position, rotation and velocity classes, data specials
void CLuaMain::AddElementClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getByID", "getElementByID" );
    lua_classfunction ( luaVM, "getAllByType", "getElementsByType" );
    lua_classfunction ( luaVM, "getByIndex", "getElementByIndex" );
    
    lua_classfunction ( luaVM, "create", "createElement" );
    lua_classfunction ( luaVM, "clone", "cloneElement" );
    lua_classfunction ( luaVM, "destroy", "destroyElement" );
    lua_classfunction ( luaVM, "clearVisibility", "clearElementVisibleTo" );
    lua_classfunction ( luaVM, "attach", "attachElements" );
    lua_classfunction ( luaVM, "detach", "detachElements" );
    lua_classfunction ( luaVM, "removeData", "removeElementData" );
    
    lua_classfunction ( luaVM, "setParent", "setElementParent" );
    lua_classfunction ( luaVM, "setVelocity", "setElementVelocity" );
    lua_classfunction ( luaVM, "setFrozen", "setElementFrozen" );
    lua_classfunction ( luaVM, "setHealth", "setElementHealth" );
    lua_classfunction ( luaVM, "setModel", "setElementModel" );
    lua_classfunction ( luaVM, "setSyncer", "setElementSyncer" );
    lua_classfunction ( luaVM, "setAlpha", "setElementAlpha" );
    lua_classfunction ( luaVM, "setDoubleSided", "setElementDoubleSided" );
    lua_classfunction ( luaVM, "setCollisionsEnabled", "setElementCollisionsEnabled" );
    lua_classfunction ( luaVM, "setData", "setElementData" );
    lua_classfunction ( luaVM, "setPosition", "setElementPosition" );
    lua_classfunction ( luaVM, "setRotation", "setElementRotation" );
    lua_classfunction ( luaVM, "setVelocity", "setElementVelocity" );
    lua_classfunction ( luaVM, "setMatrix", "setElementMatrix" );
    lua_classfunction ( luaVM, "setID", "setElementID" );
    lua_classfunction ( luaVM, "setInterior", "setElementInterior" );
    lua_classfunction ( luaVM, "setDimension", "setElementDimension" );
    lua_classfunction ( luaVM, "setAttachedOffsets", "setElementAttachedOffsets" );
    lua_classfunction ( luaVM, "setCallPropagationEnabled", "setElementCallPropagationEnabled" );
    
    lua_classfunction ( luaVM, "getAttachedOffsets", "getElementAttachedOffsets" );
    lua_classfunction ( luaVM, "getChild", "getElementChild" );
    lua_classfunction ( luaVM, "getChildren", "getElementChildren" );
    lua_classfunction ( luaVM, "getParent", "getElementParent" );
    lua_classfunction ( luaVM, "getAttachedElements", "getAttachedElements" );
    lua_classfunction ( luaVM, "getAttachedTo", "getAttachedTo" );
    lua_classfunction ( luaVM, "getVelocity", "getElementVelocity" );
    lua_classfunction ( luaVM, "getID", "getElementID" );
    lua_classfunction ( luaVM, "getZoneName", "getElementZoneName" );
    lua_classfunction ( luaVM, "getAlpha", "getElementAlpha" );
    lua_classfunction ( luaVM, "getHealth", "getElementHealth" );
    lua_classfunction ( luaVM, "getModel", "getElementModel" );
    lua_classfunction ( luaVM, "getChildrenCount", "getElementChildrenCount" );
    lua_classfunction ( luaVM, "getSyncer", "getElementSyncer" );
    lua_classfunction ( luaVM, "getAllData", "getAllElementData" );
    lua_classfunction ( luaVM, "getColShape", "getElementColShape" );
    lua_classfunction ( luaVM, "getData", "getElementData" );
    lua_classfunction ( luaVM, "getPosition", "getElementPosition", CLuaOOPDefs::GetElementPosition );
    lua_classfunction ( luaVM, "getRotation", "getElementRotation", CLuaOOPDefs::GetElementRotation );
    lua_classfunction ( luaVM, "getMatrix", "getElementMatrix", CLuaOOPDefs::GetElementMatrix );
    lua_classfunction ( luaVM, "getPosition", "getElementPosition" );
    lua_classfunction ( luaVM, "getRotation", "getElementRotation" );
    lua_classfunction ( luaVM, "getType", "getElementType" );
    lua_classfunction ( luaVM, "getInterior", "getElementInterior" );
    lua_classfunction ( luaVM, "getDimension", "getElementDimension" );
    lua_classfunction ( luaVM, "getLowLOD", "getLowLODElement" );
    
    lua_classfunction ( luaVM, "areCollisionsEnabled", "areElementCollisionsEnabled" );
    lua_classfunction ( luaVM, "isCallPropagationEnabled", "isElementCallPropagationEnabled" );
    lua_classfunction ( luaVM, "isWithinMarker", "isElementWithinMarker" );
    lua_classfunction ( luaVM, "isWithinColShape", "isElementWithinColShape" );
    lua_classfunction ( luaVM, "isFrozen", "isElementFrozen" );
    lua_classfunction ( luaVM, "isInWater", "isElementInWater" );
    lua_classfunction ( luaVM, "isDoubleSided", "isElementDoubleSided" );
    lua_classfunction ( luaVM, "isVisibleTo", "isElementVisibleTo" );
    lua_classfunction ( luaVM, "isLowLOD", "isElementLowLOD" );
    lua_classfunction ( luaVM, "isAttached", "isElementAttached" );

    lua_classvariable ( luaVM, "id", "setElementID", "getElementID" );
    lua_classvariable ( luaVM, "callPropagationEnabled", "setElementCallPropagationEnabled", "isElementCallPropagationEnabled" );
    lua_classvariable ( luaVM, "parent", "setElementParent", "getElementParent" );
    lua_classvariable ( luaVM, "zoneName", NULL, "getElementZoneName" );
    lua_classvariable ( luaVM, "attachedTo", "attachElements", "getElementAttachedTo" );
    lua_classvariable ( luaVM, "children", NULL, "getElementChildren" ); // should this be a special table using getElementChild instead?
    lua_classvariable ( luaVM, "frozen", "setElementFrozen", "isElementFrozen" );
    lua_classvariable ( luaVM, "attachedElements", NULL, "getAttachedElements" );
    lua_classvariable ( luaVM, "inWater", NULL, "isElementInWater" );
    lua_classvariable ( luaVM, "health", "setElementHealth", "getElementHealth" );
    lua_classvariable ( luaVM, "alpha", "setElementAlpha", "getElementAlpha" );
    lua_classvariable ( luaVM, "type", NULL, "getElementType" );
    lua_classvariable ( luaVM, "dimension", "setElementDimension", "getElementDimension" );
    lua_classvariable ( luaVM, "doubleSided", "setElementDoubleSided", "isElementDoubleSided" );
    lua_classvariable ( luaVM, "model", "setElementModel", "getElementModel" );
    lua_classvariable ( luaVM, "lowLOD", "setLowLODElement", "getLowLODElement" );
    lua_classvariable ( luaVM, "syncer", NULL, "isElementSyncer" );
    lua_classvariable ( luaVM, "childrenCount", NULL, "getElementChildrenCount" );
    lua_classvariable ( luaVM, "interior", "setElementInterior", "getElementInterior" );
    lua_classvariable ( luaVM, "colShape", NULL, "getElementColShape" );
    lua_classvariable ( luaVM, "collisions", "setElementCollisionsEnabled", "getElementCollisionsEnabled" );
    lua_classvariable ( luaVM, "position", "setElementPosition", "getElementPosition", CLuaElementDefs::setElementPosition, CLuaOOPDefs::GetElementPosition );
    lua_classvariable ( luaVM, "rotation", "setElementRotation", "getElementRotation", CLuaOOPDefs::SetElementRotation, CLuaOOPDefs::GetElementRotation );
    lua_classvariable ( luaVM, "matrix", "setElementMatrix", "getElementMatrix", CLuaElementDefs::setElementMatrix, CLuaOOPDefs::GetElementMatrix );
    lua_classvariable ( luaVM, "velocity", "setElementVelocity", "getElementVelocity" );
    //lua_classvariable ( luaVM, "data", "setElementData", "getElementData", CLuaOOPDefs::SetElementData, CLuaOOPDefs::GetElementData );
    //lua_classvariable ( luaVM, "visibility", "setElementVisibleTo", "isElementVisibleTo", CLuaOOPDefs::SetElementVisibleTo, CLuaOOPDefs::IsElementVisibleTo ); // .visibility[john]=false
    
    lua_registerclass ( luaVM, "Element" );
}

// TODO: specials
void CLuaMain::AddACLClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "save", "aclSave" );
    lua_classfunction ( luaVM, "get", "aclGet" );
    lua_classfunction ( luaVM, "reload", "aclReload" );
    lua_classfunction ( luaVM, "list", "aclList" );
    lua_classfunction ( luaVM, "hasObjectPermissionTo", "hasObjectPermissionTo" );

    
    //lua_classfunction ( luaVM, "addToGroup", "aclGroupAddACL" ); // swap args
    //lua_classfunction ( luaVM, "removeFromGroup", "aclGroupRemoveACL" ); // swap args
    
    lua_classfunction ( luaVM, "create", "aclCreate" );
    lua_classfunction ( luaVM, "destroy", "aclDestroy" );
    lua_classfunction ( luaVM, "listRights", "aclListRights" );
    lua_classfunction ( luaVM, "removeRight", "aclRemoveRight" );
    
    lua_classfunction ( luaVM, "getName", "aclGetName" );
    lua_classfunction ( luaVM, "getRight", "aclGetRight" );

    lua_classfunction ( luaVM, "setRight", "aclSetRight" );
    
    lua_classvariable ( luaVM, "name", NULL, "aclGetName" );
    //lua_classvariable ( luaVM, "rights", "", "aclListRights", NULL, CLuaOOPDefs::AclListRights ); // .rights[allowedType] = {..}
    //lua_classvariable ( luaVM, "right", "aclSetRight", "aclGetRight", CLuaOOPDefs::AclSetRight, CLuaOOPDefs::AclGetRight ); // .right["e.y.e"] = "illuminati"; if value == nil then aclRemoveRight(self, key)
    
    lua_registerclass ( luaVM, "ACL" );
}


// TODO: specials
void CLuaMain::AddACLGroupClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "get", "aclGetGroup" );
    lua_classfunction ( luaVM, "list", "aclGroupList" );

    lua_classfunction ( luaVM, "create", "aclCreateGroup" );
    lua_classfunction ( luaVM, "destroy", "aclDestroyGroup" );
    lua_classfunction ( luaVM, "addACL", "aclGroupAddACL" );
    lua_classfunction ( luaVM, "addObject", "aclGroupAddObject" );
    lua_classfunction ( luaVM, "removeACL", "aclGroupRemoveACL" );
    lua_classfunction ( luaVM, "removeObject", "aclGroupRemoveObject" );
    lua_classfunction ( luaVM, "listACL", "aclGroupListACL" );
    lua_classfunction ( luaVM, "listObjects", "aclGroupListObjects" );

    lua_classfunction ( luaVM, "getName", "aclGroupGetName" );
    //lua_classfunction ( luaVM, "doesContainObject", "isObjectInACLGroup" ); // swap args

    
    lua_classvariable ( luaVM, "name", NULL, "aclGroupGetName" );
    lua_classvariable ( luaVM, "aclList", NULL, "aclGroupListACL" ); // value nil = remove acl
    lua_classvariable ( luaVM, "objects", NULL, "aclGroupListObjects" ); // value nil = remove object
    
    lua_registerclass ( luaVM, "ACLGroup" );
}


// TODO: special
void CLuaMain::AddAccountClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getAll", "getAllAccounts" );
    lua_classfunction ( luaVM, "getAllBySerial", "getAccountsBySerial" );
    lua_classfunction ( luaVM, "getFromPlayer", "getPlayerAccount" );
    lua_classfunction ( luaVM, "logPlayerOut", "logOut" );

    lua_classfunction ( luaVM, "create", "getAccount" );
    lua_classfunction ( luaVM, "add", "addAccount" );
    lua_classfunction ( luaVM, "copyDataFrom", "copyAccountData" );
    //lua_classfunction ( luaVM, "copyDataTo", "copyAccountData" ); // swap args
    //lua_classfunction ( luaVM, "logIn", "logIn" ); // swap args
    lua_classfunction ( luaVM, "remove", "removeAccount" );

    lua_classfunction ( luaVM, "setData", "setAccountData" );
    lua_classfunction ( luaVM, "setPassword", "setAccountPassword" );

    lua_classfunction ( luaVM, "getSerial", "getAccountSerial" );
    lua_classfunction ( luaVM, "getData", "getAccountData" );
    lua_classfunction ( luaVM, "getAllData", "getAllAccountData" );
    lua_classfunction ( luaVM, "getName", "getAccountName" );
    lua_classfunction ( luaVM, "getPlayer", "getAccountPlayer" );
    lua_classfunction ( luaVM, "isGuest", "isGuestAccount" );

    lua_classvariable ( luaVM, "serial", NULL, "getAccountSerial" );
    lua_classvariable ( luaVM, "name", NULL, "getAccountName" );
    lua_classvariable ( luaVM, "player", NULL, "getAccountPlayer" );
    lua_classvariable ( luaVM, "guest", NULL, "isGuestAccount" );
    lua_classvariable ( luaVM, "password", "setAccountPassword", NULL );
    lua_classvariable ( luaVM, "data", NULL, "getAllAccountData" ); // allow setting
    
    lua_registerclass ( luaVM, "Account" );
    
}


// TODO: The "set" attribute of .admin, .reason and .unbanTime needs to be checked for syntax
void CLuaMain::AddBanClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "addBan" );
    lua_classfunction ( luaVM, "remove", "removeBan" );
    lua_classfunction ( luaVM, "getList", "getBans" );
    lua_classvariable ( luaVM, "list", NULL, "getBans" );
    
    lua_classfunction ( luaVM, "getAdmin", "getBanAdmin" );
    lua_classfunction ( luaVM, "getIP", "getBanIP" );
    lua_classfunction ( luaVM, "getNick", "getBanNick" );
    lua_classfunction ( luaVM, "getReason", "getBanReason" );
    lua_classfunction ( luaVM, "getSerial", "getBanSerial" );
    lua_classfunction ( luaVM, "getTime", "getBanTime" );
    lua_classfunction ( luaVM, "getUnbanTime", "getUnbanTime" );
    
    lua_classvariable ( luaVM, "admin", "setBanAdmin", "getBanAdmin" );
    lua_classvariable ( luaVM, "IP", NULL, "getBanIP" );
    lua_classvariable ( luaVM, "nick", NULL, "getBanNick" );
    lua_classvariable ( luaVM, "serial", NULL, "getBanSerial" );
    lua_classvariable ( luaVM, "time", NULL, "getBanTime" );
    lua_classvariable ( luaVM, "unbanTime", NULL, "getUnbanTime" );
    lua_classvariable ( luaVM, "reason", "setBanReason", "getBanReason" );
    
    lua_registerclass ( luaVM, "Ban" );
}


void CLuaMain::AddBlipClass( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createBlip" );
    lua_classfunction ( luaVM, "createAttachedTo", "createBlipAttachedTo" );
    
    lua_classfunction ( luaVM, "getColor", "getBlipColor" );
    lua_classfunction ( luaVM, "getVisibleDistance", "getBlipVisibleDistance" );
    lua_classfunction ( luaVM, "getOrdering", "getBlipOrdering" );
    lua_classfunction ( luaVM, "getSize", "getBlipSize" );
    lua_classfunction ( luaVM, "getIcon", "getBlipIcon" );
    
    lua_classfunction ( luaVM, "setColor", "setBlipColor" );
    lua_classfunction ( luaVM, "setVisibleDistance", "setBlipVisibleDistance" );
    lua_classfunction ( luaVM, "setOrdering", "setBlipOrdering" );
    lua_classfunction ( luaVM, "setSize", "setBlipSize" );
    lua_classfunction ( luaVM, "setIcon", "setBlipIcon" );
    
    lua_classvariable ( luaVM, "icon", "setBlipIcon", "getBlipIcon" );
    lua_classvariable ( luaVM, "size", "setBlipSize", "getBlipSize" );
    lua_classvariable ( luaVM, "ordering", "setBlipOrdering", "getBlipOrdering" );
    lua_classvariable ( luaVM, "visibleDistance", "setBlipVisibleDistance", "getBlipVisibleDistance" );
    //lua_classvariable ( luaVM, "color", "setBlipColor", "getBlipColor", "" ); color
    
    lua_registerclass ( luaVM, "Blip", "Element" );
    
}


void CLuaMain::AddColShapeClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "Circle", "createColCircle" );
    lua_classfunction ( luaVM, "Cuboid", "createColCuboid" );
    lua_classfunction ( luaVM, "Rectangle", "createColRectangle" );
    lua_classfunction ( luaVM, "Sphere", "createColSphere" );
    lua_classfunction ( luaVM, "Tube", "createColTube" );
    lua_classfunction ( luaVM, "Polygon", "createColPolygon" );

    lua_classfunction ( luaVM, "getElementsWithin", "getElementsWithinColShape" );
    //lua_classfunction ( luaVM, "isElementWithin", "isElementWithinColShape" ); TODO: swap args around.
    
    lua_registerclass ( luaVM, "ColShape", "Element" );
}


void CLuaMain::AddFileClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "fileOpen" );
    lua_classfunction ( luaVM, "destroy", "fileClose" );
    lua_classfunction ( luaVM, "close", "fileClose" );
    lua_classfunction ( luaVM, "new", "fileCreate" );
    
    lua_classfunction ( luaVM, "delete", "fileDelete" );
    lua_classfunction ( luaVM, "exists", "fileExists" );
    lua_classfunction ( luaVM, "flush", "fileFlush" );
    lua_classfunction ( luaVM, "getPos", "fileGetPos" );
    lua_classfunction ( luaVM, "getSize", "fileGetSize" );
    lua_classfunction ( luaVM, "isEOF", "fileIsEOF" );
    lua_classfunction ( luaVM, "read", "fileRead" );
    lua_classfunction ( luaVM, "rename", "fileRename" );
    lua_classfunction ( luaVM, "setPos", "fileSetPos" );
    lua_classfunction ( luaVM, "write", "fileWrite" );
    lua_classfunction ( luaVM, "copy", "fileCopy" );
    
    lua_classvariable ( luaVM, "pos", "fileSetPos", "fileGetPos" );
    lua_classvariable ( luaVM, "size", NULL, "fileGetSize" );
    lua_classvariable ( luaVM, "isEOF", NULL, "fileIsEOF" );

    lua_registerclass ( luaVM, "File" );
}


void CLuaMain::AddMarkerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createMarker" );
    lua_classfunction ( luaVM, "getCount", "getMarkerCount" );
    //lua_classfunction ( luaVM, "isElementWithin", "isElementWithinMarker", CLuaOOPDefs::isElementWithinMarker ); TODO: swap args
    
    lua_classfunction ( luaVM, "getType", "getMarkerType" );
    lua_classfunction ( luaVM, "getIcon", "getMarkerIcon" );
    lua_classfunction ( luaVM, "getSize", "getMarkerSize" );
    lua_classfunction ( luaVM, "getTarget", "getMarkerTarget" ); // vector
    lua_classfunction ( luaVM, "getColor", "getMarkerColor" ); // color
    
    lua_classfunction ( luaVM, "setType", "setMarkerType" );
    lua_classfunction ( luaVM, "setIcon", "setMarkerIcon" );
    lua_classfunction ( luaVM, "setSize", "setMarkerSize" );
    lua_classfunction ( luaVM, "setTarget", "setMarkerTarget" ); // vector
    lua_classfunction ( luaVM, "setColor", "setMarkerColor" ); // color
    
    lua_classvariable ( luaVM, "type", "setMarkerType", "getMarkerType" );
    lua_classvariable ( luaVM, "icon", "setMarkerIcon", "getMarkerIcon" );
    lua_classvariable ( luaVM, "size", "setMarkerSize", "getMarkerSize" );
    
    //lua_classvariable ( luaVM, "target", CLuaOOPDefs::SetMarkerTarget, CLuaOOPDefs::GetMarkerTarget ); vector
    //lua_classvariable ( luaVM, "color", CLuaOOPDefs::SetMarkerColor, CLuaOOPDefs::GetMarkerColor ); color

    lua_registerclass ( luaVM, "Marker", "Element" );
}


void CLuaMain::AddObjectClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createObject" );
    lua_classfunction ( luaVM, "move", "moveObject" );
    lua_classfunction ( luaVM, "stop", "stopObject" );
    
    lua_classfunction ( luaVM, "getScale", "getObjectScale" );
    lua_classfunction ( luaVM, "setScale", "setObjectScale" );
    
    lua_classvariable ( luaVM, "scale", "setObjectScale", "getObjectScale" );

    lua_registerclass ( luaVM, "Object", "Element" );
}


// TODO: specials
void CLuaMain::AddPedClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getValidModels", "getValidPedModels" );
    
    lua_classfunction ( luaVM, "create", "createPed" );
    lua_classfunction ( luaVM, "kill", "killPed" );
    lua_classfunction ( luaVM, "warpIntoVehicle", "warpPedIntoVehicle" );
    lua_classfunction ( luaVM, "addClothes", "addPedClothes" );
    lua_classfunction ( luaVM, "giveJetPack", "givePedJetPack" );
    lua_classfunction ( luaVM, "reloadWeapon", "reloadPedWeapon" );
    lua_classfunction ( luaVM, "removeClothes", "removePedClothes" );
    lua_classfunction ( luaVM, "removeFromVehicle", "removePedFromVehicle" );
    lua_classfunction ( luaVM, "removeJetPack", "removePedJetPack" );
    lua_classfunction ( luaVM, "doesHaveJetpack", "doesPedHaveJetPack" );
    
    lua_classfunction ( luaVM, "isDead", "isPedDead" );
    lua_classfunction ( luaVM, "isDucked", "isPedDucked" );
    lua_classfunction ( luaVM, "isInWater", "isPedInWater" );
    lua_classfunction ( luaVM, "isOnGround", "isPedOnGround" );
    lua_classfunction ( luaVM, "isInVehicle", "isPedInVehicle" );
    
    lua_classfunction ( luaVM, "isOnFire", "isPedOnFire" );
    lua_classfunction ( luaVM, "isChoking", "isPedChoking" );
    lua_classfunction ( luaVM, "isDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "isFrozen", "isPedFrozen" );
    lua_classfunction ( luaVM, "isHeadless", "isPedHeadless" );
    
    lua_classfunction ( luaVM, "getArmor", "getPedArmor" );
    lua_classfunction ( luaVM, "getAnalogControlState", "getPedAnalogControlState" );
    lua_classfunction ( luaVM, "getFightingStyle", "getPedFightingStyle" );
    lua_classfunction ( luaVM, "getGravity", "getPedGravity" );
    lua_classfunction ( luaVM, "getStat", "getPedStat" );
    lua_classfunction ( luaVM, "getWeaponSlot", "getPedWeaponSlot" );
    //lua_classfunction ( luaVM, "getWalkingStyle", "getPedWalkingStyle" );
    
    lua_classfunction ( luaVM, "getAmmoInClip", "getPedAmmoInClip" );
    lua_classfunction ( luaVM, "getOccupiedVehicle", "getPedOccupiedVehicle" );
    lua_classfunction ( luaVM, "getWeapon", "getPedWeapon" );
    lua_classfunction ( luaVM, "getTarget", "getPedTarget" );
    lua_classfunction ( luaVM, "getOccupiedVehicleSeat", "getPedOccupiedVehicleSeat" );
    lua_classfunction ( luaVM, "getClothes", "getPedClothes" );
    lua_classfunction ( luaVM, "getContactElement", "getPedContactElement" );
    lua_classfunction ( luaVM, "getTotalAmmo", "getPedTotalAmmo" );
    
    lua_classfunction ( luaVM, "setOnFire", "setPedOnFire" );
    lua_classfunction ( luaVM, "setChoking", "setPedChoking" );
    lua_classfunction ( luaVM, "setDoingGangDriveBy", "setPedDoingGangDriveby" );
    lua_classfunction ( luaVM, "setFrozen", "setPedFrozen" );
    lua_classfunction ( luaVM, "setHeadless", "setPedHeadless" );
    lua_classfunction ( luaVM, "setArmor", "setPedArmor" );
    lua_classfunction ( luaVM, "setAnalogControlState", "setPedAnalogControlState" );
    lua_classfunction ( luaVM, "setFightingStyle", "setPedFightingStyle" );
    lua_classfunction ( luaVM, "setGravity", "setPedGravity" );
    lua_classfunction ( luaVM, "setStat", "setPedStat" );
    lua_classfunction ( luaVM, "setWeaponSlot", "setPedWeaponSlot" );
    //lua_classfunction ( luaVM, "setWalkingStyle", "setPedWalkingStyle" );

    lua_classfunction ( luaVM, "setAnimation", "setPedAnimation" );
    lua_classfunction ( luaVM, "setAnimationProgress", "setPedAnimationProgress" );
    
    lua_classvariable ( luaVM, "inVehicle", NULL, "isPedInVehicle" );
    lua_classvariable ( luaVM, "ducked", NULL, "isPedDucked" );
    lua_classvariable ( luaVM, "inWater", NULL, "isPedInWater" );
    lua_classvariable ( luaVM, "onGround", NULL, "isPedOnGround" );
    lua_classvariable ( luaVM, "onFire", "setPedOnFire", "isPedOnFire" );
    lua_classvariable ( luaVM, "choking", "setPedChoking", "isPedChoking" );
    lua_classvariable ( luaVM, "doingGangDriveby", "setPedDoingGangDriveby", "isPedDoingGangDriveby" );
    lua_classvariable ( luaVM, "frozen", "setPedFrozen", "isPedFrozen" );
    lua_classvariable ( luaVM, "headless", "setPedHeadless", "isPedHeadless" );
    lua_classvariable ( luaVM, "armor", "setPedArmor", "getPedArmor" );
    lua_classvariable ( luaVM, "fightingStyle", "setPedFightingStyle", "getPedFightingStyle" );
    lua_classvariable ( luaVM, "gravity", "setPedGravity", "getPedGravity" );
    lua_classvariable ( luaVM, "weaponSlot", "setPedWeaponSlot", "getPedWeaponSlot" );
    lua_classvariable ( luaVM, "ammoInClipOfCurrentWeaponSlot", NULL, "getPedAmmoInClip" );
    lua_classvariable ( luaVM, "weaponInCurrentWeaponSlot", NULL, "getPedWeapon" );
    lua_classvariable ( luaVM, "target", NULL, "getPedTarget" );
    lua_classvariable ( luaVM, "occupiedVehicleSeat", NULL, "getPedOccupiedVehicleSeat" );
    lua_classvariable ( luaVM, "contactElement", NULL, "getPedContactElement" );
    lua_classvariable ( luaVM, "totalAmmoInWeaponOfCurrentWeaponSlot", NULL, "getPedTotalAmmo" );
    lua_classvariable ( luaVM, "vehicle", "warpPedIntoVehicle", "getPedOccupiedVehicle", CLuaOOPDefs::SetPedOccupiedVehicle, CLuaFunctionDefinitions::GetPedOccupiedVehicle ); // what about removePedFromVehicle?
    lua_classvariable ( luaVM, "dead", "killPed", "isPedDead" ); // Discuss whether it is appropriate to require a nil value to actually make the ped dead, should a special be done here?
    //lua_classvariable ( luaVM, "walkingStyle", "setPedWalkingStyle", "getPedWalkingStyle", CLuaOOPDefs::SetPedWalkingStyle, CLuaOOPDefs::GetPedWalkingStyle );
    //lua_classvariable ( luaVM, "stats", "setPedStat", "getPedStat", CLuaOOPDefs::SetPedStat, CLuaOOPDefs::GetPedStat ); // table
    //lua_classvariable ( luaVM, "controlState", "setPedControlState", "getPedControlState", CLuaOOPDefs::SetPedControlState, CLuaOOPDefs::GetPedControlState ); // TODO: .controlState["control"] = value
    // This commented code will syntax error, a new method has to be implemented to support this
    //lua_classvariable ( luaVM, "jetpack", {"removePedJetPack","givePedJetPack}, "doesPedHaveJetPack", CLuaOOPDefs::SetJetpackActive, "doesPedHaveJetPack" ); // very specialised!

    lua_registerclass ( luaVM, "Ped", "Element" );
}


void CLuaMain::AddPickupClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createPickup" );
    lua_classfunction ( luaVM, "use", "usePickup" );
    
    lua_classfunction ( luaVM, "getAmmo", "getPickupAmmo" );
    lua_classfunction ( luaVM, "getAmount", "getPickupAmount" );
    lua_classfunction ( luaVM, "getWeapon", "getPickupWeapon" );
    lua_classfunction ( luaVM, "getRespawnInterval", "getPickupRespawnInterval" );
    lua_classfunction ( luaVM, "getType", "getPickupType" );
    lua_classfunction ( luaVM, "setType", "setPickupType" );
    lua_classfunction ( luaVM, "setRespawnInterval", "setPickupRespawnInterval" );
    
    lua_classvariable ( luaVM, "ammo", NULL, "getPickupAmmo" );
    lua_classvariable ( luaVM, "amount", NULL, "getPickupAmount" );
    lua_classvariable ( luaVM, "spawned", NULL, "isPickupSpawned" );
    lua_classvariable ( luaVM, "weapon", NULL, "getPickupWeapon" );
    lua_classvariable ( luaVM, "type", "setPickupType", "getPickupType" );
    lua_classvariable ( luaVM, "respawnInterval", "setPickupRespawnInterval", "getPickupRespawnInterval" );
    
    lua_registerclass ( luaVM, "Pickup", "Element" );
}


// TODO: color class, table specials
void CLuaMain::AddPlayerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "getAllAlive", "getAlivePlayers" );
    lua_classfunction ( luaVM, "getAllDead", "getDeadPlayers" );
    lua_classfunction ( luaVM, "getRandom", "getRandomPlayer" );
    lua_classfunction ( luaVM, "getCount", "getPlayerCount" );
    lua_classfunction ( luaVM, "isVoiceEnabled", "isVoiceEnabled" );
    
    lua_classfunction ( luaVM, "create", "getPlayerFromName" );
    lua_classfunction ( luaVM, "ban", "banPlayer" );
    lua_classfunction ( luaVM, "kick", "kickPlayer" );
    lua_classfunction ( luaVM, "redirect", "redirectPlayer" );
    lua_classfunction ( luaVM, "resendModInfo", "resendPlayerModInfo" );
    lua_classfunction ( luaVM, "spawn", "spawnPlayer" );
    lua_classfunction ( luaVM, "takeMoney", "takePlayerMoney" );
    lua_classfunction ( luaVM, "takeScreenshot", "takePlayerScreenShot" );
    lua_classfunction ( luaVM, "giveMoney", "givePlayerMoney" );
    lua_classfunction ( luaVM, "showHudComponent", "showPlayerHudComponent" );
    lua_classfunction ( luaVM, "logOut", "logOut" );
    //lua_classfunction ( luaVM, "observeDisplay", "textDisplayAddObserver" ); // swap args
    //lua_classfunction ( luaVM, "stopObservingDisplay", "textDisplayRemoveObserver" ); // swap args
    //lua_classfunction ( luaVM, "isObservingDisplay", "textDisplayIsObserver" ); // swap args
    
    lua_classfunction ( luaVM, "forceMap", "forcePlayerMap" );
    lua_classfunction ( luaVM, "setTeam", "setPlayerTeam" );
    lua_classfunction ( luaVM, "setMuted", "setPlayerMuted" );
    lua_classfunction ( luaVM, "setName", "setPlayerName" );
    lua_classfunction ( luaVM, "setBlurLevel", "setPlayerBlurLevel" );
    lua_classfunction ( luaVM, "setWantedLevel", "setPlayerWantedLevel" );
    lua_classfunction ( luaVM, "setMoney", "setPlayerMoney" );
    lua_classfunction ( luaVM, "setNametagText", "setPlayerNametagText" );
    lua_classfunction ( luaVM, "setNametagShowing", "setPlayerNametagShowing" );
    lua_classfunction ( luaVM, "setNametagColor", "setPlayerNametagColor" ); // color
    lua_classfunction ( luaVM, "setAnnounceValue", "setPlayerAnnounceValue" );
    lua_classfunction ( luaVM, "setVoiceBroadcastTo", "setPlayerVoiceBroadcastTo" );
    lua_classfunction ( luaVM, "setVoiceIgnoreFrom", "setPlayerVoiceIgnoreFrom" );
    lua_classfunction ( luaVM, "setHudComponentVisible", "setPlayerHudComponentVisible" );
    
    lua_classfunction ( luaVM, "isMapForced", "isPlayerMapForced" );
    lua_classfunction ( luaVM, "isMuted", "isPlayerMuted" );
    lua_classfunction ( luaVM, "isNametagShowing", "isPlayerNametagShowing" );
    lua_classfunction ( luaVM, "getNametagText", "getPlayerNametagText" );
    lua_classfunction ( luaVM, "getNametagColor", "getPlayerNametagColor" ); // color
    lua_classfunction ( luaVM, "getName", "getPlayerName" );
    lua_classfunction ( luaVM, "getIdleTime", "getPlayerIdleTime" );
    lua_classfunction ( luaVM, "getPing", "getPlayerPing" );
    lua_classfunction ( luaVM, "getAccount", "getPlayerAccount" );
    lua_classfunction ( luaVM, "getWantedLevel", "getPlayerWantedLevel" );
    lua_classfunction ( luaVM, "getSerial", "getPlayerSerial" );
    lua_classfunction ( luaVM, "getIP", "getPlayerIP" );
    lua_classfunction ( luaVM, "getTeam", "getPlayerTeam" );
    lua_classfunction ( luaVM, "getBlurLevel", "getPlayerBlurLevel" );
    lua_classfunction ( luaVM, "getVersion", "getPlayerVersion" );
    lua_classfunction ( luaVM, "getMoney", "getPlayerMoney" );
    lua_classfunction ( luaVM, "getAnnounceValue", "getPlayerAnnounceValue" );
    lua_classfunction ( luaVM, "getACInfo", "getPlayerACInfo" );

    lua_classvariable ( luaVM, "ACInfo", NULL, "getPlayerACInfo" );
    lua_classvariable ( luaVM, "voiceBroadcastTo", "setPlayerVoiceBroadcastTo", NULL );
    lua_classvariable ( luaVM, "voiceIgnoreFrom", "setPlayerVoiceIgnoreFrom", NULL );
    lua_classvariable ( luaVM, "money", "setPlayerMoney", "getPlayerMoney" );
    lua_classvariable ( luaVM, "version", NULL, "getPlayerVersion" );
    lua_classvariable ( luaVM, "wantedLevel", "setPlayerWantedLevel", "getPlayerWantedLevel" );
    lua_classvariable ( luaVM, "blurLevel", "setPlayerBlurLevel", "getPlayerBlurLevel" );
    lua_classvariable ( luaVM, "name", "setPlayerName", "getPlayerName" );
    lua_classvariable ( luaVM, "muted", "setPlayerMuted", "isPlayerMuted" );
    lua_classvariable ( luaVM, "idleTime", NULL, "getPlayerIdleTime" );
    lua_classvariable ( luaVM, "ping", NULL, "getPlayerPing" );
    lua_classvariable ( luaVM, "serial", NULL, "getPlayerSerial" );
    lua_classvariable ( luaVM, "ip", NULL, "getPlayerIP" );
    lua_classvariable ( luaVM, "team", "setPlayerTeam", "getPlayerTeam" );
    lua_classvariable ( luaVM, "mapForced", "forcePlayerMap", "isPlayerMapForced" );
    lua_classvariable ( luaVM, "nametagText", "setPlayerNametagText", "getPlayerNametagText" );
    lua_classvariable ( luaVM, "nametagShowing", "setPlayerNametagShowing", "isPlayerNametagShowing" );
    //lua_classvariable ( luaVM, "nametagColor", "setPlayerNametagColor", "getPlayerNametagColor", CLuaFunctionDefs::SetPlayerNametagColor, CLuaOOPDefs::GetPlayerNametagColor ); // color class
    //lua_classvariable ( luaVM, "announceValue", "setPlayerAnnounceValue", "getPlayerAnnounceValue", CLuaFunctionDefs::SetPlayerAnnounceValue, CLuaOOPDefs::GetPlayerAnnounceValue ); // .announceValue[key]=value
    //lua_classvariable ( luaVM, "hudComponent", "setHudComponentVisible", "", CLuaOOPDefs::SetHudComponentVisible, NULL ); .hudComponent["radar"]=true (a get function to needs to be implemented too)
    lua_classvariable ( luaVM, "random", NULL, "getRandomPlayer" );
    lua_classvariable ( luaVM, "count", NULL, "getPlayerCount" );
    lua_classvariable ( luaVM, "voiceEnabled", NULL, "isVoiceEnabled" );
    
    lua_registerclass ( luaVM, "Player", "Ped" );
}


void CLuaMain::AddRadarAreaClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createRadarArea" );
    lua_classfunction ( luaVM, "isInside", "isInsideRadarArea" ); // vector2
    
    lua_classfunction ( luaVM, "isFlashing", "isRadarAreaFlashing" );
    lua_classfunction ( luaVM, "getSize", "getRadarAreaSize" );
    lua_classfunction ( luaVM, "getColor", "getRadarAreaColor" );
    
    lua_classfunction ( luaVM, "setSize", "setRadarAreaSize" );
    lua_classfunction ( luaVM, "setFlashing", "setRadarAreaFlashing" );
    lua_classfunction ( luaVM, "setColor", "setRadarAreaColor" );
    
    lua_classvariable ( luaVM, "flashing", "isRadarAreaFlashing", "setRadarAreaFlashing" );
    //lua_classvariable ( luaVM, "color", "getRadarAreaColor", "setRadarAreaColor", "" ); color
    //lua_classvariable ( luaVM, "size", "getRadarAreaSize", "setRadarAreaSize", "" ); vector2

    lua_registerclass ( luaVM, "RadarArea", "Element" );
}


void CLuaMain::AddResourceClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_registerclass ( luaVM, "Resource" );
}


void CLuaMain::AddConnectionClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "dbConnect" );
    lua_classfunction ( luaVM, "exec", "dbExec" );
    lua_classfunction ( luaVM, "query", "dbQuery" ); // TODO: Accommodate for callbacks and arguments

    lua_registerclass ( luaVM, "Connection", "Element" );
}


// TODO: We need code to integrate this class into the handles returned by the db functions
void CLuaMain::AddQueryHandleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "poll", "dbPoll" );
    lua_classfunction ( luaVM, "free", "dbFree" );

    lua_registerclass ( luaVM, "QueryHandle" );
}


void CLuaMain::AddTeamClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createTeam" );
    lua_classfunction ( luaVM, "getFromName", "getTeamFromName" );
    lua_classfunction ( luaVM, "countPlayers", "countPlayersInTeam" );
    lua_classfunction ( luaVM, "getPlayers", "getPlayersInTeam" );
    
    lua_classfunction ( luaVM, "getFriendlyFire", "getTeamFriendlyFire" );
    lua_classfunction ( luaVM, "getName", "getTeamName" );
    lua_classfunction ( luaVM, "getColor", "getTeamColor" ); // color
    
    lua_classfunction ( luaVM, "setName", "setTeamName" );
    lua_classfunction ( luaVM, "setColor", "setTeamColor" ); // color
    lua_classfunction ( luaVM, "setFriendlyFire", "setTeamFriendlyFire" );
    
    lua_classvariable ( luaVM, "playerCount", NULL, "countPlayersInTeam" );
    lua_classvariable ( luaVM, "friendlyFire", "setTeamFriendlyFire", "getTeamFriendlyFire" );
    lua_classvariable ( luaVM, "players", NULL, "getPlayersInTeam" ); // todo: perhaps table.insert/nilvaluing?
    lua_classvariable ( luaVM, "name", "setTeamName", "getTeamName" );
    //lua_classvariable ( luaVM, "color", "setTeamColor", "getTeamColor", , ); //color

    lua_registerclass ( luaVM, "Team", "Element" );
}


void CLuaMain::AddTextDisplayClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "textCreateDisplay" );
    lua_classfunction ( luaVM, "destroy", "textDestroyDisplay" );
    lua_classfunction ( luaVM, "addObserver", "textDisplayAddObserver" );
    lua_classfunction ( luaVM, "removeObserver", "textDisplayRemoveObserver" );
    lua_classfunction ( luaVM, "addText", "textDisplayAddText" );
    lua_classfunction ( luaVM, "removeText", "textDisplayRemoveText" );
    
    lua_classfunction ( luaVM, "isObserver", "textDisplayIsObserver" );
    lua_classfunction ( luaVM, "getObservers", "textDisplayGetObservers" );

    lua_classvariable ( luaVM, "observers", NULL, "textDisplayGetObservers" );

    lua_registerclass ( luaVM, "TextDisplay" );
}


// TODO: color? vector?
void CLuaMain::AddTextItemClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "textCreateTextItem" );
    lua_classfunction ( luaVM, "destroy", "textDestroyTextItem" );
    
    lua_classfunction ( luaVM, "getColor", "textItemGetColor" );
    lua_classfunction ( luaVM, "getPosition", "textItemGetPosition" );
    lua_classfunction ( luaVM, "getPriority", "textItemGetPriority" );
    lua_classfunction ( luaVM, "getScale", "textItemGetScale" );
    lua_classfunction ( luaVM, "getText", "textItemGetText" );
    
    lua_classfunction ( luaVM, "setColor", "textItemSetColor" );
    lua_classfunction ( luaVM, "setPosition", "textItemSetPosition" );
    lua_classfunction ( luaVM, "setPriority", "textItemSetPriority" );
    lua_classfunction ( luaVM, "setScale", "textItemSetScale" );
    lua_classfunction ( luaVM, "setText", "textItemSetText" );
    
    lua_classvariable ( luaVM, "priority", "textItemSetPriority", "textItemGetPriority" );
    lua_classvariable ( luaVM, "scale", "textItemSetScale", "textItemGetScale" );
    lua_classvariable ( luaVM, "text", "textItemSetText", "textItemGetText" );
    //lua_classvariable ( luaVM, "color", "textItemSetColor", "textItemGetColor", CLuaOOPDefs::TextItemSetColor, CLuaOOPDefs::TextItemGetColor ); // color
    //lua_classvariable ( luaVM, "position", "textItemSetPosition", "textItemGetPosition", CLuaOOPDefs::TextItemSetPosition, CLuaOOPDefs::TextItemGetPosition ); // vector 2

    lua_registerclass ( luaVM, "TextItem" );
}


void CLuaMain::AddVehicleClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "getVehicleModelFromName", "getModelFromName" );
    lua_classfunction ( luaVM, "getVehicleNameFromModel", "getNameFromModel" );
    lua_classfunction ( luaVM, "getVehiclesOfType", "getAllOfType" );
    lua_classfunction ( luaVM, "getModelHandling", "getModelHandling" );
    lua_classfunction ( luaVM, "setModelHandling", "setModelHandling" );
    lua_classfunction ( luaVM, "getOriginalHandling", "getOriginalHandling" );
    lua_classfunction ( luaVM, "getVehicleUpgradeSlotName", "getUpgradeSlotName" );
    
    lua_classfunction ( luaVM, "createVehicle", "create" );
    lua_classfunction ( luaVM, "blowVehicle", "blow" );
    lua_classfunction ( luaVM, "fixVehicle", "fix" );
    lua_classfunction ( luaVM, "detachTrailerFromVehicle", "detatchTrailer" );
    lua_classfunction ( luaVM, "addVehicleUpgrade", "addUpgrade" );
    lua_classfunction ( luaVM, "attachTrailerToVehicle", "attachTrailer" );
    lua_classfunction ( luaVM, "removeVehicleUpgrade", "removeUpgrade" );
    lua_classfunction ( luaVM, "resetVehicleIdleTime", "resetIdleTime" );
    lua_classfunction ( luaVM, "resetVehicleExplosionTime", "resetExplosionTime" );
    lua_classfunction ( luaVM, "respawnVehicle", "respawn" );
    lua_classfunction ( luaVM, "spawnVehicle", "spawn" );
    lua_classfunction ( luaVM, "toggleVehicleRespawn", "toggleRespawn" );
    lua_classfunction ( luaVM, "removeVehicleSirens", "removeSirens" );
    lua_classfunction ( luaVM, "addVehicleSirens", "addSirens" );
    
    lua_classfunction ( luaVM, "isVehicleDamageProof", "isDamageProof" );
    lua_classfunction ( luaVM, "isVehicleFuelTankExplodable", "isFuelTankExplodable" );
    lua_classfunction ( luaVM, "isVehicleLocked", "isLocked" );
    lua_classfunction ( luaVM, "isVehicleOnGround", "isOnGround" );
    lua_classfunction ( luaVM, "isTrainDerailable", "isDerailable" );
    lua_classfunction ( luaVM, "isTrainDerailed", "isDerailed" );
    lua_classfunction ( luaVM, "isVehicleBlown", "isBlown" );
    lua_classfunction ( luaVM, "isVehicleTaxiLightOn", "isTaxiLightOn" );
    lua_classfunction ( luaVM, "getVehicleSirenParams", "getSirenParams" );
    lua_classfunction ( luaVM, "getVehicleVariant", "getVarient" );
    lua_classfunction ( luaVM, "getVehicleSirens", "getSirens" );
    lua_classfunction ( luaVM, "getTrainDirection", "getDirection" );
    lua_classfunction ( luaVM, "getTrainSpeed", "getSpeed" );
    lua_classfunction ( luaVM, "getVehicleHeadLightColor", "getHeadLightColor" );
    lua_classfunction ( luaVM, "getVehicleColor", "getColor" );
    lua_classfunction ( luaVM, "getVehicleCompatibleUpgrades", "getCompatibleUpgrades" );
    lua_classfunction ( luaVM, "getVehicleController", "getController" );
    lua_classfunction ( luaVM, "getVehicleDoorState", "getDoorState" );
    lua_classfunction ( luaVM, "getVehicleEngineState", "getEngineState" );
    lua_classfunction ( luaVM, "getVehicleLandingGearDown", "getLandingGearDown" );
    lua_classfunction ( luaVM, "getVehicleLightState", "getLightState" );
    lua_classfunction ( luaVM, "getVehicleMaxPassengers", "getMaxPassenger" );
    lua_classfunction ( luaVM, "getVehicleName", "getName" );
    lua_classfunction ( luaVM, "getVehicleOccupant", "getOccupant" );
    lua_classfunction ( luaVM, "getVehicleOccupants", "getOccupants" );
    lua_classfunction ( luaVM, "getVehicleOverrideLights", "getOverrideLights" );
    lua_classfunction ( luaVM, "getPlateText", "getVehiclePlateText" );
    lua_classfunction ( luaVM, "getVehiclePaintjob", "getPainjob" );
    lua_classfunction ( luaVM, "getVehiclePanelState", "getPanelState" );
    lua_classfunction ( luaVM, "getVehicleSirensOn", "areSirensOn" );
    lua_classfunction ( luaVM, "getVehicleTowedByVehicle", "getTowedByVehicle" );
    lua_classfunction ( luaVM, "getVehicleTowingVehicle", "getTowingVehicle" );
    lua_classfunction ( luaVM, "getVehicleTurnVelocity", "getTurnVelocity" );
    lua_classfunction ( luaVM, "getVehicleTurretPosition", "getTurretPosition" );
    lua_classfunction ( luaVM, "getVehicleType", "getVehicleType" ); // favours element type
    lua_classfunction ( luaVM, "getVehicleUpgradeOnSlot", "getUpgradeOnSlot" );
    lua_classfunction ( luaVM, "getVehicleUpgrades", "getUpgrades" );
    lua_classfunction ( luaVM, "getVehicleWheelStates", "getWheelStates" );
    lua_classfunction ( luaVM, "getVehicleDoorOpenRatio", "getDoorOpenRatio" );
    lua_classfunction ( luaVM, "getVehicleHandling", "getHandling" );
    
    lua_classfunction ( luaVM, "setVehicleColor", "setColor" );
    lua_classfunction ( luaVM, "setVehicleDamageProof", "setDamageProof" );
    lua_classfunction ( luaVM, "setVehicleDoorState", "setDoorState" );
    lua_classfunction ( luaVM, "setVehicleDoorsUndamageable", "setDoorsUndamageable" );
    lua_classfunction ( luaVM, "setVehicleEngineState", "setEngineState" );
    lua_classfunction ( luaVM, "setVehicleFuelTankExplodable", "setFuelTankExplodable" );
    lua_classfunction ( luaVM, "setVehicleIdleRespawnDelay", "setIdleRespawnDelay" );
    lua_classfunction ( luaVM, "setVehicleLandingGearDown", "setLandingGearDown" );
    lua_classfunction ( luaVM, "setVehicleLightState", "setLightState" );
    lua_classfunction ( luaVM, "setVehicleLocked", "setLocked" );
    lua_classfunction ( luaVM, "setVehicleOverrideLights", "setOverrideLights" );
    lua_classfunction ( luaVM, "setVehiclePaintjob", "setPaintJob" );
    lua_classfunction ( luaVM, "setVehiclePanelState", "setPanelState" );
    lua_classfunction ( luaVM, "setVehicleRespawnDelay", "setRespawnDelay" );
    lua_classfunction ( luaVM, "setVehicleRespawnPosition", "setRespawnPosition" );
    lua_classfunction ( luaVM, "setVehicleSirensOn", "setSirensOn" );
    lua_classfunction ( luaVM, "setVehicleTurretPosition", "setTurretPosition" );
    lua_classfunction ( luaVM, "setVehicleDoorOpenRatio", "setDoorOpenRatio" );
    lua_classfunction ( luaVM, "setVehicleHandling", "setHandling" );
    lua_classfunction ( luaVM, "setVehicleTurnVelocity", "setTurnVelocity" );
    lua_classfunction ( luaVM, "setVehicleWheelStates", "setWheelStates" );
    lua_classfunction ( luaVM, "setVehicleHeadLightColor", "setHeadLightColor" );
    lua_classfunction ( luaVM, "setVehicleTaxiLightOn", "setTaxiLightOn" );
    lua_classfunction ( luaVM, "setVehicleVariant", "setVariant" );
    lua_classfunction ( luaVM, "setVehicleSirens", "setSirens" );
    lua_classfunction ( luaVM, "setVehiclePlateText", "setPlateText" );
    lua_classfunction ( luaVM, "setTrainDerailable", "setDerailable" );
    lua_classfunction ( luaVM, "setTrainDerailed", "setDerailed" );
    lua_classfunction ( luaVM, "setTrainDirection", "setDirection" );
    lua_classfunction ( luaVM, "setTrainSpeed", "setSpeed" );

    lua_classvariable ( luaVM, "damageProof", "setVehicleDamageProof", "isVehicleDamageProof" );
    lua_classvariable ( luaVM, "locked", "setVehicleLocked", "isVehicleLocked" );
    lua_classvariable ( luaVM, "derailable", "setTrainDerailable", "isTrainDerailable" );
    lua_classvariable ( luaVM, "derailed", "setTrainDerailed", "isTrainDerailed" );
    lua_classvariable ( luaVM, "blown", "blowVehicle", "isVehicleBlown" );
    lua_classvariable ( luaVM, "direction", "setTrainDirection", "getTrainDirection" );
    lua_classvariable ( luaVM, "speed", "setTrainSpeed", "getTrainSpeed" );
    lua_classvariable ( luaVM, "taxiLightOn", "setVehicleTaxiLightOn", "isVehicleTaxiLightOn" );
    lua_classvariable ( luaVM, "fuelTankExplodable", "setVehicleFuelTankExplodable", "isVehicleFuelTankExplodable" );
    lua_classvariable ( luaVM, "plateText", "setVehiclePlateText", "getVehiclePlateText" );
    lua_classvariable ( luaVM, "sirensOn", "setVehicleSirensOn", "getVehicleSirensOn" );
    lua_classvariable ( luaVM, "sirenParams", NULL, "getVehicleSirenParams" );
    lua_classvariable ( luaVM, "controller", NULL, "getVehicleController" );
    lua_classvariable ( luaVM, "engineState", "setVehicleEngineState", "getVehicleEngineState" );
    lua_classvariable ( luaVM, "paintjob", "setVehiclePaintjob", "getVehiclePaintjob" );
    lua_classvariable ( luaVM, "occupants", NULL, "getVehicleOccupants" );
    lua_classvariable ( luaVM, "compatibleUpgrades", NULL, "getVehicleCompatibleUpgrades" );
    lua_classvariable ( luaVM, "doorsUndamageable", "setVehicleDoorsUndamageable", NULL );
    lua_classvariable ( luaVM, "towingVehicle", NULL, "getVehicleTowingVehicle" );
    lua_classvariable ( luaVM, "towedByVehicle", NULL, "getVehicleTowedByVehicle" );
    lua_classvariable ( luaVM, "landingGearDown", "setVehicleLandingGearDown", "getVehicleLandingGearDown" );
    lua_classvariable ( luaVM, "maxPassengers", NULL, "getVehicleMaxPassengers" );
    lua_classvariable ( luaVM, "upgrades", NULL, "getVehicleUpgrades" );
    lua_classvariable ( luaVM, "turretPosition", "setVehicleTurretPosition", "getVehicleTurretPosition" );
    lua_classvariable ( luaVM, "turnVelocity", "setVehicleTurnVelocity", "getVehicleTurnVelocity" );
    lua_classvariable ( luaVM, "overrideLights", "setVehicleOverrideLights", "getVehicleOverrideLights" );
    lua_classvariable ( luaVM, "idleRespawnDelay", "setVehicleIdleRespawnDelay", NULL );
    lua_classvariable ( luaVM, "respawnDelay", "setVehicleRespawnDelay", NULL );
    lua_classvariable ( luaVM, "respawnPosition", "setVehicleRespawnPosition", NULL );
    lua_classvariable ( luaVM, "onGround", NULL, "isVehicleOnGround" );
    lua_classvariable ( luaVM, "name", NULL, "getVehicleName" );
    lua_classvariable ( luaVM, "vehicleType", NULL, "getVehicleType" );
    //lua_classvariable ( luaVM, "upgradeOnSlot", NULL, "getVehicleUpgradeOnSlot", NULL, CLuaOOPDefs::GetVehicleUpgradeOnSlot ); // .upgradeOnSlot[slot]=int
    //lua_classvariable ( luaVM, "doorOpenRatio", "setVehicleDoorOpenRatio", "getVehicleDoorOpenRatio", CLuaOOPDefs::SetVehicleDoorOpenRation, CLuaOOPDefs::GetVehicleDoorOpenRatio ); // .doorOpenRatio[door]=ratio
    //lua_classvariable ( luaVM, "doorState", "setVehicleDoorState", "getVehicleDoorState", CLuaOOPDefs::SetVehicleDoorState, CLuaOOPDefs::GetVehicleDoorState ); // .doorState[id]=state
    //lua_classvariable ( luaVM, "lightState", "setVehicleLightState", "getVehicleLightState", CLuaOOPDefs::SetVehicleLightState, CLuaOOPDefs::GetVehicleLightState ); // .lightState[id]=state
    //lua_classvariable ( luaVM, "panelState", "setVehiclePanelState", "getVehiclePanelState", CLuaOOPDefs::SetVehiclePanelState, CLuaOOPDefs::GetVehiclePanelState ); // .panelState[panel]=state
    //lua_classvariable ( luaVM, "headLightColor", "setVehicleHeadLightColor", "getVehicleHeadLightColor" ); // color
    //lua_classvariable ( luaVM, "color", "setVehicleColor", "getVehicleColor" ); // color
    lua_classvariable ( luaVM, "sirens", NULL, "geVehicleSirens" ); // TODO: support .sirens[point] = {...}
    lua_classvariable ( luaVM, "handling", NULL, "setVehicleHandling" ); // .handling[property]=value
    lua_classvariable ( luaVM, "occupant", NULL, "getVehicleOccupant" ); // Currrently returns driver, support .occupant[seat]=ped
    
    lua_registerclass ( luaVM, "Vehicle", "Element" );
}


void CLuaMain::AddWaterClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "createWater" );

    lua_classfunction ( luaVM, "getVertexPosition", "getWaterVertexPosition" ); // vector3
    lua_classfunction ( luaVM, "getWaveHeight", "getWaveHeight" );
    lua_classfunction ( luaVM, "getColor", "getWaterColor" ); // color
    
    lua_classfunction ( luaVM, "setWaveHeight", "setWaveHeight" );
    lua_classfunction ( luaVM, "setColor", "setWaterColor" ); // color
    lua_classfunction ( luaVM, "setVertexPosition", "setWaterVertexPosition" ); // vector3
    lua_classfunction ( luaVM, "setLevel", "setWaterLevel" ); // vector3d
    
    lua_classfunction ( luaVM, "resetColor", "resetWaterColor" );
    lua_classfunction ( luaVM, "resetLevel", "resetWaterLevel" );
    
    lua_classvariable ( luaVM, "level", "setWaterLevel", NULL );
    lua_classvariable ( luaVM, "height", "setWaveHeight", "getWaveHeight" );
    //lua_classvariable ( luaVM, "color", "setWaterColor", "getWaterColor", "" ); // color

    lua_registerclass ( luaVM, "Water", "Element" );
}


// TODO: Potential special
void CLuaMain::AddXMLClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "load", "xmlLoadFile" );
    lua_classfunction ( luaVM, "destroy", "xmlUnloadFile" );
    lua_classfunction ( luaVM, "copy", "xmlCopyFile" );
    lua_classfunction ( luaVM, "create", "xmlCreateFile" );
    lua_classfunction ( luaVM, "destroy", "xmlDestroyNode" );
    lua_classfunction ( luaVM, "loadMapData", "loadMapData" );
    lua_classfunction ( luaVM, "saveMapData", "saveMapData" );
    
    lua_classfunction ( luaVM, "setValue", "xmlNodeGetValue" );
    lua_classfunction ( luaVM, "setAttribute", "xmlNodeSetAttribute" );
    lua_classfunction ( luaVM, "setValue", "xmlNodeSetValue" );
    lua_classfunction ( luaVM, "saveFile", "xmlSaveFile" );
    lua_classfunction ( luaVM, "createChild", "xmlCreateChild" );
    lua_classfunction ( luaVM, "findChild", "xmlFindChild" );
    lua_classfunction ( luaVM, "setName", "xmlNodeSetName" );
    
    lua_classfunction ( luaVM, "getAttributes", "xmlNodeGetAttributes" );
    lua_classfunction ( luaVM, "getChildren", "xmlNodeGetChildren" );
    lua_classfunction ( luaVM, "getName", "xmlNodeGetName" );
    lua_classfunction ( luaVM, "getParent", "xmlNodeGetParent" );
    lua_classfunction ( luaVM, "getAttribute", "xmlNodeGetAttribute" ); // table classvar?
    
    lua_classvariable ( luaVM, "value", "xmlNodeSetValue", "xmlNodeGetValue" );
    lua_classvariable ( luaVM, "name", "xmlNodeSetName", "xmlNodeGetName" );
    lua_classvariable ( luaVM, "attributes", NULL, "xmlNodeGetAttributes" );
    lua_classvariable ( luaVM, "children", NULL, "xmlNodeGetChildren" );
    lua_classvariable ( luaVM, "parent", NULL, "xmlNodeGetParent" );

    lua_registerclass ( luaVM, "XML" );
}


void CLuaMain::AddTimerClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );
    
    lua_classfunction ( luaVM, "create", "setTimer" );
    lua_classfunction ( luaVM, "destroy", "killTimer" );
    lua_classfunction ( luaVM, "reset", "resetTimer" );
    lua_classfunction ( luaVM, "isValid", "isTimer" );
    
    lua_classfunction ( luaVM, "getDetails", "getTimerDetails" );
    
    lua_classvariable ( luaVM, "valid", NULL, "isTimer" );
    
    lua_registerclass ( luaVM, "Timer" );
}


void CLuaMain::InitClasses ( lua_State* luaVM )
{
    lua_initclasses             ( luaVM );
    lua_newclass ( luaVM );
    
    // Vector and Matrix classes
    AddVector4DClass            ( luaVM );
    AddVector3DClass            ( luaVM );
    AddVector2DClass            ( luaVM );
    AddMatrixClass              ( luaVM );

    // OOP based classes
    if ( !m_bEnableOOP )
        return;

    AddElementClass             ( luaVM );
    AddACLClass                 ( luaVM );
    AddACLGroupClass            ( luaVM );
    AddAccountClass             ( luaVM );
    AddBanClass                 ( luaVM );
    AddBlipClass                ( luaVM );
    AddColShapeClass            ( luaVM );
    AddFileClass                ( luaVM );
    AddMarkerClass              ( luaVM );
    AddObjectClass              ( luaVM );
    AddPedClass                 ( luaVM );
    AddPickupClass              ( luaVM );
    AddPlayerClass              ( luaVM );
    AddRadarAreaClass           ( luaVM );
    AddResourceClass            ( luaVM );
    AddConnectionClass          ( luaVM );
    AddQueryHandleClass         ( luaVM );
    AddTeamClass                ( luaVM );
    AddTextDisplayClass         ( luaVM );
    AddTextItemClass            ( luaVM );
    AddVehicleClass             ( luaVM );
    AddWaterClass               ( luaVM );
    AddXMLClass                 ( luaVM );
    AddTimerClass               ( luaVM );
}


void CLuaMain::InitVM ( void )
{
    assert( !m_luaVM );

    // Create a new VM
    m_luaVM = lua_open ();
    m_pLuaManager->OnLuaMainOpenVM( this, m_luaVM );

    // Set the instruction count hook
    lua_sethook ( m_luaVM, InstructionCountHook, LUA_MASKCOUNT, HOOK_INSTRUCTION_COUNT );

    // Load LUA libraries
    luaopen_base ( m_luaVM );
    luaopen_math ( m_luaVM );
    luaopen_string ( m_luaVM );
    luaopen_table ( m_luaVM );
    luaopen_debug ( m_luaVM );

    // Initialize security restrictions. Very important to prevent lua trojans and viruses!
    InitSecurity ();

    // Registering C functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );

    // Create class metatables
    InitClasses ( m_luaVM );

    // Oli: Don't forget to add new ones to CLuaManager::LoadCFunctions. Thanks!

    // create global vars
    lua_pushelement ( m_luaVM, g_pGame->GetMapManager()->GetRootElement() );
    lua_setglobal ( m_luaVM, "root" );

    lua_pushresource ( m_luaVM, m_pResource );
    lua_setglobal ( m_luaVM, "resource" );

    lua_pushelement ( m_luaVM, m_pResource->GetResourceRootElement () );
    lua_setglobal ( m_luaVM, "resourceRoot" );

    // Load pre-loaded lua code
    LoadScript ( szPreloadedScript );
}


// Special function(s) that are only visible to HTMLD scripts
void CLuaMain::RegisterHTMLDFunctions ( void )
{
    CLuaHTTPDefs::LoadFunctions ( m_luaVM );
}


void CLuaMain::InstructionCountHook ( lua_State* luaVM, lua_Debug* pDebug )
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Above max time?
        if ( pLuaMain->m_FunctionEnterTimer.Get () > HOOK_MAXIMUM_TIME )
        {
            // Print it in the console
            CLogger::ErrorPrintf ( "Infinite/too long execution (%s)\n", pLuaMain->GetScriptName () );
            
            SString strAbortInf = "Aborting; infinite running script in ";
            strAbortInf += pLuaMain->GetScriptName ();
            
            // Error out
            lua_pushstring ( luaVM, strAbortInf );
            lua_error ( luaVM );
        }
    }
}


bool CLuaMain::LoadScriptFromBuffer ( const char* cpInBuffer, unsigned int uiInSize, const char* szFileName )
{
    SString strNiceFilename = ConformResourcePath( szFileName );

    // Decrypt if required
    const char* cpBuffer;
    uint uiSize;
    if ( !g_pRealNetServer->DecryptScript( cpInBuffer, uiInSize, &cpBuffer, &uiSize, strNiceFilename ) )
    {
        // Problems problems
        if ( GetTimeString( true ) <= INVALID_COMPILED_SCRIPT_CUTOFF_DATE )
        {
            SString strMessage( "%s is invalid and will not work after %s. Please re-compile at http://luac.mtasa.com/", *strNiceFilename, INVALID_COMPILED_SCRIPT_CUTOFF_DATE ); 
            g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script warning: %s", *strMessage );
            // cpBuffer is always valid after call to DecryptScript
        }
        else
        {
            SString strMessage( "%s is invalid. Please re-compile at http://luac.mtasa.com/", *strNiceFilename ); 
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", *strMessage );
            return false;
        }
    }

    bool bUTF8;

    // UTF-8 BOM?  Compare by checking the standard UTF-8 BOM
    if ( IsUTF8BOM( cpBuffer, uiSize ) == false )
    {
        // Maybe not UTF-8, if we have a >80% heuristic detection confidence, assume it is
        bUTF8 = ( GetUTF8Confidence ( (const unsigned char*)cpBuffer, uiSize ) >= 80 );
    }
    else
    {
        // If there's a BOM, load ignoring the first 3 bytes
        bUTF8 = true;
        cpBuffer += 3;
        uiSize -= 3;
    }

    // If compiled script, make sure correct chunkname is embedded
    EmbedChunkName( strNiceFilename, &cpBuffer, &uiSize );

    if ( m_luaVM )
    {
        // Are we not marked as UTF-8 already, and not precompiled?
        std::string strUTFScript;
        if ( !bUTF8 && !IsLuaCompiledScript( cpBuffer, uiSize ) )
        {
            std::string strBuffer = std::string(cpBuffer, uiSize);
#ifdef WIN32
            std::setlocale(LC_CTYPE,""); // Temporarilly use locales to read the script
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
            std::setlocale(LC_CTYPE,"C");
#else
            strUTFScript = UTF16ToMbUTF8(ANSIToUTF16( strBuffer ));
#endif

            if ( uiSize != strUTFScript.size() )
            {
                uiSize = strUTFScript.size();
                g_pGame->GetScriptDebugging()->LogWarning ( m_luaVM, "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", strNiceFilename.c_str() );
            }
        }
        else
            strUTFScript = std::string(cpBuffer, uiSize);

        // Run the script
        if ( luaL_loadbuffer ( m_luaVM, bUTF8 ? cpBuffer : strUTFScript.c_str(), uiSize, SString ( "@%s", *strNiceFilename ) ) )
        {
            // Print the error
            std::string strRes = lua_tostring( m_luaVM, -1 );
            if ( strRes.length () )
            {
                CLogger::LogPrintf ( "SCRIPT ERROR: %s\n", strRes.c_str () );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed: %s", strRes.c_str () );
            }
            else
            {
                CLogger::LogPrint ( "SCRIPT ERROR: Unknown\n" );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading script failed for unknown reason" );
            }
        }
        else
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                SString strRes = lua_tostring( m_luaVM, -1 );
        
                std::vector <SString> vecSplit;
                strRes.Split ( ":", vecSplit );
                
                if ( vecSplit.size ( ) >= 3 )
                {
                    SString strFile = vecSplit[0];
                    int     iLine   = atoi ( vecSplit[1].c_str ( ) );
                    SString strMsg  = vecSplit[2].substr ( 1 );
                    
                    g_pGame->GetScriptDebugging()->LogError ( strFile, iLine, strMsg );
                }
                else
                {
                    if ( !strRes.ContainsI ( ExtractFilename ( strNiceFilename ) ) )
                    {
                        // Add filename to error message, if not already present
                        strRes = SString ( "%s (global scope) - %s", *strNiceFilename, *strRes );
                    }
                    g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "%s", strRes.c_str () );
                }
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
            return true;
        }
    }

    return false;
}


bool CLuaMain::LoadScript ( const char* szLUAScript )
{
    if ( m_luaVM )
    {
        // Run the script
        if ( !luaL_loadbuffer ( m_luaVM, szLUAScript, strlen(szLUAScript), NULL ) )
        {
            ResetInstructionCount ();
            int luaSavedTop = lua_gettop ( m_luaVM );
            int iret = this->PCall ( m_luaVM, 0, LUA_MULTRET, 0 ) ;
            if ( iret == LUA_ERRRUN || iret == LUA_ERRMEM )
            {
                std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
                g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Executing in-line script failed: %s", strRes.c_str () );
            }
            // Cleanup any return values
            if ( lua_gettop ( m_luaVM ) > luaSavedTop )
                lua_settop( m_luaVM, luaSavedTop );
        }
        else
        {
            std::string strRes = ConformResourcePath ( lua_tostring( m_luaVM, -1 ) );
            g_pGame->GetScriptDebugging()->LogError ( m_luaVM, "Loading in-line script failed: %s", strRes.c_str () );
        }
    }
    else
        return false;

    return true;
}


// TODO: Check the purpose of this function
void CLuaMain::Start ( void )
{

}


void CLuaMain::UnloadScript ( void )
{
    // Delete all timers and events
    m_pLuaTimerManager->RemoveAllTimers ();

    // Delete all keybinds
    list < CPlayer* > ::const_iterator iter = m_pPlayerManager->IterBegin ();
    for ( ; iter != m_pPlayerManager->IterEnd (); ++iter )
    {
        if ( (*iter)->IsJoined () )
            (*iter)->GetKeyBinds ()->RemoveAllKeys ( this );
    }

    // End the lua vm
    if ( m_luaVM )
    {
        m_pLuaManager->OnLuaMainCloseVM( this, m_luaVM );
        CLuaFunctionRef::RemoveLuaFunctionRefsForVM( m_luaVM );
        lua_close( m_luaVM );
        m_luaVM = NULL;
    }
}


void CLuaMain::DoPulse ( void )
{
    m_pLuaTimerManager->DoPulse ( this );
}


// Keep count of the number of open files in this resource and issue a warning if too high
void CLuaMain::OnOpenFile( const SString& strFilename )
{
    m_OpenFilenameList.push_back( strFilename );
    if ( m_OpenFilenameList.size() >= m_uiOpenFileCountWarnThresh )
    {
        m_uiOpenFileCountWarnThresh = m_OpenFilenameList.size() * 2;
        CLogger::LogPrintf ( "Notice: There are now %d open files in resource '%s'\n", m_OpenFilenameList.size(), GetScriptName() );
    }
}


void CLuaMain::OnCloseFile( const SString& strFilename )
{
    ListRemoveFirst( m_OpenFilenameList, strFilename );
}


CXMLFile * CLuaMain::CreateXML ( const char * szFilename )
{
    CXMLFile * pFile = g_pServerInterface->GetXML ()->CreateXML ( szFilename, true );
    if ( pFile )
    {
        m_XMLFiles.push_back ( pFile );
        if ( m_XMLFiles.size() >= m_uiOpenXMLFileCountWarnThresh )
        {
            m_uiOpenXMLFileCountWarnThresh = m_XMLFiles.size() * 2;
            CLogger::LogPrintf ( "Notice: There are now %d open XML files in resource '%s'\n", m_XMLFiles.size(), GetScriptName() );
        }
    }
    return pFile;
}


void CLuaMain::DestroyXML ( CXMLFile * pFile )
{
    m_XMLFiles.remove ( pFile );
    delete pFile;
}


void CLuaMain::DestroyXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); ++iter )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                m_XMLFiles.erase ( iter );
                delete file;
                break;
            }
        }
    }
}


void CLuaMain::SaveXML ( CXMLNode * pRootNode )
{
    list<CXMLFile *>::iterator iter;
    for ( iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); ++iter )
    {
        CXMLFile * file = (*iter);
        if ( file )
        {
            if ( file->GetRootNode() == pRootNode )
            {
                file->Write();
                break;
            }
        }
    }
    if ( m_pResource )
    {
        list < CResourceFile* > ::iterator iter = m_pResource->IterBegin ();
        for ( ; iter != m_pResource->IterEnd () ; ++iter )
        {
            CResourceFile* pResourceFile = *iter;
            if ( pResourceFile->GetType () == CResourceFile::RESOURCE_FILE_TYPE_CONFIG )
            {
                CResourceConfigItem* pConfigItem = static_cast < CResourceConfigItem* > ( pResourceFile );
                if ( pConfigItem->GetRoot () == pRootNode )
                {
                    CXMLFile* pFile = pConfigItem->GetFile ();
                    if ( pFile )
                    {
                        pFile->Write ();
                    }
                    break;
                }
            }
        }
    }
}


CTextDisplay * CLuaMain::CreateDisplay ( )
{
    CTextDisplay * pDisplay = new CTextDisplay;
    m_Displays.push_back ( pDisplay );
    return pDisplay;
}


void CLuaMain::DestroyDisplay ( CTextDisplay * pDisplay )
{
    m_Displays.remove ( pDisplay );
    delete pDisplay;
}


CTextItem * CLuaMain::CreateTextItem ( const char* szText, float fX, float fY, eTextPriority priority, const SColor color, float fScale, unsigned char format, unsigned char ucShadowAlpha )
{
    CTextItem * pTextItem = new CTextItem( szText, CVector2D ( fX, fY ), priority, color, fScale, format, ucShadowAlpha );
    m_TextItems.push_back ( pTextItem );
    return pTextItem;
}


void CLuaMain::DestroyTextItem ( CTextItem * pTextItem )
{
    m_TextItems.remove ( pTextItem );
    delete pTextItem;
}


CTextDisplay* CLuaMain::GetTextDisplayFromScriptID ( uint uiScriptID )
{
    CTextDisplay* pTextDisplay = (CTextDisplay*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_DISPLAY );
    dassert ( !pTextDisplay || ListContains ( m_Displays, pTextDisplay ) );
    return pTextDisplay;
}


CTextItem* CLuaMain::GetTextItemFromScriptID ( uint uiScriptID )
{
    CTextItem* pTextItem = (CTextItem*) CIdArray::FindEntry ( uiScriptID, EIdClass::TEXT_ITEM );
    dassert ( !pTextItem || ListContains ( m_TextItems, pTextItem ) );
    return pTextItem;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::GetFunctionTag
//
// Turn iFunctionNumber into something human readable
//
///////////////////////////////////////////////////////////////
const SString& CLuaMain::GetFunctionTag ( int iLuaFunction )
{
    // Find existing
    SString* pTag = MapFind ( m_FunctionTagMap, iLuaFunction );
#ifndef CHECK_FUNCTION_TAG
    if ( !pTag )
#endif
    {
        // Create if required
        SString strText;

        lua_Debug debugInfo;
        lua_getref ( m_luaVM, iLuaFunction );
        if ( lua_getinfo( m_luaVM, ">nlS", &debugInfo ) )
        {
            // Make sure this function isn't defined in a string
            if ( debugInfo.source[0] == '@' )
            {
                //std::string strFilename2 = ConformResourcePath ( debugInfo.source );
                SString strFilename = debugInfo.source;

                int iPos = strFilename.find_last_of ( "/\\" );
                if ( iPos >= 0 )
                    strFilename = strFilename.substr ( iPos + 1 );

                strText = SString ( "@%s:%d", strFilename.c_str (), debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined, iLuaFunction );
            }
            else
            {
                strText = SString ( "@func_%d %s", iLuaFunction, debugInfo.short_src );
            }
        }
        else
        {
            strText = SString ( "@func_%d NULL", iLuaFunction );
        }

    #ifdef CHECK_FUNCTION_TAG
        if ( pTag )
        {
            // Check tag remains unchanged
            assert ( strText == *pTag );
            return *pTag;
        }
    #endif

        MapSet ( m_FunctionTagMap, iLuaFunction, strText );
        pTag = MapFind ( m_FunctionTagMap, iLuaFunction );
    }
    return *pTag;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::PCall
//
// lua_pcall call wrapper
//
///////////////////////////////////////////////////////////////
int CLuaMain::PCall ( lua_State *L, int nargs, int nresults, int errfunc )
{
    if ( m_uiPCallDepth++ == 0 )
        m_WarningTimer.Reset();   // Only restart timer if initial call

    g_pGame->GetScriptDebugging()->PushLuaMain ( this );
    int iret = lua_pcall ( L, nargs, nresults, errfunc );
    g_pGame->GetScriptDebugging()->PopLuaMain ( this );

    --m_uiPCallDepth;
    return iret;
}


///////////////////////////////////////////////////////////////
//
// CLuaMain::CheckExecutionTime
//
// Issue warning if execution time is too long
//
///////////////////////////////////////////////////////////////
void CLuaMain::CheckExecutionTime( void )
{
    // Do time check
    if ( m_WarningTimer.Get() < 5000 )
        return;
    m_WarningTimer.Reset();

    // No warning if no players
    if ( g_pGame->GetPlayerManager()->Count() == 0 )
        return;

    // Issue warning about script execution time
    CLogger::LogPrintf ( "WARNING: Long execution (%s)\n", GetScriptName () );
}

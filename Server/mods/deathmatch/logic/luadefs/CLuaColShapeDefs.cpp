/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaColShapeDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CLuaColShapeDefs::LoadFunctions()
{
    CLuaCFunctions::AddFunction("createColCircle", CreateColCircle);
    CLuaCFunctions::AddFunction("createColCuboid", CreateColCuboid);
    CLuaCFunctions::AddFunction("createColSphere", CreateColSphere);
    CLuaCFunctions::AddFunction("createColRectangle", CreateColRectangle);
    CLuaCFunctions::AddFunction("createColPolygon", CreateColPolygon);
    CLuaCFunctions::AddFunction("createColTube", CreateColTube);

    CLuaCFunctions::AddFunction("isInsideColShape", IsInsideColShape);
    CLuaCFunctions::AddFunction("getColShapeType", GetColShapeType);
    CLuaCFunctions::AddFunction("setColPolygonHeight", SetColPolygonHeight);
    CLuaCFunctions::AddFunction("getColPolygonHeight", GetColPolygonHeight);
}

void CLuaColShapeDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "Circle", "createColCircle");
    lua_classfunction(luaVM, "Cuboid", "createColCuboid");
    lua_classfunction(luaVM, "Rectangle", "createColRectangle");
    lua_classfunction(luaVM, "Sphere", "createColSphere");
    lua_classfunction(luaVM, "Tube", "createColTube");
    lua_classfunction(luaVM, "Polygon", "createColPolygon");

    lua_classfunction(luaVM, "getElementsWithin", "getElementsWithinColShape");
    lua_classfunction(luaVM, "isInside", "isInsideColShape");

    lua_classfunction(luaVM, "getShapeType", "getColShapeType");
    lua_classvariable(luaVM, "shapeType", nullptr, "getColShapeType");

    lua_registerclass(luaVM, "ColShape", "Element");
}

int CLuaColShapeDefs::GetColShapeType(lua_State* luaVM)
{
    // Verify the arguments
    CColShape*       pColShape = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            lua_pushnumber(luaVM, pColShape->GetShapeType());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColCircle(lua_State* luaVM)
{
    CVector2D vecPosition;
    float     fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadNumber(fRadius);

    if (!argStream.HasErrors())
    {
        if (fRadius < 0.0f)
            fRadius = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle(pResource, vecPosition, fRadius);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
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

int CLuaColShapeDefs::CreateColCuboid(lua_State* luaVM)
{
    CVector vecPosition;
    CVector vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecSize);

    if (!argStream.HasErrors())
    {
        if (vecSize.fX < 0.0f)
            vecSize.fX = 0.1f;
        if (vecSize.fY < 0.0f)
            vecSize.fY = 0.1f;
        if (vecSize.fZ < 0.0f)
            vecSize.fZ = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid(pResource, vecPosition, vecSize);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
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

int CLuaColShapeDefs::CreateColSphere(lua_State* luaVM)
{
    CVector vecPosition;
    float   fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius);

    if (!argStream.HasErrors())
    {
        if (fRadius < 0.0f)
            fRadius = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere(pResource, vecPosition, fRadius);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
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

int CLuaColShapeDefs::CreateColRectangle(lua_State* luaVM)
{
    CVector2D vecPosition;
    CVector2D vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);

    if (!argStream.HasErrors())
    {
        if (vecSize.fX < 0.0f)
            vecSize.fX = 0.1f;
        if (vecSize.fY < 0.0f)
            vecSize.fY = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle(pResource, vecPosition, vecSize);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
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

int CLuaColShapeDefs::CreateColPolygon(lua_State* luaVM)
{
    //  colshape createColPolygon ( float fX, float fY, float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, ... )
    std::vector<CVector2D> vecPointList;

    CScriptArgReader argStream(luaVM);
    for (uint i = 0; i < 4 || argStream.NextCouldBeNumber(); i++)
    {
        CVector2D vecPoint;
        argStream.ReadVector2D(vecPoint);
        vecPointList.push_back(vecPoint);
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon(pResource, vecPointList);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
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

int CLuaColShapeDefs::CreateColTube(lua_State* luaVM)
{
    CVector vecPosition;
    float   fHeight, fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius);
    argStream.ReadNumber(fHeight);

    if (!argStream.HasErrors())
    {
        if (fRadius < 0.0f)
            fRadius = 0.1f;
        if (fHeight < 0.0f)
            fHeight = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColTube* pShape = CStaticFunctionDefinitions::CreateColTube(pResource, vecPosition, fRadius, fHeight);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
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

int CLuaColShapeDefs::IsInsideColShape(lua_State* luaVM)
{
    //  bool isInsideColShape ( colshape theColShape, float posX, float posY, float posZ )
    CColShape* pColShape;
    CVector    vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        bool bInside = false;
        if (CStaticFunctionDefinitions::IsInsideColShape(pColShape, vecPosition, bInside))
        {
            lua_pushboolean(luaVM, bInside);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}


int CLuaColShapeDefs::GetColPolygonHeight(lua_State* luaVM)
{
    CColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (!argStream.HasErrors())
    {
        if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
        {
            CColPolygon* pColPolygon = (CColPolygon*)pColShape;
            float fFloor, fCeil;
            pColPolygon->GetFloorAndCeil(fFloor, fCeil);
            if (fFloor == std::numeric_limits<float>::min())
            {
                lua_pushboolean(luaVM, false);
            }
            else
            {
                lua_pushnumber(luaVM, fFloor);
            }
            if (fCeil == std::numeric_limits<float>::max())
            {
                lua_pushboolean(luaVM, false);
            }
            else
            {
                lua_pushnumber(luaVM, fCeil);
            }
            return 2;
        }
        else
            argStream.SetCustomError("Colshape have to be type: Polygon to use this function!");
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::SetColPolygonHeight(lua_State* luaVM)
{
    //  bool SetColPolygonHeight ( colshape theColShape, float floor, float ceil )
    CColShape* pColShape;
    float            fFloor;
    float            fCeil;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(fFloor, std::numeric_limits<float>::min());
    argStream.ReadNumber(fCeil, std::numeric_limits<float>::max());

    if (!argStream.HasErrors())
    {
        if (fCeil > fFloor)
        {
            if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
            {
                CColPolygon* pColPolygon = (CColPolygon*)pColShape;
                pColPolygon->SetFloorAndCeil(fFloor, fCeil);
                lua_pushboolean(luaVM, true);
                return 1;
            }
            else
                argStream.SetCustomError("Colshape have to be type: Polygon to use this function!");
        }
        else
            argStream.SetCustomError("Floor Height have to be greater then ceil Height!");
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

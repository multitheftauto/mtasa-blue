/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector4Defs.cpp
 *  PURPOSE:     Js Vector4 class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

CV8ExportClassBase*                  CJsVector4Defs::m_jsClass;
CJsClassConstructionInfo<CVector4D>* CJsVector4Defs::m_constructionInfo;

void CJsVector4Defs::AddClass()
{
    m_constructionInfo = new CJsClassConstructionInfo<CVector4D>("Vector4", EClass::Vector4);

    m_constructionInfo->SetConstructor(JsArgumentParser<Constructor>);

    m_constructionInfo->SetAccessor<&CVector4D::fX, float>("x");
    m_constructionInfo->SetAccessor<&CVector4D::fY, float>("y");
    m_constructionInfo->SetAccessor<&CVector4D::fZ, float>("z");
    m_constructionInfo->SetAccessor<&CVector4D::fW, float>("w");

    m_constructionInfo->SetInheritance(EClass::Vector3);
}

CVector4D* CJsVector4Defs::Constructor(float x, float y, float z, float w)
{
    return new CVector4D(x, y, z, w);
}

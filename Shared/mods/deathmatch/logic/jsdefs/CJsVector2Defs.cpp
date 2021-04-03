/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector2Defs.cpp
 *  PURPOSE:     Js Vector2 class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

CV8ExportClassBase*                  CJsVector2Defs::m_jsClass;
CJsClassConstructionInfo<CVector2D>* CJsVector2Defs::m_constructionInfo;

void CJsVector2Defs::AddClass()
{
    m_constructionInfo = new CJsClassConstructionInfo<CVector2D>("Vector2", EClass::Vector2);

    m_constructionInfo->SetConstructor(JsArgumentParser<Constructor>);

    m_constructionInfo->SetAccessor<&CVector2D::fX, float>("x");
    m_constructionInfo->SetAccessor<&CVector2D::fY, float>("y");
}

CVector2D* CJsVector2Defs::Constructor(float x, float y)
{
    return new CVector2D(x, y);
}

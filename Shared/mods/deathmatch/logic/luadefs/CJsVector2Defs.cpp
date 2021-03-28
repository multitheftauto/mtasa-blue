/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector2Defs.cpp
 *  PURPOSE:     Lua general class functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "luadefs/CJsClassConstructionInfo.h"

CV8ClassBase* CJsVector2Defs::m_jsClass;
CJsClassConstructionInfo<CVector2D>* CJsVector2Defs::m_constructionInfo;

void CJsVector2Defs::AddClass()
{
    m_constructionInfo = new CJsClassConstructionInfo<CVector2D>("Vector2", EClass::Vector2);

    m_constructionInfo->SetConstructor<float, float>();

    m_constructionInfo->SetAccessor<&CVector2D::fX, float>("x");
    m_constructionInfo->SetAccessor<&CVector2D::fY, float>("y");
}

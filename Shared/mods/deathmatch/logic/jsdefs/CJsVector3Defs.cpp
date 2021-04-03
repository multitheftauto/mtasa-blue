/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector2Defs.cpp
 *  PURPOSE:     Js Vector3 class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

CV8ExportClassBase*                CJsVector3Defs::m_jsClass;
CJsClassConstructionInfo<CVector>* CJsVector3Defs::m_constructionInfo;

void CJsVector3Defs::AddClass()
{
    m_constructionInfo = new CJsClassConstructionInfo<CVector>("Vector3", EClass::Vector3);

    m_constructionInfo->SetConstructor<Constructor>();

    m_constructionInfo->SetAccessor<&CVector::fX, float>("x");
    m_constructionInfo->SetAccessor<&CVector::fY, float>("y");
    m_constructionInfo->SetAccessor<&CVector::fZ, float>("z");

    m_constructionInfo->SetMethod<GetLength>("getLength");

    m_constructionInfo->SetInheritance(EClass::Vector2);
}

CVector* CJsVector3Defs::Constructor(float x, float y, float z)
{
    return new CVector(x, y, z);
}

float CJsVector3Defs::GetLength(That that)
{
    return that->Length();
}

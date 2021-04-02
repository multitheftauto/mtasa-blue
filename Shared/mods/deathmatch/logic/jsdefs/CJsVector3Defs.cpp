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

CV8ExportClassBase*                CJsVector3Defs::m_jsClass;
CJsClassConstructionInfo<CVector>* CJsVector3Defs::m_constructionInfo;

void CJsVector3Defs::AddClass()
{
    m_constructionInfo = new CJsClassConstructionInfo<CVector>("Vector3", EClass::Vector3);

    m_constructionInfo->SetConstructor<float, float, float>();

    m_constructionInfo->SetAccessor<&CVector::fX, float>("x");
    m_constructionInfo->SetAccessor<&CVector::fY, float>("y");
    m_constructionInfo->SetAccessor<&CVector::fZ, float>("z");
}

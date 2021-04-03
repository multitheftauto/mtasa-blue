/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsColorDefs.cpp
 *  PURPOSE:     Js Color class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <lua/CLuaFunctionParser.h>

CV8ExportClassBase*                  CJsColorDefs::m_jsClass;
CJsClassConstructionInfo<SColor>* CJsColorDefs::m_constructionInfo;

void CJsColorDefs::AddClass()
{
    m_constructionInfo = new CJsClassConstructionInfo<SColor>("Color", EClass::Color);

    m_constructionInfo->SetConstructor(JsArgumentParser<Constructor>);

    m_constructionInfo->SetAccessor<&SColor::R, unsigned char>("r");
    m_constructionInfo->SetAccessor<&SColor::G, unsigned char>("g");
    m_constructionInfo->SetAccessor<&SColor::B, unsigned char>("b");
    m_constructionInfo->SetAccessor<&SColor::A, unsigned char>("a");
}

SColor* CJsColorDefs::Constructor(float r, float g, float b, float a)
{
    SColor* color = new SColor();
    color->R = r;
    color->G = g;
    color->B = b;
    color->A = a;
    return color;
}

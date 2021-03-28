/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsVector2Defs.h
 *  PURPOSE:     Js CVector2 class construction info
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "jsdefs/CJsDefs.h"

template<typename T>
class CJsClassConstructionInfo;

class CJsVector2Defs : public CJsDefs
{
public:
    static void AddClass();

    static CV8ClassBase* m_jsClass;
    static CJsClassConstructionInfo<CVector2D>* m_constructionInfo;
};

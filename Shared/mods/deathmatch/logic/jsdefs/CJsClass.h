/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CJsClass.h
 *  PURPOSE:     Js base class construction info
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "jsdefs/CJsDefs.h"

template <typename T>
class CJsClassConstructionInfo;

template <typename T>
class CJsClass : public CJsDefs
{
public:
    class That
    {
    public:
        That(T* value) : m_value(value) {}

        T* operator->() { return m_value; }

    private:
        T* m_value;
    };

    static CV8ExportClassBase*          m_jsClass;
    static CJsClassConstructionInfo<T>* m_constructionInfo;
};

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CCustomData.cpp
 *  PURPOSE:     Custom entity data class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#ifdef MTA_CLIENT
    #define DECLARE_PROFILER_SECTION_CCustomData
    #include "profiler/SharedUtil.Profiler.h"
#else



CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    for (auto& [name, data] : m_Data)
    {
        switch (data.Variable.GetType())
        {
            case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(name.c_str());
                attr->SetValue(data.Variable.GetString().c_str());
                break;
            }
            case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(name.c_str());
                attr->SetValue((float)data.Variable.GetNumber());
                break;
            }
            case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(name.c_str());
                attr->SetValue(data.Variable.GetBoolean());
                break;
            }
        }
    }
    return pNode;
}
#endif
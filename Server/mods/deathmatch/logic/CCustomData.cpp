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

CXMLNode* CCustomData::OutputToXML(CXMLNode* pNode)
{
    std::map<std::string, SCustomData>::const_iterator iter = m_Data.begin();
    for (; iter != m_Data.end(); iter++)
    {
        CLuaArgument* arg = (CLuaArgument*)&iter->second.Variable;

        switch (arg->GetType())
        {
            case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.c_str());
                attr->SetValue(arg->GetString().c_str());
                break;
            }
            case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.c_str());
                attr->SetValue((float)arg->GetNumber());
                break;
            }
            case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create(iter->first.c_str());
                attr->SetValue(arg->GetBoolean());
                break;
            }
        }
    }
    return pNode;
}

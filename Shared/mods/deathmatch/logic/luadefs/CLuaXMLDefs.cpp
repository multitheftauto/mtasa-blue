/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaXMLDefs.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaXMLDefs.h"
#include "CScriptArgReader.h"

void CLuaXMLDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"xmlCreateFile", xmlCreateFile},
        {"xmlLoadFile", xmlLoadFile},
        {"xmlLoadString", xmlLoadString},
        {"xmlCopyFile", xmlCopyFile},
        {"xmlSaveFile", xmlSaveFile},
        {"xmlUnloadFile", xmlUnloadFile},
        {"xmlCreateChild", xmlCreateChild},
        {"xmlDestroyNode", xmlDestroyNode},
        {"xmlFindChild", xmlNodeFindChild},

        {"xmlNodeGetChildren", xmlNodeGetChildren},
        {"xmlNodeGetParent", xmlNodeGetParent},
        {"xmlNodeGetValue", xmlNodeGetValue},
        {"xmlNodeGetAttributes", xmlNodeGetAttributes},
        {"xmlNodeGetAttribute", xmlNodeGetAttribute},
        {"xmlNodeGetName", xmlNodeGetName},

        {"xmlNodeSetValue", xmlNodeSetValue},
        {"xmlNodeSetAttribute", xmlNodeSetAttribute},
        {"xmlNodeSetName", xmlNodeSetName},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaXMLDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "load", "xmlLoadFile");
    lua_classfunction(luaVM, "loadstring", "xmlLoadString");
    lua_classfunction(luaVM, "unload", "xmlUnloadFile");
    lua_classfunction(luaVM, "copy", "xmlCopyFile");
    lua_classfunction(luaVM, "create", "xmlCreateFile");
    lua_classfunction(luaVM, "destroy", "xmlDestroyNode");
#ifndef MTA_CLIENT
    lua_classfunction(luaVM, "loadMapData", "loadMapData");
    lua_classfunction(luaVM, "saveMapData", "saveMapData");
#endif // !MTA_CLIENT

    lua_classfunction(luaVM, "getValue", "xmlNodeGetValue");
    lua_classfunction(luaVM, "setAttribute", "xmlNodeSetAttribute");
    lua_classfunction(luaVM, "setValue", "xmlNodeSetValue");
    lua_classfunction(luaVM, "saveFile", "xmlSaveFile");
    lua_classfunction(luaVM, "createChild", "xmlCreateChild");
    lua_classfunction(luaVM, "findChild", "xmlFindChild");
    lua_classfunction(luaVM, "setName", "xmlNodeSetName");

    lua_classfunction(luaVM, "getAttributes", "xmlNodeGetAttributes");
    lua_classfunction(luaVM, "getChildren", "xmlNodeGetChildren");
    lua_classfunction(luaVM, "getName", "xmlNodeGetName");
    lua_classfunction(luaVM, "getParent", "xmlNodeGetParent");
    lua_classfunction(luaVM, "getAttribute", "xmlNodeGetAttribute");

    lua_classvariable(luaVM, "value", "xmlNodeSetValue", "xmlNodeGetValue");
    lua_classvariable(luaVM, "name", "xmlNodeSetName", "xmlNodeGetName");
    lua_classvariable(luaVM, "attributes", NULL, "xmlNodeGetAttributes");
    lua_classvariable(luaVM, "children", NULL, "xmlNodeGetChildren");
    lua_classvariable(luaVM, "parent", NULL, "xmlNodeGetParent");

    lua_registerclass(luaVM, "XML");
}

int CLuaXMLDefs::xmlCreateFile(lua_State* luaVM)
{
#ifndef MTA_CLIENT
    if (lua_type(luaVM, 3) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "xmlCreateFile may be using an outdated syntax. Please check and update.");
#endif // !MTA_CLIENT

    // Grab our resource
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        SString strInputPath, strRootNodeName;

        CScriptArgReader argStream(luaVM);
        argStream.ReadString(strInputPath);
        argStream.ReadString(strRootNodeName);

        if (!argStream.HasErrors())
        {
            SString    strPath;
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pOtherResource = pThisResource;            // clientside, this variable will always be pThisResource

            // Resolve other resource from name
            if (CResourceManager::ParseResourcePathInput(strInputPath, pOtherResource, &strPath, nullptr))
            {
                CheckCanModifyOtherResource(argStream, pThisResource, pOtherResource);
                CheckCanAccessOtherResourceFile(argStream, pThisResource, pOtherResource, strPath);
                if (!argStream.HasErrors())
                {
                    // Make sure the dir exists so we can successfully make the file
                    MakeSureDirExists(strPath);

                    // Create the XML file
                    CXMLFile* xmlFile = pLuaMain->CreateXML(strPath);
                    if (xmlFile)
                    {
                        // Create its root node
                        CXMLNode* pRootNode = xmlFile->CreateRootNode(strRootNodeName);
                        if (pRootNode)
                        {
                            lua_pushxmlnode(luaVM, pRootNode);
                            return 1;
                        }
                        // Destroy it if we failed
                        pLuaMain->DestroyXML(xmlFile);
                    }
                }
            }
        }

        if (argStream.HasErrors())
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlLoadFile(lua_State* luaVM)
{
#ifndef MTA_CLIENT
    if (lua_type(luaVM, 2) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "xmlLoadFile may be using an outdated syntax. Please check and update.");
#endif // !MTA_CLIENT

    // Grab our resource
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        SString strFileInput;
        bool    bReadOnly;

        CScriptArgReader argStream(luaVM);
        argStream.ReadString(strFileInput);
        argStream.ReadIfNextIsBool(bReadOnly, false);

        if (!argStream.HasErrors())
        {
            SString    strPath;
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pOtherResource = pThisResource;

            // Resolve other resource from name
            if (CResourceManager::ParseResourcePathInput(strFileInput, pOtherResource, &strPath))
            {
                CheckCanModifyOtherResource(argStream, pThisResource, pOtherResource);
                CheckCanAccessOtherResourceFile(argStream, pThisResource, pOtherResource, strPath, &bReadOnly);
                if (!argStream.HasErrors())
                {
                    // Make sure the dir exists so we can successfully make the file
                    MakeSureDirExists(strPath);

                    // Create the XML
                    CXMLFile* xmlFile = pLuaMain->CreateXML(strPath.c_str(), true, bReadOnly);
                    if (xmlFile)
                    {
                        // Try to parse it
                        if (xmlFile->Parse())
                        {
                            // Grab the root node. If it didn't exist, create one
                            CXMLNode* pRootNode = xmlFile->GetRootNode();
                            if (!pRootNode)
                                pRootNode = xmlFile->CreateRootNode("root");

                            // Could we create one?
                            if (pRootNode)
                            {
                                // Return the root node
                                lua_pushxmlnode(luaVM, pRootNode);
                                return 1;
                            }
                        }

                        if (FileExists(strPath))
                        {
                            SString strError;
                            xmlFile->GetLastError(strError);
                            if (!strError.empty())
                                argStream.SetCustomError(strError, SString("Unable to read XML file %s", strFileInput.c_str()));
                        }
                        // Destroy it if we failed
                        pLuaMain->DestroyXML(xmlFile);
                    }
                }
            }
        }

        if (argStream.HasErrors())
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlLoadString(lua_State* luaVM)
{
    SString strXmlContent;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strXmlContent);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    // Grab our resource
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLuaMain)
    {
        CXMLNode* rootNode = pLuaMain->ParseString(strXmlContent);

        if (rootNode && rootNode->IsValid())
        {
            lua_pushxmlnode(luaVM, rootNode);
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom(luaVM, "Unable to load XML string");
    }

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlCopyFile(lua_State* luaVM)
{
#ifndef MTA_CLIENT
    if (lua_type(luaVM, 3) == LUA_TLIGHTUSERDATA)
        m_pScriptDebugging->LogCustom(luaVM, "xmlCopyFile may be using an outdated syntax. Please check and update.");
#endif // !MTA_CLIENT

    // Grab our resource
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine(luaVM);
    if (pLUA)
    {
        SString   strFile;
        CXMLNode* pSourceNode;

        CScriptArgReader argStream(luaVM);
        argStream.ReadUserData(pSourceNode);
        argStream.ReadString(strFile);

        if (!argStream.HasErrors())
        {
            SString    strPath;
            CResource* pThisResource = pLUA->GetResource();
            CResource* pOtherResource = pThisResource;

            // Resolve other resource from name
            if (CResourceManager::ParseResourcePathInput(strFile, pOtherResource, &strPath, NULL))
            {
                CheckCanModifyOtherResource(argStream, pThisResource, pOtherResource);
                CheckCanAccessOtherResourceFile(argStream, pThisResource, pOtherResource, strPath);
                if (!argStream.HasErrors())
                {
                    if (pSourceNode)
                    {
                        // Make sure the dir exists so we can successfully make the file
                        MakeSureDirExists(strPath);

                        // Grab the roots tag name
                        std::string strRootTagName;
                        strRootTagName = pSourceNode->GetTagName();

                        // Create the new XML file and its root node
                        CXMLFile* pNewXML = pLUA->CreateXML(strPath.c_str());
                        if (pNewXML)
                        {
                            // Grab the root of the new XML
                            CXMLNode* pNewRoot = pNewXML->CreateRootNode(strRootTagName);
                            if (pNewRoot)
                            {
                                // Copy over the attributes from the root
                                int            iAttributeCount = pSourceNode->GetAttributes().Count();
                                int            i = 0;
                                CXMLAttribute* pAttribute;
                                for (; i < iAttributeCount; i++)
                                {
                                    pAttribute = pSourceNode->GetAttributes().Get(i);
                                    if (pAttribute)
                                        pNewRoot->GetAttributes().Create(*pAttribute);
                                }

                                // Copy the stuff from the given source node to the destination root
                                if (pSourceNode->CopyChildrenInto(pNewRoot, true))
                                {
                                    lua_pushxmlnode(luaVM, pNewRoot);
                                    return 1;
                                }
                            }

                            // Delete the XML again
                            pLUA->DestroyXML(pNewXML);
                        }
                    }
                    else
                        argStream.SetCustomError(SString("Unable to copy XML file %s", strFile.c_str()), "Bad filepath");
                }
            }
        }

        if (argStream.HasErrors())
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    // Error
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlSaveFile(lua_State* luaVM)
{
    CXMLNode* pNode = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            if (luaMain->SaveXML(pNode))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlUnloadFile(lua_State* luaVM)
{
    CXMLNode* pNode = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            if (luaMain->DestroyXML(pNode))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlCreateChild(lua_State* luaVM)
{
    CXMLNode* pNode = nullptr;
    SString   strChildName;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadString(strChildName);

    if (!argStream.HasErrors())
    {
        CXMLNode* pXMLSubNode = pNode->CreateSubNode(strChildName);
        if (pXMLSubNode)
        {
            lua_pushxmlnode(luaVM, pXMLSubNode);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlDestroyNode(lua_State* luaVM)
{
    // Verify the argument type
    CXMLNode*        pNode = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        // Grab the parent so that we can delete this node from it
        CXMLNode* pParent = pNode->GetParent();
        if (pParent)
        {
            // Delete it
            pParent->DeleteSubNode(pNode);

            // Return success
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeFindChild(lua_State* luaVM)
{
    CXMLNode*    pNode;
    SString      strTagName;
    unsigned int uiIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadString(strTagName);
    argStream.ReadNumber(uiIndex);

    if (!argStream.HasErrors())
    {
        CXMLNode* pFoundNode = pNode->FindSubNode(strTagName, uiIndex);
        if (pFoundNode)
        {
            lua_pushxmlnode(luaVM, pFoundNode);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeGetChildren(lua_State* luaVM)
{
    CXMLNode*    pNode;
    unsigned int uiIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadNumber(uiIndex, -1);

    if (!argStream.HasErrors())
    {
        bool bGetAllChildren = uiIndex == -1;
        if (!bGetAllChildren)
        {
            CXMLNode* pFoundNode = pNode->GetSubNode(uiIndex);
            if (pFoundNode)
            {
                lua_pushxmlnode(luaVM, pFoundNode);
                return 1;
            }
        }
        else
        {
            lua_newtable(luaVM);
            uiIndex = 0;
            for (auto iter = pNode->ChildrenBegin(); iter != pNode->ChildrenEnd(); ++iter)
            {
                lua_pushnumber(luaVM, ++uiIndex);
                lua_pushxmlnode(luaVM, *iter);
                lua_settable(luaVM, -3);
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeGetParent(lua_State* luaVM)
{
    CXMLNode*        pNode = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        CXMLNode* pParent = pNode->GetParent();
        if (pParent)
        {
            lua_pushxmlnode(luaVM, pParent);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeGetValue(lua_State* luaVM)
{
    CXMLNode*        pNode = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pNode->GetTagContent().c_str());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeGetAttributes(lua_State* luaVM)
{
    CXMLNode* pNode = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        lua_newtable(luaVM);
        for (auto iter = pNode->GetAttributes().ListBegin(); iter != pNode->GetAttributes().ListEnd(); ++iter)
        {
            lua_pushstring(luaVM, (*iter)->GetName().c_str());
            lua_pushstring(luaVM, (*iter)->GetValue().c_str());
            lua_settable(luaVM, -3);
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeGetAttribute(lua_State* luaVM)
{
    CXMLNode*        pNode = nullptr;
    SString          strAttributeName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadString(strAttributeName);

    if (!argStream.HasErrors())
    {
        // Find the attribute with that name
        CXMLAttribute* pAttribute = pNode->GetAttributes().Find(strAttributeName);
        if (pAttribute)
        {
            // Read the attribute and return the string
            lua_pushstring(luaVM, pAttribute->GetValue().c_str());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeGetName(lua_State* luaVM)
{
    CXMLNode* pNode = nullptr;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);

    if (!argStream.HasErrors())
    {
        lua_pushstring(luaVM, pNode->GetTagName().c_str());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeSetValue(lua_State* luaVM)
{
    CXMLNode* pNode = nullptr;
    SString   strValue = "";
    bool      bUseCDATA;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadString(strValue);
    argStream.ReadBool(bUseCDATA, false);

    if (!argStream.HasErrors())
    {
        pNode->SetTagContent(strValue, bUseCDATA);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeSetAttribute(lua_State* luaVM)
{
    CXMLNode*        pNode = nullptr;
    SString          strAttributeName = "";
    SString          strAttributeValue = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadString(strAttributeName);
    argStream.ReadString(strAttributeValue, "");

    if (!argStream.HasErrors())
    {
        // Are we going to set it to a value?
        if (argStream.NextCouldBeString(-1))
        {
            // Write the node
            CXMLAttribute* pAttribute = pNode->GetAttributes().Create(strAttributeName);
            if (pAttribute)
            {
                pAttribute->SetValue(strAttributeValue);

                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
        {
            // Delete the attribute if it exists
            CXMLAttribute* pAttribute = pNode->GetAttributes().Find(strAttributeName);
            if (pAttribute)
            {
                delete pAttribute;

                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaXMLDefs::xmlNodeSetName(lua_State* luaVM)
{
    CXMLNode*        pNode = nullptr;
    SString          strTagName = "";
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pNode);
    argStream.ReadString(strTagName);

    if (!argStream.HasErrors())
    {
        pNode->SetTagName(strTagName);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

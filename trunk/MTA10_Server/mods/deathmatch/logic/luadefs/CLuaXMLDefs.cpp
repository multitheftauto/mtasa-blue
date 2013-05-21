/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaXMLDefs.cpp
*  PURPOSE:     Lua XML function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               Kevin Whiteside <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaXMLDefs::LoadFunctions ( void )
{
    // ** BACKWARDS COMPATIBILITY FUNCS. SHOULD BE REMOVED BEFORE FINAL RELEASE! **
    CLuaCFunctions::AddFunction ( "xmlNodeGetSubNodes", CLuaXMLDefs::xmlNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlCreateSubNode", CLuaXMLDefs::xmlCreateChild );
    CLuaCFunctions::AddFunction ( "xmlFindSubNode", CLuaXMLDefs::xmlNodeFindChild );
    // ** END OF BACKWARDS COMPATIBILITY FUNCS. **

    CLuaCFunctions::AddFunction ( "xmlCreateFile", CLuaXMLDefs::xmlCreateFile );
    CLuaCFunctions::AddFunction ( "xmlLoadFile", CLuaXMLDefs::xmlLoadFile );
    CLuaCFunctions::AddFunction ( "xmlCopyFile", CLuaXMLDefs::xmlCopyFile );
    CLuaCFunctions::AddFunction ( "xmlSaveFile", CLuaXMLDefs::xmlSaveFile );
    CLuaCFunctions::AddFunction ( "xmlUnloadFile", CLuaXMLDefs::xmlUnloadFile );

    CLuaCFunctions::AddFunction ( "xmlCreateChild", CLuaXMLDefs::xmlCreateChild );
    CLuaCFunctions::AddFunction ( "xmlDestroyNode", CLuaXMLDefs::xmlDestroyNode );
    CLuaCFunctions::AddFunction ( "xmlFindChild", CLuaXMLDefs::xmlNodeFindChild );
    CLuaCFunctions::AddFunction ( "xmlNodeGetChildren", CLuaXMLDefs::xmlNodeGetChildren );
    CLuaCFunctions::AddFunction ( "xmlNodeGetParent", CLuaXMLDefs::xmlNodeGetParent );

    CLuaCFunctions::AddFunction ( "xmlNodeGetValue", CLuaXMLDefs::xmlNodeGetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeSetValue", CLuaXMLDefs::xmlNodeSetValue );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttributes", CLuaXMLDefs::xmlNodeGetAttributes );
    CLuaCFunctions::AddFunction ( "xmlNodeGetAttribute", CLuaXMLDefs::xmlNodeGetAttribute );
    CLuaCFunctions::AddFunction ( "xmlNodeSetAttribute", CLuaXMLDefs::xmlNodeSetAttribute );

    CLuaCFunctions::AddFunction ( "xmlNodeGetName", CLuaXMLDefs::xmlNodeGetName );
    CLuaCFunctions::AddFunction ( "xmlNodeSetName", CLuaXMLDefs::xmlNodeSetName );
}


int CLuaXMLDefs::xmlCreateFile ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 3 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "xmlCreateFile may be using an outdated syntax. Please check and update." );

    // Grab our resource
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLUA )
    {
        SString strFile, strRootNodeName;
        
        CScriptArgReader argStream ( luaVM );
        argStream.ReadString ( strFile );
        argStream.ReadString ( strRootNodeName );

        if ( !argStream.HasErrors () )
        {
            SString strPath;
            CResource* pThisResource = pLUA->GetResource ();
            CResource* pResource = pThisResource;
            
            if ( CResourceManager::ParseResourcePathInput ( strFile, pThisResource, &strPath, NULL ) )
            {
                // We have access to modify this resource?
                if ( pResource == pThisResource ||
                    m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                    "ModifyOtherObjects",
                                                    CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                    false ) )
                {

                    // Make sure the dir exists so we can successfully make the file
                    MakeSureDirExists ( strPath );

                    // Create the XML file
                    CXMLFile * xmlFile = pLUA->CreateXML ( strPath );
                    if ( xmlFile )
                    {
                        // Create its root node
                        CXMLNode* pRootNode = xmlFile->CreateRootNode ( strRootNodeName );
                        if ( pRootNode )
                        {
                            lua_pushxmlnode ( luaVM, pRootNode );
                            return 1;
                        }
                        // Delete it again
                        pLUA->DestroyXML ( xmlFile );
                    }
                }
            }
        
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }
            
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlLoadFile ( lua_State* luaVM )
{
     if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "xmlLoadFile may be using an outdated syntax. Please check and update." );

    // Grab our resource
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLUA )
    {
        SString strFile;
        
        CScriptArgReader argStream ( luaVM );
        argStream.ReadString ( strFile );

        if ( !argStream.HasErrors () )
        {
            SString strPath;
            CResource* pThisResource = pLUA->GetResource ();
            CResource* pResource = pThisResource;
            
            if ( CResourceManager::ParseResourcePathInput ( strFile, pThisResource, &strPath, NULL ) )
            {
                // We have access to modify this resource?
                if ( pResource == pThisResource ||
                    m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                    "ModifyOtherObjects",
                                                    CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                    false ) )
                {

                    // Make sure the dir exists so we can successfully make the file
                    MakeSureDirExists ( strPath );

                    // Create the XML
                    CXMLFile* xmlFile = pLUA->CreateXML ( strPath.c_str () );
                    if ( xmlFile )
                    {
                        // Try to parse it
                        if ( xmlFile->Parse () )
                        {
                            // Grab the root node. If it didn't exist, create one
                            CXMLNode * pRootNode = xmlFile->GetRootNode ();
                            if ( !pRootNode )
                                pRootNode = xmlFile->CreateRootNode ( "root" );

                            // Could we create one?
                            if ( pRootNode )
                            {
                                // Return the root node
                                lua_pushxmlnode ( luaVM, pRootNode );
                                return 1;
                            }
                        }

                        // Destroy it if we failed
                        pLUA->DestroyXML ( xmlFile );
                    }
                }
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlCopyFile ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 3 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "xmlCopyFile may be using an outdated syntax. Please check and update." );

    // Grab our resource
    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLUA )
    {
        SString strFile;
        CXMLNode* pSourceNode;
        
        CScriptArgReader argStream ( luaVM );
        argStream.ReadUserData ( pSourceNode );
        argStream.ReadString ( strFile );

        if ( !argStream.HasErrors () )
        {
            SString strPath;
            CResource* pThisResource = pLUA->GetResource ();
            CResource* pResource = pThisResource;
            
            if ( CResourceManager::ParseResourcePathInput ( strFile, pThisResource, &strPath, NULL ) )
            {
                // We have access to modify this resource?
                if ( pResource == pThisResource ||
                    m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                    "ModifyOtherObjects",
                                                    CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                    false ) )
                {

                    // Make sure the dir exists so we can successfully make the file
                    MakeSureDirExists ( strPath );

                    // Grab the roots tag name
                    std::string strRootTagName;
                    strRootTagName = pSourceNode->GetTagName ();

                    // Create the new XML file and its root node
                    CXMLFile* pNewXML = pLUA->CreateXML ( strPath.c_str () );
                    if ( pNewXML )
                    {
                        // Grab the root of the new XML
                        CXMLNode* pNewRoot = pNewXML->CreateRootNode ( strRootTagName );
                        if ( pNewRoot )
                        {
                            // Copy over the attributes from the root
                            int iAttributeCount = pSourceNode->GetAttributes ().Count ();
                            int i = 0;
                            CXMLAttribute* pAttribute;
                            for ( ; i < iAttributeCount; i++ )
                            {
                                pAttribute = pSourceNode->GetAttributes ().Get ( i );
                                if ( pAttribute )
                                    pNewRoot->GetAttributes ().Create ( *pAttribute );
                            }

                            // Copy the stuff from the given source node to the destination root
                            if ( pSourceNode->CopyChildrenInto ( pNewRoot, true ) )
                            {
                                lua_pushxmlnode ( luaVM, pNewRoot );
                                return 1;
                            }
                        }

                        // Delete the XML again
                        pLUA->DestroyXML ( pNewXML );
                    }
                }
                else
                    CLogger::ErrorPrintf ( "Unable to copy xml file; bad filepath\n" );
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );

        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    }

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlSaveFile ( lua_State* luaVM )
{
    CXMLNode* pNode;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            luaMain->SaveXML ( pNode );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlUnloadFile ( lua_State* luaVM )
{
    CXMLNode* pNode;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            luaMain->DestroyXML ( pNode );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlCreateChild ( lua_State* luaVM )
{
    CXMLNode* pNode;
    SString strSubNodeName;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadString ( strSubNodeName );

    if ( !argStream.HasErrors () )
    {
        CXMLNode* pXMLSubNode = pNode->CreateSubNode ( strSubNodeName );
        if ( pXMLSubNode )
        {
            lua_pushxmlnode ( luaVM, pXMLSubNode );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlDestroyNode ( lua_State* luaVM )
{
    CXMLNode* pNode;
    SString strSubNodeName;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        CXMLNode* pParent = pNode->GetParent ();
        if ( pParent )
        {
            // Delete it
            pParent->DeleteSubNode ( pNode );

            // Return success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeFindChild ( lua_State* luaVM )
{   
    CXMLNode* pNode;
    SString strTagName;
    unsigned int uiIndex;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadString ( strTagName );
    argStream.ReadNumber ( uiIndex );

    if ( !argStream.HasErrors () )
    {
        CXMLNode * pFoundNode = pNode->FindSubNode ( strTagName, uiIndex );
        if ( pFoundNode )
        {
            lua_pushxmlnode ( luaVM, pFoundNode );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetChildren ( lua_State* luaVM )
{
    CXMLNode* pNode;
    unsigned int uiIndex;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadNumber ( uiIndex, 0 );

    bool bGetAllChildren = uiIndex == 0;

    if ( !argStream.HasErrors () )
    {
        CXMLNode * pFoundNode = NULL;
        if ( !bGetAllChildren )
        {
            pFoundNode = pNode->GetSubNode ( uiIndex );
            if ( pFoundNode )
            {
                lua_pushxmlnode ( luaVM, pFoundNode );
                return 1;
            }
        }
        else
        {
            lua_newtable ( luaVM );
            list < CXMLNode * > ::iterator iter = pNode->ChildrenBegin ();
            for ( ; iter != pNode->ChildrenEnd () ; iter++ )
            {
                lua_pushnumber ( luaVM, ++uiIndex );
                lua_pushxmlnode ( luaVM, *iter );
                lua_settable ( luaVM, -3 );
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetParent ( lua_State* luaVM )
{
    CXMLNode* pNode;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        CXMLNode * pParent = pNode->GetParent ( );
        if ( pParent )
        {
            lua_pushxmlnode ( luaVM, pParent );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaXMLDefs::xmlNodeGetValue ( lua_State* luaVM )
{    
    CXMLNode* pNode;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        lua_pushstring(luaVM, pNode->GetTagContent ( ).c_str());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeSetValue ( lua_State* luaVM )
{
    CXMLNode* pNode;
    SString strData;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadString ( strData );

    if ( !argStream.HasErrors () )
    {
        pNode->SetTagContent ( strData );
        lua_pushboolean(luaVM, true); 
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetAttributes ( lua_State* luaVM )
{  
    CXMLNode* pNode;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        lua_newtable ( luaVM );
        list < class CXMLAttribute * > ::iterator iter = pNode->GetAttributes().ListBegin();
        for ( ; iter != pNode->GetAttributes().ListEnd() ; iter++ )
        {
            lua_pushstring ( luaVM, ( *iter )->GetName().c_str () );
            lua_pushstring ( luaVM, ( *iter )->GetValue().c_str () );
            lua_settable ( luaVM, -3 );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetAttribute ( lua_State* luaVM )
{
    CXMLNode* pNode;
    SString strAttributeName;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadString ( strAttributeName );

    if ( !argStream.HasErrors () )
    {
        lua_pushstring(luaVM, pNode->GetAttributeValue(strAttributeName));
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeSetAttribute ( lua_State* luaVM )
{
    CXMLNode* pNode;
    SString strAttributeName;
    SString strAttributeValue;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadString ( strAttributeName );

    if ( !argStream.HasErrors () )
    {
        if ( argStream.NextIsString() )
        {
            // Write
            argStream.ReadString( strAttributeValue );
            
            CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( strAttributeName );
            if ( pAttribute )
            {
                pAttribute->SetValue ( strAttributeValue );

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
        {
            // Delete the attribute if it exists
            CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( strAttributeName );
            if ( pAttribute )
            {
                delete pAttribute;

                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
            

int CLuaXMLDefs::xmlNodeGetName ( lua_State* luaVM )
{
    CXMLNode* pNode;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );

    if ( !argStream.HasErrors () )
    {
        lua_pushstring(luaVM, pNode->GetTagName().c_str());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeSetName ( lua_State* luaVM )
{
    CXMLNode* pNode;
    SString strName;
        
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pNode );
    argStream.ReadString ( strName );

    if ( !argStream.HasErrors () )
    {
        pNode->SetTagName(strName);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

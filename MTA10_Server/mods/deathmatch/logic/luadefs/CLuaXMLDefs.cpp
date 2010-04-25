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
        CResource* pThisResource = pLUA->GetResource ();
        CResource* pResource = pThisResource;

        // Filename
        if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) ||
             ( lua_type ( luaVM, 2 ) != LUA_TSTRING ) )
        {
            m_pScriptDebugging->LogBadType ( luaVM, "xmlCreateFile" );

            lua_pushboolean ( luaVM, false );
            return 1;
        }
        else
        {
            std::string strFile = lua_tostring ( luaVM, 1 );
            std::string strPath;

            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strPath, NULL ) )
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
                    MakeSureDirExists ( strPath.c_str () );

                    // Grab the root
                    const char* szRootName = lua_tostring ( luaVM, 2 );

                    // Create the XML file
                    CXMLFile * xmlFile = pLUA->CreateXML ( strPath.c_str () );
                    if ( xmlFile )
                    {
                        // Create its root node
                        CXMLNode* pRootNode = xmlFile->CreateRootNode ( szRootName );
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
        CResource* pThisResource = pLUA->GetResource ();
        CResource* pResource = pThisResource;
        
        // Filename
        if ( lua_type ( luaVM, 1 ) != LUA_TSTRING )
        {
            m_pScriptDebugging->LogBadType ( luaVM, "xmlLoadFile" );

            lua_pushboolean ( luaVM, false );
            return 1;
        }
        // Grab the filename passed
        std::string strFile = lua_tostring ( luaVM, 1 );
        std::string strPath;


        if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strPath, NULL ) )
        {
            if ( pResource == pThisResource ||
                m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                "ModifyOtherObjects",
                                                CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                false ) )
            {
                // Make sure the dir exists so we can successfully make the file
                MakeSureDirExists ( strPath.c_str () );

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
            else
                m_pScriptDebugging->LogError ( luaVM, "xmlLoadFile failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
        }
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
        CResource* pThisResource = pLUA->GetResource ();
        CResource* pResource = pThisResource;

        // Verify the argument types passed
        if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  &&
            lua_type ( luaVM, 2 ) == LUA_TSTRING )
        {
            // Grab the filename passed
            std::string strFile = lua_tostring ( luaVM, 2 );
            std::string strPath;
            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strPath, NULL ) )
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
                    MakeSureDirExists ( strPath.c_str () );

                    // Grab the source node
                    CXMLNode* pSourceNode = lua_toxmlnode ( luaVM, 1 );
                    if ( pSourceNode )
                    {
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
                        CLogger::ErrorPrintf ( "Unable to copy xml file; bad filepath" );
                }
                else
                    m_pScriptDebugging->LogError ( luaVM,"xmlCopyFile failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "xmlCopyFile" );
    }

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlSaveFile ( lua_State* luaVM )
{
    // CXMLNode*
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlSaveFile" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                luaMain->SaveXML ( pNode );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlUnloadFile ( lua_State* luaVM )
{
    // CXMLNode*
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlUnloadFile" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( luaMain )
            {
                luaMain->DestroyXML ( pNode );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlCreateChild ( lua_State* luaVM )
{
    // Node name
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
         lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlCreateChild" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        // Get the Node
        CXMLNode* pXMLNode = lua_toxmlnode ( luaVM, 1 );
        if ( pXMLNode )
        {
            // Grab the subnode name
            const char* szSubNodeName = lua_tostring ( luaVM, 2 );
            if ( szSubNodeName )
            {
                CXMLNode* pXMLSubNode = pXMLNode->CreateSubNode ( szSubNodeName );
                if ( pXMLSubNode )
                {
                    lua_pushxmlnode ( luaVM, pXMLSubNode );
                    return 1;
                }
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlDestroyNode ( lua_State* luaVM )
{
    // Verify the argument type
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Get the Node
        CXMLNode* pXMLNode = lua_toxmlnode ( luaVM, 1 );
        if ( pXMLNode )
        {
            // Grab the parent so that we can delete this node from it
            CXMLNode* pParent = pXMLNode->GetParent ();
            if ( pParent )
            {
                // Delete it
                pParent->DeleteSubNode ( pXMLNode );

                // Return success
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlDestroyNode" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeFindChild ( lua_State* luaVM )
{
    // xmlNode*, node name, index
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
         lua_type ( luaVM, 2 ) != LUA_TSTRING ||
         lua_type ( luaVM, 3 ) != LUA_TNUMBER )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeFindChild" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            const char * szTagName = lua_tostring ( luaVM, 2 );
            unsigned int iIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
            CXMLNode * pFoundNode = pNode->FindSubNode ( szTagName, iIndex );
            if ( pFoundNode )
            {
                lua_pushxmlnode ( luaVM, pFoundNode );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetChildren ( lua_State* luaVM )
{
    // xmlNode*, [index]
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            unsigned int uiIndex = 0;
            bool bGetAllChildren = true;
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
            {
                uiIndex = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
                bGetAllChildren = false;
            }
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
                uiIndex = 0;
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
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetChildren" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetParent ( lua_State* luaVM )
{
    // xmlNode*
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            CXMLNode * pParent = pNode->GetParent ();
            if ( pParent )
            {
                lua_pushxmlnode ( luaVM, pParent );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetParent" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetValue ( lua_State* luaVM )
{
    // pNode, [Buffer Size]
    int iSecondVariableType = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
        !( iSecondVariableType == LUA_TNONE || iSecondVariableType == LUA_TNUMBER ) )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetValue" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            unsigned int iBufferSize = 1024;
            if ( iSecondVariableType == LUA_TNUMBER )
            {
                iBufferSize = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
                if ( iBufferSize > 1024 )
                    iBufferSize = 255;
            }
            lua_pushstring ( luaVM, pNode->GetTagContent ().c_str () );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeSetValue ( lua_State* luaVM )
{
    // pNode, Value
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
         lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetValue" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            const char * szTagContents = lua_tostring ( luaVM, 2 );
            pNode->SetTagContent ( szTagContents );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetAttributes ( lua_State* luaVM )
{
    // pNode, Attribute Name, [Buffer Size]
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
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
            m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetAttributes" );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetAttributes" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetAttribute ( lua_State* luaVM )
{
    // pNode, Attribute Name, [Buffer Size]
    int iThirdVariableType = lua_type ( luaVM, 3 );
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA ||
        !( iThirdVariableType == LUA_TNONE || iThirdVariableType == LUA_TNUMBER ) ||
        lua_type ( luaVM, 2 ) != LUA_TSTRING )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetAttribute" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        // Grab the XML node
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            // Find the attribute with that name
            const char * szAttributeName = lua_tostring ( luaVM, 2 );
            CXMLAttribute * pAttribute = pNode->GetAttributes().Find ( szAttributeName );
            if ( pAttribute )
            {
                // Limit reading to that many characters
                unsigned int iBufferSize = 255;
                if ( iThirdVariableType == LUA_TNUMBER )
                {
                    iBufferSize = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
                    if ( iBufferSize > 1024 )
                        iBufferSize = 255;
                }

                // Read the attribute and return the string
                lua_pushstring ( luaVM, pAttribute->GetValue ().c_str () );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeSetAttribute ( lua_State* luaVM )
{
    // pNode, Attribute Name, Value
    int iType1 = lua_type ( luaVM, 1 );
    int iType2 = lua_type ( luaVM, 2 );
    int iType3 = lua_type ( luaVM, 3 );
    if ( ( iType1 == LUA_TLIGHTUSERDATA ) &&
         ( iType2 == LUA_TSTRING ) &&
         ( iType3 == LUA_TSTRING || iType3 == LUA_TNUMBER || iType3 == LUA_TNIL ) )
    {
        // Grab the xml node
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            // Grab the attribute name and value
            const char * szAttributeName = lua_tostring ( luaVM, 2 );

            // Are we going to set it to a value?
            if ( iType3 == LUA_TSTRING || iType3 == LUA_TNUMBER )
            {
                const char * szAttributeValue = lua_tostring ( luaVM, 3 );

                // Write the node
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Create ( szAttributeName );
                if ( pAttribute )
                {
                    pAttribute->SetValue ( szAttributeValue );

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
            {
                // Delete the attribute if it exists
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( szAttributeName );
                if ( pAttribute )
                {
                    delete pAttribute;

                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
        }
    }
    else
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetAttribute" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeGetName ( lua_State* luaVM )
{
    // xmlNode*, node name, index
    if ( lua_type ( luaVM, 1 ) != LUA_TLIGHTUSERDATA )
    {
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeGetName" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
    else
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            if ( pNode )
            {
                std::string strTagName;
                lua_pushstring ( luaVM, pNode->GetTagName ().c_str () );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaXMLDefs::xmlNodeSetName ( lua_State* luaVM )
{
    int iType1 = lua_type ( luaVM, 1 );
    int iType2 = lua_type ( luaVM, 2 );
    if ( ( iType1 == LUA_TLIGHTUSERDATA ) &&
         ( iType2 == LUA_TSTRING ) )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            std::string strTagName ( lua_tostring ( luaVM, 2 ) );
            pNode->SetTagName ( strTagName );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "xmlNodeSetName", "xml-node", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetName" );

    lua_pushboolean ( luaVM, false );
    return 1;
}

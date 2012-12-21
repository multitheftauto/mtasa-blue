/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.XML.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

#define MAX_STRING_LENGTH 2048

int CLuaFunctionDefs::XMLNodeFindChild ( lua_State* luaVM )
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
        // Grab the XML node
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


int CLuaFunctionDefs::XMLNodeGetChildren ( lua_State* luaVM )
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
                    lua_pushxmlnode ( luaVM, ( CXMLNode * ) ( *iter ) );
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


int CLuaFunctionDefs::XMLNodeGetValue ( lua_State* luaVM )
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


int CLuaFunctionDefs::XMLNodeSetValue ( lua_State* luaVM )
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


int CLuaFunctionDefs::XMLNodeGetName ( lua_State* luaVM )
{
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
                lua_pushstring ( luaVM, pNode->GetTagName ().c_str () );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::XMLNodeSetName ( lua_State* luaVM )
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


int CLuaFunctionDefs::XMLNodeGetAttributes ( lua_State* luaVM )
{
    // pNode, Attribute Name, [Buffer Size]
    int iThirdVariableType = lua_type ( luaVM, 3 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CXMLNode* pNode = lua_toxmlnode ( luaVM, 1 );
        if ( pNode )
        {
            lua_newtable ( luaVM );
            unsigned int uiIndex = 0;
            list < CXMLAttribute * > ::iterator iter = pNode->GetAttributes ().ListBegin ();
            for ( ; iter != pNode->GetAttributes ().ListEnd () ; iter++ )
            {
                lua_pushstring ( luaVM, ( *iter )->GetName ().c_str () );
                lua_pushstring ( luaVM, ( *iter )->GetValue ().c_str () );
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


int CLuaFunctionDefs::XMLNodeGetAttribute ( lua_State* luaVM )
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
            CXMLAttribute * pAttribute = pNode->GetAttributes ().Find ( szAttributeName ); 
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


int CLuaFunctionDefs::XMLNodeSetAttribute ( lua_State* luaVM )
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
        m_pScriptDebugging->LogBadType ( luaVM, "xmlNodeSetAttribute" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::XMLNodeGetParent ( lua_State* luaVM )
{
    // xmlNode*, node name, index
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA  )
    {
        // Grab the XML node
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


int CLuaFunctionDefs::XMLLoadFile ( lua_State* luaVM )
{
//  xmlnode xmlLoadFile ( string filePath )
    SString filePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );

    if ( !argStream.HasErrors () )
    {
        CLuaMain * luaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( luaMain )
        {
            CResource* pResource = luaMain->GetResource();
            SString strFilename;
            if ( CResourceManager::ParseResourcePathInput( filePath, pResource, strFilename ) )
            {
                // Create the XML
                CXMLFile * xmlFile = luaMain->CreateXML ( strFilename );
                if ( xmlFile )
                {
                    // Parse it
                    if ( xmlFile->Parse() )
                    {
                        // Create the root node if it doesn't exist
                        CXMLNode* pRootNode = xmlFile->GetRootNode ();
                        if ( !pRootNode )
                            pRootNode = xmlFile->CreateRootNode ( "root" );

                        // Got a root node?
                        if ( pRootNode )
                        {
                            lua_pushxmlnode ( luaVM, pRootNode );
                            return 1;
                        }
                    }

                    // Destroy the XML
                    luaMain->DestroyXML ( xmlFile );
                }   
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::XMLCreateFile ( lua_State* luaVM )
{
//  xmlnode xmlCreateFile ( string filePath, string rootNodeName )
    SString filePath; SString rootNodeName;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );
    argStream.ReadString ( rootNodeName );

    // Safety check: Don't allow the rootNodeName "private" incase user forget to declare a node name
    if ( rootNodeName == EnumToString ( ACCESS_PRIVATE ) )
    {
        argStream.SetCustomError( "Expected string at argument 2, got access-type" );
    }

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource();
            SString strFile;
            if ( CResourceManager::ParseResourcePathInput( filePath, pResource, strFile ) )
            {
                // Make sure the directory exists
                MakeSureDirExists ( strFile.c_str () );

                // Create the XML
                CXMLFile * xmlFile = pLuaMain->CreateXML ( strFile.c_str () );
                if ( xmlFile )
                {
                    // Create its root node
                    CXMLNode* pRootNode = xmlFile->CreateRootNode ( rootNodeName );
                    if ( pRootNode )
                    {
                        lua_pushxmlnode ( luaVM, pRootNode );
                        return 1;
                    }

                    // Delete the XML again
                    pLuaMain->DestroyXML ( xmlFile );
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::XMLSaveFile ( lua_State* luaVM )
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


int CLuaFunctionDefs::XMLUnloadFile ( lua_State* luaVM )
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


int CLuaFunctionDefs::XMLCreateChild ( lua_State* luaVM )
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
            SString strSubNodeName ( "%s", lua_tostring ( luaVM, 2 ) );
            CXMLNode* pXMLSubNode = pXMLNode->CreateSubNode ( strSubNodeName );
            if ( pXMLSubNode )
            {
                lua_pushxmlnode ( luaVM, pXMLSubNode );
                return 1;
            }
        }
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::XMLDestroyNode ( lua_State* luaVM )
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


int CLuaFunctionDefs::XMLCopyFile ( lua_State* luaVM )
{
//  xmlnode xmlCopyFile ( xmlnode nodeToCopy, string newFilePath )
    CXMLNode* pSourceNode; SString newFilePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSourceNode );
    argStream.ReadString ( newFilePath );

    if ( !argStream.HasErrors () )
    {
        // Grab the virtual machine for this luastate
        CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLUA )
        {
            CResource* pResource = pLUA->GetResource();
            SString strFilename;
            if ( CResourceManager::ParseResourcePathInput( newFilePath, pResource, strFilename ) )
            {
                // Grab the source node
                CXMLNode* pSourceNode = lua_toxmlnode ( luaVM, 1 );
                if ( pSourceNode )
                {
                    // Grab the roots tag name
                    std::string strRootTagName;
                    strRootTagName = pSourceNode->GetTagName ();

                    // Grab our lua VM
                    CLuaMain* pLUA = m_pLuaManager->GetVirtualMachine ( luaVM );

                    // Create the new XML file and its root node
                    CXMLFile* pNewXML = pLUA->CreateXML ( strFilename );
                    if ( pNewXML )
                    {
                        // Create root for new XML
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
            }
            else
                CLogger::ErrorPrintf ( "Unable to copy xml file; bad filepath" );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Error
    lua_pushboolean ( luaVM, false );
    return 1;
}



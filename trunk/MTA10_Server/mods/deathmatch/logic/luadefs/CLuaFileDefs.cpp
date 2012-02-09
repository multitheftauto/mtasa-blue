/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaFileDefs.cpp
*  PURPOSE:     Lua file handling definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               lil_Toady <>
*               Cecill Etheredge <>
*               Florian Busse <flobu@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define DEFAULT_MAX_FILESIZE 52428800

void CLuaFileDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "fileCreate", CLuaFileDefs::fileCreate );
    CLuaCFunctions::AddFunction ( "fileExists", CLuaFileDefs::fileExists );
    CLuaCFunctions::AddFunction ( "fileOpen", CLuaFileDefs::fileOpen );
    CLuaCFunctions::AddFunction ( "fileIsEOF", CLuaFileDefs::fileIsEOF );
    CLuaCFunctions::AddFunction ( "fileGetPos", CLuaFileDefs::fileGetPos );
    CLuaCFunctions::AddFunction ( "fileSetPos", CLuaFileDefs::fileSetPos );
    CLuaCFunctions::AddFunction ( "fileGetSize", CLuaFileDefs::fileGetSize );
    CLuaCFunctions::AddFunction ( "fileRead", CLuaFileDefs::fileRead );
    CLuaCFunctions::AddFunction ( "fileWrite", CLuaFileDefs::fileWrite );
    CLuaCFunctions::AddFunction ( "fileFlush", CLuaFileDefs::fileFlush );
    CLuaCFunctions::AddFunction ( "fileClose", CLuaFileDefs::fileClose );
    CLuaCFunctions::AddFunction ( "fileDelete", CLuaFileDefs::fileDelete );
    CLuaCFunctions::AddFunction ( "fileRename", CLuaFileDefs::fileRename );
}


int CLuaFileDefs::fileCreate ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "fileCreate may be using an outdated syntax. Please check and update." );

    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check argument types
        if ( argtype ( 1, LUA_TSTRING ) )
        {
            // Grab the filename
            std::string strFile = lua_tostring ( luaVM, 1 );
            std::string strAbsPath;
            std::string strSubPath;

            // We have a resource argument?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strAbsPath, &strSubPath ) )
            {
                // Do we have permissions?
                if ( pResource == pThisResource ||
                     m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                {
                    // Make sure the destination folder exist so we can create the file
                    MakeSureDirExists ( strAbsPath.c_str () );

                    // Create the file to create
                    CScriptFile* pFile = new CScriptFile ( NULL, strSubPath.c_str (), DEFAULT_MAX_FILESIZE );
                    assert ( pFile );

                    // Try to load it
                    if ( pFile->Load ( CScriptFile::MODE_CREATE ) )
                    {
                        // Add it to the scrpt resource element group
                        CElementGroup* pGroup = pThisResource->GetElementGroup ();
                        if ( pGroup )
                        {
                            pGroup->Add ( pFile );
                        }

                        // Success. Return the file.
                        lua_pushelement ( luaVM, pFile );
                        return 1;
                    }
                    else
                    {
                        // Delete the file again
                        delete pFile;

                        // Output error
                        m_pScriptDebugging->LogWarning ( luaVM, "fileCreate; unable to load file" );
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "fileCreate failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "fileCreate" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileExists ( lua_State* luaVM )
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check argument types
        if ( argtype ( 1, LUA_TSTRING ) )
        {
            // Grab the filename
            std::string strFile = lua_tostring ( luaVM, 1 );
            std::string strAbsPath;
            std::string strSubPath;

            // We have a resource argument?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strAbsPath, &strSubPath ) )
            {
                string strFilePath;

                // Does file exist?
                if ( pResource->GetFilePath ( strSubPath.c_str(), strFilePath ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
                else
                {
                    lua_pushboolean ( luaVM, false );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "fileExists" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileOpen ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 3 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "fileOpen may be using an outdated syntax. Please check and update." );

    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check argument types
        if ( argtype ( 1, LUA_TSTRING ) )
        {
            // We have a read only argument?
            bool bReadOnly = false;
            if ( argtype ( 2, LUA_TBOOLEAN ) )
            {
                bReadOnly = lua_toboolean ( luaVM, 2 ) ? true:false;
            }

            // Grab the filename
            std::string strFile = lua_tostring ( luaVM, 1 );
            std::string strAbsPath;
            std::string strSubPath;

            // We have a resource argument?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strAbsPath, &strSubPath ) )
            {
                // Do we have permissions?
                if ( pResource == pThisResource ||
                     m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                {

                    // Create the file to create
                    CScriptFile* pFile = new CScriptFile ( NULL, strSubPath.c_str (), DEFAULT_MAX_FILESIZE );
                    assert ( pFile );

                    // Try to load it
                    if ( ( bReadOnly && pFile->Load ( CScriptFile::MODE_READ ) ) ||
                        ( !bReadOnly && pFile->Load ( CScriptFile::MODE_READWRITE ) ) )
                    {
                        // Add it to the scrpt resource element group
                        CElementGroup* pGroup = pThisResource->GetElementGroup ();
                        if ( pGroup )
                        {
                            pGroup->Add ( pFile );
                        }

                        // Success. Return the file.
                        lua_pushelement ( luaVM, pFile );
                        return 1;
                    }
                    else
                    {
                        // Delete the file again
                        delete pFile;

                        // Output error
                        m_pScriptDebugging->LogWarning ( luaVM, "fileOpen; unable to load file" );
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "fileOpen failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "fileOpen" );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileIsEOF ( lua_State* luaVM )
{
    // bool fileIsEOF ( file )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        // Return its EOF state
        lua_pushboolean ( luaVM, pFile->IsEOF () );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileIsEOF", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetPos ( lua_State* luaVM )
{
    // int fileGetPos ( file )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        long lPosition = pFile->GetPointer ();
        if ( lPosition != -1 )
        {
            // Return its position
            lua_pushnumber ( luaVM, lPosition );
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileGetPos", "file", 1 );
            lua_pushnil ( luaVM );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileGetPos", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileSetPos ( lua_State* luaVM )
{
    // bool fileSetPos ( file )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        // Check argument 2
        if ( argtype2 ( 2, LUA_TNUMBER, LUA_TSTRING ) )
        {
            // Grab position
            long lPosition = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );
            if ( lPosition >= 0 )
            {
                long lResultPosition = pFile->SetPointer ( static_cast < unsigned long > ( lPosition ) );
                if ( lResultPosition != -1 )
                {
                    // Set the position and return where we actually got it put
                    lua_pushnumber ( luaVM, lResultPosition );
                }
                else
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "file", 1 );
                    lua_pushnil ( luaVM );
                }
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "number", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileSetPos", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetSize ( lua_State* luaVM )
{
    // int fileGetSize ( file )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        long lSize = pFile->GetSize ();
        if ( lSize != -1 )
        {
            // Return its size
            lua_pushnumber ( luaVM, lSize );
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileGetSize", "file", 1 );
            lua_pushnil ( luaVM );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileGetSize", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileRead ( lua_State* luaVM )
{
    // string fileRead ( file, count )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        // Check argument 2
        if ( argtype2 ( 2, LUA_TNUMBER, LUA_TSTRING ) )
        {
            // Grab count arg
            long lCount = static_cast < long > ( lua_tonumber ( luaVM, 2 ) );
            if ( lCount > 0 )
            {
                // Allocate a buffer to read the stuff into and read some shit into it
                char* pReadContent = new char [lCount + 1];
                long lBytesRead = pFile->Read ( static_cast < unsigned long > ( lCount ),
                                                            pReadContent );

                if ( lBytesRead != -1 )
                {
                    // Push the string onto the lua stack. Use pushlstring so we are binary
                    // compatible. Normal push string takes zero terminated strings.
                    lua_pushlstring ( luaVM, pReadContent, lBytesRead );
                }
                else
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "file", 1 );
                    lua_pushnil ( luaVM );
                }

                // Delete our read content. Lua should've stored it
                delete [] pReadContent;

                // We're returning the result string
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "number", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "number", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileRead", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileWrite ( lua_State* luaVM )
{
    // string fileWrite ( file, string [, string2, string3, ...] )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        // Check argument 2
        if ( argtype ( 2, LUA_TSTRING ) )
        {
            // While we're not out of string arguments
            long lBytesWritten = 0;
            long lArgBytesWritten = 0;
            unsigned int uiCurrentArg = 2;
            do
            {
                // Grab argument and length
                const char* pData = lua_tostring ( luaVM, uiCurrentArg );
                unsigned long ulDataLen = static_cast < unsigned long > ( lua_strlen ( luaVM, uiCurrentArg ) );

                // Write it and add the bytes written to our total bytes written
                lArgBytesWritten = pFile->Write ( ulDataLen, pData );
                if ( lArgBytesWritten == -1 )
                {
                    m_pScriptDebugging->LogBadPointer ( luaVM, "fileWrite", "file", 1 );
                    lua_pushnil ( luaVM );
                    return 1;
                }
                lBytesWritten += lArgBytesWritten;

                // Increment current argument
                ++uiCurrentArg;
            }
            while ( argtype ( uiCurrentArg, LUA_TSTRING ) );

            // Return the number of bytes we wrote
            lua_pushnumber ( luaVM, lBytesWritten );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "fileWrite", "string", 2 );
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileWrite", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileFlush ( lua_State* luaVM )
{
    // string fileFlush ( file )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        // Flush the file
        pFile->Flush ();

        // Success. Return true
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileFlush", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileClose ( lua_State* luaVM )
{
    // string fileClose ( file )

    // Grab the file pointer
    CScriptFile* pFile = lua_tofile ( luaVM, 1 );
    if ( pFile )
    {
        // Close the file and delete it
        pFile->Unload ();
        m_pElementDeleter->Delete ( pFile );

        // Success. Return true
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogBadPointer ( luaVM, "fileClose", "file", 1 );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFileDefs::fileDelete ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 2 ) == LUA_TLIGHTUSERDATA )
        m_pScriptDebugging->LogCustom ( luaVM, "fileDelete may be using an outdated syntax. Please check and update." );

    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check argument types
        if ( argtype ( 1, LUA_TSTRING ) )
        {
            // Grab the filename
            std::string strFile = lua_tostring ( luaVM, 1 );
            std::string strPath;

            // We have a resource argument?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strPath, NULL ) )
            {
                // Do we have permissions?
                if ( pResource == pThisResource ||
                     m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                {
                    // Make sure the dir exists so we can remove the file
                    MakeSureDirExists ( strPath.c_str () );
                    if ( remove ( strPath.c_str () ) == 0 )
                    {
                        // If file removed return success
                        lua_pushboolean ( luaVM, true );
                        return 1;
                    }
                    else
                    {
                        // Output error
                        m_pScriptDebugging->LogWarning ( luaVM, "fileDelete; unable to delete file" );
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "fileDelete failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "fileDelete" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFileDefs::fileRename ( lua_State* luaVM )
{
    // Grab our lua VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Check arguments types
        if ( argtype ( 1, LUA_TSTRING ) && argtype ( 2, LUA_TSTRING ) )
        {
            // Grab the filenames
            std::string strCurFile = lua_tostring ( luaVM, 1 );
            std::string strNewFile = lua_tostring ( luaVM, 2 );
            std::string strCurPath;
            std::string strNewPath;
            std::string strCurSubPath;
            std::string strNewSubPath;

            // We have a resource arguments?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pCurResource = pThisResource;
            CResource* pNewResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( strCurFile, pCurResource, &strCurPath, &strCurSubPath ) &&
                 CResourceManager::ParseResourcePathInput ( strNewFile, pNewResource, &strNewPath, &strNewSubPath ) )
            {
                // Do we have permissions?
                if ( ( pCurResource == pThisResource && 
                       pNewResource == pThisResource ) ||
                     m_pACLManager->CanObjectUseRight ( pThisResource->GetName ().c_str (),
                                                        CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE,
                                                        "ModifyOtherObjects",
                                                        CAccessControlListRight::RIGHT_TYPE_GENERAL,
                                                        false ) )
                {
                    string strCurFilePath;
                    string strNewFilePath;

                    // Does `current` file path exist and `new` file path doesn't exist?
                    if ( pCurResource->GetFilePath ( strCurSubPath.c_str(), strCurFilePath ) &&
                        !pNewResource->GetFilePath ( strNewSubPath.c_str(), strNewFilePath ) )
                    {
                        // Make sure the destination folder exist so we can move the file
                        MakeSureDirExists ( strNewPath.c_str () );

                        if ( FileRename ( strCurPath.c_str (), strNewPath.c_str () ) )
                        {
                            // If file renamed/moved return success
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                        else
                        {
                            // Output error
                            m_pScriptDebugging->LogWarning ( luaVM, "fileRename; unable to rename/move file" );
                        }
                    }
                    else
                    {
                        // Output error
                        m_pScriptDebugging->LogWarning ( luaVM, "fileRename failed; source file doesn't exist or destination file already exists" );
                    }
                }
                // Do we have not permissions to both - `current` and `new` resources?
                else if ( pThisResource != pCurResource && pThisResource != pNewResource )
                    m_pScriptDebugging->LogError ( luaVM, "fileRename failed; ModifyOtherObjects in ACL denied resource %s to access %s and %s", pThisResource->GetName ().c_str (), pCurResource->GetName ().c_str (), pNewResource->GetName ().c_str () );
                // Do we have not permissions to `current` resource?
                else if ( pThisResource != pCurResource && pThisResource == pNewResource )
                    m_pScriptDebugging->LogError ( luaVM, "fileRename failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pCurResource->GetName ().c_str () );
                // Do we have not permissions to `new` resource?
                else
                    m_pScriptDebugging->LogError ( luaVM, "fileRename failed; ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), pNewResource->GetName ().c_str () );
            }
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "fileRename" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

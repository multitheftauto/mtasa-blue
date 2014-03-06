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
    CLuaCFunctions::AddFunction ( "fileCopy", CLuaFileDefs::fileCopy );
}


int CLuaFileDefs::fileCreate ( lua_State* luaVM )
{
//  file fileCreate ( string filePath )
    SString strFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFile );

    if ( argStream.NextIsUserData () )
        m_pScriptDebugging->LogCustom ( luaVM, "fileCreate may be using an outdated syntax. Please check and update." );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab the filename
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
                    CScriptFile* pFile = new CScriptFile ( pThisResource->GetScriptID(), strSubPath.c_str (), DEFAULT_MAX_FILESIZE );
                    assert ( pFile );

                    // Try to load it
                    if ( pFile->Load ( pResource, CScriptFile::MODE_CREATE ) )
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
                        m_pScriptDebugging->LogWarning ( luaVM, "%s; unable to load file", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileExists ( lua_State* luaVM )
{
//  bool fileExists ( string filePath )
    SString strFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFile );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            std::string strAbsPath;
            std::string strSubPath;

            // We have a resource argument?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( strFile, pResource, &strAbsPath, &strSubPath ) )
            {
                std::string strFilePath;

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
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileOpen ( lua_State* luaVM )
{
//  file fileOpen ( string filePath [, bool readOnly = false ])
    SString strFile; bool bReadOnly;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFile );
    argStream.ReadBool ( bReadOnly, false );

    if ( argStream.NextIsUserData () )
        m_pScriptDebugging->LogCustom ( luaVM, "fileOpen may be using an outdated syntax. Please check and update." );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Grab the filename
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
                    CScriptFile* pFile = new CScriptFile ( pThisResource->GetScriptID(), strSubPath.c_str (), DEFAULT_MAX_FILESIZE );
                    assert ( pFile );

                    // Try to load it
                    if ( ( bReadOnly && pFile->Load ( pResource, CScriptFile::MODE_READ ) ) ||
                        ( !bReadOnly && pFile->Load ( pResource, CScriptFile::MODE_READWRITE ) ) )
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
                        m_pScriptDebugging->LogWarning ( luaVM, "%s: unable to load file", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileIsEOF ( lua_State* luaVM )
{
//  bool fileIsEOF ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        // Return its EOF state
        lua_pushboolean ( luaVM, pFile->IsEOF () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetPos ( lua_State* luaVM )
{
//  int fileGetPos ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        long lPosition = pFile->GetPointer ();
        if ( lPosition != -1 )
        {
            // Return its position
            lua_pushnumber ( luaVM, lPosition );
            return 1;
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileSetPos ( lua_State* luaVM )
{
//  int fileSetPos ( file theFile, int offset )
    CScriptFile* pFile; long lPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );
    argStream.ReadNumber ( lPosition );

    if ( !argStream.HasErrors () )
    {
        if ( lPosition >= 0 )
        {
            long lResultPosition = pFile->SetPointer ( static_cast < unsigned long > ( lPosition ) );
            if ( lResultPosition != -1 )
            {
                // Set the position and return where we actually got it put
                lua_pushnumber ( luaVM, lResultPosition );
                return 1;
            }
            else
            {
                m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
            }
        }
        else
            m_pScriptDebugging->LogCustom ( luaVM, "Bad file position" );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetSize ( lua_State* luaVM )
{
//  int fileGetSize ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        long lSize = pFile->GetSize ();
        if ( lSize != -1 )
        {
            // Return its size
            lua_pushnumber ( luaVM, lSize );
            return 1;
        }
        else
            argStream.SetCustomError ( "Bad file handle" );
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileRead ( lua_State* luaVM )
{
//  string fileRead ( file theFile, int count )
    CScriptFile* pFile; long lCount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );
    argStream.ReadNumber ( lCount );

    if ( !argStream.HasErrors () )
    {
        if ( lCount > 0 )
        {
            // Allocate a buffer to read the stuff into and read some shit into it
            char* pReadContent = new char [lCount + 1];
            long lBytesRead = pFile->Read ( static_cast < unsigned long > ( lCount ), pReadContent );

            if ( lBytesRead != -1 )
            {
                // Push the string onto the lua stack. Use pushlstring so we are binary
                // compatible. Normal push string takes zero terminated strings.
                lua_pushlstring ( luaVM, pReadContent, lBytesRead );
            }
            else
            {
                m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
                lua_pushnil ( luaVM );
            }

            // Delete our read content. Lua should've stored it
            delete [] pReadContent;

            // We're returning the result string
            return 1;
        }
        else
            argStream.SetCustomError ( "Bad number of bytes" );
    }
    
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileWrite ( lua_State* luaVM )
{
//  int fileWrite ( file theFile, string string1 [, string string2, string string3 ...])
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );
    
    if ( !argStream.NextIsString () )
        argStream.SetTypeError ( "string" );

    if ( !argStream.HasErrors () )
    {
        // While we're not out of string arguments
        long lBytesWritten = 0;
        long lArgBytesWritten = 0;
        
        do
        {
            // Grab argument and length
            SString strData;
            argStream.ReadString ( strData );
            unsigned long ulDataLen = strData.length ();

            // Write it and add the bytes written to our total bytes written
            lArgBytesWritten = pFile->Write ( ulDataLen, strData );
            if ( lArgBytesWritten == -1 )
            {
                m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
                lua_pushnil ( luaVM );
                return 1;
            }
            lBytesWritten += lArgBytesWritten;
        }
        while ( argStream.NextIsString () );
        
        // Return the number of bytes we wrote
        lua_pushnumber ( luaVM, lBytesWritten );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileFlush ( lua_State* luaVM )
{
//  bool fileFlush ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        // Flush the file
        pFile->Flush ();

        // Success. Return true
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileClose ( lua_State* luaVM )
{
//  bool fileClose ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        // Close the file and delete it
        pFile->Unload ();
        m_pElementDeleter->Delete ( pFile );

        // Success. Return true
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFileDefs::fileDelete ( lua_State* luaVM )
{
//  bool fileDelete ( string filePath )
    SString strFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strFile );

    if ( argStream.NextIsUserData () )
        m_pScriptDebugging->LogCustom ( luaVM, "fileDelete may be using an outdated syntax. Please check and update." );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
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
                        m_pScriptDebugging->LogWarning ( luaVM, "%s; unable to delete file", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                    }
                }
                else
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pResource->GetName ().c_str () );
            }
        }    
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFileDefs::fileRename ( lua_State* luaVM )
{
//  bool fileRename ( string filePath, string newFilePath )
    SString strCurFile; SString strNewFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strCurFile );
    argStream.ReadString ( strNewFile );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
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
                            m_pScriptDebugging->LogWarning ( luaVM, "%s; unable to rename/move file", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                        }
                    }
                    else
                    {
                        // Output error
                        m_pScriptDebugging->LogWarning ( luaVM, "%s failed; source file doesn't exist or destination file already exists", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
                    }
                }
                // Do we have not permissions to both - `current` and `new` resources?
                else if ( pThisResource != pCurResource && pThisResource != pNewResource )
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s and %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pCurResource->GetName ().c_str (), pNewResource->GetName ().c_str () );
                // Do we have not permissions to `current` resource?
                else if ( pThisResource != pCurResource && pThisResource == pNewResource )
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pCurResource->GetName ().c_str () );
                // Do we have not permissions to `new` resource?
                else
                    m_pScriptDebugging->LogError ( luaVM, "%s failed; ModifyOtherObjects in ACL denied resource %s to access %s", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), pThisResource->GetName ().c_str (), pNewResource->GetName ().c_str () );
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileCopy ( lua_State* luaVM )
{
//  bool fileCopy ( string filePath, string newFilePath, bool overwrite = false )
    SString filePath; SString newFilePath; bool bOverwrite;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );
    argStream.ReadString ( newFilePath );
    argStream.ReadBool ( bOverwrite, false );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            std::string strCurAbsPath;
            std::string strNewAbsPath;
            std::string strCurMetaPath;
            std::string strNewMetaPath;

            // We have a resource arguments?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pCurResource = pThisResource;
            CResource* pNewResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( filePath, pCurResource, &strCurAbsPath, &strCurMetaPath ) &&
                 CResourceManager::ParseResourcePathInput ( newFilePath, pNewResource, &strNewAbsPath, &strNewMetaPath ) )
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
                    std::string strCurFilePath;     // Same as strCurAbsPath
                    std::string strNewFilePath;     // Same as strNewAbsPath

                     // Does source file exist?
                    if ( pCurResource->GetFilePath ( strCurMetaPath.c_str(), strCurFilePath ) )
                    {
                        // Does destination file exist?
                        if ( !bOverwrite && pNewResource->GetFilePath ( strNewMetaPath.c_str(), strNewFilePath ) )
                        {
                            argStream.SetCustomError ( SString ( "Destination file already exists (%s)", *newFilePath ), "File error" );
                        }
                        else
                        {
                            // Make sure the destination folder exists so we can copy the file
                            MakeSureDirExists ( strNewAbsPath );

                            if ( FileCopy ( strCurAbsPath, strNewAbsPath ) )
                            {
                                // If file copied return success
                                lua_pushboolean ( luaVM, true );
                                return 1;
                            }
                            else
                            {
                                argStream.SetCustomError ( SString ( "Unable to copy %s to %s", *filePath, *newFilePath ), "File error" );
                            }
                        }
                    }
                    else
                    {
                        argStream.SetCustomError ( SString ( "Source file doesn't exist (%s)", *filePath ), "File error" );
                    }
                }
                else
                {
                    // Make permissions error message
                    SString strWho;
                    if ( pThisResource != pCurResource )
                        strWho += pCurResource->GetName ();
                    if ( pThisResource != pNewResource )
                    {
                        if ( !strWho.empty () )
                            strWho += " and ";
                        strWho += pNewResource->GetName ();
                    }
                    argStream.SetCustomError ( SString ( "ModifyOtherObjects in ACL denied resource %s to access %s", pThisResource->GetName ().c_str (), *strWho ), "ACL issue" );
                }
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


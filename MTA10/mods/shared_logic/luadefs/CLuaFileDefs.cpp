/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFileDefs.cpp
*  PURPOSE:     Lua file definitions class
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
    CLuaCFunctions::AddFunction ( "fileCreate", fileCreate );
    CLuaCFunctions::AddFunction ( "fileExists", fileExists );
    CLuaCFunctions::AddFunction ( "fileOpen", fileOpen );
    CLuaCFunctions::AddFunction ( "fileIsEOF", fileIsEOF );
    CLuaCFunctions::AddFunction ( "fileGetPos", fileGetPos );
    CLuaCFunctions::AddFunction ( "fileSetPos", fileSetPos );
    CLuaCFunctions::AddFunction ( "fileGetSize", fileGetSize );
    CLuaCFunctions::AddFunction ( "fileRead", fileRead );
    CLuaCFunctions::AddFunction ( "fileWrite", fileWrite );
    CLuaCFunctions::AddFunction ( "fileFlush", fileFlush );
    CLuaCFunctions::AddFunction ( "fileClose", fileClose );
    CLuaCFunctions::AddFunction ( "fileDelete", fileDelete );
    CLuaCFunctions::AddFunction ( "fileRename", fileRename );
    CLuaCFunctions::AddFunction ( "fileCopy", fileCopy );
    CLuaCFunctions::AddFunction ( "fileGetPath", fileGetPath );
}


void CLuaFileDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classmetamethod ( luaVM, "__gc", fileCloseGC );

    lua_classfunction ( luaVM, "create", "fileOpen" );
    lua_classfunction ( luaVM, "destroy", "fileClose" );
    lua_classfunction ( luaVM, "close", "fileClose" );
    lua_classfunction ( luaVM, "new", "fileCreate" );

    lua_classfunction ( luaVM, "delete", "fileDelete" );
    lua_classfunction ( luaVM, "exists", "fileExists" );
    lua_classfunction ( luaVM, "flush", "fileFlush" );
    lua_classfunction ( luaVM, "getPos", "fileGetPos" );
    lua_classfunction ( luaVM, "getSize", "fileGetSize" );
    lua_classfunction ( luaVM, "isEOF", "fileIsEOF" );
    lua_classfunction ( luaVM, "read", "fileRead" );
    lua_classfunction ( luaVM, "rename", "fileRename" );
    lua_classfunction ( luaVM, "setPos", "fileSetPos" );
    lua_classfunction ( luaVM, "write", "fileWrite" );
    lua_classfunction ( luaVM, "copy", "fileCopy" );
    lua_classfunction ( luaVM, "getPath", "fileGetPath" );

    lua_classvariable ( luaVM, "pos", "fileSetPos", "fileGetPos" );
    lua_classvariable ( luaVM, "size", NULL, "fileGetSize" );
    lua_classvariable ( luaVM, "isEOF", NULL, "fileIsEOF" );
    lua_classvariable ( luaVM, "path", NULL, "fileGetPath" );

    lua_registerclass ( luaVM, "File" );
}

int CLuaFileDefs::fileCreate ( lua_State* luaVM )
{
//  file fileCreate ( string filePath )
    SString filePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( !g_pNet->ValidateBinaryFileName ( filePath ) )
        {
            argStream.SetCustomError ( SString ( "Filename not allowed %s", *filePath ), "File error" );
        }
        else
        if ( pLuaMain )
        {
            SString strAbsPath;
            SString strMetaPath;
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( filePath, pResource, strAbsPath, strMetaPath ) )
            {
                // Inform file verifier
                g_pClientGame->GetResourceManager()->OnFileModifedByScript( strAbsPath, "fileCreate" );

                // Make sure the destination folder exist so we can create the file
                MakeSureDirExists ( strAbsPath.c_str () );

                // Create the file to create
                eAccessType accessType = filePath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                CScriptFile* pFile = new CScriptFile( pThisResource->GetScriptID( ), strMetaPath.c_str(), DEFAULT_MAX_FILESIZE, accessType);
                assert ( pFile );

                // Try to load it
                if ( pFile->Load ( pResource, CScriptFile::MODE_CREATE ) )
                {
                    // Make it a child of the resource's file root
                    pFile->SetParent ( pResource->GetResourceDynamicEntity () );

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
                    argStream.SetCustomError ( SString ( "Unable to create %s", *filePath ), "File error" );
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


int CLuaFileDefs::fileExists ( lua_State* luaVM )
{
//  bool fileExists ( string filePath )
    SString filePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            std::string strAbsPath;
            CResource* pResource = pLuaMain->GetResource ();
            if ( CResourceManager::ParseResourcePathInput ( filePath, pResource, strAbsPath ) )
            {
                bool bResult = FileExists ( strAbsPath );
                lua_pushboolean ( luaVM, bResult );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );


    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileOpen ( lua_State* luaVM )
{
//  file fileOpen ( string filePath [, bool readOnly = false ] )
    SString filePath; bool readOnly;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );
    argStream.ReadBool ( readOnly, false );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            SString strAbsPath;
            SString strMetaPath;
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( filePath, pResource, strAbsPath, strMetaPath ) )
            {
                // Create the file to create
                eAccessType accessType = filePath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                CScriptFile* pFile = new CScriptFile( pThisResource->GetScriptID( ), strMetaPath.c_str( ), DEFAULT_MAX_FILESIZE, accessType );
                assert ( pFile );

                // Try to load it
                if ( pFile->Load ( pResource, readOnly ? CScriptFile::MODE_READ : CScriptFile::MODE_READWRITE ) )
                {
                    // Make it a child of the resource's file root
                    pFile->SetParent ( pResource->GetResourceDynamicEntity () );

                    // Grab its owner resource
                    CResource* pParentResource = pLuaMain->GetResource ();
                    if ( pParentResource )
                    {
                        // Add it to the scrpt resource element group
                        CElementGroup* pGroup = pParentResource->GetElementGroup ();
                        if ( pGroup )
                        {
                            pGroup->Add ( pFile );
                        }
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
                    argStream.SetCustomError( SString( "unable to load file '%s'", *filePath ) );
                }
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFileDefs::fileIsEOF ( lua_State* luaVM )
{
    // bool fileIsEOF ( file )
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetPos ( lua_State* luaVM )
{
    // int fileGetPos ( file )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors ( ) )
    {
        long lPosition = pFile->GetPointer ();
        if ( lPosition != -1 )
        {
            // Return its position
            lua_pushnumber ( luaVM, lPosition );
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
            lua_pushnil ( luaVM );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );
    
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileSetPos ( lua_State* luaVM )
{
    // bool fileSetPos ( file )
    CScriptFile* pFile; unsigned long ulPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );
    argStream.ReadNumber ( ulPosition );

    if ( !argStream.HasErrors ( ) )
    {
        long lResultPosition = pFile->SetPointer ( ulPosition );
        if ( lResultPosition != -1 )
        {
            // Set the position and return where we actually got it put
            lua_pushnumber ( luaVM, lResultPosition );
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
            lua_pushnil ( luaVM );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Error
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileGetSize ( lua_State* luaVM )
{
    // int fileGetSize ( file )

    CScriptFile* pFile;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors ( ) )
    {
        long lSize = pFile->GetSize ();
        if ( lSize != -1 )
        {
            // Return its size
            lua_pushnumber ( luaVM, lSize );
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
            lua_pushnil ( luaVM );
        }
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileRead ( lua_State* luaVM )
{
    // string fileRead ( file, count )
    CScriptFile* pFile; unsigned long ulCount = 0;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );
    argStream.ReadNumber ( ulCount );

    if ( !argStream.HasErrors ( ) )
    {
        if ( ulCount > 0 )
        {
            // Allocate a buffer to read the stuff into and read some shit into it
            CBuffer buffer;
            long lBytesRead = pFile->Read ( ulCount, buffer );

            if ( lBytesRead != -1 )
            {
                // Push the string onto the lua stack. Use pushlstring so we are binary
                // compatible. Normal push string takes zero terminated strings.
                lua_pushlstring ( luaVM, buffer.GetData(), lBytesRead );
            }
            else
            {
                m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
                lua_pushnil ( luaVM );
            }

            // We're returning the result string
            return 1;
        }
        else
        {
            // Reading zero bytes from a file results in an empty string
            lua_pushstring ( luaVM, "" );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileWrite ( lua_State* luaVM )
{
    // string fileWrite ( file, string [, string2, string3, ...] )
    CScriptFile* pFile; SString strMessage;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );
    argStream.ReadString ( strMessage );

    if ( !argStream.HasErrors ( ) )
    {
        // While we're not out of string arguments
        long lBytesWritten = 0;
        long lArgBytesWritten = 0;
        do
        {
            unsigned long ulDataLen = strMessage.length ( );

            // Write it and add the bytes written to our total bytes written
            lArgBytesWritten = pFile->Write ( ulDataLen, strMessage.c_str ( ) );
            if ( lArgBytesWritten == -1 )
            {
                m_pScriptDebugging->LogBadPointer ( luaVM, "file", 1 );
                lua_pushnil ( luaVM );
                return 1;
            }
            lBytesWritten += lArgBytesWritten;

            if ( !argStream.NextIsString ( ) )
                break;

            argStream.ReadString ( strMessage );
        }
        while ( true );

        // Inform file verifier
        if ( lBytesWritten > 0 )
            g_pClientGame->GetResourceManager()->OnFileModifedByScript( pFile->GetAbsPath(), "fileWrite" );

        // Return the number of bytes we wrote
        lua_pushnumber ( luaVM, lBytesWritten );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFileDefs::fileFlush ( lua_State* luaVM )
{
    // string fileFlush ( file )

    // Grab the file pointer
    CScriptFile* pFile;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors ( ) )
    {
        // Flush the file
        pFile->Flush ();

        // Success. Return true
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushnil ( luaVM );
    return 1;
}

int CLuaFileDefs::fileDelete ( lua_State* luaVM )
{
//  bool fileDelete ( string filePath )
    SString filePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            std::string strPath;
            CResource* pResource = pLuaMain->GetResource ();
            if ( CResourceManager::ParseResourcePathInput ( filePath, pResource, strPath ) )
            {
                // Inform file verifier
                g_pClientGame->GetResourceManager()->OnFileModifedByScript( strPath, "fileDelete" );

                if ( FileDelete ( strPath.c_str () ) )
                {
                    // If file removed return success
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
                else
                {
                    // Output error
                    argStream.SetCustomError( SString( "unable to delete file '%s'", *filePath ) );
                }
            }
        }
    }
    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFileDefs::fileRename ( lua_State* luaVM )
{
//  bool fileRename ( string filePath, string newFilePath )
    SString filePath; SString newFilePath;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( filePath );
    argStream.ReadString ( newFilePath );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );

        if ( !g_pNet->ValidateBinaryFileName ( newFilePath ) )
        {
            argStream.SetCustomError ( SString ( "Filename not allowed %s", *newFilePath ), "File error" );
        }
        else
        if ( pLuaMain )
        {
            std::string strCurAbsPath;
            std::string strNewAbsPath;

            // We have a resource arguments?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pCurResource = pThisResource;
            CResource* pNewResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( filePath, pCurResource, strCurAbsPath ) &&
                 CResourceManager::ParseResourcePathInput ( newFilePath, pNewResource, strNewAbsPath ) )
            {
                 // Does source file exist?
                if ( FileExists ( strCurAbsPath.c_str() ) )
                {
                    // Does destination file exist?
                    if ( FileExists ( strNewAbsPath.c_str() ) )
                    {
                        argStream.SetCustomError ( SString ( "Destination file already exists (%s)", *newFilePath ), "File error" );
                    }
                    else
                    {
                        // Inform file verifier
                        g_pClientGame->GetResourceManager()->OnFileModifedByScript( strCurAbsPath, "fileRename" );

                        // Make sure the destination folder exist so we can move the file
                        MakeSureDirExists ( strNewAbsPath.c_str () );

                        if ( FileRename ( strCurAbsPath.c_str (), strNewAbsPath.c_str () ) )
                        {
                            // If file renamed/moved return success
                            lua_pushboolean ( luaVM, true );
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError ( SString ( "Unable to rename/move %s to %s", *filePath, *newFilePath ), "File error" );
                        }
                    }
                }
                else
                {
                    argStream.SetCustomError ( SString ( "Source file doesn't exist (%s)", *filePath ), "File error" );
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

        if ( !g_pNet->ValidateBinaryFileName ( newFilePath ) )
        {
            argStream.SetCustomError ( SString ( "Filename not allowed %s", *newFilePath ), "File error" );
        }
        else
        if ( pLuaMain )
        {
            std::string strCurAbsPath;
            std::string strNewAbsPath;

            // We have a resource arguments?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pCurResource = pThisResource;
            CResource* pNewResource = pThisResource;
            if ( CResourceManager::ParseResourcePathInput ( filePath, pCurResource, strCurAbsPath ) &&
                 CResourceManager::ParseResourcePathInput ( newFilePath, pNewResource, strNewAbsPath ) )
            {
                 // Does source file exist?
                if ( FileExists ( strCurAbsPath ) )
                {
                    // Does destination file exist?
                    if ( !bOverwrite && FileExists ( strNewAbsPath ) )
                    {
                        argStream.SetCustomError ( SString ( "Destination file already exists (%s)", *newFilePath ), "File error" );
                    }
                    else
                    {
                        // Inform file verifier
                        g_pClientGame->GetResourceManager()->OnFileModifedByScript( strNewAbsPath, "fileCopy" );

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
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}



int CLuaFileDefs::fileGetPath ( lua_State* luaVM )
{
    //  string fileGetPath ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // We have a resource argument?
            CResource* pThisResource = pLuaMain->GetResource ();
            CResource* pFileResource = pFile->GetResource ();

            SString strFilePath = pFile->GetFilePath ();

            // If the calling resource is not the resource the file resides in 
            // we need to prepend :resourceName to the path
            if ( pThisResource != pFileResource )
            {
                SString strResourceName = pFileResource->GetName ();
                strFilePath = ":" + strResourceName + "/" + strFilePath;
            }

            lua_pushlstring ( luaVM, strFilePath.c_str (), strFilePath.length () );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFileDefs::fileClose ( lua_State* luaVM )
{
    // string fileClose ( file )
    CScriptFile* pFile;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors ( ) )
    {
        // Close the file and delete it
        pFile->Unload ();
        m_pElementDeleter->Delete ( pFile );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushnil ( luaVM );
    return 1;
}

// Called by Lua when file userdatas are garbage collected
int CLuaFileDefs::fileCloseGC ( lua_State* luaVM )
{
    CScriptFile* pFile;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pFile );

    if ( !argStream.HasErrors () )
    {
        // Close the file and delete it
        pFile->Unload ();
        m_pElementDeleter->Delete ( pFile );

        // This file wasn't closed, so we should warn 
        // the scripter that they forgot to close it.
        m_pScriptDebugging->LogWarning ( luaVM, "Unclosed file (%s) was garbage collected. Check your resource for dereferenced files.", pFile->GetFilePath ().c_str () );
        // TODO: The debug info reported when Lua automatically garbage collects will
        //       actually be the exact point Lua pauses for collection. Find a way to
        //       remove the line number & script file completely.

        lua_pushboolean ( luaVM, true );
        return 1;
    }

    lua_pushnil ( luaVM );
    return 1;
}

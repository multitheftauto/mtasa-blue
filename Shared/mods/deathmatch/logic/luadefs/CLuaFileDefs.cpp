/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/luadefs/CLuaFileDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define DEFAULT_MAX_FILESIZE 52428800

void CLuaFileDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"fileOpen", fileOpen},
        {"fileCreate", fileCreate},
        {"fileExists", fileExists},
        {"fileCopy", fileCopy},
        {"fileRename", fileRename},
        {"fileDelete", fileDelete},

        {"fileClose", fileClose},
        {"fileFlush", fileFlush},
        {"fileRead", fileRead},
        {"fileWrite", fileWrite},

        {"fileGetPos", fileGetPos},
        {"fileGetSize", fileGetSize},
        {"fileGetPath", fileGetPath},
        {"fileIsEOF", fileIsEOF},

        {"fileSetPos", fileSetPos},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaFileDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classmetamethod(luaVM, "__gc", fileCloseGC);

#ifdef MTA_CLIENT
    lua_classfunction(luaVM, "create", CLuaFileDefs::File);
#else
    lua_classfunction(luaVM, "create", "fileCreate", CLuaFileDefs::File);
#endif

    lua_classfunction(luaVM, "open", "fileOpen");
    lua_classfunction(luaVM, "new", "fileCreate");
    lua_classfunction(luaVM, "exists", "fileExists");
    lua_classfunction(luaVM, "copy", "fileCopy");
    lua_classfunction(luaVM, "rename", "fileRename");
    lua_classfunction(luaVM, "delete", "fileDelete");

    lua_classfunction(luaVM, "close", "fileClose");
    lua_classfunction(luaVM, "flush", "fileFlush");
    lua_classfunction(luaVM, "read", "fileRead");
    lua_classfunction(luaVM, "write", "fileWrite");

    lua_classfunction(luaVM, "getPos", "fileGetPos");
    lua_classfunction(luaVM, "getSize", "fileGetSize");
    lua_classfunction(luaVM, "getPath", "fileGetPath");
    lua_classfunction(luaVM, "isEOF", "fileIsEOF");

    lua_classfunction(luaVM, "setPos", "fileSetPos");

    lua_classvariable(luaVM, "pos", "fileSetPos", "fileGetPos");
    lua_classvariable(luaVM, "size", nullptr, "fileGetSize");
    lua_classvariable(luaVM, "eof", nullptr, "fileIsEOF");
    lua_classvariable(luaVM, "path", nullptr, "fileGetPath");

    lua_registerclass(luaVM, "File");
}

int CLuaFileDefs::File(lua_State* luaVM)
{
    SString strInputPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);

        if (pLuaMain)
        {
            SString    strAbsPath;
            SString    strMetaPath;
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pResource = pThisResource;

            if (CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath, &strMetaPath))
            {
                CheckCanModifyOtherResource(argStream, pResource, pResource);
                CheckCanAccessOtherResourceFile(argStream, pResource, pResource, strAbsPath);

                if (!argStream.HasErrors())
                {
                    CScriptFile::eMode eFileMode = CScriptFile::MODE_READWRITE;

                    if (!FileExists(strAbsPath))
                    {
                        eFileMode = CScriptFile::MODE_CREATE;

#ifdef MTA_CLIENT
                        if (!g_pNet->ValidateBinaryFileName(strInputPath))
                        {
                            argStream.SetCustomError(SString("Filename not allowed %s", *strInputPath), "File error");
                            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
                            lua_pushboolean(luaVM, false);
                            return 1;
                        }

                        // Inform file verifier
                        g_pClientGame->GetResourceManager()->OnFileModifedByScript(strAbsPath, "File");
#endif

                        // Make sure the destination folder exist so we can create the file
                        MakeSureDirExists(strAbsPath);
                    }

                    // Create the file
#ifdef MTA_CLIENT
                    eAccessType  accessType = strInputPath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                    CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE, accessType);
#else
                    CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE);
#endif

                    // Try to load it
                    if (pFile->Load(pResource, eFileMode))
                    {
#ifdef MTA_CLIENT
                        // Make it a child of the resource's file root
                        pFile->SetParent(pResource->GetResourceDynamicEntity());
#endif
                        // Add it to the script resource element group
                        CElementGroup* pGroup = pThisResource->GetElementGroup();

                        if (pGroup)
                        {
                            pGroup->Add(pFile);
                        }

                        // Success. Return the file.
                        lua_pushelement(luaVM, pFile);
                        return 1;
                    }
                    else
                    {
                        // Delete the file again
                        delete pFile;

                        // Output error
                        argStream.SetCustomError(SString("unable to load file '%s'", *strInputPath));
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileOpen(lua_State* luaVM)
{
    //  file fileOpen ( string filePath [, bool readOnly = false ] )
    SString strInputPath;
    bool    bReadOnly;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);
    argStream.ReadBool(bReadOnly, false);

    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "fileOpen may be using an outdated syntax. Please check and update.");

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            SString    strAbsPath;
            SString    strMetaPath;
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pResource = pThisResource;
            if (CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath, &strMetaPath))
            {
                CheckCanModifyOtherResource(argStream, pThisResource, pResource);
                CheckCanAccessOtherResourceFile(argStream, pThisResource, pResource, strAbsPath, &bReadOnly);
                if (!argStream.HasErrors())
                {
#ifndef MTA_CLIENT // IF SERVER
                    // Create the file to create
                    CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE);
#else
                    eAccessType  accessType = strInputPath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                    CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE, accessType);
#endif
                    // Try to load it
                    if (pFile->Load(pResource, bReadOnly ? CScriptFile::MODE_READ : CScriptFile::MODE_READWRITE))
                    {
#ifdef MTA_CLIENT
                        // Make it a child of the resource's file root
                        pFile->SetParent(pResource->GetResourceDynamicEntity());
                        pFile->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#else
                        pFile->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#endif
                        // Grab its owner resource
                        CResource* pParentResource = pLuaMain->GetResource();
                        if (pParentResource)
                        {
                            // Add it to the scrpt resource element group
                            CElementGroup* pGroup = pParentResource->GetElementGroup();
                            if (pGroup)
                            {
                                pGroup->Add(pFile);
                            }
                        }

                        // Success. Return the file.
                        lua_pushelement(luaVM, pFile);
                        return 1;
                    }
                    else
                    {
                        // Delete the file again
                        delete pFile;

                        // Output error
                        argStream.SetCustomError(SString("unable to load file '%s'", *strInputPath));
                    }
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileCreate(lua_State* luaVM)
{
    //  file fileCreate ( string filePath )
    SString strInputPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);

    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "fileCreate may be using an outdated syntax. Please check and update.");

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (!pLuaMain)
        {
            // Failed
            lua_pushboolean(luaVM, false);
            return 1;
        }

#ifdef MTA_CLIENT
        if (!g_pNet->ValidateBinaryFileName(strInputPath))
        {
            argStream.SetCustomError(SString("Filename not allowed %s", *strInputPath), "File error");
            m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

            // Failed
            lua_pushboolean(luaVM, false);
            return 1;
        }
#endif // MTA_CLIENT

        SString    strAbsPath;
        SString    strMetaPath;
        CResource* pThisResource = pLuaMain->GetResource();
        CResource* pResource = pThisResource;
        if (CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath, &strMetaPath))
        {
            CheckCanModifyOtherResource(argStream, pThisResource, pResource);
            CheckCanAccessOtherResourceFile(argStream, pThisResource, pResource, strAbsPath);
            if (!argStream.HasErrors())
            {
#ifdef MTA_CLIENT
                // Inform file verifier
                g_pClientGame->GetResourceManager()->OnFileModifedByScript(strAbsPath, "fileCreate");
#endif

                // Make sure the destination folder exist so we can create the file
                MakeSureDirExists(strAbsPath);

                // Create the file to create
#ifdef MTA_CLIENT
                eAccessType  accessType = strInputPath[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE, accessType);
#else
                CScriptFile* pFile = new CScriptFile(pThisResource->GetScriptID(), strMetaPath, DEFAULT_MAX_FILESIZE);
#endif

                // Try to load it
                if (pFile->Load(pResource, CScriptFile::MODE_CREATE))
                {
#ifdef MTA_CLIENT
                    // Make it a child of the resource's file root
                    pFile->SetParent(pResource->GetResourceDynamicEntity());
                    pFile->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#else
                    pFile->SetLuaDebugInfo(g_pGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
#endif

                    // Add it to the scrpt resource element group
                    CElementGroup* pGroup = pThisResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pFile);
                    }

                    // Success. Return the file.
                    lua_pushelement(luaVM, pFile);
                    return 1;
                }
                else
                {
                    // Delete the file again
                    delete pFile;

                    // Output error
                    argStream.SetCustomError(SString("Unable to create %s", *strInputPath), "File error");
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileExists(lua_State* luaVM)
{
    //  bool fileExists ( string filePath )
    SString strInputPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            SString strAbsPath;

            CResource* pResource = pLuaMain->GetResource();
            if (CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath))
            {
                SString strFilePath;

                // Does file exist?
                bool bResult = FileExists(strAbsPath);
                lua_pushboolean(luaVM, bResult);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileCopy(lua_State* luaVM)
{
    //  bool fileCopy ( string filePath, string newFilePath, bool overwrite = false )
    SString strInputSrcPath;
    SString strInputDestPath;
    bool    bOverwrite;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputSrcPath);
    argStream.ReadString(strInputDestPath);
    argStream.ReadBool(bOverwrite, false);

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (!pLuaMain)
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }

#ifdef MTA_CLIENT
        if (!g_pNet->ValidateBinaryFileName(strInputDestPath))
        {
            argStream.SetCustomError(SString("Filename not allowed %s", *strInputDestPath), "File error");
            m_pScriptDebugging->LogError(luaVM, argStream.GetFullErrorMessage());

            lua_pushboolean(luaVM, false);
            return 1;
        }
#endif

        // absPath: the real absolute path to the file
        // metaPath: path relative to the target resource (as would be defined in the meta.xml file)
        SString strSrcAbsPath;
        SString strDestAbsPath;

        CResource* pThisResource = pLuaMain->GetResource();
        CResource* pSrcResource = pThisResource;
        CResource* pDestResource = pThisResource;

        if (CResourceManager::ParseResourcePathInput(strInputSrcPath, pSrcResource, &strSrcAbsPath) &&
            CResourceManager::ParseResourcePathInput(strInputDestPath, pDestResource, &strDestAbsPath))
        {
            CheckCanModifyOtherResources(argStream, pThisResource, { pSrcResource, pDestResource });
            CheckCanAccessOtherResourceFile(argStream, pThisResource, pSrcResource, strSrcAbsPath);
            CheckCanAccessOtherResourceFile(argStream, pThisResource, pDestResource, strDestAbsPath);
            if (!argStream.HasErrors())
            {
                // Does `current` file path exist and `new` file path doesn't exist?
                if (FileExists(strSrcAbsPath))
                {
                    // If we can overwrite, we continue. Otherwise, we have to make sure the destination file does not exist.
                    if (bOverwrite || !FileExists(strDestAbsPath))
                    {
#ifdef MTA_CLIENT
                        // Inform file verifier
                        g_pClientGame->GetResourceManager()->OnFileModifedByScript(strDestAbsPath, "fileCopy");
#endif
                        // Make sure the destination folder exists so we can move the file
                        MakeSureDirExists(strDestAbsPath);

                        if (FileCopy(strSrcAbsPath, strDestAbsPath))
                        {
                            // If file renamed/moved return success
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }

                        // Output error
                        argStream.SetCustomError(SString("Unable to copy %s to %s", *strInputSrcPath, *strInputDestPath), "Operation failed");
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Destination file already exists (%s)", *strInputDestPath), "Operation failed");
                    }
                }
                else
                {
                    argStream.SetCustomError(SString("Source file doesn't exist (%s)", *strInputSrcPath), "Operation failed");
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileRename(lua_State* luaVM)
{
    //  bool fileRename ( string filePath, string newFilePath )
    SString strInputSrcPath;
    SString strInputDestPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputSrcPath);
    argStream.ReadString(strInputDestPath);

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (!pLuaMain)
        {
            lua_pushboolean(luaVM, false);
            return 1;
        }

#ifdef MTA_CLIENT
        if (!g_pNet->ValidateBinaryFileName(strInputDestPath))
        {
            argStream.SetCustomError(SString("Filename not allowed %s", *strInputDestPath), "File error");
            m_pScriptDebugging->LogError(luaVM, argStream.GetFullErrorMessage());

            lua_pushboolean(luaVM, false);
            return 1;
        }
#endif

        // absPath: the real absolute path to the file
        // metaPath: path relative to the target resource (as would be defined in the meta.xml file)
        SString strSrcAbsPath;
        SString strDestAbsPath;

        CResource* pThisResource = pLuaMain->GetResource();
        CResource* pSrcResource = pThisResource;
        CResource* pDestResource = pThisResource;

        if (CResourceManager::ParseResourcePathInput(strInputSrcPath, pSrcResource, &strSrcAbsPath) &&
            CResourceManager::ParseResourcePathInput(strInputDestPath, pDestResource, &strDestAbsPath))
        {
            CheckCanModifyOtherResources(argStream, pThisResource, { pSrcResource, pDestResource });
            CheckCanAccessOtherResourceFile(argStream, pThisResource, pSrcResource, strSrcAbsPath);
            CheckCanAccessOtherResourceFile(argStream, pThisResource, pDestResource, strDestAbsPath);
            if (!argStream.HasErrors())
            {
                // Does `current` file path exist and `new` file path doesn't exist?
                if (FileExists(strSrcAbsPath))
                {
                    if (!FileExists(strDestAbsPath))
                    {
#ifdef MTA_CLIENT
                        // Inform file verifier
                        g_pClientGame->GetResourceManager()->OnFileModifedByScript(strSrcAbsPath, "fileRename");
#endif
                        // Make sure the destination folder exists so we can move the file
                        MakeSureDirExists(strDestAbsPath);

                        int errorCode;
                        if (FileRename(strSrcAbsPath, strDestAbsPath, &errorCode))
                        {
                            // If file renamed/moved return success
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }

                        // Output error
                        m_pScriptDebugging->LogWarning(luaVM, SString("fileRename failed; unable to rename file (Error %d)", errorCode));
                    }
                    else
                    {
                        m_pScriptDebugging->LogWarning(luaVM, "fileRename failed; destination file already exists");
                    }
                }
                else
                {
                    m_pScriptDebugging->LogWarning(luaVM, "fileRename failed; source file doesn't exist");
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileDelete(lua_State* luaVM)
{
    //  bool fileDelete ( string filePath )
    SString strInputPath;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strInputPath);

    // This is only really necessary server side
    if (argStream.NextIsUserData())
        m_pScriptDebugging->LogCustom(luaVM, "fileDelete may be using an outdated syntax. Please check and update.");

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            SString    strAbsPath;
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pResource = pThisResource;
            if (CResourceManager::ParseResourcePathInput(strInputPath, pResource, &strAbsPath))
            {
                CheckCanModifyOtherResource(argStream, pThisResource, pResource);
                CheckCanAccessOtherResourceFile(argStream, pThisResource, pResource, strAbsPath);
                if (!argStream.HasErrors())
                {
#ifdef MTA_CLIENT
                    // Inform file verifier
                    g_pClientGame->GetResourceManager()->OnFileModifedByScript(strAbsPath, "fileDelete");
#endif
                    if (FileDelete(strAbsPath))
                    {
                        // If file removed successfully, return true
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }

                    // Output error "Operation failed @ 'fileDelete' [strInputPath]"
                    argStream.SetCustomError(strInputPath, "Operation failed");
                }
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileClose(lua_State* luaVM)
{
    //  bool fileClose ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        // Close the file and delete it
        pFile->Unload();
        m_pElementDeleter->Delete(pFile);

        // Success. Return true
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileFlush(lua_State* luaVM)
{
    //  bool fileFlush ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        // Flush the file
        pFile->Flush();

        // Success. Return true
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileRead(lua_State* luaVM)
{
    //  string fileRead ( file theFile, int count )
    CScriptFile*  pFile;
    unsigned long ulCount = 0;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);
    argStream.ReadNumber(ulCount);

    if (!argStream.HasErrors())
    {
        // Reading zero bytes from a file results in an empty string
        if (ulCount == 0)
        {
            lua_pushstring(luaVM, "");
            return 1;
        }

        // Allocate a buffer to read the stuff into and read some :~ into it
        SString buffer;
        long lBytesRead = pFile->Read(ulCount, buffer);

        if (lBytesRead >= 0)
        {
            // Push the string onto the Lua stack. Use pushlstring so we are binary
            // compatible. Normal push string takes zero terminated strings.
            lua_pushlstring(luaVM, buffer.data(), lBytesRead);
            return 1;
        }
        else if (lBytesRead == -2)
        {
            m_pScriptDebugging->LogWarning(luaVM, "out of memory");
        }
        else
        {
            m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileWrite(lua_State* luaVM)
{
    //  int fileWrite ( file theFile, string string1 [, string string2, string string3 ...])
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    // Ensure we have at least one string
    if (!argStream.NextIsString())
        argStream.SetTypeError("string");

    if (!argStream.HasErrors())
    {
        long lBytesWritten = 0;            // Total bytes written

        // While we're not out of string arguments
        // (we will always have at least one string because we validated it above)
        while (argStream.NextIsString())
        {
            // Grab argument and length
            SCharStringRef strData;
            argStream.ReadCharStringRef(strData);

            if (argStream.HasErrors() || !strData.pData)
                continue;

            // Write the data
            long lArgBytesWritten = pFile->Write(strData.uiSize, strData.pData);

            // Did the file mysteriously disappear?
            if (lArgBytesWritten == -1)
            {
                m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
                lua_pushnil(luaVM);
                return 1;
            }

            // Add the number of bytes written to our counter
            lBytesWritten += lArgBytesWritten;
        }

#ifdef MTA_CLIENT
        // Inform file verifier
        if (lBytesWritten != 0)
            g_pClientGame->GetResourceManager()->OnFileModifedByScript(pFile->GetAbsPath(), "fileWrite");
#endif

        // Return the number of bytes we wrote
        lua_pushnumber(luaVM, lBytesWritten);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileGetPos(lua_State* luaVM)
{
    //  int fileGetPos ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        long lPosition = pFile->GetPointer();
        if (lPosition != -1)
        {
            // Return its position
            lua_pushnumber(luaVM, lPosition);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileGetSize(lua_State* luaVM)
{
    //  int fileGetSize ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        long lSize = pFile->GetSize();
        if (lSize != -1)
        {
            // Return its size
            lua_pushnumber(luaVM, lSize);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileGetPath(lua_State* luaVM)
{
    //  string fileGetPath ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        // Grab our lua VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pThisResource = pLuaMain->GetResource();
            CResource* pFileResource = pFile->GetResource();

            SString strFilePath = pFile->GetFilePath();

            // If the calling resource is not the resource the file resides in
            // we need to prepend :resourceName to the path
            if (pThisResource != pFileResource)
            {
#ifdef MTA_CLIENT
                strFilePath = SString(":%s/%s", pFileResource->GetName(), *strFilePath);
#else
                strFilePath = SString(":%s/%s", *pFileResource->GetName(), *strFilePath);
#endif
            }

            lua_pushlstring(luaVM, strFilePath, strFilePath.length());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFileDefs::fileIsEOF(lua_State* luaVM)
{
    //  bool fileIsEOF ( file theFile )
    CScriptFile* pFile;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        // Return its EOF state
        lua_pushboolean(luaVM, pFile->IsEOF());
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

int CLuaFileDefs::fileSetPos(lua_State* luaVM)
{
    //  int fileSetPos ( file theFile, int offset )
    CScriptFile*  pFile;
    unsigned long ulPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);
    argStream.ReadNumber(ulPosition);

    if (!argStream.HasErrors())
    {
        long lResultPosition = pFile->SetPointer(ulPosition);
        if (lResultPosition != -1)
        {
            // Set the position and return where we actually got it put
            lua_pushnumber(luaVM, lResultPosition);
            return 1;
        }

        m_pScriptDebugging->LogBadPointer(luaVM, "file", 1);
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Error
    lua_pushnil(luaVM);
    return 1;
}

// Called by Lua when file userdatas are garbage collected
int CLuaFileDefs::fileCloseGC(lua_State* luaVM)
{
    CScriptFile*     pFile;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pFile);

    if (!argStream.HasErrors())
    {
        // This file wasn't closed, so we should warn
        // the scripter that they forgot to close it.
        m_pScriptDebugging->LogWarning(pFile->GetLuaDebugInfo(), "Unclosed file (%s) was garbage collected. Check your resource for dereferenced files.", *pFile->GetFilePath());

        // Close the file and delete it from elements
        pFile->Unload();
        m_pElementDeleter->Delete(pFile);

        lua_pushboolean(luaVM, true);
        return 1;
    }

    lua_pushnil(luaVM);
    return 1;
}

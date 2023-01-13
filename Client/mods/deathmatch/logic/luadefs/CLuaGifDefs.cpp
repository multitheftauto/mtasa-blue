/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"
#include "CLuaGifDefs.h"
#include "CScriptFile.h"

#ifndef DEFAULT_MAX_FILESIZE
    #define DEFAULT_MAX_FILESIZE 52428800
#endif

void CLuaGifDefs::LoadFunctions(){
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"gifCreate", GifCreate},
        {"gifPlay", GifPlay},
        {"gifStop", GifStop},
        {"gifNavigateToThumbnail", GifNavigateToThumbnail},
        {"isGifPlaying", IsGifPlaying},
    };
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaGifDefs::AddClass(lua_State* luaVM){
    lua_newclass(luaVM);
    lua_classfunction(luaVM, "play", "gifPlay");
    lua_classfunction(luaVM, "stop", "gifStop");
    lua_classfunction(luaVM, "navigateToThumbnail", "gifNavigateToThumbnail");
    lua_classfunction(luaVM, "isPlaying", "isGifPlaying");
    lua_registerclass(luaVM, "Gif");
}

int CLuaGifDefs::GifCreate(lua_State* luaVM) {
    SString buffer;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(buffer);
    if (!argStream.HasErrors()) {
        CLuaMain& luaMain = lua_getownercluamain(luaVM);
        CResource* resource = luaMain.GetResource();
        SString path;
        SString metaPath;
        SString fileBuffer;
        SString lastPath = buffer;
        SString format = buffer.substr(0,6);
        CClientGifLoader loader;
        if (format == "GIF87a" || format == "GIF89a") {
            loader = CClientGifLoader(buffer);
        }else if (CResourceManager::ParseResourcePathInput(buffer,resource,&path,&metaPath)){
            if (FileExists(path)) {
                eAccessType  access = buffer[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
                CScriptFile* file = new CScriptFile(resource->GetScriptID(), metaPath, DEFAULT_MAX_FILESIZE,access);
                if (file->Load(resource, CScriptFile::MODE_READ)) {
                    file->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
                    long size = file->GetSize();
                    long bytesRead = file->Read(size,fileBuffer);
                    if (bytesRead >= 0){
                        loader = CClientGifLoader(fileBuffer);
                    }else if (bytesRead == -2){ 
                        m_pScriptDebugging->LogWarning(luaVM, "out of memory");
                    }else{
                        m_pScriptDebugging->LogError(luaVM, SString("error while reading file [%s]", lastPath.c_str()).c_str());
                    }
                    file->Unload();
                    m_pElementDeleter->Delete(file);
                }else{
                    delete file;
                    m_pScriptDebugging->LogError(luaVM, SString("couldn't load file [%s]",lastPath.c_str()).c_str());
                    lua_pushboolean(luaVM, false);
                    return 1;
                }
            }else{
                m_pScriptDebugging->LogError(luaVM, SString("file [%s] doesn't exists!",lastPath.c_str()).c_str());
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }else{
            loader = CClientGifLoader(buffer);
        }
        std::vector<std::vector<unsigned char>> frames;
        std::vector<int> delays;
        loader.Load(frames, delays, 0L);
        if (!loader) {
            m_pScriptDebugging->LogError(luaVM, "wrong file format");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        if (frames.size() < 1) {
            m_pScriptDebugging->LogError(luaVM, "gif has no frames");
            lua_pushboolean(luaVM,false);
            return 1;
        }
        uint width = (uint)loader.GetWidth();
        uint height = (uint)loader.GetHeight();
        if (width < 1 || height < 1) {
            m_pScriptDebugging->LogError(luaVM, "gif must be 1x1 at least");
            lua_pushboolean(luaVM,false);
            return 1;
        }
        CClientGif* gif = g_pClientGame->GetManager()->GetRenderElementManager()->CreateGif(width, height);
        if (!gif) {
            m_pScriptDebugging->LogError(luaVM, "error while creating gif");
            lua_pushboolean(luaVM, false);
            return 1;
        }
        gif->SetParent(resource->GetResourceDynamicEntity());
        gif->SetResource(resource);
        gif->SetFrameCount(frames.size());
        gif->Register(std::move(frames),std::move(delays));
        gif->SetFormat(loader.GetFormat());
        lua_pushelement(luaVM, gif);
        return 1;
    }else{
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM,false);
    return 1;
}

int CLuaGifDefs::GifPlay(lua_State* luaVM) {
    CClientGif* gif = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(gif);
    if (!gif) {
        m_pScriptDebugging->LogCustom(luaVM,argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM, false);
        return 1;
    }
    if (!argStream.HasErrors()) {
        gif->Play();
        lua_pushboolean(luaVM,true);
        return 1;
    }else{
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM,false);
    return 1;
}

int CLuaGifDefs::GifStop(lua_State* luaVM) {
    CClientGif* gif = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(gif);
    if (!gif) {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM,false);
        return 1;
    }
    if (!argStream.HasErrors()) {
        gif->Stop();
        lua_pushboolean(luaVM, true);
        return 1;
    }else{
        m_pScriptDebugging->LogCustom(luaVM,argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM,false);
    return 1;
}

int CLuaGifDefs::GifNavigateToThumbnail(lua_State* luaVM) {
    CClientGif* gif = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(gif);
    if (!gif) {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM,false);
        return 1;
    }
    if (!argStream.HasErrors()) {
        gif->NavigateToThumbnail();
        lua_pushboolean(luaVM, true);
        return 1;
    }else{
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM,false);
    return 1;
}

int CLuaGifDefs::IsGifPlaying(lua_State* luaVM) {
    CClientGif* gif = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(gif);
    if (!gif) {
        m_pScriptDebugging->LogCustom(luaVM,argStream.GetFullErrorMessage());
        lua_pushboolean(luaVM,false);
        return 1;
    }
    if (!argStream.HasErrors()) {
        lua_pushboolean(luaVM, gif->IsPlaying());
        return 1;
    }else{
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }
    lua_pushboolean(luaVM,false);
    return 1;
}

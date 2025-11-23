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

void CLuaGifDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"gifCreate", ArgumentParser<GifCreate>},
        {"gifPlay", ArgumentParser<GifPlay>},
        {"gifStop", ArgumentParser<GifStop>},
        {"gifSetProperty", ArgumentParser<GifSetProperty>},
        {"gifGetProperty", ArgumentParser<GifGetProperty>},
        {"gifIsPlaying", ArgumentParser<GifIsPlaying>},
    };
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaGifDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);
    lua_classfunction(luaVM, "play", "gifPlay");
    lua_classfunction(luaVM, "stop", "gifStop");
    lua_classfunction(luaVM, "setProperty", "gifSetProperty");
    lua_classfunction(luaVM, "getProperty", "gifGetProperty");
    lua_classfunction(luaVM, "isPlaying", "GifIsPlaying");
    lua_registerclass(luaVM, "Gif");
}

CClientGif* CLuaGifDefs::GifCreate(lua_State* luaVM, std::string pathOrRawdata)
{
    SString          buffer = pathOrRawdata;
    CLuaMain&        luaMain = lua_getownercluamain(luaVM);
    CResource*       resource = luaMain.GetResource();
    SString          path;
    SString          metaPath;
    SString          fileBuffer;
    SString          lastPath = buffer;
    SString          format = buffer.substr(0, 6);
    CClientGifLoader loader;
    if (format == "GIF87a" || format == "GIF89a")
    {
        loader = CClientGifLoader(buffer);
    }
    else if (CResourceManager::ParseResourcePathInput(buffer, resource, &path, &metaPath))
    {
        if (FileExists(path))
        {
            eAccessType  access = buffer[0] == '@' ? eAccessType::ACCESS_PRIVATE : eAccessType::ACCESS_PUBLIC;
            CScriptFile* file = new CScriptFile(resource->GetScriptID(), metaPath, DEFAULT_MAX_FILESIZE, access);
            if (file->Load(resource, CScriptFile::MODE_READ))
            {
                file->SetLuaDebugInfo(g_pClientGame->GetScriptDebugging()->GetLuaDebugInfo(luaVM));
                long size = file->GetSize();
                long bytesRead = file->Read(size, fileBuffer);
                if (bytesRead >= 0)
                {
                    loader = CClientGifLoader(fileBuffer);
                }
                else if (bytesRead == -2)
                {
                    m_pScriptDebugging->LogWarning(luaVM, "out of memory");
                }
                else
                {
                    m_pScriptDebugging->LogError(luaVM, SString("error while reading file [%s]", lastPath.c_str()).c_str());
                }
                file->Unload();
                m_pElementDeleter->Delete(file);
            }
            else
            {
                delete file;
                m_pScriptDebugging->LogError(luaVM, SString("couldn't load file [%s]", lastPath.c_str()).c_str());
                return nullptr;
            }
        }
        else
        {
            m_pScriptDebugging->LogError(luaVM, SString("file [%s] doesn't exists!", lastPath.c_str()).c_str());
            return nullptr;
        }
    }
    else
    {
        loader = CClientGifLoader(buffer);
    }
    std::vector<std::vector<unsigned char>> frames;
    std::vector<int>                        delays;
    loader.Load(frames, delays, 0L);
    if (!loader)
    {
        m_pScriptDebugging->LogError(luaVM, "wrong file format");
        return nullptr;
    }
    if (frames.size() < 1)
    {
        m_pScriptDebugging->LogError(luaVM, "gif has no frames");
        return nullptr;
    }
    uint width = (uint)loader.GetWidth();
    uint height = (uint)loader.GetHeight();
    if (width < 1 || height < 1)
    {
        m_pScriptDebugging->LogError(luaVM, "gif must be 1x1 at least");
        return nullptr;
    }
    CClientGif* gif = g_pClientGame->GetManager()->GetRenderElementManager()->CreateGif(width, height);
    if (!gif)
    {
        m_pScriptDebugging->LogError(luaVM, "error while creating gif");
        return nullptr;
    }
    gif->SetParent(resource->GetResourceDynamicEntity());
    gif->SetResource(resource);
    gif->SetFrameCount(frames.size());
    gif->Register(std::move(frames), std::move(delays));
    gif->SetFormat(loader.GetFormat());
    return gif;
}

bool CLuaGifDefs::GifPlay(CClientGif* gif)
{
    gif->Play();
    return true;
}

bool CLuaGifDefs::GifStop(CClientGif* gif)
{
    gif->Stop();
    return true;
}

bool CLuaGifDefs::GifSetProperty(lua_State* luaVM, CClientGif* gif, std::string property, std::variant<int, std::string> value, std::optional<int> frame)
{
    bool valIsNumber = false;
    int  numberValue = -1;
    if (std::holds_alternative<int>(value))
    {
        valIsNumber = true;
        numberValue = std::get<int>(value);
        if (numberValue < 0)
        {
            m_pScriptDebugging->LogError(luaVM, "property value can't be a negative number");
            return false;
        }
    }
    if (property.empty())
    {
        m_pScriptDebugging->LogError(luaVM, "property can't be empty");
        return false;
    }
    if (frame.has_value())
    {
        int frameCount = gif->GetFrameCount();
        if (frame > frameCount)
        {
            m_pScriptDebugging->LogError(luaVM, "frame doesn't exist");
            return false;
        }
        if (property == "delay")
        {
            if (!valIsNumber)
            {
                m_pScriptDebugging->LogError(luaVM, "property `delay` must be a number value");
                return false;
            }
            gif->SetFrameDelay(frame.value(), numberValue);
            return true;
        }
        else
        {
            m_pScriptDebugging->LogError(luaVM, "property doesn't exist for frame");
        }
    }
    else
    {
        if (property == "showing_frame")
        {
            if (!valIsNumber)
            {
                m_pScriptDebugging->LogError(luaVM, "property `showing_frame` must be a number value");
                return false;
            }
            gif->SetFrame(numberValue);
            return true;
        }
        else
        {
            m_pScriptDebugging->LogError(luaVM, "property doens't exist for gif");
        }
    }
    return false;
}

std::variant<bool, std::string, int> CLuaGifDefs::GifGetProperty(lua_State* luaVM, CClientGif* gif, std::string property, std::optional<int> frame)
{
    if (property.empty())
    {
        m_pScriptDebugging->LogError(luaVM, "property can't be empty");
        return false;
    }
    if (frame.has_value())
    {
        int frameCount = gif->GetFrameCount();
        if (frame > frameCount)
        {
            m_pScriptDebugging->LogError(luaVM, "frame doesn't exist");
            return false;
        }
        if (property == "delay")
        {
            return (int)gif->GetFrameDelay(frame.value());
        }
        else if (property == "default_delay")
        {
            return (int)gif->GetFrameDefaultDelay(frame.value());
        }
        else
        {
            m_pScriptDebugging->LogError(luaVM, "property doesn't exist for frame");
        }
    }
    else
    {
        if (property == "showing_frame")
        {
            return (int)gif->GetShowingFrame();
        }
        else if (property == "frame_count")
        {
            return (int)gif->GetFrameCount();
        }
        else if (property == "format")
        {
            return (std::string)gif->GetFormat();
        }
        else if (property == "tick")
        {
            return (int)gif->GetTick();
        }
        else
        {
            m_pScriptDebugging->LogError(luaVM, "property doesn't exist for gif");
        }
    }
    return false;
}

bool CLuaGifDefs::GifIsPlaying(CClientGif* gif)
{
    return gif->IsPlaying();
}

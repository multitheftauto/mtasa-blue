/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CFBXInterface.h
 *  PURPOSE:     FBX interface class
 *
 *****************************************************************************/

#pragma once

class CFBXScene;
class SString;
namespace ofbx
{
    class IScene;
    class Mesh;
    class Object;
}            // namespace ofbx

class CFBXInterface
{
public:
    virtual CFBXScene* AddScene(ofbx::IScene* pScene) = 0;
    virtual void       Render() = 0;
};

class CFBXSceneInterface
{
public:
    virtual bool IsMeshValid(const SString& strHierarchyMesh) = 0;
    virtual bool IsObjectValid(long long int ulId) = 0;
    virtual void GetAllObjectsIds(std::vector<unsigned long long>& vecIds) = 0;

    virtual const ofbx::Mesh const*    GetMeshByName(const SString& strHierarchyMesh) = 0;
    virtual const ofbx::Object* const* GetObjectById(long long int ulId) = 0;
};

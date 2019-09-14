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
class CClientFBXInterface;
class CFBXLoading;
enum eCullMode;
class D3DXCOLOR;

namespace ofbx
{
    class IScene;
    class Mesh;
    class Object;
    class Vec3;
}            // namespace ofbx

class CFBXInterface
{
public:
    virtual CFBXScene*  AddScene(ofbx::IScene* pScene, CClientFBXInterface* pInterface) = 0;
    virtual void        RemoveScene(CFBXScene* pScene) = 0;
    virtual void        Render() = 0;
    virtual void        Initialize() = 0;
    virtual bool        HasAnyFBXLoaded() = 0;
    virtual const char* GetObjectType(const ofbx::Object const* pObject) = 0;
    virtual D3DLIGHT9*  GetGlobalLight() = 0;
    virtual D3DXCOLOR*  GetGlobalAmbient() = 0;
    virtual float       GetGlobalLighting() = 0;
};

class CFBXSceneInterface
{
public:
    virtual bool IsObjectValid(unsigned long long ullId) = 0;
    virtual bool IsTemplateValid(unsigned int uiId) = 0;
    virtual bool IsTemplateModelValid(unsigned int uiTemplate, unsigned int uiModel) = 0;
    virtual void GetAllObjectsIds(std::vector<unsigned long long>& vecIds) = 0;
    virtual void GetAllTemplatesIds(std::vector<unsigned int>& vecIds) = 0;
    virtual bool GetAllTemplatesModelsIds(std::vector<unsigned int>& vecIds, unsigned int uiTemplateId) = 0;

    virtual void GetTemplatePosition(unsigned int uiTemplateId, CVector& position) = 0;
    virtual void GetTemplateRotation(unsigned int uiTemplateId, CVector& rotation) = 0;
    virtual void GetTemplateScale(unsigned int uiTemplateId, CVector& scale) = 0;
    virtual void SetTemplatePosition(unsigned int uiTemplateId, CVector& position) = 0;
    virtual void SetTemplateRotation(unsigned int uiTemplateId, CVector& rotation) = 0;
    virtual void SetTemplateScale(unsigned int uiTemplateId, CVector& scale) = 0;
    virtual void GetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale) = 0;
    virtual void GetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position) = 0;
    virtual void GetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation) = 0;
    virtual void SetTemplateModelScale(unsigned int uiTemplateId, unsigned int uiModelId, CVector& scale) = 0;
    virtual void SetTemplateModelPosition(unsigned int uiTemplateId, unsigned int uiModelId, CVector& position) = 0;
    virtual void SetTemplateModelRotation(unsigned int uiTemplateId, unsigned int uiModelId, CVector& rotation) = 0;
    virtual void GetTemplateDrawDistance(unsigned int uiTemplateId, float& drawDistance) = 0;
    virtual void SetTemplateDrawDistance(unsigned int uiTemplateId, float drawDistance) = 0;
    virtual void GetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float& fDrawDistance) = 0;
    virtual void SetTemplateModelDrawDistance(unsigned int uiTemplateId, unsigned int uiModelId, float fDrawDistance) = 0;
    virtual void SetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode cullMode) = 0;
    virtual void GetTemplateModelCullMode(unsigned int uiTemplateId, unsigned int uiModelId, eCullMode& cullMode) = 0;

    virtual float GetUnitScaleFactor() = 0;

    virtual const ofbx::Object* const* GetObjectById(long long int ulId) = 0;
    virtual unsigned int               AddMeshToTemplate(unsigned int uiTemplate, unsigned long long uiModelId) = 0;
    virtual unsigned int               CreateTemplate() = 0;
    virtual void                       RemoveTemplate(unsigned int uiTemplateId) = 0;
    virtual void                       AddToRenderQueue(unsigned int uiTemplateId, CVector vecPosition, CVector vecRotation, CVector vecScale) = 0;
    virtual bool                       GetBoundingBox(unsigned long long ullObjectId, CVector& min, CVector& max, float& fRadius) = 0;
    virtual bool                       GetMeshName(long long int ulId, SString& strMeshName) = 0;
    virtual bool                       GetTexturePath(long long int ulId, SString& strTexturePath) = 0;
};

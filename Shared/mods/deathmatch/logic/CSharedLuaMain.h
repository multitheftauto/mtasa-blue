class CLuaPhysicsRigidBodyManager;
class CLuaPhysicsStaticCollisionManager;
class CLuaPhysicsShapeManager;
class CLuaPhysicsElement;
class CLuaPhysicsWorldElement;
class CResource;
class CLuaPhysicsRigidBody;
class CLuaPhysicsStaticCollision;
class CLuaPhysicsShape;

#pragma once

class CSharedLuaMain
{
public:
    CSharedLuaMain(CResource* pResource, CLuaPhysicsRigidBodyManager* pLuaPhysicsRigidBodyManager, CLuaPhysicsStaticCollisionManager* pLuaPhysicsStaticCollisionManager, CLuaPhysicsShapeManager* pLuaPhysicsShapeManager);
    ~CSharedLuaMain();

    void UnloadScript();

    CLuaPhysicsRigidBodyManager*       GetPhysicsRigidBodyManager() const { return m_pLuaPhysicsRigidBodyManager; };
    CLuaPhysicsStaticCollisionManager* GetPhysicsStaticCollisionManager() const { return m_pLuaPhysicsStaticCollisionManager; };
    CLuaPhysicsShapeManager*           GetPhysicsShapeManager() const { return m_pLuaPhysicsShapeManager; };
    CLuaPhysicsRigidBody*              GetRigidBodyFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsStaticCollision*        GetStaticCollisionFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsShape*                  GetShapeFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsElement*                GetPhysicsElementFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsWorldElement*           GetPhysicsWorldElementFromScriptID(unsigned int uiScriptID);

protected:
    CResource* m_pResource;

private:
    CLuaPhysicsRigidBodyManager*       m_pLuaPhysicsRigidBodyManager;
    CLuaPhysicsStaticCollisionManager* m_pLuaPhysicsStaticCollisionManager;
    CLuaPhysicsShapeManager*           m_pLuaPhysicsShapeManager;
};

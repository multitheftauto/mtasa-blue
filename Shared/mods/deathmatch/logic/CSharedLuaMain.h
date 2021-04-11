class CLuaPhysicsRigidBodyManager;
class CLuaPhysicsStaticCollisionManager;
class CLuaPhysicsShapeManager;
class CLuaPhysicsElement;
class CResource;

class CSharedLuaMain
{
public:
    CSharedLuaMain(CResource* pResource);
    ~CSharedLuaMain();

#ifdef MTA_CLIENT
    template <class T, typename... Ty>
    T* CreateElement(Ty... args)
    {
        static_assert(std::is_base_of<CClientEntity, T>::value);
        if (m_pResource)
        {
            T*             pElement = new T(CLuaDefs::m_pManager, INVALID_ELEMENT_ID, this, args...);
            CElementGroup* pGroup = m_pResource->GetElementGroup();
            if (pGroup)
            {
                pGroup->Add((CClientEntity*)pElement);
            }
            return pElement;
        }
        return nullptr;
    }

#else
    template <class T, typename... Ty>
    T* CreateElement(Ty... args)
    {
        static_assert(std::is_base_of<CElement, T>::value);
        if (m_pResource)
        {
            T*             pElement = new T(m_pResource->GetDynamicElementRoot(), this, args...);
            CElementGroup* pGroup = m_pResource->GetElementGroup();
            if (pGroup)
            {
                pGroup->Add((CElement*)pElement);
            }
            return pElement;
        }
        return nullptr;
    }
#endif

    void UnloadScript();

    CLuaPhysicsRigidBodyManager*       GetPhysicsRigidBodyManager() const { return m_pLuaPhysicsRigidBodyManager.get(); };
    CLuaPhysicsStaticCollisionManager* GetPhysicsStaticCollisionManager() const { return m_pLuaPhysicsStaticCollisionManager.get(); };
    CLuaPhysicsShapeManager*           GetPhysicsShapeManager() const { return m_pLuaPhysicsShapeManager.get(); };
    CLuaPhysicsRigidBody*              GetRigidBodyFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsStaticCollision*        GetStaticCollisionFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsShape*                  GetShapeFromScriptID(unsigned int uiScriptID);
    CLuaPhysicsElement*                GetPhysicsElementFromScriptID(unsigned int uiScriptID);

protected:
    CResource* m_pResource;

private:
    std::unique_ptr<CLuaPhysicsRigidBodyManager>       m_pLuaPhysicsRigidBodyManager;
    std::unique_ptr<CLuaPhysicsStaticCollisionManager> m_pLuaPhysicsStaticCollisionManager;
    std::unique_ptr<CLuaPhysicsShapeManager>           m_pLuaPhysicsShapeManager;
};

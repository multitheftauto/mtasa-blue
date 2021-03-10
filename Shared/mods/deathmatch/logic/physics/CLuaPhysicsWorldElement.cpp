/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsWorldElement.cpp
 *  PURPOSE:     Lua physics common functionality for elements whichin world
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsWorldElement::CLuaPhysicsWorldElement(CBulletPhysics* pPhysics, EIdClass::EIdClassType classType) : CLuaPhysicsElement(pPhysics, classType)
{
}

CLuaPhysicsWorldElement::~CLuaPhysicsWorldElement()
{
}

void CLuaPhysicsWorldElement::ClearCollisionContacts()
{
    m_vecCollisionContacts.clear();
}

void CLuaPhysicsWorldElement::ReportCollisionContact(CLuaPhysicsWorldElement* pElement)
{
    m_vecCollisionContacts.push_back(pElement);
}


void CLuaPhysicsWorldElement::GetContactManifoldsWithElement(CLuaPhysicsWorldElement* pElement)
{
}

void CLuaPhysicsWorldElement::GetAllContacts(std::vector<CLuaPhysicsRigidBody*>& vecRigidBodies, std::vector<CLuaPhysicsStaticCollision*>& vecStaticCollisions)
{
    CBulletPhysics::WorldContext          world(GetPhysics());

    btPersistentManifold** pManifolds = world->getDispatcher()->getInternalManifoldPointer();

    int iNumManifolds = world->getDispatcher()->getNumManifolds();
    for (int i = 0; i < iNumManifolds; i++)
    {
        btPersistentManifold* pManifold = pManifolds[i];

        CLuaPhysicsWorldElement* pElement0 = (CLuaPhysicsWorldElement*)(pManifold->getBody0()->getUserPointer());
        CLuaPhysicsWorldElement* pElement1 = (CLuaPhysicsWorldElement*)(pManifold->getBody1()->getUserPointer());
        if (pElement0 == this)
        {
            switch (pElement0->GetType())
            {
                case ePhysicsElementType::StaticCollision:
                    vecStaticCollisions.push_back((CLuaPhysicsStaticCollision*)pElement1);
                    break;
                case ePhysicsElementType::RigidBody:
                    vecRigidBodies.push_back((CLuaPhysicsRigidBody*)pElement1);
                    break;
            }
        }
        else if (pElement1 == this)
        {
            switch (pElement1->GetType())
            {
                case ePhysicsElementType::StaticCollision:
                    vecStaticCollisions.push_back((CLuaPhysicsStaticCollision*)pElement1);
                    break;
                case ePhysicsElementType::RigidBody:
                    vecRigidBodies.push_back((CLuaPhysicsRigidBody*)pElement1);
                    break;
            }
        }
    }
}

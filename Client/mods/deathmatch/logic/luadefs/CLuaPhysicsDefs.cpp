/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaPhysicsDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "lua/CLuaPhysicsRigidBodyManager.h"
#include "lua/CLuaPhysicsStaticCollisionManager.h"
#include "lua/CLuaPhysicsConstraintManager.h"
#include "lua/CLuaPhysicsShapeManager.h"
#include "lua/CLuaPhysicsSharedLogic.h"

#define MINIMUM_SHAPE_SIZE 0.05f
#define MAXIMUM_SHAPE_SIZE 10000.0f

void CLuaPhysicsDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"physicsCreateWorld", PhysicsCreateWorld},            // finished
        {"physicsDestroy", PhysicsDestroy},
        {"physicsCreateRigidBody", PhysicsCreateRigidBody},            // finished
        {"physicsCreateRigidBodyFromModel", PhysicsCreateRigidBodyFromModel},
        {"physicsCreateStaticCollision", PhysicsCreateStaticCollision},
        {"physicsCreateShape", PhysicsCreateShape},
        {"physicsAddShape", PhysicsAddShape},
        {"physicsGetShapes", PhysicsGetShapes},
        {"physicsGetRigidBodies", PhysicsGetRigidBodies},
        {"physicsGetStaticCollisions", PhysicsGetStaticCollisions},
        {"physicsGetConstraints", PhysicsGetConstraints},
        {"physicsSetProperties", PhysicsSetProperties},
        {"physicsGetProperties", PhysicsGetProperties},
        {"physicsDrawDebug", PhysicsDrawDebug},            // seems to be finished
        {"physicsSetDebugMode", PhysicsSetDebugMode},
        {"physicsBuildCollisionFromGTA", PhysicsBuildCollisionFromGTA},
        {"physicsApplyForce", PhysicsApplyForce},
        {"physicsApplyCentralForce", PhysicsApplyCentralForce},
        {"physicsApplyCentralImpulse", PhysicsApplyCentralImpulse},
        {"physicsApplyDamping", PhysicsApplyDamping},
        {"physicsApplyImpulse", PhysicsApplyImpulse},
        {"physicsApplyTorque", PhysicsApplyTorque},
        {"physicsApplyTorqueImpulse", PhysicsApplyTorqueImpulse},
        {"physicsCreateConstraint", PhysicsCreateConstraint},
        {"physicsRayCast", PhysicsRayCast},
        {"physicsShapeCast", PhysicsShapeCast},
    };

    // Add functions
    for (const auto& pair : functions)
    {
        CLuaCFunctions::AddFunction(pair.first, pair.second);
    }
}

void CLuaPhysicsDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "set", "PhysicsTestSet");
    // lua_classfunction(luaVM, "get", "PhysicsTestGet");

    lua_registerstaticclass(luaVM, "Physics");
}

int CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM)
{
    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);
    CVector          vecGravity;
    argStream.ReadVector3D(vecGravity, CVector(0, 0, -9.81));

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CClientPhysics* pPhysics = new CClientPhysics(m_pManager, INVALID_ELEMENT_ID, pLuaMain);
                CElementGroup*  pGroup = pResource->GetElementGroup();
                if (pGroup)
                {
                    pGroup->Add((CClientEntity*)pPhysics);
                }
                pPhysics->SetGravity(vecGravity);
                lua_pushelement(luaVM, pPhysics);
                return 1;
            }
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateShape(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsShape*    pShape;
        CVector              vector;
        float                fRadius;
        float                fHeight;
        float                fHalf;
        float                fInitialChildCapacity;
        int                  iSizeX, iSizeY;
        std::vector<CVector> vecList;
        std::vector<float>   vecHeightfieldData;
        switch (shapeType)
        {
            case PHYSICS_SHAPE_BOX:
                if (argStream.NextIsVector3D())
                {
                    argStream.ReadVector3D(vector);
                }
                else
                {
                    argStream.ReadNumber(fHalf);
                    vector = CVector(fHalf, fHalf, fHalf);
                }
                if (!argStream.HasErrors())
                {
                    if (vector.fX >= MINIMUM_SHAPE_SIZE && vector.fY >= MINIMUM_SHAPE_SIZE && vector.fZ >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape = pPhysics->CreateShape();
                        pShape->InitializeWithBox(vector);
                    }
                    else
                    {
                        argStream.SetCustomError(
                            SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    if (fRadius >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape = pPhysics->CreateShape();
                        pShape->InitializeWithSphere(fRadius);
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Minimum radius must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_CAPSULE:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius >= MINIMUM_SHAPE_SIZE && fHeight >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape = pPhysics->CreateShape();
                        pShape->InitializeWithCapsule(fRadius, fHeight);
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_CONE:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius >= MINIMUM_SHAPE_SIZE && fHeight >= MINIMUM_SHAPE_SIZE)
                    {
                        pShape = pPhysics->CreateShape();
                        pShape->InitializeWithCone(fRadius, fHeight);
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                    }
                }
                break;
            case PHYSICS_SHAPE_CYLINDER:
                argStream.ReadVector3D(vector);
                if (!argStream.HasErrors())
                {
                    pShape = pPhysics->CreateShape();
                    pShape->InitializeWithCylinder(vector);
                }
                break;
            case PHYSICS_SHAPE_COMPOUND:
                argStream.ReadNumber(fInitialChildCapacity, 0);
                if (!argStream.HasErrors())
                {
                    if (fInitialChildCapacity >= 0 && fInitialChildCapacity <= 1024)
                    {
                        pShape = pPhysics->CreateShape();
                        pShape->InitializeWithCompound(fInitialChildCapacity);
                    }
                    else
                    {
                        argStream.SetCustomError("Initial capacity should be between 0 and 1024");
                    }
                }
                break;
            case PHYSICS_SHAPE_CONVEX_HULL:
                while (argStream.NextIsVector3D())
                {
                    argStream.ReadVector3D(vector);
                    vecList.push_back(vector);
                }
                if (!argStream.HasErrors())
                {
                    pShape = pPhysics->CreateShape();
                    pShape->InitializeWithConvexHull(vecList);
                }
                break;
            case PHYSICS_SHAPE_TRIANGLE_MESH:
                while (argStream.NextIsVector3D())
                {
                    argStream.ReadVector3D(vector);
                    vecList.push_back(vector);
                }
                if (!argStream.HasErrors())
                {
                    if (vecList.size() % 3 == 0)
                    {
                        pShape = pPhysics->CreateShape();
                        pShape->InitializeWithTriangleMesh(vecList);
                    }
                    else
                    {
                        argStream.SetCustomError("Triangle mesh needs vertices count divisible by 3");
                    }
                }
                break;
            case PHYSICS_SHAPE_HEIGHTFIELD_TERRAIN:
                argStream.ReadNumber(iSizeX);
                argStream.ReadNumber(iSizeY);
                if (argStream.NextIsTable())
                    argStream.ReadNumberTable(vecHeightfieldData, iSizeX * iSizeY);
                else            // fill with empty table
                    for (int i = 0; i < iSizeX * iSizeY; i++)
                        vecHeightfieldData.emplace_back(0);

                if (!argStream.HasErrors())
                {
                    if (iSizeX >= 3 && iSizeY >= 3 && iSizeX <= 8192 && iSizeY <= 8192)
                    {
                        if (vecHeightfieldData.size() == iSizeX * iSizeY)
                        {
                            pShape = pPhysics->CreateShape();
                            pShape->InitializeWithHeightfieldTerrain(iSizeX, iSizeY, vecHeightfieldData);
                        }
                        else
                        {
                            argStream.SetCustomError(SString("Heigthfield of size %ix%i require %i floats, got %i floats", iSizeX, iSizeY, iSizeX * iSizeY,
                                                             vecHeightfieldData.size())
                                                         .c_str());
                        }
                    }
                    else
                    {
                        argStream.SetCustomError(
                            SString("Size of heghtfield terrain must be between 3x3 and 8192x8192, got size %ix%i", iSizeX, iSizeY, vecHeightfieldData.size())
                                .c_str());
                    }
                }
                break;
        }

        if (!argStream.HasErrors())
        {
            lua_pushshape(luaVM, pShape);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsDestroy(lua_State* luaVM)
{
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    CLuaPhysicsConstraint*      pConstraint = nullptr;
    CLuaPhysicsShape*           pShape = nullptr;

    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBody);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
        argStream.ReadUserData(pStaticCollision);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
        argStream.ReadUserData(pConstraint);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
        argStream.ReadUserData(pShape);

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            if (pRigidBody != nullptr)
            {
                pLuaMain->GetPhysicsRigidBodyManager()->RemoveRigidBody(pRigidBody);
                lua_pushboolean(luaVM, false);
                return 1;
            }
            if (pStaticCollision != nullptr)
            {
                pLuaMain->GetPhysicsStaticCollisionManager()->RemoveStaticCollision(pStaticCollision);
                lua_pushboolean(luaVM, false);
                return 1;
            }
            if (pConstraint != nullptr)
            {
                pLuaMain->GetPhysicsConstraintManager()->RemoveContraint(pConstraint);
                lua_pushboolean(luaVM, false);
                return 1;
            }
            if (pShape != nullptr)
            {
                pLuaMain->GetPhysicsShapeManager()->RemoveShape(pShape);
                lua_pushboolean(luaVM, false);
                return 1;
            }
        }
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsBuildCollisionFromGTA(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    bool             bBuildByRadius = false;
    CVector          vecPosition;
    float            fRadius;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    if (argStream.NextIsVector4D())
    {
        argStream.ReadVector3D(vecPosition);
        argStream.ReadNumber(fRadius);
        bBuildByRadius = true;
    }

    if (!argStream.HasErrors())
    {
        if (bBuildByRadius)
        {
            pPhysics->BuildCollisionFromGTAInRadius(vecPosition, fRadius);
        }
        else
        {
            pPhysics->StartBuildCollisionFromGTA();
        }
        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaPhysicsDefs::PhysicsSetDebugMode(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsDebugMode eDebugMode;
    bool              bEnabled;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eDebugMode);
    argStream.ReadBool(bEnabled);

    if (!argStream.HasErrors())
    {
        if (pPhysics->SetDebugMode(eDebugMode, bEnabled))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}
int CLuaPhysicsDefs::PhysicsDrawDebug(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);

    if (!argStream.HasErrors())
    {
        pPhysics->DrawDebug();
    }
    return 1;
}

int CLuaPhysicsDefs::PhysicsShapeCast(lua_State* luaVM)
{
    CClientPhysics*             pPhysics;
    CLuaPhysicsStaticCollision* pStaticCollision;
    CVector                     vecStartPosition, vecStartRotation;
    CVector                     vecEndPosition, vecEndRotation;
    CScriptArgReader            argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadUserData(pStaticCollision);
    argStream.ReadVector3D(vecStartPosition);
    argStream.ReadVector3D(vecStartRotation);
    argStream.ReadVector3D(vecEndPosition);
    argStream.ReadVector3D(vecEndRotation);

    if (!argStream.HasErrors())
    {
        btTransform startTransform;
        btTransform endTransform;
        startTransform.setIdentity();
        endTransform.setIdentity();
        CLuaPhysicsSharedLogic::SetPosition(startTransform, vecStartPosition);
        CLuaPhysicsSharedLogic::SetRotation(startTransform, vecStartPosition);
        CLuaPhysicsSharedLogic::SetPosition(endTransform, vecEndPosition);
        CLuaPhysicsSharedLogic::SetRotation(endTransform, vecEndRotation);
        btVector3                                     from = btVector3(vecStartPosition.fX, vecStartPosition.fY, vecStartPosition.fZ);
        btVector3                                     to = btVector3(vecEndPosition.fX, vecEndPosition.fY, vecEndPosition.fZ);
        btCollisionWorld::ClosestConvexResultCallback result(from, to);
        pPhysics->ShapeCast(pStaticCollision, startTransform, endTransform, result);
        lua_newtable(luaVM);
        lua_pushstring(luaVM, "hit");
        lua_pushboolean(luaVM, result.hasHit());
        lua_settable(luaVM, -3);
        if (result.hasHit())
        {
            lua_pushstring(luaVM, "closeshitfraction");
            lua_pushnumber(luaVM, result.m_closestHitFraction);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "hitpoint");
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, result.m_hitPointWorld.getX());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, result.m_hitPointWorld.getY());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, result.m_hitPointWorld.getZ());
            lua_settable(luaVM, -3);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "hitnormal");
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, result.m_hitNormalWorld.getX());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, result.m_hitNormalWorld.getY());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, result.m_hitNormalWorld.getZ());
            lua_settable(luaVM, -3);
            lua_settable(luaVM, -3);
        }

        return 1;
    }
    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateRigidBody(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    CLuaPhysicsShape* pShape;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadUserData(pShape);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsRigidBody* pRigidBody = pPhysics->CreateRigidBody(pShape);
        lua_pushrigidbody(luaVM, pRigidBody);
        return 1;
    }
    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateRigidBodyFromModel(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    unsigned short   usModel;
    CVector          vecPosition;
    CVector          vecRotation;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadNumber(usModel);
    argStream.ReadVector3D(vecPosition, CVector(0, 0, 0));
    argStream.ReadVector3D(vecRotation, CVector(0, 0, 0));

    if (!argStream.HasErrors())
    {
        CLuaPhysicsRigidBody* pRigidBody = pPhysics->CreateRigidBodyFromModel(usModel, vecPosition, vecRotation);
        if (pRigidBody != nullptr)
            lua_pushrigidbody(luaVM, pRigidBody);
        else
            lua_pushboolean(luaVM, false);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateStaticCollision(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsShapeType shapeType;
    CLuaPhysicsShape* pShape;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadUserData(pShape);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsStaticCollision* pStaticCollision = pPhysics->CreateStaticCollision(pShape->GetBtShape());
        lua_pushstaticcollision(luaVM, pStaticCollision);
        return 1;
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsAddShape(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    ePhysicsShapeType     shapeType;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadEnumString(shapeType);

    if (!argStream.HasErrors())
    {
        CVector vector;
        float   fRadius;
        switch (shapeType)
        {
            case PHYSICS_SHAPE_BOX:
                argStream.ReadVector3D(vector);
                if (!argStream.HasErrors())
                {
                    pRigidBody->AddBox(vector);
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    pRigidBody->AddSphere(fRadius);
                }
                break;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyForce(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               from, to;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(from);
    argStream.ReadVector3D(to);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyForce(from, to);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyCentralForce(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyCentralForce(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyDamping(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    float                 fDamping;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadNumber(fDamping);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyDamping(fDamping);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyImpulse(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               from, to;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(from);
    argStream.ReadVector3D(to);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyImpulse(from, to);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyTorque(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyTorque(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyTorqueImpulse(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyTorqueImpulse(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsApplyCentralImpulse(lua_State* luaVM)
{
    CLuaPhysicsRigidBody* pRigidBody;
    CVector               force;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pRigidBody);
    argStream.ReadVector3D(force);

    if (!argStream.HasErrors())
    {
        pRigidBody->ApplyCentralImpulse(force);
        lua_pushboolean(luaVM, true);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsSetProperties(lua_State* luaVM)
{
    CClientPhysics*             pPhysics = nullptr;
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    CLuaPhysicsConstraint*      pStaticConstraint = nullptr;
    CLuaPhysicsShape*           pShape = nullptr;

    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserDataOfType<CClientPhysics>())
        argStream.ReadUserData(pPhysics);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBody);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
        argStream.ReadUserData(pStaticCollision);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
        argStream.ReadUserData(pStaticConstraint);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
        argStream.ReadUserData(pShape);

    argStream.ReadEnumString(eProperty);
    if (!argStream.HasErrors())
    {
        bool    boolean;
        CVector vector;
        float   floatNumber[2];
        int     intNumber;
        SColor  color;

        if (pPhysics)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_GRAVITY:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pPhysics->SetGravity(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_USE_CONTINOUS:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pPhysics->SetUseContinous(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_SPEED:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] >= 0 && floatNumber[0] <= 1000)
                        {
                            pPhysics->SetSpeed(floatNumber[0]);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Speed must be between 0 and 1000");
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_SIMULATION_ENABLED:
                    argStream.ReadNumber(boolean);
                    if (!argStream.HasErrors())
                    {
                        pPhysics->SetSimulationEnabled(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_SUBSTEPS:
                    argStream.ReadNumber(intNumber);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber >= 1 && intNumber <= 256)
                        {
                            pPhysics->SetSubSteps(intNumber);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Substeps must be between 1 and 256");
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_TRIGGEREVENTS:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pPhysics->SetTriggerEvents(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_TRIGGERCOLLISIONEVENTS:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pPhysics->SetTriggerCollisionEvents(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_TRIGGERCONSTRAINTEVENTS:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pPhysics->SetTriggerConstraintEvents(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_WORLDSIZE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= 1 && vector.fY >= 1 && vector.fZ >= 1)
                        {
                            pPhysics->SetWorldSize(vector);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("World size can not be smaller than cube 1x1x1");
                        }
                    }
                    break;
                default:
                    argStream.SetCustomError(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pRigidBody)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_STATIC:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetStatic(boolean);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_MASS:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetMass(floatNumber[0]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_POSITION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetPosition(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_ROTATION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetRotation(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
                    argStream.ReadNumber(floatNumber[0]);
                    argStream.ReadNumber(floatNumber[1]);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetSleepingThresholds(floatNumber[0], floatNumber[1]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_RESTITUTION:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetRestitution(floatNumber[0]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetScale(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    if (argStream.NextIsBool())
                    {
                        argStream.ReadBool(boolean);
                        if (boolean == false)
                        {
                            pRigidBody->RemoveDebugColor();
                        }
                    }
                    else
                    {
                        argStream.ReadColor(color);
                        if (!argStream.HasErrors())
                        {
                            pRigidBody->SetDebugColor(color);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_MASK:
                    argStream.ReadNumber(intNumber);
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber > 0 && intNumber < 32)
                        {
                            pRigidBody->SetFilterMask((char)intNumber, boolean);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_GROUP:
                    argStream.ReadNumber(intNumber);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber > 0 && intNumber <= 32)
                        {
                            pRigidBody->SetFilterGroup(intNumber);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_MOTION_THRESHOLD:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] >= 0.0f && floatNumber[0] <= 1000.0f)
                        {
                            pRigidBody->SetMotionThreshold(floatNumber[0]);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_SWEPT_SPHERE_RADIUS:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] > 0.0f && floatNumber[0] <= 1000.0f)
                        {
                            pRigidBody->SetSweptSphereRadius(floatNumber[0]);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                default:
                    argStream.SetCustomError(SString("Physics rigid body does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pStaticCollision)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_POSITION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetPosition(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_ROTATION:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetRotation(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetScale(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    argStream.ReadColor(color);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetDebugColor(color);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_MASK:
                    argStream.ReadNumber(intNumber);
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        if (intNumber > 0 && intNumber < 32)
                        {
                            pStaticCollision->SetFilterMask((char)intNumber - 1, boolean);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_FILTER_GROUP:
                    argStream.ReadNumber(intNumber);
                    if (!argStream.HasErrors())
                    {
                        pStaticCollision->SetFilterGroup(intNumber);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                default:
                    argStream.SetCustomError(SString("Physics static collision does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pStaticConstraint)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_STIFFNESS:
                    argStream.ReadNumber(intNumber);
                    argStream.ReadNumber(floatNumber[0]);
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetStiffness(intNumber, floatNumber[0], boolean));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_PIVOT_A:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetPivotA(vector));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_PIVOT_B:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetPivotB(vector));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_LOWER_LIN_LIMIT:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetLowerLinLimit(floatNumber[0]));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_UPPER_LIN_LIMIT:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetUpperLinLimit(floatNumber[0]));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_LOWER_ANG_LIMIT:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetLowerAngLimit(floatNumber[0]));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_UPPER_ANG_LIMIT:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        lua_pushboolean(luaVM, pStaticConstraint->SetUpperAngLimit(floatNumber[0]));
                        return 1;
                    }
                    break;
                case PHYSICS_PROPERTY_BREAKING_IMPULSE_THRESHOLD:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        pStaticConstraint->SetBreakingImpulseThreshold(floatNumber[0]);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                    break;
                default:
                    argStream.SetCustomError(SString("Physics constraint does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pShape)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_SIZE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= MINIMUM_SHAPE_SIZE && vector.fY >= MINIMUM_SHAPE_SIZE && vector.fZ >= MINIMUM_SHAPE_SIZE)
                        {
                            if (pShape->SetSize(vector))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            else
                            {
                                argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
                            }
                        }
                        else
                        {
                            argStream.SetCustomError(
                                SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= MINIMUM_SHAPE_SIZE && vector.fY >= MINIMUM_SHAPE_SIZE && vector.fZ >= MINIMUM_SHAPE_SIZE)
                        {
                            if (pShape->SetScale(vector))
                            {
                                lua_pushboolean(luaVM, true);
                                return 1;
                            }
                            else
                            {
                                argStream.SetCustomError(SString("Shape '%s' does not support scale property", pShape->GetType()));
                            }
                        }
                        else
                        {
                            argStream.SetCustomError(
                                SString("Minimum scale width, height and length must be equal or greater than %.02f units", MINIMUM_SHAPE_SIZE).c_str());
                        }
                    }
                    break;
                default:
                    argStream.SetCustomError(SString("Physics shape does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsGetProperties(lua_State* luaVM)
{
    CClientPhysics*             pPhysics = nullptr;
    CLuaPhysicsRigidBody*       pRigidBody = nullptr;
    CLuaPhysicsStaticCollision* pStaticCollision = nullptr;
    CLuaPhysicsConstraint*      pConstraint = nullptr;
    CLuaPhysicsShape*           pShape = nullptr;

    ePhysicsProperty eProperty;
    CScriptArgReader argStream(luaVM);

    if (argStream.NextIsUserDataOfType<CClientPhysics>())
        argStream.ReadUserData(pPhysics);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBody);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsStaticCollision>())
        argStream.ReadUserData(pStaticCollision);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsConstraint>())
        argStream.ReadUserData(pConstraint);
    else if (argStream.NextIsUserDataOfType<CLuaPhysicsShape>())
        argStream.ReadUserData(pShape);

    argStream.ReadEnumString(eProperty);
    if (!argStream.HasErrors())
    {
        bool             boolean;
        CVector          vector, vector2;
        SColor           color;
        float            floatNumber[2];
        btVector3        btVector;
        int              i = 0;
        btJointFeedback* pFeedback;

        if (pPhysics)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_GRAVITY:
                    pPhysics->GetGravity(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_TRIGGERCOLLISIONEVENTS:
                    pPhysics->GetTriggerCollisionEvents(boolean);
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                case PHYSICS_PROPERTY_TRIGGERCONSTRAINTEVENTS:
                    pPhysics->GetTriggerConstraintvents(boolean);
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                case PHYSICS_PROPERTY_USE_CONTINOUS:
                    boolean = pPhysics->GetUseContinous();
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                default:
                    argStream.SetCustomError(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pRigidBody)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_POSITION:
                    pRigidBody->GetPosition(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_ROTATION:
                    pRigidBody->GetRotation(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
                    pRigidBody->GetSleepingThresholds(floatNumber[0], floatNumber[1]);
                    lua_pushnumber(luaVM, floatNumber[0]);
                    lua_pushnumber(luaVM, floatNumber[1]);
                    return 2;
                case PHYSICS_PROPERTY_RESTITUTION:
                    pRigidBody->GetRestitution(floatNumber[0]);
                    lua_pushnumber(luaVM, floatNumber[0]);
                    return 1;
                case PHYSICS_PROPERTY_SCALE:
                    pRigidBody->GetScale(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    pRigidBody->GetDebugColor(color);
                    lua_pushnumber(luaVM, color.R);
                    lua_pushnumber(luaVM, color.G);
                    lua_pushnumber(luaVM, color.B);
                    return 3;
                case PHYSICS_PROPERTY_MOTION_THRESHOLD:
                    floatNumber[0] = pRigidBody->GetMotionThreshold();
                    lua_pushnumber(luaVM, floatNumber[0]);
                    return 1;
                case PHYSICS_PROPERTY_SWEPT_SPHERE_RADIUS:
                    floatNumber[0] = pRigidBody->GetSweptSphereRadius();
                    lua_pushnumber(luaVM, floatNumber[0]);
                    return 1;
                case PHYSICS_PROPERTY_SLEEP:
                    boolean = pRigidBody->IsSleeping();
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                case PHYSICS_PROPERTY_WANTS_SLEEPING:
                    boolean = pRigidBody->WantsSleeping();
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                default:
                    argStream.SetCustomError(SString("Physics rigid body does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pStaticCollision)
        {
        }
        else if (pShape)
        {
            btCollisionShape* pBtShape;
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_SIZE:
                    if (pShape->GetSize(vector))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        return 3;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    if (pShape->GetScale(vector))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        return 3;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support scale property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_BOUNDING_BOX:
                    if (pShape->GetBoundingBox(vector, vector2))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        lua_pushnumber(luaVM, vector2.fX);
                        lua_pushnumber(luaVM, vector2.fY);
                        lua_pushnumber(luaVM, vector2.fZ);
                        return 6;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support bounding box property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_BOUNDING_SPHERE:
                    if (pShape->GetBoundingSphere(vector, floatNumber[0]))
                    {
                        lua_pushnumber(luaVM, vector.fX);
                        lua_pushnumber(luaVM, vector.fY);
                        lua_pushnumber(luaVM, vector.fZ);
                        lua_pushnumber(luaVM, floatNumber[0]);
                        return 4;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support bounding box property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_IS_COMPOUND:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isCompound());
                    return 1;
                case PHYSICS_PROPERTY_IS_CONCAVE:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isConcave());
                    return 1;
                case PHYSICS_PROPERTY_IS_CONVEX:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isConvex());
                    return 1;
                case PHYSICS_PROPERTY_IS_CONVEX2D:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isConvex2d());
                    return 1;
                case PHYSICS_PROPERTY_IS_INFINITE:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isInfinite());
                    return 1;
                case PHYSICS_PROPERTY_IS_NON_MOVING:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isNonMoving());
                    return 1;
                case PHYSICS_PROPERTY_IS_POLYHEDRAL:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isPolyhedral());
                    return 1;
                case PHYSICS_PROPERTY_IS_SOFT_BODY:
                    pBtShape = pShape->GetBtShape();
                    lua_pushboolean(luaVM, pBtShape->isSoftBody());
                    return 1;
                default:
                    argStream.SetCustomError(SString("Physics shape does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pConstraint)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_BREAKING_IMPULSE_THRESHOLD:
                    lua_pushnumber(luaVM, pConstraint->GetBreakingImpulseThreshold());
                    return 1;
                case PHYSICS_PROPERTY_APPLIED_IMPULSE:
                    lua_pushnumber(luaVM, pConstraint->GetAppliedImpulse());
                    return 1;
                case PHYSICS_PROPERTY_JOINTS_FEEDBACK:
                    pFeedback = pConstraint->GetJoinFeedback();
                    btVector = pFeedback->m_appliedForceBodyA;
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, btVector.getX());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, btVector.getY());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, btVector.getZ());
                    lua_settable(luaVM, -3);
                    btVector = pFeedback->m_appliedTorqueBodyA;
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, btVector.getX());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, btVector.getY());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, btVector.getZ());
                    lua_settable(luaVM, -3);
                    btVector = pFeedback->m_appliedForceBodyB;
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, btVector.getX());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, btVector.getY());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, btVector.getZ());
                    lua_settable(luaVM, -3);
                    btVector = pFeedback->m_appliedTorqueBodyB;
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, btVector.getX());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, btVector.getY());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, btVector.getZ());
                    lua_settable(luaVM, -3);
                    return 4;
                case PHYSICS_PROPERTY_RIGID_BODY_A:
                    pRigidBody = pConstraint->GetRigidBodyA();
                    if (pRigidBody)
                        lua_pushrigidbody(luaVM, pRigidBody);
                    else
                        lua_pushboolean(luaVM, false);
                    return 1;
                case PHYSICS_PROPERTY_RIGID_BODY_B:
                    pRigidBody = pConstraint->GetRigidBodyB();
                    if (pRigidBody)
                        lua_pushrigidbody(luaVM, pRigidBody);
                    else
                        lua_pushboolean(luaVM, false);
                    return 1;
                case PHYSICS_PROPERTY_CONSTRAINT_BROKEN:
                    lua_pushboolean(luaVM, pConstraint->IsBroken());
                    return 1;
                default:
                    argStream.SetCustomError(SString("Physics element does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsCreateConstraint(lua_State* luaVM)
{
    CClientPhysics*       pPhysics;
    bool                  bDisableCollisionsBetweenLinkedBodies;
    CLuaPhysicsRigidBody* pRigidBodyA;
    CLuaPhysicsRigidBody* pRigidBodyB = nullptr;
    ePhysicsConstraint    eConstraint;
    CScriptArgReader      argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eConstraint);
    if (argStream.NextIsBool())
        argStream.ReadBool(bDisableCollisionsBetweenLinkedBodies, true);

    argStream.ReadUserData(pRigidBodyA);
    if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
        argStream.ReadUserData(pRigidBodyB);

    if (!argStream.HasErrors())
    {
        CLuaPhysicsConstraint* pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
        CVector                vector[4];
        switch (eConstraint)
        {
            case PHYSICS_CONTRAINT_POINTTOPOINT:
                argStream.ReadVector3D(vector[0], CVector(0, 0, 0));
                argStream.ReadVector3D(vector[1], CVector(0, 0, 0));
                pConstraint->CreatePointToPointConstraint(vector[0], vector[1], bDisableCollisionsBetweenLinkedBodies);
                break;
            case PHYSICS_CONTRAINT_HIDGE:
                argStream.ReadVector3D(vector[0]);
                argStream.ReadVector3D(vector[1]);
                argStream.ReadVector3D(vector[2]);
                argStream.ReadVector3D(vector[3]);
                pConstraint->CreateHidgeConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
                break;
            case PHYSICS_CONTRAINT_FIXED:
                if (pRigidBodyA && pRigidBodyB)
                {
                    argStream.ReadVector3D(vector[0]);
                    argStream.ReadVector3D(vector[1]);
                    argStream.ReadVector3D(vector[2]);
                    argStream.ReadVector3D(vector[3]);
                    pConstraint->CreateFixedConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
                }
                else
                {
                    pPhysics->DestroyCostraint(pConstraint);
                    pConstraint = nullptr;
                    argStream.SetCustomError("Physics fixed constraint requires both rigid bodies.");
                }
                break;
            case PHYSICS_CONTRAINT_SLIDER:
                argStream.ReadVector3D(vector[0]);
                argStream.ReadVector3D(vector[1]);
                argStream.ReadVector3D(vector[2]);
                argStream.ReadVector3D(vector[3]);
                pConstraint->CreateSliderConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
                break;
        }
        if (pConstraint)
        {
            lua_pushconstraint(luaVM, pConstraint);
            return 1;
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsRayCast(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    ePhysicsRayType  eRayType;
    CVector          from;
    CVector          to;
    bool             bFilterBackfaces;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eRayType, PHYSICS_RAY_DEFAULT);
    argStream.ReadVector3D(from);
    argStream.ReadVector3D(to);
    argStream.ReadBool(bFilterBackfaces, false);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            if (eRayType == PHYSICS_RAY_IS_CLEAR)
            {
                bool bHasHit = pPhysics->RayCastIsClear(from, to);
                lua_pushboolean(luaVM, bHasHit);
                return 1;
            }
            else if (eRayType == PHYSICS_RAY_DEFAULT)
            {
                btCollisionWorld::ClosestRayResultCallback rayResult = pPhysics->RayCastDefault(from, to, bFilterBackfaces);
                lua_newtable(luaVM);
                lua_pushstring(luaVM, "hit");
                lua_pushboolean(luaVM, rayResult.hasHit());
                lua_settable(luaVM, -3);
                if (rayResult.hasHit())
                {
                    lua_pushstring(luaVM, "hitpoint");
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getX());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getY());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, rayResult.m_hitPointWorld.getZ());
                    lua_settable(luaVM, -3);
                    lua_settable(luaVM, -3);

                    lua_pushstring(luaVM, "hitnormal");
                    lua_newtable(luaVM);
                    lua_pushnumber(luaVM, 1);
                    lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getX());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 2);
                    lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getY());
                    lua_settable(luaVM, -3);
                    lua_pushnumber(luaVM, 3);
                    lua_pushnumber(luaVM, rayResult.m_hitNormalWorld.getZ());
                    lua_settable(luaVM, -3);
                    lua_settable(luaVM, -3);

                    const btCollisionObject* pCollisionObject = rayResult.m_collisionObject;
                    const btCollisionShape*     pShape = pCollisionObject->getCollisionShape();
                    const btRigidBody*          pRigidBody = btRigidBody::upcast(pCollisionObject);

                    CLuaPhysicsShape*           pLuaShape = nullptr;
                    CLuaPhysicsRigidBody*       pLuaRigidBody = nullptr;
                    CLuaPhysicsStaticCollision* pLuaStaticCollision = nullptr;

                    if (pShape != nullptr)
                    {
                        pLuaShape = (CLuaPhysicsShape*)pShape->getUserPointer();
                    }

                    if (pRigidBody != nullptr)
                    {
                        pLuaRigidBody = (CLuaPhysicsRigidBody*)pRigidBody->getUserPointer();
                    }
                    else if (pCollisionObject != nullptr)
                    {
                        pLuaStaticCollision = (CLuaPhysicsStaticCollision*)pCollisionObject->getUserPointer();
                    }

                    lua_pushstring(luaVM, "shape");
                    if (pLuaShape)
                        lua_pushshape(luaVM, pLuaShape);
                    else
                        lua_pushboolean(luaVM, false);
                    lua_settable(luaVM, -3);

                    lua_pushstring(luaVM, "rigidbody");
                    if (pLuaRigidBody)
                        lua_pushrigidbody(luaVM, pLuaRigidBody);
                    else
                        lua_pushboolean(luaVM, false);
                    lua_settable(luaVM, -3);

                    lua_pushstring(luaVM, "staticcollision");
                    if (pLuaStaticCollision)
                        lua_pushstaticcollision(luaVM, pLuaStaticCollision);
                    else
                        lua_pushboolean(luaVM, false);
                    lua_settable(luaVM, -3);
                }
                return 1;
            }
            else if (eRayType == PHYSICS_RAY_DETAILED)
            {
                pPhysics->RayCastDetailed(luaVM, from, to, bFilterBackfaces);
                return 1;
            }
            else if (eRayType == PHYSICS_RAY_MULTIPLE)
            {
                pPhysics->RayCastMultiple(luaVM, from, to, bFilterBackfaces);
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

int CLuaPhysicsDefs::PhysicsGetShapes(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    ePhysicsRayType  eRayType;
    CVector          from;
    CVector          to;
    bool             bFilterBackfaces;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CLuaPhysicsShapeManager* pShapeManager = luaMain->GetPhysicsShapeManager();

            lua_newtable(luaVM);
            int i = 1;
            for (auto iter = pShapeManager->IterBegin(); iter != pShapeManager->IterEnd(); ++iter)
            {
                lua_pushnumber(luaVM, i++);
                lua_pushshape(luaVM, (*iter));
                lua_settable(luaVM, -3);
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsGetRigidBodies(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    ePhysicsRayType  eRayType;
    CVector          from;
    CVector          to;
    bool             bFilterBackfaces;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CLuaPhysicsRigidBodyManager* pRigidBodyManager = luaMain->GetPhysicsRigidBodyManager();

            lua_newtable(luaVM);
            int i = 1;
            for (auto iter = pRigidBodyManager->IterBegin(); iter != pRigidBodyManager->IterEnd(); ++iter)
            {
                lua_pushnumber(luaVM, i++);
                lua_pushrigidbody(luaVM, (*iter));
                lua_settable(luaVM, -3);
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsGetStaticCollisions(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    ePhysicsRayType  eRayType;
    CVector          from;
    CVector          to;
    bool             bFilterBackfaces;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CLuaPhysicsStaticCollisionManager* pStaticCollisionManager = luaMain->GetPhysicsStaticCollisionManager();
            lua_newtable(luaVM);
            int i = 1;
            for (auto iter = pStaticCollisionManager->IterBegin(); iter != pStaticCollisionManager->IterEnd(); ++iter)
            {
                lua_pushnumber(luaVM, i++);
                lua_pushstaticcollision(luaVM, (*iter));
                lua_settable(luaVM, -3);
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsGetConstraints(lua_State* luaVM)
{
    CClientPhysics*  pPhysics;
    ePhysicsRayType  eRayType;
    CVector          from;
    CVector          to;
    bool             bFilterBackfaces;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPhysics);

    if (!argStream.HasErrors())
    {
        CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (luaMain)
        {
            CLuaPhysicsConstraintManager* pConstraintManager = luaMain->GetPhysicsConstraintManager();
            lua_newtable(luaVM);
            int i = 1;
            for (auto iter = pConstraintManager->IterBegin(); iter != pConstraintManager->IterEnd(); ++iter)
            {
                lua_pushnumber(luaVM, i++);
                lua_pushconstraint(luaVM, (*iter));
                lua_settable(luaVM, -3);
            }
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

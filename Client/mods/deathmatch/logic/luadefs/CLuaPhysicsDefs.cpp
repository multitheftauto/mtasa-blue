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

#include "lua/CLuaPhysicsElement.h"
#include "lua/CLuaPhysicsBoxShape.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsSphereShape.h"
#include "lua/CLuaPhysicsCapsuleShape.h"
#include "lua/CLuaPhysicsConeShape.h"
#include "lua/CLuaPhysicsCylinderShape.h"
#include "lua/CLuaPhysicsConvexHullShape.h"
#include "lua/CLuaPhysicsCompoundShape.h"
#include "lua/CLuaPhysicsTriangleMeshShape.h"
#include "lua/CLuaPhysicsHeightfieldTerrainShape.h"

#include "lua/CLuaPhysicsPointToPointConstraint.h"

void CLuaPhysicsDefs::LoadFunctions(void)
{
    std::map<const char*, lua_CFunction> functions{
        {"physicsCreateWorld", ArgumentParser<PhysicsCreateWorld>},
        {"physicsDestroy", ArgumentParser<PhysicsDestroy>},
        {"physicsCreateRigidBody", ArgumentParser<PhysicsCreateRigidBody>},
        {"physicsCreateStaticCollision", ArgumentParser<PhysicsCreateStaticCollision>},
        {"physicsCreateConstraint", ArgumentParser<PhysicsCreatePointToPointConstraintVariantA, PhysicsCreatePointToPointConstraintVariantB>},
        {"physicsCreateShape", PhysicsCreateShape},
        {"physicsCreateShapeFromModel", ArgumentParser<PhysicsCreateShapeFromModel>},
        {"physicsAddChildShape", ArgumentParser<PhysicsAddChildShape>},
        {"physicsRemoveChildShape", ArgumentParser<PhysicsRemoveChildShape>},
        {"physicsGetChildShapes", ArgumentParser<PhysicsGetChildShapes>},
        {"physicsGetChildShapeOffsetPosition", ArgumentParser<PhysicsGetChildShapeOffsetPosition>},
        {"physicsGetChildShapeOffsetRotation", ArgumentParser<PhysicsGetChildShapeOffsetRotation>},
        {"physicsSetChildShapeOffsets", ArgumentParser<PhysicsSetChildShapeOffsets>},
        {"physicsGetShapes", ArgumentParser<PhysicsGetShapes>},
        {"physicsGetRigidBodies", ArgumentParser<PhysicsGetRigidBodies>},
        {"physicsGetStaticCollisions", ArgumentParser<PhysicsGetStaticCollisions>},
        {"physicsGetConstraints", ArgumentParser<PhysicsGetConstraints>},
        {"physicsSetProperties", PhysicsSetProperties},
        {"physicsGetProperties", PhysicsGetProperties},
        {"physicsDrawDebug", ArgumentParser<PhysicsDrawDebug>},
        {"physicsSetDebugMode", PhysicsSetDebugMode},
        {"physicsGetDebugMode", PhysicsGetDebugMode},
        {"physicsBuildCollisionFromGTA", PhysicsBuildCollisionFromGTA},
        {"physicsApplyVelocityForce", ArgumentParser<PhysicsApplyVelocityForce>},
        {"physicsApplyVelocity", ArgumentParser<PhysicsApplyVelocity>},
        {"physicsApplyAngularVelocity", ArgumentParser<PhysicsApplyAngularVelocity>},
        {"physicsApplyAngularVelocityForce", ArgumentParser<PhysicsApplyAngularVelocityForce > },
        {"physicsApplyDamping", ArgumentParser<PhysicsApplyDamping>},
        {"physicsRayCast", PhysicsRayCast},
        {"physicsShapeCast", PhysicsShapeCast},
        {"physicsGetElementType", ArgumentParser<PhysicsGetElementType>},
        {"isPhysicsElement", ArgumentParser<IsPhysicsElement>},
    };

    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaPhysicsDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    // lua_classfunction(luaVM, "set", "PhysicsTestSet");
    // lua_classfunction(luaVM, "get", "PhysicsTestGet");

    lua_registerstaticclass(luaVM, "Physics");
}

CClientPhysics* CLuaPhysicsDefs::PhysicsCreateWorld(lua_State* luaVM, std::optional<CVector> vecGravity)
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
            pPhysics->SetGravity(vecGravity.value_or(CVector(0, 0, -9.81)));
            return pPhysics;
        }
    }
    throw std::invalid_argument("Unknown error");
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
        int                  index = 0;
        std::vector<CVector> vecList;
        std::vector<float>   vecFloat;
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
                    if (vector.fX < MINIMUM_PRIMITIVE_SIZE || vector.fY < MINIMUM_PRIMITIVE_SIZE || vector.fZ < MINIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    if (vector.fX > MAXIMUM_PRIMITIVE_SIZE || vector.fY > MAXIMUM_PRIMITIVE_SIZE && vector.fZ > MAXIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Maximum width, height and length must be equal or smaller than %.02f units", MAXIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    pShape = new CLuaPhysicsBoxShape(pPhysics, vector);
                }
                break;
            case PHYSICS_SHAPE_SPHERE:
                argStream.ReadNumber(fRadius);
                if (!argStream.HasErrors())
                {
                    if (fRadius < MINIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(SString("Minimum radius must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }

                    if (fRadius > MAXIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(SString("Maximum radius must be equal or smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    pShape = new CLuaPhysicsSphereShape(pPhysics, fRadius);
                }
                break;
            case PHYSICS_SHAPE_CAPSULE:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius < MINIMUM_PRIMITIVE_SIZE || fHeight < MINIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    if (fRadius > MAXIMUM_PRIMITIVE_SIZE || fHeight > MAXIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Maximum radius and height must be equal or smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    pShape = new CLuaPhysicsCapsuleShape(pPhysics, fRadius, fHeight);
                }
                break;
            case PHYSICS_SHAPE_CONE:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius < MINIMUM_PRIMITIVE_SIZE || fHeight < MINIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    if (fRadius > MAXIMUM_PRIMITIVE_SIZE || fHeight > MAXIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Maximum radius and height must be equal or smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    pShape = new CLuaPhysicsConeShape(pPhysics, fRadius, fHeight);
                }
                break;
            case PHYSICS_SHAPE_CYLINDER:
                argStream.ReadNumber(fRadius);
                argStream.ReadNumber(fHeight);
                if (!argStream.HasErrors())
                {
                    if (fRadius < MINIMUM_PRIMITIVE_SIZE || fHeight < MINIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Minimum radius and height must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    if (fRadius > MAXIMUM_PRIMITIVE_SIZE || fHeight > MAXIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Maximum radius and height must be equal or smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }
                    pShape = new CLuaPhysicsCylinderShape(pPhysics, CVector(fRadius, fHeight, fRadius));
                }
                break;
            case PHYSICS_SHAPE_COMPOUND:
                argStream.ReadNumber(fInitialChildCapacity, 0);
                if (!argStream.HasErrors())
                {
                    if (fInitialChildCapacity >= 0 && fInitialChildCapacity <= 512)
                    {
                        pShape = new CLuaPhysicsCompoundShape(pPhysics, fInitialChildCapacity);
                    }
                    else
                    {
                        argStream.SetCustomError("Initial capacity should be between 0 and 512");
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
                    if (vecList.size() >= 3)
                    {
                        pShape = new CLuaPhysicsConvexHullShape(pPhysics, vecList);
                    }
                    else
                    {
                        argStream.SetCustomError("Convex hull shape require at least 3 vertices");
                        break;
                    }
                }
                break;
            case PHYSICS_SHAPE_TRIANGLE_MESH:
                while (argStream.NextIsTable())
                {
                    vecFloat.clear();

                    argStream.ReadNumberTable(vecFloat);
                    index++;
                    if (vecFloat.size() != 3)
                    {
                        argStream.SetCustomError(SString("Triangle mesh vertex at index %i does not have 3 float numbers", index).c_str());
                        break;
                    }

                    if (std::abs(vecFloat[0]) > MAXIMUM_PRIMITIVE_SIZE || std::abs(vecFloat[1]) > MAXIMUM_PRIMITIVE_SIZE ||
                        std::abs(vecFloat[2]) > MAXIMUM_PRIMITIVE_SIZE)
                    {
                        argStream.SetCustomError(
                            SString("Triangle mesh vertex at index %i is outside maximum primivie size %.2f.", index, MAXIMUM_PRIMITIVE_SIZE).c_str());
                        break;
                    }

                    vecList.emplace_back(vecFloat[0], vecFloat[1], vecFloat[2]);
                }
                if (!argStream.HasErrors())
                {
                    if (vecList.size() < 3)
                    {
                        argStream.SetCustomError("Triangle mesh require at least 3 vertices");
                        break;
                    }
                    if (vecList.size() % 3 != 0)
                    {
                        argStream.SetCustomError("Triangle mesh needs vertices count divisible by 3");
                        break;
                    }
                    pShape = new CLuaPhysicsTriangleMeshShape(pPhysics, vecList);
                }
                break;
            case PHYSICS_SHAPE_HEIGHTFIELD_TERRAIN:
                argStream.ReadNumber(iSizeX);
                argStream.ReadNumber(iSizeY);
                if (argStream.NextIsTable())
                    argStream.ReadNumberTable(vecFloat);
                else            // fill with empty table
                    for (int i = 0; i < iSizeX * iSizeY; i++)
                        vecFloat.emplace_back(0);

                if (!argStream.HasErrors())
                {
                    if (iSizeX >= 3 && iSizeY >= 3 && iSizeX <= 8192 && iSizeY <= 8192)
                    {
                        if (vecFloat.size() == iSizeX * iSizeY)
                        {
                            pShape = new CLuaPhysicsHeightfieldTerrainShape(pPhysics, iSizeX, iSizeY, vecFloat);
                        }
                        else
                        {
                            argStream.SetCustomError(
                                SString("Heigthfield of size %ix%i require %i floats, got %i floats", iSizeX, iSizeY, iSizeX * iSizeY, vecFloat.size())
                                    .c_str());
                        }
                    }
                    else
                    {
                        argStream.SetCustomError(
                            SString("Size of heghtfield terrain must be between 3x3 and 8192x8192, got size %ix%i", iSizeX, iSizeY, vecFloat.size()).c_str());
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
    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    }

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaPhysicsDefs::PhysicsDestroy(CLuaPhysicsElement* physicsElement)
{
    physicsElement->Destroy();
    return true;
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
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eDebugMode);
    if (!argStream.HasErrors())
    {
        if (eDebugMode == PHYSICS_DEBUG_LINE_WIDTH)
        {
            float fWidth;
            argStream.ReadNumber(fWidth);
            if (!argStream.HasErrors())
            {
                pPhysics->SetDebugLineWidth(fWidth);
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
        else
        {
            bool bEnabled;
            argStream.ReadBool(bEnabled);
            if (!argStream.HasErrors())
            {
                if (pPhysics->SetDebugMode(eDebugMode, bEnabled))
                {
                    lua_pushboolean(luaVM, true);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaPhysicsDefs::PhysicsGetDebugMode(lua_State* luaVM)
{
    CClientPhysics*   pPhysics;
    ePhysicsDebugMode eDebugMode;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pPhysics);
    argStream.ReadEnumString(eDebugMode);
    if (!argStream.HasErrors())
    {
        if (eDebugMode == PHYSICS_DEBUG_LINE_WIDTH)
        {
            lua_pushnumber(luaVM, pPhysics->GetDebugLineWidth());
            return 1;
        }
        else
        {
            lua_pushboolean(luaVM, pPhysics->GetDebugMode(eDebugMode));
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaPhysicsDefs::PhysicsDrawDebug(CClientPhysics* pPhysics)
{
    pPhysics->DrawDebug();
    return true;
}

int CLuaPhysicsDefs::PhysicsShapeCast(lua_State* luaVM)
{
    CLuaPhysicsShape* pShape;
    CVector           vecStartPosition, vecStartRotation;
    CVector           vecEndPosition, vecEndRotation;
    CScriptArgReader  argStream(luaVM);
    argStream.ReadUserData(pShape);
    argStream.ReadVector3D(vecStartPosition);
    argStream.ReadVector3D(vecEndPosition);
    argStream.ReadVector3D(vecStartRotation, CVector(0, 0, 0));
    argStream.ReadVector3D(vecEndRotation, CVector(0, 0, 0));

    if (!argStream.HasErrors())
    {
        switch (pShape->GetType())
        {
            case BOX_SHAPE_PROXYTYPE:
            case SPHERE_SHAPE_PROXYTYPE:
            case CONE_SHAPE_PROXYTYPE:
            case CYLINDER_SHAPE_PROXYTYPE:
                break;
            default:
                m_pScriptDebugging->LogCustom(luaVM, SString("Shape casting does not support %s shape type.", pShape->GetName()).c_str());
                lua_pushboolean(luaVM, false);
                return 1;
        }
        CClientPhysics* pPhysics = pShape->GetPhysics();
        btTransform     startTransform;
        btTransform     endTransform;
        startTransform.setIdentity();
        endTransform.setIdentity();
        CLuaPhysicsSharedLogic::SetPosition(startTransform, vecStartPosition);
        CLuaPhysicsSharedLogic::SetRotation(startTransform, vecStartRotation);
        CLuaPhysicsSharedLogic::SetPosition(endTransform, vecEndPosition);
        CLuaPhysicsSharedLogic::SetRotation(endTransform, vecEndRotation);

        btVector3                                     from = btVector3(vecStartPosition.fX, vecStartPosition.fY, vecStartPosition.fZ);
        btVector3                                     to = btVector3(vecEndPosition.fX, vecEndPosition.fY, vecEndPosition.fZ);
        btCollisionWorld::ClosestConvexResultCallback result(from, to);
        pPhysics->ShapeCast(pShape, startTransform, endTransform, result);
        lua_newtable(luaVM);
        lua_pushstring(luaVM, "hit");
        lua_pushboolean(luaVM, result.hasHit());
        lua_settable(luaVM, -3);
        if (result.hasHit())
        {
            btVector3    vecShapePosition = from.lerp(to, result.m_closestHitFraction);
            btQuaternion startQuaternion = startTransform.getRotation();
            btQuaternion endQuaternion = endTransform.getRotation();
            btQuaternion shapeQuaternion = startQuaternion.slerp(endQuaternion, result.m_closestHitFraction);

            btVector3 vecShapeRotation;
            CLuaPhysicsSharedLogic::QuaternionToEuler(shapeQuaternion, vecShapeRotation);

            lua_pushstring(luaVM, "shapeposition");
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vecShapePosition.getX());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vecShapePosition.getY());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vecShapePosition.getZ());
            lua_settable(luaVM, -3);
            lua_settable(luaVM, -3);

            lua_pushstring(luaVM, "shaperotation");
            lua_newtable(luaVM);
            lua_pushnumber(luaVM, 1);
            lua_pushnumber(luaVM, vecShapeRotation.getX());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 2);
            lua_pushnumber(luaVM, vecShapeRotation.getY());
            lua_settable(luaVM, -3);
            lua_pushnumber(luaVM, 3);
            lua_pushnumber(luaVM, vecShapeRotation.getZ());
            lua_settable(luaVM, -3);
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

            const btCollisionObject* pCollisionObject = result.m_hitCollisionObject;
            const btCollisionShape*  pShape = pCollisionObject->getCollisionShape();
            const btRigidBody*       pRigidBody = btRigidBody::upcast(pCollisionObject);

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
    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

CLuaPhysicsRigidBody* CLuaPhysicsDefs::PhysicsCreateRigidBody(CLuaPhysicsShape* pShape, std::optional<float> fMass, std::optional<CVector> vecLocalInertia,
                                                              std::optional<CVector> vecCenterOfMass)
{
    if (pShape->GetType() == BroadphaseNativeTypes::TERRAIN_SHAPE_PROXYTYPE)
        throw std::invalid_argument("Terrain shape is not supported");

    if (fMass <= 0)
        throw std::invalid_argument("Mass must bet greater than 0");

    CClientPhysics*       pPhysics = pShape->GetPhysics();
    std::unique_ptr<CLuaPhysicsRigidBody> pRigidBody = std::make_unique<CLuaPhysicsRigidBody>(pShape, fMass.value_or(1.0f), vecLocalInertia.value_or(CVector(0, 0, 0)), vecCenterOfMass.value_or(CVector(0, 0, 0)));
    CLuaPhysicsRigidBody* pRigidBodyPtr = pRigidBody.get();
    pPhysics->AddRigidBody(std::move(pRigidBody));
    return pRigidBodyPtr;
}

CLuaPhysicsShape* CLuaPhysicsDefs::PhysicsCreateShapeFromModel(CClientPhysics* pPhysics, unsigned short usModel)
{
//    return pPhysics->CreateShapeFromModel(usModel);
    return nullptr;
}

CLuaPhysicsStaticCollision* CLuaPhysicsDefs::PhysicsCreateStaticCollision(CLuaPhysicsShape* pShape)
{
    CClientPhysics*             pPhysics = pShape->GetPhysics();
    CLuaPhysicsStaticCollision* pStaticCollision = new CLuaPhysicsStaticCollision(pShape);
    return pStaticCollision;
}

bool CLuaPhysicsDefs::PhysicsAddChildShape(CLuaPhysicsCompoundShape* pCompoundShape, CLuaPhysicsShape* pShape, std::optional<CVector> vecPosition, std::optional<CVector> vecRotation)
{
    if (pCompoundShape->GetPhysics() != pShape->GetPhysics())
        throw std::invalid_argument("Shapes need to belong to the same physics world");

    if (pShape->GetType() == COMPOUND_SHAPE_PROXYTYPE)
        throw std::invalid_argument("Shape can not be compound");

    pCompoundShape->AddShape(pShape, vecPosition.value_or(CVector(0, 0, 0)), vecRotation.value_or(CVector(0, 0, 0)));
    return true;
}

std::vector<CLuaPhysicsShape*> CLuaPhysicsDefs::PhysicsGetChildShapes(CLuaPhysicsCompoundShape* pCompoundShape)
{
    return pCompoundShape->GetChildShapes();
}

bool CLuaPhysicsDefs::PhysicsRemoveChildShape(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex)
{
    if (pCompoundShape->RemoveChildShape(iIndex))
        return true;

    throw std::invalid_argument("Invalid shape index");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetPosition(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex)
{
    CVector position, rotation;
    if (pCompoundShape->GetChildShapeOffsets(iIndex, position, rotation))
        return position;

    throw std::invalid_argument("Invalid shape index");
}

CVector CLuaPhysicsDefs::PhysicsGetChildShapeOffsetRotation(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex)
{
    CVector position, rotation;
    if (pCompoundShape->GetChildShapeOffsets(iIndex, position, rotation))
        return rotation;

    throw std::invalid_argument("Invalid shape index");
}

bool CLuaPhysicsDefs::PhysicsSetChildShapeOffsets(CLuaPhysicsCompoundShape* pCompoundShape, int iIndex, CVector position, CVector rotation)
{
    if (pCompoundShape->SetChildShapeOffsets(iIndex, position, rotation))
        return true;

    throw std::invalid_argument("Invalid shape index");
}

bool CLuaPhysicsDefs::PhysicsApplyVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative)
{
    if (vecRelative.has_value())
        pRigidBody->ApplyImpulse(vecVelocity, vecRelative.value());
    else
        pRigidBody->ApplyCentralImpulse(vecVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity, std::optional<CVector> vecRelative)
{
    if (vecRelative.has_value())
        pRigidBody->ApplyForce(vecVelocity, vecRelative.value());
    else
        pRigidBody->ApplyCentralForce(vecVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyDamping(CLuaPhysicsRigidBody* pRigidBody, float fDamping)
{
    pRigidBody->ApplyDamping(fDamping);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyAngularVelocityForce(CLuaPhysicsRigidBody* pRigidBody, CVector vecVelocity)
{
    pRigidBody->ApplyTorque(vecVelocity);
    return true;
}

bool CLuaPhysicsDefs::PhysicsApplyAngularVelocity(CLuaPhysicsRigidBody* pRigidBody, CVector vecAngularVelocity)
{
    pRigidBody->ApplyTorqueImpulse(vecAngularVelocity);
    return true;
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
                case PHYSICS_PROPERTY_SIMULATION_ENABLED:
                    argStream.ReadBool(boolean);
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
                case PHYSICS_PROPERTY_SLEEP:
                    argStream.ReadBool(boolean);
                    if (!argStream.HasErrors())
                    {
                        if (!boolean)
                        {
                            pRigidBody->Activate();
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_MASS:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] >= 0)
                        {
                            pRigidBody->SetMass(floatNumber[0]);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Mass can not be negative");
                        }
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
                case PHYSICS_PROPERTY_VELOCITY:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetLinearVelocity(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                case PHYSICS_PROPERTY_ANGULAR_VELOCITY:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        pRigidBody->SetAngularVelocity(vector);
                        lua_pushboolean(luaVM, true);
                        return 1;
                    }
                case PHYSICS_PROPERTY_SLEEPING_THRESHOLDS:
                    argStream.ReadNumber(floatNumber[0]);
                    argStream.ReadNumber(floatNumber[1]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] >= 0 && floatNumber[1] >= 0)
                        {
                            pRigidBody->SetSleepingThresholds(floatNumber[0], floatNumber[1]);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Sleeping thresholds can not be negative");
                        }
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
                        if (vector.fX >= 0 && vector.fY >= 0 && vector.fZ >= 0)
                        {
                            pRigidBody->SetScale(vector);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Scale can not be negative");
                        }
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
                        if (floatNumber[0] > MINIMUM_PRIMITIVE_SIZE && floatNumber[0] <= MAXIMUM_PRIMITIVE_SIZE)
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
                        if (vector.fX >= 0 && vector.fY >= 0 && vector.fZ >= 0)
                        {
                            pStaticCollision->SetScale(vector);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Scale can not be negative");
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    if (argStream.NextIsBool())
                    {
                        argStream.ReadBool(boolean);
                        if (boolean == false)
                        {
                            pStaticCollision->RemoveDebugColor();
                        }
                    }
                    else
                    {
                        argStream.ReadColor(color);
                        if (!argStream.HasErrors())
                        {
                            pStaticCollision->SetDebugColor(color);
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
                    if (argStream.NextIsVector3D())
                        argStream.ReadVector3D(vector);
                    else
                    {
                        argStream.ReadNumber(floatNumber[0]);
                        vector.fX = floatNumber[0];
                        vector.fY = floatNumber[0];
                        vector.fZ = floatNumber[0];
                    }
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= MINIMUM_PRIMITIVE_SIZE && vector.fY >= MINIMUM_PRIMITIVE_SIZE && vector.fZ >= MINIMUM_PRIMITIVE_SIZE)
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
                                SString("Minimum width, height and length must be equal or greater than %.02f units", MINIMUM_PRIMITIVE_SIZE).c_str());
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_RADIUS:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] < MINIMUM_PRIMITIVE_SIZE)
                        {
                            argStream.SetCustomError(SString("Radius of sphere can not be smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE));
                            break;
                        }
                        if (floatNumber[0] > MAXIMUM_PRIMITIVE_SIZE)
                        {
                            argStream.SetCustomError(SString("Radius of sphere can not be greater than %.02f units", MAXIMUM_PRIMITIVE_SIZE));
                            break;
                        }
                        if (pShape->SetRadius(floatNumber[0]))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError(SString("Shape '%s' does not support radius property", pShape->GetType()));
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_HEIGHT:
                    argStream.ReadNumber(floatNumber[0]);
                    if (!argStream.HasErrors())
                    {
                        if (floatNumber[0] < MINIMUM_PRIMITIVE_SIZE)
                        {
                            argStream.SetCustomError(SString("Height of sphere can not be smaller than %.02f units", MINIMUM_PRIMITIVE_SIZE));
                            break;
                        }
                        if (floatNumber[0] > MAXIMUM_PRIMITIVE_SIZE)
                        {
                            argStream.SetCustomError(SString("Height of sphere can not be greater than %.02f units", MAXIMUM_PRIMITIVE_SIZE));
                            break;
                        }
                        if (pShape->SetHeight(floatNumber[0]))
                        {
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError(SString("Shape '%s' does not support height property", pShape->GetType()));
                        }
                    }
                    break;
                case PHYSICS_PROPERTY_SCALE:
                    argStream.ReadVector3D(vector);
                    if (!argStream.HasErrors())
                    {
                        if (vector.fX >= 0 && vector.fY >= 0 && vector.fZ >= 0)
                        {
                            pShape->SetScale(vector);
                            lua_pushboolean(luaVM, true);
                            return 1;
                        }
                        else
                        {
                            argStream.SetCustomError("Scale can not be negative");
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
                case PHYSICS_PROPERTY_SIMULATION_ENABLED:
                    pPhysics->GetSimulationEnabled(boolean);
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                case PHYSICS_PROPERTY_USE_CONTINOUS:
                    boolean = pPhysics->GetUseContinous();
                    lua_pushboolean(luaVM, boolean);
                    return 1;
                case PHYSICS_PROPERTY_SUBSTEPS:
                    pPhysics->GetSubSteps(i);
                    lua_pushnumber(luaVM, i);
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
                case PHYSICS_PROPERTY_VELOCITY:
                    pRigidBody->GetAngularVelocity(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_ANGULAR_VELOCITY:
                    pRigidBody->GetAngularVelocity(vector);
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
                case PHYSICS_PROPERTY_MASS:
                    floatNumber[0] = pRigidBody->GetMass();
                    lua_pushnumber(luaVM, floatNumber[0]);
                    return 1;
                default:
                    argStream.SetCustomError(SString("Physics rigid body does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pStaticCollision)
        {
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_DEBUG_COLOR:
                    pRigidBody->GetDebugColor(color);
                    lua_pushnumber(luaVM, color.R);
                    lua_pushnumber(luaVM, color.G);
                    lua_pushnumber(luaVM, color.B);
                    return 3;
                case PHYSICS_PROPERTY_POSITION:
                    pStaticCollision->GetPosition(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_ROTATION:
                    pStaticCollision->GetRotation(vector);
                    lua_pushnumber(luaVM, vector.fX);
                    lua_pushnumber(luaVM, vector.fY);
                    lua_pushnumber(luaVM, vector.fZ);
                    return 3;
                case PHYSICS_PROPERTY_SCALE:
                    if (pStaticCollision->GetScale(vector))
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
                default:
                    argStream.SetCustomError(SString("Physics static collision does not support %s property.", EnumToString(eProperty).c_str()));
                    break;
            }
        }
        else if (pShape)
        {
            btCollisionShape* pBtShape;
            switch (eProperty)
            {
                case PHYSICS_PROPERTY_SIZE:
                    if (pShape->GetSize(vector))
                    {
                        pShape->GetMargin(floatNumber[0]);
                        lua_pushnumber(luaVM, vector.fX + floatNumber[0]);
                        lua_pushnumber(luaVM, vector.fY + floatNumber[0]);
                        lua_pushnumber(luaVM, vector.fZ + floatNumber[0]);
                        return 3;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support size property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_RADIUS:
                    if (pShape->GetRadius(floatNumber[0]))
                    {
                        lua_pushnumber(luaVM, floatNumber[0]);
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support radius property", pShape->GetType()));
                    }
                    break;
                case PHYSICS_PROPERTY_HEIGHT:
                    if (pShape->GetHeight(floatNumber[0]))
                    {
                        lua_pushnumber(luaVM, floatNumber[0]);
                        return 1;
                    }
                    else
                    {
                        argStream.SetCustomError(SString("Shape '%s' does not support height property", pShape->GetType()));
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


CLuaPhysicsPointToPointConstraint* CLuaPhysicsDefs::PhysicsCreatePointToPointConstraintVariantB(ePhysicsConstraint    eConstraint,
                                                                                                CLuaPhysicsRigidBody* pRigidBody, CVector& position,
                                                                                                CVector&            anchor,
                                                                                                std::optional<bool> bDisableCollisionsBetweenLinkedBodies)
{
    if (eConstraint != ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT)
        throw std::invalid_argument("Invalid constraint type");

    std::unique_ptr<CLuaPhysicsPointToPointConstraint> pConstraint =
        std::make_unique<CLuaPhysicsPointToPointConstraint>(pRigidBody, anchor, bDisableCollisionsBetweenLinkedBodies.value_or(true));
    CLuaPhysicsPointToPointConstraint* pConstraintPtr = pConstraint.get();
    pRigidBody->GetPhysics()->AddConstraint(std::move(pConstraint));
    return pConstraintPtr;
}

CLuaPhysicsPointToPointConstraint* CLuaPhysicsDefs::PhysicsCreatePointToPointConstraintVariantA(ePhysicsConstraint eConstraint, CLuaPhysicsRigidBody* pRigidBodyA,
                                                                                        CLuaPhysicsRigidBody* pRigidBodyB, CVector& anchorA, CVector& anchorB,
                                                                                        std::optional<bool> bDisableCollisionsBetweenLinkedBodies)
{
    if (eConstraint != ePhysicsConstraint::PHYSICS_CONTRAINT_POINTTOPOINT)
        throw std::invalid_argument("Invalid constraint type");

    if (pRigidBodyA->GetPhysics() != pRigidBodyB->GetPhysics())
        throw std::invalid_argument("Rigid bodies need to belong to the same physics world");

    std::unique_ptr<CLuaPhysicsPointToPointConstraint> pConstraint = std::make_unique<CLuaPhysicsPointToPointConstraint>(pRigidBodyA, pRigidBodyB, anchorA, anchorB, bDisableCollisionsBetweenLinkedBodies.value_or(true));
    CLuaPhysicsPointToPointConstraint* pConstraintPtr = pConstraint.get();
    pRigidBodyA->GetPhysics()->AddConstraint(std::move(pConstraint));
    return pConstraintPtr;
    //CClientPhysics*       pPhysics;
    //bool                  bDisableCollisionsBetweenLinkedBodies = true;
    //CLuaPhysicsRigidBody* pRigidBodyA;
    //CLuaPhysicsRigidBody* pRigidBodyB = nullptr;
    //ePhysicsConstraint    eConstraint;
    //CScriptArgReader      argStream(luaVM);
    //argStream.ReadEnumString(eConstraint);
    //if (argStream.NextIsBool())
    //    argStream.ReadBool(bDisableCollisionsBetweenLinkedBodies, true);

    //argStream.ReadUserData(pRigidBodyA);
    //if (argStream.NextIsUserDataOfType<CLuaPhysicsRigidBody>())
    //    argStream.ReadUserData(pRigidBodyB);

    //if (pRigidBodyA && pRigidBodyB)
    //{
    //    if (pRigidBodyA->GetPhysics() != pRigidBodyB->GetPhysics())
    //    {
    //        argStream.SetCustomError("Rigid bodies need to belong to the same physics world");
    //    }
    //}
    //if (!argStream.HasErrors())
    //{
    //    pPhysics = pRigidBodyA->GetPhysics();
    //    CLuaPhysicsConstraint* pConstraint = nullptr;
    //    CVector                vector[4];
    //    switch (eConstraint)
    //    {
    //        case PHYSICS_CONTRAINT_HIDGE:
    //            argStream.ReadVector3D(vector[0]);
    //            argStream.ReadVector3D(vector[1]);
    //            argStream.ReadVector3D(vector[2]);
    //            argStream.ReadVector3D(vector[3]);
    //            pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
    //            pConstraint->CreateHidgeConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
    //            break;
    //        case PHYSICS_CONTRAINT_FIXED:
    //            if (pRigidBodyA && pRigidBodyB)
    //            {
    //                argStream.ReadVector3D(vector[0]);
    //                argStream.ReadVector3D(vector[1]);
    //                argStream.ReadVector3D(vector[2]);
    //                argStream.ReadVector3D(vector[3]);
    //                pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
    //                pConstraint->CreateFixedConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
    //            }
    //            else
    //            {
    //                argStream.SetCustomError("Physics fixed constraint requires both rigid bodies");
    //            }
    //            break;
    //        case PHYSICS_CONTRAINT_SLIDER:
    //            argStream.ReadVector3D(vector[0]);
    //            argStream.ReadVector3D(vector[1]);
    //            argStream.ReadVector3D(vector[2]);
    //            argStream.ReadVector3D(vector[3]);
    //            pConstraint = pPhysics->CreateConstraint(pRigidBodyA, pRigidBodyB);
    //            pConstraint->CreateSliderConstraint(vector[0], vector[1], vector[2], vector[3], bDisableCollisionsBetweenLinkedBodies);
    //            break;
    //    }
    //    if (pConstraint)
    //    {
    //        lua_pushconstraint(luaVM, pConstraint);
    //        return 1;
    //    }
    //}

    //if (argStream.HasErrors())
    //    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    //// Failed
    //lua_pushboolean(luaVM, false);
    //return 1;
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
                    const btCollisionShape*  pShape = pCollisionObject->getCollisionShape();
                    const btRigidBody*       pRigidBody = btRigidBody::upcast(pCollisionObject);

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

std::vector<CLuaPhysicsShape*> CLuaPhysicsDefs::PhysicsGetShapes(CClientPhysics* pPhysics)
{
    return pPhysics->GetShapes();
}

std::vector<CLuaPhysicsRigidBody*> CLuaPhysicsDefs::PhysicsGetRigidBodies(CClientPhysics* pPhysics)
{
    return pPhysics->GetRigidBodies();
}

std::vector<CLuaPhysicsStaticCollision*> CLuaPhysicsDefs::PhysicsGetStaticCollisions(CClientPhysics* pPhysics)
{
    return pPhysics->GetStaticCollisions();
}
std::vector<CLuaPhysicsConstraint*> CLuaPhysicsDefs::PhysicsGetConstraints(CClientPhysics* pPhysics)
{
    return pPhysics->GetConstraints();
}

std::string CLuaPhysicsDefs::PhysicsGetElementType(CLuaPhysicsElement* pPhysicsElement)
{
    switch (pPhysicsElement->GetClassType())
    {
        case EIdClassType::RIGID_BODY:
            return "rigidbody";
        case EIdClassType::SHAPE:
            return "staticcollision";
        case EIdClassType::STATIC_COLLISION:
            return "constraint";
        case EIdClassType::CONSTRAINT:
            return "shape";
        default:
            return "unknown";
    }
}

bool CLuaPhysicsDefs::IsPhysicsElement(CLuaPhysicsElement* pPhysicsElement)
{
    return true;
}

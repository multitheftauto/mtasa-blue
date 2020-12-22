/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBvhTriangleMeshShape.cpp
 *  PURPOSE:     Lua physics triangle mesh shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsBvhTriangleMeshShape.h"
#include "CLuaPhysicsShapeManager.h"

CLuaPhysicsBvhTriangleMeshShape::CLuaPhysicsBvhTriangleMeshShape(CClientPhysics* pPhysics, std::vector<CVector>& vecVertices)
    : CLuaPhysicsShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateBvhTriangleMesh(vecVertices)))
{
    m_verticesCount = vecVertices.size();
}

CLuaPhysicsBvhTriangleMeshShape::~CLuaPhysicsBvhTriangleMeshShape()
{
}

void CLuaPhysicsBvhTriangleMeshShape::Update()
{
    std::lock_guard guard(m_lock);
    m_bNeedsUpdate = false;

    InternalUpdateVerticesPositions();
}

void CLuaPhysicsBvhTriangleMeshShape::InternalUpdateVerticesPositions()
{
    if (m_vecVerticesUpdate.size() == 0)
        return;

    btBvhTriangleMeshShape*  pShape = (btBvhTriangleMeshShape*)GetBtShape();
    btStridingMeshInterface* pMeshInterface = pShape->getMeshInterface();

    CVector*       vertexbase = 0;
    int*           indexbase = 0;
    int            numverts;
    int            vertexStride;
    int            indexstride;
    int            numfaces;
    PHY_ScalarType scalarType;
    PHY_ScalarType indicesScalarType;

    pMeshInterface->getLockedVertexIndexBase((unsigned char**)&vertexbase, numverts, scalarType, vertexStride, (unsigned char**)&indexbase, indexstride,
                                             numfaces, indicesScalarType, 0);

    if (scalarType == PHY_ScalarType::PHY_FLOAT)
    {
        for (auto const& update : m_vecVerticesUpdate)
        {
            vertexbase[update.vertex] = update.position;
        }
    }

    pShape->recalcLocalAabb();
    m_vecVerticesUpdate.clear();
}

void CLuaPhysicsBvhTriangleMeshShape::SetVertexPosition(int iVertex, CVector vecPosition)
{
    std::lock_guard guard(m_lock);
    m_vecVerticesUpdate.emplace_back(iVertex, vecPosition);

    NeedsUpdate();
}

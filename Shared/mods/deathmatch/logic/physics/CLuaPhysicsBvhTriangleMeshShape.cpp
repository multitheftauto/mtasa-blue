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

CLuaPhysicsBvhTriangleMeshShape::CLuaPhysicsBvhTriangleMeshShape(CBulletPhysics* pPhysics, std::vector<CVector>& vecVertices)
    : CLuaPhysicsConcaveShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateBvhTriangleMesh(vecVertices)))
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

STriangleInfo CLuaPhysicsBvhTriangleMeshShape::GetTriangleInfo(int iTriangleIndex)
{
    btBvhTriangleMeshShape*  pShape = (btBvhTriangleMeshShape*)GetBtShape();
    btStridingMeshInterface* pMeshInterface = pShape->getMeshInterface();

    const CVector4D* vertexbase = 0;
    const int*     indexbase = 0;
    int            numverts;
    int            vertexStride;
    int            indexstride;
    int            numfaces;
    PHY_ScalarType scalarType;
    PHY_ScalarType indicesScalarType;

    pMeshInterface->getLockedReadOnlyVertexIndexBase((const unsigned char**)&vertexbase, numverts, scalarType, vertexStride, (const unsigned char**)&indexbase,
                                                     indexstride, numfaces, indicesScalarType, 0);

    if (scalarType == PHY_ScalarType::PHY_FLOAT)
        if (indicesScalarType == PHY_ScalarType::PHY_INTEGER)
        {
            int vertex1 = indexbase[iTriangleIndex * 3];
            int vertex2 = indexbase[iTriangleIndex * 3 + 1];
            int vertex3 = indexbase[iTriangleIndex * 3 + 2];

            STriangleInfo triangleInfo = STriangleInfo();
            triangleInfo.vertex1 = vertex1;
            triangleInfo.vertex2 = vertex2;
            triangleInfo.vertex3 = vertex3;
            triangleInfo.vecVertex1 = vertexbase[vertex1];
            triangleInfo.vecVertex2 = vertexbase[vertex2];
            triangleInfo.vecVertex3 = vertexbase[vertex3];
            return triangleInfo;
        }
}

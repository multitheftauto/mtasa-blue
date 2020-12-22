/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBhvTriangleMeshShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsBvhTriangleMeshShape;

#pragma once

// Define includes
#include "../LuaCommon.h"
#include "../CLuaArguments.h"
#include "CLuaPhysicsShape.h"

class CLuaPhysicsBvhTriangleMeshShape : public CLuaPhysicsShape
{
    struct SVertexUpdate
    {
        SVertexUpdate(int vertex, CVector position) : vertex(vertex), position(position) {}
        int     vertex;
        CVector position;
    };

public:
    CLuaPhysicsBvhTriangleMeshShape(CClientPhysics* pPhysics, std::vector<CVector>& vecVertices);
    ~CLuaPhysicsBvhTriangleMeshShape();

    void SetVertexPosition(int iVertex, CVector vecPosition);

    void Update();

    int GetVerticesNum() const { return m_verticesCount; }

private:
    void InternalUpdateVerticesPositions();

    int                        m_verticesCount = 0;
    std::vector<SVertexUpdate> m_vecVerticesUpdate;
    mutable std::mutex         m_lock;
};

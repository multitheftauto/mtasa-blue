/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColModelSA.h
 *  PURPOSE:     Header file for collision model entity class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <game/CColModel.h>
#include <game/CColPoint.h>
#include "CCompressedVectorSA.h"

#define FUNC_CColModel_Constructor      0x40FB60
#define FUNC_CColModel_Destructor       0x40F700

struct CBoxSA
{
    CVector m_vecMin;
    CVector m_vecMax;
};
static_assert(sizeof(CBoxSA) == 0x18, "Invalid size for CBoxSA");

struct CBoundingBoxSA : CBoxSA
{
};
static_assert(sizeof(CBoundingBoxSA) == 0x18, "Invalid size for CBoundingBoxSA");

struct CColBoxSA : CBoxSA
{
    EColSurface  m_material;
    std::uint8_t m_flags;
    CColLighting m_lighting;
    std::uint8_t m_brightness;
};
static_assert(sizeof(CColBoxSA) == 0x1C, "Invalid size for CColBoxSA");

struct CColLineSA
{
    CVector m_vecStart;
    float   m_startSize;
    CVector m_vecStop;
    float   m_stopSize;
};
static_assert(sizeof(CColLineSA) == 0x20, "Invalid size for CColLineSA");

struct CColDiskSA
{
    CVector      m_startPosition;
    float        m_startRadius;
    std::uint8_t m_material;
    std::uint8_t m_piece;
    std::uint8_t m_lighting;
    CVector      m_stopPosition;
    float        m_stopRadius;
};
static_assert(sizeof(CColDiskSA) == 0x24, "Invalid size for CColDiskSA");

struct CSphereSA
{
    CVector m_center;
    float   m_radius;
};
static_assert(sizeof(CSphereSA) == 0x10, "Invalid size for CSphereSA");

struct CColSphereSA : CSphereSA
{
    union
    {
        EColSurface  m_material{};
        std::uint8_t m_collisionSlot;
    };

    union
    {
        std::uint8_t m_flags{};

        struct
        {
            std::uint8_t m_hasSpheresBoxesTriangles : 1;
            std::uint8_t m_isSingleAllocationCollisionData : 1;
            std::uint8_t m_isActive : 1;
            std::uint8_t m_flag0x08 : 1;
            std::uint8_t m_flag0x10 : 1;
            std::uint8_t m_flag0x20 : 1;
            std::uint8_t m_flag0x40 : 1;
            std::uint8_t m_flag0x80 : 1;
        };
    };

    std::uint8_t m_lighting{};
    std::uint8_t m_light{};

    CColSphereSA() = default;
    CColSphereSA(const CSphereSA& sp) :
        CSphereSA{ sp }
    {
    }
};
static_assert(sizeof(CColSphereSA) == 0x14, "Invalid size for CColSphereSA");

struct CColTriangleSA
{
    std::uint16_t m_indices[3];
    EColSurface   m_material;
    CColLighting  m_lighting;
};
static_assert(sizeof(CColTriangleSA) == 0x8, "Invalid size for CColTriangleSA");

struct CColTrianglePlaneSA
{
    CCompressedVectorSA m_normal;
    std::uint16_t       m_distance;
    std::uint8_t        m_orientation;
};
static_assert(sizeof(CColTrianglePlaneSA) == 0xA, "Invalid size for CColTrianglePlaneSA");

struct ColModelFileHeader
{
    char  version[4];
    DWORD size;
    char  name[0x18];
};

struct CColDataSA
{
    std::uint16_t m_numSpheres;
    std::uint16_t m_numBoxes;
    std::uint16_t m_numTriangles;
    std::uint8_t  m_numSuspensionLines;
    union
    {
        std::uint8_t m_flags;

        struct
        {
            std::uint8_t m_usesDisks : 1;
            std::uint8_t m_notEmpty : 1;
            std::uint8_t m_hasShadowInfo : 1;
            std::uint8_t m_hasFaceGroups : 1;
            std::uint8_t m_hasShadow : 1;
        };
    };
    CColSphereSA* m_spheres;
    CColBoxSA*    m_boxes;
    union
    {
        CColLineSA* m_suspensionLines;
        CColDiskSA* m_disks;
    };
    CCompressedVectorSA* m_vertices;
    CColTriangleSA*      m_triangles;
    CColTrianglePlaneSA* m_trianglePlanes;
    std::uint32_t        m_numShadowTriangles;
    std::uint32_t        m_numShadowVertices;
    CCompressedVectorSA* m_shadowVertices;
    CColTriangleSA*      m_shadowTriangles;
};
static_assert(sizeof(CColDataSA) == 0x30, "Invalid size for CColDataSA");

struct CColModelSAInterface
{
    CBoundingBoxSA m_bounds;
    CColSphereSA   m_sphere;
    CColDataSA*    m_data;
};
static_assert(sizeof(CColModelSAInterface) == 0x30, "Invalid size for CColModelSAInterface");

class CColModelSA : public CColModel
{
public:
    CColModelSA();
    CColModelSA(CColModelSAInterface* pInterface);
    ~CColModelSA();

    CColModelSAInterface* GetInterface() override { return m_pInterface; }
    void                  Destroy() override { delete this; }

private:
    CColModelSAInterface* m_pInterface;
    bool                  m_bDestroyInterface;
};

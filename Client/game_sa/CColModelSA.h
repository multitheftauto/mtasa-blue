/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColModelSA.h
 *  PURPOSE:     Header file for collision model entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CColModel.h>
#include "CCompressedVectorSA.h"

#define FUNC_CColModel_Constructor      0x40FB60
#define FUNC_CColModel_Destructor       0x40F700

struct CBoxSA
{
    CVector vecMin;
    CVector vecMax;
};
static_assert(sizeof(CBoxSA) == 0x18, "Invalid size for CBoxSA");

struct CBoundingBoxSA : CBoxSA
{
};
static_assert(sizeof(CBoundingBoxSA) == 0x18, "Invalid size for CBoundingBoxSA");

struct CColBoxSA : CBoxSA
{
    EColSurface  material;
    std::uint8_t flags;
    CColLighting lighting;
    std::uint8_t brightness;
};
static_assert(sizeof(CColBoxSA) == 0x1C, "Invalid size for CColBoxSA");

struct CColLineSA
{
    CVector vecStart;
    float   startSize;
    CVector vecStop;
    float   stopSize;
};
static_assert(sizeof(CColLineSA) == 0x20, "Invalid size for CColLineSA");

struct CSphereSA
{
    CVector vecCenter;
    float   radius;
};
static_assert(sizeof(CSphereSA) == 0x10, "Invalid size for CSphereSA");

struct CColSphereSA : CSphereSA
{
    union
    {
        EColSurface  material;
        std::uint8_t collisionSlot;
    };

    union
    {
        std::uint8_t flags;

        struct
        {
            std::uint8_t hasSpheresBoxesTriangles : 1;
            std::uint8_t isSingleAllocationCollisionData : 1;
            std::uint8_t flag0x04 : 1;
            std::uint8_t flag0x08 : 1;
            std::uint8_t flag0x10 : 1;
            std::uint8_t flag0x20 : 1;
            std::uint8_t flag0x40 : 1;
            std::uint8_t flag0x80 : 1;
        };
    };

    std::uint8_t lighting;
    std::uint8_t light;

    CColSphereSA()
    {
        collisionSlot = 0;
        flags = 0;
        lighting = 0;
        light = 0;
    }
};
static_assert(sizeof(CColSphereSA) == 0x14, "Invalid size for CColSphereSA");

struct CColTriangleSA
{
    std::uint16_t indices[3];
    EColSurface   material;
    CColLighting  lighting;
};
static_assert(sizeof(CColTriangleSA) == 0x8, "Invalid size for CColTriangleSA");

struct CColTrianglePlaneSA
{
    CCompressedVectorSA normal;
    std::uint16_t       distance;
    std::uint8_t        orientation;
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
    std::uint16_t        numSpheres;
    std::uint16_t        numBoxes;
    std::uint16_t        numTriangles;
    std::uint8_t         numSuspensionLines;
    std::uint8_t         flags;
    CColSphereSA*        spheres;
    CColBoxSA*           boxes;
    CColLineSA*          suspensionLines;
    CCompressedVectorSA* vertices;
    CColTriangleSA*      triangles;
    CColTrianglePlaneSA* trianglePlanes;
    std::uint32_t        numShadowTriangles;
    std::uint32_t        numShadowVertices;
    CCompressedVectorSA* shadowVertices;
    CColTriangleSA*      shadowTriangles;
};
static_assert(sizeof(CColDataSA) == 0x30, "Invalid size for CColDataSA");

struct CColModelSAInterface
{
    CBoundingBoxSA bounds;
    CColSphereSA   sphere;
    CColDataSA*    data;
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

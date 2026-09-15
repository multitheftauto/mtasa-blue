/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CRuntimeColModel_Tests.cpp
 *  PURPOSE:     Runtime collision model serializer tests
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "../../Client/mods/deathmatch/logic/CRuntimeColModel.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <utility>

namespace
{
#pragma pack(push, 1)
    struct Header
    {
        char          version[4];
        std::uint32_t size;
        char          name[24];
    };

    struct Surface
    {
        std::uint8_t material;
        std::uint8_t piece;
        std::uint8_t brightness;
        std::uint8_t lighting;
    };

    struct Bounds
    {
        float radius;
        float centerX;
        float centerY;
        float centerZ;
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;
    };

    struct Face
    {
        std::uint32_t a;
        std::uint32_t b;
        std::uint32_t c;
        Surface       surface;
    };
#pragma pack(pop)

    template <class T>
    T Read(const std::string& buffer, std::size_t offset)
    {
        T value{};
        if (offset > buffer.size() || sizeof(T) > buffer.size() - offset)
        {
            ADD_FAILURE() << "Attempted to read past the serialized collision buffer";
            return value;
        }

        std::memcpy(&value, buffer.data() + offset, sizeof(T));
        return value;
    }

    RuntimeCollision::Mesh Triangle(float offset, std::uint8_t material)
    {
        RuntimeCollision::Mesh mesh;
        mesh.material = material;
        mesh.vertices = {
            CVector(-1.0f + offset, -1.0f, 0.0f),
            CVector(1.0f + offset, -1.0f, 0.0f),
            CVector(offset, 1.0f, 0.0f),
        };
        mesh.indices = {0, 1, 2};
        return mesh;
    }

    TEST(CRuntimeColModel, SerializesMixedCollision)
    {
        RuntimeCollision::Model model;
        model.spheres.push_back({CVector(0.0f, 0.0f, 1.5f), 1.0f, 2});
        model.boxes.push_back({CVector(0.0f, 0.0f, 0.0f), CVector(2.0f, 4.0f, 1.0f), 1});
        model.meshes.push_back(Triangle(0.0f, 3));

        std::string buffer;
        std::string error;
        ASSERT_TRUE(RuntimeCollision::BuildCOLBuffer(model, buffer, error)) << error;
        ASSERT_GE(buffer.size(), sizeof(Header) + sizeof(Bounds));

        const Header header = Read<Header>(buffer, 0);
        EXPECT_EQ(std::string(header.version, 4), "COLL");
        EXPECT_EQ(header.size, buffer.size() - 8);

        const Bounds bounds = Read<Bounds>(buffer, sizeof(Header));
        EXPECT_FLOAT_EQ(bounds.minX, -1.0f);
        EXPECT_FLOAT_EQ(bounds.minY, -2.0f);
        EXPECT_FLOAT_EQ(bounds.minZ, -0.5f);
        EXPECT_FLOAT_EQ(bounds.maxX, 1.0f);
        EXPECT_FLOAT_EQ(bounds.maxY, 2.0f);
        EXPECT_FLOAT_EQ(bounds.maxZ, 2.5f);

        std::size_t offset = sizeof(Header) + sizeof(Bounds);
        EXPECT_EQ(Read<std::uint32_t>(buffer, offset), 1u);
        offset += sizeof(std::uint32_t) + 0x14;

        EXPECT_EQ(Read<std::uint32_t>(buffer, offset), 0u);
        offset += sizeof(std::uint32_t);

        EXPECT_EQ(Read<std::uint32_t>(buffer, offset), 1u);
        offset += sizeof(std::uint32_t) + 0x1C;

        EXPECT_EQ(Read<std::uint32_t>(buffer, offset), 3u);
        offset += sizeof(std::uint32_t) + 3 * 0x0C;

        EXPECT_EQ(Read<std::uint32_t>(buffer, offset), 1u);
        offset += sizeof(std::uint32_t);

        const Face face = Read<Face>(buffer, offset);
        EXPECT_EQ(face.a, 0u);
        EXPECT_EQ(face.b, 1u);
        EXPECT_EQ(face.c, 2u);
        EXPECT_EQ(face.surface.material, 3u);
        EXPECT_EQ(face.surface.lighting, 0xFFu);
    }

    TEST(CRuntimeColModel, OffsetsIndicesAcrossMeshes)
    {
        RuntimeCollision::Model model;
        model.meshes.push_back(Triangle(-3.0f, 1));
        model.meshes.push_back(Triangle(3.0f, 2));

        std::string buffer;
        std::string error;
        ASSERT_TRUE(RuntimeCollision::BuildCOLBuffer(model, buffer, error)) << error;

        std::size_t offset = sizeof(Header) + sizeof(Bounds);
        offset += sizeof(std::uint32_t);  // spheres
        offset += sizeof(std::uint32_t);  // lines
        offset += sizeof(std::uint32_t);  // boxes

        const std::uint32_t vertexCount = Read<std::uint32_t>(buffer, offset);
        EXPECT_EQ(vertexCount, 6u);
        offset += sizeof(std::uint32_t) + vertexCount * 0x0C;

        const std::uint32_t faceCount = Read<std::uint32_t>(buffer, offset);
        ASSERT_EQ(faceCount, 2u);
        offset += sizeof(std::uint32_t);

        const Face first = Read<Face>(buffer, offset);
        const Face second = Read<Face>(buffer, offset + sizeof(Face));

        EXPECT_EQ(first.a, 0u);
        EXPECT_EQ(first.b, 1u);
        EXPECT_EQ(first.c, 2u);
        EXPECT_EQ(second.a, 3u);
        EXPECT_EQ(second.b, 4u);
        EXPECT_EQ(second.c, 5u);
        EXPECT_EQ(second.surface.material, 2u);
    }

    TEST(CRuntimeColModel, RejectsMissingGeometry)
    {
        RuntimeCollision::Model model;
        std::string             buffer;
        std::string             error;

        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_FALSE(error.empty());
    }

    TEST(CRuntimeColModel, RejectsInvalidTriangleIndex)
    {
        RuntimeCollision::Model model;
        auto                    mesh = Triangle(0.0f, 0);
        mesh.indices = {0, 1, 3};
        model.meshes.push_back(std::move(mesh));

        std::string buffer;
        std::string error;
        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_NE(error.find("missing vertex"), std::string::npos);
    }

    TEST(CRuntimeColModel, RejectsRepeatedTriangleVertex)
    {
        RuntimeCollision::Model model;
        auto                    mesh = Triangle(0.0f, 0);
        mesh.indices = {0, 1, 1};
        model.meshes.push_back(std::move(mesh));

        std::string buffer;
        std::string error;
        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_NE(error.find("three different vertices"), std::string::npos);
    }

    TEST(CRuntimeColModel, RejectsCompressedVertexOverflow)
    {
        RuntimeCollision::Model model;
        auto                    mesh = Triangle(0.0f, 0);
        mesh.vertices[0].fX = 256.0f;
        model.meshes.push_back(std::move(mesh));

        std::string buffer;
        std::string error;
        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_NE(error.find("GTA collision range"), std::string::npos);
    }

    TEST(CRuntimeColModel, RejectsInvalidPrimitiveDimensions)
    {
        RuntimeCollision::Model model;
        model.spheres.push_back({CVector(), 0.0f, 0});

        std::string buffer;
        std::string error;
        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_NE(error.find("radius"), std::string::npos);
    }

    TEST(CRuntimeColModel, RejectsInvalidMaterial)
    {
        RuntimeCollision::Model model;
        model.spheres.push_back({CVector(), 1.0f, 179});

        std::string buffer;
        std::string error;
        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_NE(error.find("material"), std::string::npos);
    }

    TEST(CRuntimeColModel, RejectsNonFiniteGeometry)
    {
        RuntimeCollision::Model model;
        model.boxes.push_back({CVector(), CVector(std::numeric_limits<float>::infinity(), 1.0f, 1.0f), 0});

        std::string buffer;
        std::string error;
        EXPECT_FALSE(RuntimeCollision::BuildCOLBuffer(model, buffer, error));
        EXPECT_NE(error.find("finite"), std::string::npos);
    }
}  // namespace

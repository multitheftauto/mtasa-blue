/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRuntimeColModel.cpp
 *  PURPOSE:     Runtime collision model serialization
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef RUNTIME_COL_MODEL_STANDALONE
    #include "StdInc.h"
#endif
#include "CRuntimeColModel.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace RuntimeCollision
{
    namespace
    {
#pragma pack(push, 1)
        struct ColFileHeader
        {
            char          version[4];
            std::uint32_t size;
            char          name[24];
        };

        struct SurfaceRecord
        {
            std::uint8_t material;
            std::uint8_t piece;
            std::uint8_t brightness;
            std::uint8_t lighting;
        };

        struct BoundsRecord
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

        struct SphereRecord
        {
            float         radius;
            float         x;
            float         y;
            float         z;
            SurfaceRecord surface;
        };

        struct BoxRecord
        {
            float         minX;
            float         minY;
            float         minZ;
            float         maxX;
            float         maxY;
            float         maxZ;
            SurfaceRecord surface;
        };

        struct VertexRecord
        {
            float x;
            float y;
            float z;
        };

        struct FaceRecord
        {
            std::uint32_t a;
            std::uint32_t b;
            std::uint32_t c;
            SurfaceRecord surface;
        };
#pragma pack(pop)

        static_assert(sizeof(ColFileHeader) == 0x20);
        static_assert(sizeof(SurfaceRecord) == 0x4);
        static_assert(sizeof(BoundsRecord) == 0x28);
        static_assert(sizeof(SphereRecord) == 0x14);
        static_assert(sizeof(BoxRecord) == 0x1C);
        static_assert(sizeof(VertexRecord) == 0x0C);
        static_assert(sizeof(FaceRecord) == 0x10);

        constexpr std::size_t  kMaxNativeCount = std::numeric_limits<std::uint16_t>::max();
        constexpr std::size_t  kMaxNativeVertices = kMaxNativeCount + 1;
        constexpr std::uint8_t kMaxSurfaceMaterial = 178;
        constexpr float        kMinCompressedVertex = -256.0f;
        constexpr float        kMaxCompressedVertex = 32767.0f / 128.0f;

        bool IsFinite(float value)
        {
            return std::isfinite(value);
        }

        bool IsFinite(const CVector& value)
        {
            return IsFinite(value.fX) && IsFinite(value.fY) && IsFinite(value.fZ);
        }

        bool ValidateMaterial(std::uint8_t material, std::string& outError)
        {
            if (material <= kMaxSurfaceMaterial)
                return true;

            outError = "material must be in range 0-178";
            return false;
        }

        SurfaceRecord MakeSurface(std::uint8_t material)
        {
            return SurfaceRecord{material, 0, 0, 0xFF};
        }

        template <class T>
        void Append(std::string& buffer, const T& value)
        {
            buffer.append(reinterpret_cast<const char*>(&value), sizeof(value));
        }

        void ExtendBounds(const CVector& point, CVector& minimum, CVector& maximum, bool& initialized)
        {
            if (!initialized)
            {
                minimum = point;
                maximum = point;
                initialized = true;
                return;
            }

            minimum.fX = std::min(minimum.fX, point.fX);
            minimum.fY = std::min(minimum.fY, point.fY);
            minimum.fZ = std::min(minimum.fZ, point.fZ);
            maximum.fX = std::max(maximum.fX, point.fX);
            maximum.fY = std::max(maximum.fY, point.fY);
            maximum.fZ = std::max(maximum.fZ, point.fZ);
        }

        bool IsCompressedVertexInRange(const CVector& vertex)
        {
            return vertex.fX >= kMinCompressedVertex && vertex.fX <= kMaxCompressedVertex && vertex.fY >= kMinCompressedVertex &&
                   vertex.fY <= kMaxCompressedVertex && vertex.fZ >= kMinCompressedVertex && vertex.fZ <= kMaxCompressedVertex;
        }
    }  // namespace

    bool BuildCOLBuffer(const Model& model, std::string& outBuffer, std::string& outError)
    {
        outBuffer.clear();
        outError.clear();

        if (model.spheres.empty() && model.boxes.empty() && model.meshes.empty())
        {
            outError = "collision must contain at least one sphere, box, or mesh";
            return false;
        }

        if (model.spheres.size() > kMaxNativeCount)
        {
            outError = "too many spheres";
            return false;
        }

        if (model.boxes.size() > kMaxNativeCount)
        {
            outError = "too many boxes";
            return false;
        }

        std::size_t totalVertices = 0;
        std::size_t totalTriangles = 0;

        for (const Mesh& mesh : model.meshes)
        {
            if (!ValidateMaterial(mesh.material, outError))
                return false;

            if (mesh.vertices.empty())
            {
                outError = "mesh must contain vertices";
                return false;
            }

            if (mesh.indices.empty() || mesh.indices.size() % 3 != 0)
            {
                outError = "mesh indices must contain complete triangles";
                return false;
            }

            if (mesh.vertices.size() > kMaxNativeVertices - totalVertices)
            {
                outError = "combined mesh vertex count exceeds 65536";
                return false;
            }

            totalVertices += mesh.vertices.size();

            const std::size_t meshTriangles = mesh.indices.size() / 3;
            if (meshTriangles > kMaxNativeCount - totalTriangles)
            {
                outError = "combined triangle count exceeds 65535";
                return false;
            }

            totalTriangles += meshTriangles;
        }

        std::vector<VertexRecord> vertices;
        std::vector<FaceRecord>   faces;
        vertices.reserve(totalVertices);
        faces.reserve(totalTriangles);

        CVector boundsMin;
        CVector boundsMax;
        bool    boundsInitialized = false;

        for (const Sphere& sphere : model.spheres)
        {
            if (!IsFinite(sphere.position) || !IsFinite(sphere.radius) || sphere.radius <= 0.0f)
            {
                outError = "sphere position and radius must be finite, with radius > 0";
                return false;
            }

            if (!ValidateMaterial(sphere.material, outError))
                return false;

            CVector minimum{sphere.position.fX - sphere.radius, sphere.position.fY - sphere.radius, sphere.position.fZ - sphere.radius};
            CVector maximum{sphere.position.fX + sphere.radius, sphere.position.fY + sphere.radius, sphere.position.fZ + sphere.radius};
            if (!IsFinite(minimum) || !IsFinite(maximum))
            {
                outError = "sphere bounds overflowed";
                return false;
            }

            ExtendBounds(minimum, boundsMin, boundsMax, boundsInitialized);
            ExtendBounds(maximum, boundsMin, boundsMax, boundsInitialized);
        }

        for (const Box& box : model.boxes)
        {
            if (!IsFinite(box.position) || !IsFinite(box.size) || box.size.fX <= 0.0f || box.size.fY <= 0.0f || box.size.fZ <= 0.0f)
            {
                outError = "box position and size must be finite, with all size components > 0";
                return false;
            }

            if (!ValidateMaterial(box.material, outError))
                return false;

            const CVector half{box.size.fX * 0.5f, box.size.fY * 0.5f, box.size.fZ * 0.5f};
            CVector       minimum{box.position.fX - half.fX, box.position.fY - half.fY, box.position.fZ - half.fZ};
            CVector       maximum{box.position.fX + half.fX, box.position.fY + half.fY, box.position.fZ + half.fZ};
            if (!IsFinite(minimum) || !IsFinite(maximum))
            {
                outError = "box bounds overflowed";
                return false;
            }

            ExtendBounds(minimum, boundsMin, boundsMax, boundsInitialized);
            ExtendBounds(maximum, boundsMin, boundsMax, boundsInitialized);
        }

        std::size_t vertexBase = 0;
        for (const Mesh& mesh : model.meshes)
        {
            for (const CVector& vertex : mesh.vertices)
            {
                if (!IsFinite(vertex))
                {
                    outError = "mesh vertices must be finite";
                    return false;
                }

                if (!IsCompressedVertexInRange(vertex))
                {
                    outError = "mesh vertex coordinate exceeds GTA collision range [-256, 255.9921875]";
                    return false;
                }

                vertices.push_back(VertexRecord{vertex.fX, vertex.fY, vertex.fZ});
                ExtendBounds(vertex, boundsMin, boundsMax, boundsInitialized);
            }

            for (std::size_t index = 0; index < mesh.indices.size(); index += 3)
            {
                const std::uint32_t localA = mesh.indices[index];
                const std::uint32_t localB = mesh.indices[index + 1];
                const std::uint32_t localC = mesh.indices[index + 2];

                if (localA >= mesh.vertices.size() || localB >= mesh.vertices.size() || localC >= mesh.vertices.size())
                {
                    outError = "mesh triangle index references a missing vertex";
                    return false;
                }

                if (localA == localB || localA == localC || localB == localC)
                {
                    outError = "mesh triangle must reference three different vertices";
                    return false;
                }

                const std::size_t globalA = vertexBase + localA;
                const std::size_t globalB = vertexBase + localB;
                const std::size_t globalC = vertexBase + localC;
                if (globalA > kMaxNativeCount || globalB > kMaxNativeCount || globalC > kMaxNativeCount)
                {
                    outError = "mesh triangle index exceeds 65535";
                    return false;
                }

                faces.push_back(FaceRecord{static_cast<std::uint32_t>(globalA), static_cast<std::uint32_t>(globalB), static_cast<std::uint32_t>(globalC),
                                           MakeSurface(mesh.material)});
            }

            vertexBase += mesh.vertices.size();
        }

        if (!boundsInitialized)
        {
            outError = "collision has no bounds";
            return false;
        }

        const CVector center{static_cast<float>((static_cast<double>(boundsMin.fX) + boundsMax.fX) * 0.5),
                             static_cast<float>((static_cast<double>(boundsMin.fY) + boundsMax.fY) * 0.5),
                             static_cast<float>((static_cast<double>(boundsMin.fZ) + boundsMax.fZ) * 0.5)};
        const double  extentX = static_cast<double>(boundsMax.fX) - center.fX;
        const double  extentY = static_cast<double>(boundsMax.fY) - center.fY;
        const double  extentZ = static_cast<double>(boundsMax.fZ) - center.fZ;
        const double  radiusDouble = std::sqrt(extentX * extentX + extentY * extentY + extentZ * extentZ);

        if (!std::isfinite(radiusDouble) || radiusDouble <= 0.0 || radiusDouble > std::numeric_limits<float>::max())
        {
            outError = "collision bounds produce an invalid bounding sphere";
            return false;
        }

        const BoundsRecord bounds{static_cast<float>(radiusDouble),
                                  center.fX,
                                  center.fY,
                                  center.fZ,
                                  boundsMin.fX,
                                  boundsMin.fY,
                                  boundsMin.fZ,
                                  boundsMax.fX,
                                  boundsMax.fY,
                                  boundsMax.fZ};

        std::string payload;
        payload.reserve(sizeof(bounds) + sizeof(std::uint32_t) * 5 + model.spheres.size() * sizeof(SphereRecord) + model.boxes.size() * sizeof(BoxRecord) +
                        vertices.size() * sizeof(VertexRecord) + faces.size() * sizeof(FaceRecord));

        Append(payload, bounds);

        const std::uint32_t sphereCount = static_cast<std::uint32_t>(model.spheres.size());
        Append(payload, sphereCount);
        for (const Sphere& sphere : model.spheres)
        {
            Append(payload, SphereRecord{sphere.radius, sphere.position.fX, sphere.position.fY, sphere.position.fZ, MakeSurface(sphere.material)});
        }

        const std::uint32_t lineCount = 0;
        Append(payload, lineCount);

        const std::uint32_t boxCount = static_cast<std::uint32_t>(model.boxes.size());
        Append(payload, boxCount);
        for (const Box& box : model.boxes)
        {
            const CVector half{box.size.fX * 0.5f, box.size.fY * 0.5f, box.size.fZ * 0.5f};
            Append(payload, BoxRecord{box.position.fX - half.fX, box.position.fY - half.fY, box.position.fZ - half.fZ, box.position.fX + half.fX,
                                      box.position.fY + half.fY, box.position.fZ + half.fZ, MakeSurface(box.material)});
        }

        const std::uint32_t vertexCount = static_cast<std::uint32_t>(vertices.size());
        Append(payload, vertexCount);
        for (const VertexRecord& vertex : vertices)
            Append(payload, vertex);

        const std::uint32_t triangleCount = static_cast<std::uint32_t>(faces.size());
        Append(payload, triangleCount);
        for (const FaceRecord& face : faces)
            Append(payload, face);

        ColFileHeader header{};
        std::memcpy(header.version, "COLL", 4);
        std::memcpy(header.name, "runtime", 7);
        header.size = static_cast<std::uint32_t>(sizeof(header) - 8 + payload.size());

        outBuffer.reserve(sizeof(header) + payload.size());
        Append(outBuffer, header);
        outBuffer.append(payload);
        return true;
    }
}  // namespace RuntimeCollision

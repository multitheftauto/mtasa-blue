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

#include <windows.h>
#include <game/CColModel.h>

#define FUNC_CColModel_Constructor 0x40FB60
#define FUNC_CColModel_Destructor  0x40F700

enum class eCollisionShape;

typedef struct
{
    CVector vecMin;
    CVector vecMax;
    CVector vecOffset;
    FLOAT   fRadius;
} CBoundingBoxSA;

typedef struct
{
    CVector      vecCenter;
    float        fRadius;
    uchar        material;
    uchar        flags;
    CColLighting lighting;
    uchar        light;
} CColSphereSA;

typedef struct
{
    CVector      min;
    CVector      max;
    uchar        material;
    uchar        flags;
    CColLighting lighting;
    uchar        light;
} CColBoxSA;

typedef struct
{
    unsigned short vertex[3];
    uchar          material;
    CColLighting   lighting;
} CColTriangleSA;

typedef struct
{
    BYTE pad0[12];
} CColTrianglePlaneSA;

typedef struct
{
    char  version[4];
    DWORD size;
    char  name[0x18];
} ColModelFileHeader;

typedef struct
{
    CVector m_vecStart;
    float   m_fStartSize;
    CVector m_vecEnd;
    float   m_fEndSize;
} CColLine;

typedef struct
{
    CVector       m_vecStart;
    float         m_fStartRadius;
    unsigned char m_nMaterial;
    unsigned char m_nPiece;
    unsigned char m_nLighting;
    char          _pad13;
    CVector       m_vecEnd;
    float         m_fEndRadius;
} CColDisk;

typedef struct
{
    signed __int16 x;
    signed __int16 y;
    signed __int16 z;
    CVector        getVector() const { return CVector(x * 0.0078125f, y * 0.0078125f, z * 0.0078125f); }
    void           setVector(CVector vec)
    {
        x = static_cast<signed __int16>(vec.fX * 128);
        y = static_cast<signed __int16>(vec.fY * 128);
        z = static_cast<signed __int16>(vec.fZ * 128);
    }
} CompressedVector;

typedef struct
{
    WORD                 numColSpheres;
    WORD                 numColBoxes;
    WORD                 numColTriangles;
    BYTE                 ucNumWheels;
    BYTE                 m_nFlags;
    CColSphereSA*        pColSpheres;
    CColBoxSA*           pColBoxes;
    void*                pSuspensionLines;
    CompressedVector*    pVertices;
    CColTriangleSA*      pColTriangles;
    CColTrianglePlaneSA* pColTrianglePlanes;
    unsigned int         m_nNumShadowTriangles;
    unsigned int         m_nNumShadowVertices;
    CompressedVector*    m_pShadowVertices;
    CColTriangleSA*      m_pShadowTriangles;

    std::map<ushort, CompressedVector> getAllVertices() const
    {
        std::map<ushort, CompressedVector> vertices;
        for (uint i = 0; numColTriangles > i; i++)
        {
            vertices[pColTriangles[i].vertex[0]] = pVertices[pColTriangles[i].vertex[0]];
            vertices[pColTriangles[i].vertex[1]] = pVertices[pColTriangles[i].vertex[1]];
            vertices[pColTriangles[i].vertex[2]] = pVertices[pColTriangles[i].vertex[2]];
        }
        return vertices;
    }

    unsigned short getNumVertices() const
    {
        unsigned short numVertices = 0;
        for (uint i = 0; numColTriangles > i; i++)
        {
            numVertices = std::max(std::max(numVertices, pColTriangles[i].vertex[0]), std::max(pColTriangles[i].vertex[1], pColTriangles[i].vertex[2]));
        }
        return numVertices;
    }

    bool isValidIndex(eCollisionShape eShape, ushort usIndex, ushort numVertices = 0) const
    {
        switch (eShape)
        {
            case eCollisionShape::BOX:
                return (usIndex >= 0 && usIndex < numColBoxes);
                break;
            case eCollisionShape::SPHERE:
                return (usIndex >= 0 && usIndex < numColSpheres);
                break;
            case eCollisionShape::TRIANGLE:
                return (usIndex >= 0 && usIndex < numColTriangles);
                break;
            case eCollisionShape::VERTEX:
                return (usIndex >= 0 && usIndex < numVertices);
                break;
        }
        return false;
    }

    std::vector<ushort> getTrianglesByVertex(ushort usVertex) const
    {
        std::vector<ushort> vecTriangles;
        CColTriangleSA      colTriangle;
        for (ushort i = 0; i < numColTriangles; i++)
        {
            colTriangle = pColTriangles[i];
            if (colTriangle.vertex[0] == usVertex || colTriangle.vertex[1] == usVertex || colTriangle.vertex[2] == usVertex)
                vecTriangles.push_back(i);
        }
        return vecTriangles;
    }

    std::map<ushort, ushort> getVerticesUsage() const
    {
        std::map<ushort, ushort> verticesUsage;
        for (ushort i = 0; i < numColTriangles; i++)
            for (char cVertex = 0; cVertex < 3; cVertex++)
                verticesUsage[pColTriangles[i].vertex[cVertex]]++;
        return verticesUsage;
    }
} CColDataSA;

class CColModelSAInterface
{
public:
    CBoundingBoxSA boundingBox;
    BYTE           level;
    BYTE           unknownFlags;
    BYTE           pad[2];
    CColDataSA*    pColData;
};

class CColModelSA : public CColModel
{
public:
    CColModelSA();
    CColModelSA(CColModelSAInterface* pInterface);
    ~CColModelSA();

    CColModelSAInterface* GetInterface() { return m_pInterface; }
    void                  Destroy() { delete this; }

private:
    CColModelSAInterface* m_pInterface;
    bool                  m_bDestroyInterface;
};

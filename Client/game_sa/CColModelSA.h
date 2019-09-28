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
#define FUNC_CColModel_Destructor 0x40F700

typedef struct
{
    CVector vecMin;
    CVector vecMax;
    CVector vecOffset;
    FLOAT   fRadius;
} CBoundingBoxSA;

typedef struct
{
    CVector vecCenter;
    float   fRadius;
} CColSphereSA;

typedef struct
{
    CVector min;
    CVector max;
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
    BYTE                 pad3;
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

    size_t getNumVertices() const
    {
        std::map<ushort, bool> vertices;
        for (uint i = 0; numColTriangles > i; i++)
        {
            vertices[pColTriangles[i].vertex[0]] = true;
            vertices[pColTriangles[i].vertex[1]] = true;
            vertices[pColTriangles[i].vertex[2]] = true;
        }
        return vertices.size();
    }

    bool isValidIndex(char eShape, ushort usIndex, ushort numVertices = 0) const
    {
        switch (eShape)
        {
            case 0:
                return (usIndex >= 0 && usIndex < numColBoxes);
                break;
            case 1:
                return (usIndex >= 0 && usIndex < numColSpheres);
                break;
            case 2:
                return (usIndex >= 0 && usIndex < numColTriangles);
                break;
            case 3:
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

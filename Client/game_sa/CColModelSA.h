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

#define FUNC_CColModel_Constructor      0x40FB60
#define FUNC_CColModel_Destructor       0x40F700

typedef void *(__cdecl * hCMemoryMgr_Malloc) (unsigned int size);
auto CMemoryMgr_Malloc = (hCMemoryMgr_Malloc)0x72F420;
typedef void(__cdecl * hCMemoryMgr_Free) (void *memory);
auto CMemoryMgr_Free = (hCMemoryMgr_Free)0x72F430;

typedef struct
{
    CVector vecMin;
    CVector vecMax;
    CVector vecOffset;
    FLOAT   fRadius;
} CBoundingBoxSA;

typedef struct
{
    CVector        vecCenter;
    float          fRadius;
    uchar          material;
} CColSphereSA;

typedef struct
{
    CVector        min;
    CVector        max;
    uchar          material;
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
    CVector getVector()
    {
        return CVector(x * 0.0078125f, y * 0.0078125f, z * 0.0078125f);
    }
    void setVector(CVector vec)
    {
        x = vec.fX * 128;
        y = vec.fY * 128;
        z = vec.fZ * 128;
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

    ushort createCollisionBox(CVector vecMin, CVector vecMax, uchar cMaterial = 0)
    {
        CColBoxSA* newArr = new CColBoxSA[numColBoxes + 1];
        memcpy(newArr, pColBoxes, sizeof(CColBoxSA) * numColBoxes);

        CColBoxSA* newBox = new CColBoxSA;
        memcpy(newArr + sizeof(CColBoxSA) * numColBoxes, newBox, sizeof(CColBoxSA));

        CColBoxSA* lastBox = &newArr[numColBoxes];
        lastBox->min = vecMin;
        lastBox->max = vecMax;
        lastBox->material = cMaterial;
        numColBoxes++;
        pColBoxes = newArr;
        return numColBoxes;
    }

    ushort createCollisionSphere(CVector vecPosition, float fRadius, uchar cMaterial = 0)
    {
        CColSphereSA* newArr = new CColSphereSA[numColSpheres + 1];
        memcpy(newArr, pColSpheres, sizeof(CColSphereSA) * numColSpheres);

        CColSphereSA* newSphere = new CColSphereSA;
        memcpy(newArr + sizeof(CColSphereSA) * numColSpheres, newSphere, sizeof(CColSphereSA));

        CColSphereSA* lastSphere = &newArr[numColSpheres];
        lastSphere->vecCenter = vecPosition;
        lastSphere->material = cMaterial;
        lastSphere->fRadius = fRadius;
        numColSpheres++;
        pColSpheres = newArr;
        return numColSpheres;
    }

    ushort createCollisionTriangle(ushort usVertex1, ushort usVertex2, ushort usVertex3, CColLighting cLighting, uchar cMaterial = 0)
    {
        CColTriangleSA* newArr = reinterpret_cast<CColTriangleSA*>(CMemoryMgr_Malloc(sizeof(CColTriangleSA) * numColTriangles + sizeof(CColTriangleSA)));

        memcpy(newArr, pColTriangles, sizeof(CColTriangleSA) * numColTriangles);

        CColTriangleSA* newTriangle = new CColTriangleSA;
        memcpy(newArr + sizeof(CColTriangleSA) * numColTriangles, newTriangle, sizeof(CColTriangleSA));

        CColTriangleSA* lastTriangle = &newArr[numColTriangles];
        lastTriangle->vertex[0] = usVertex1;
        lastTriangle->vertex[1] = usVertex2;
        lastTriangle->vertex[2] = usVertex3;
        lastTriangle->lighting = cLighting;
        lastTriangle->material = cMaterial;
        numColTriangles++;
        //CMemoryMgr_Free(reinterpret_cast<void*>(pColTriangles));
        pColTriangles = newArr;
        return numColTriangles;
    }

    ushort createCollisionVertex(CVector vecPosition, uchar ucAddedSize = 0) // we don't know how many vertices already is
    {
        ushort numVertices = getNumVertices() + ucAddedSize;
        CompressedVector* newPVertices = reinterpret_cast<CompressedVector*>(CMemoryMgr_Malloc(sizeof(CompressedVector) * numVertices + sizeof(CompressedVector)));
        memcpy(newPVertices, pVertices, sizeof(CompressedVector) * numVertices);

        CompressedVector* newVertex = new CompressedVector;
        memcpy(newPVertices + sizeof(CompressedVector) * numVertices, newVertex, sizeof(CompressedVector));

        CompressedVector* lastVertex = &newPVertices[numVertices];
        lastVertex->setVector(vecPosition);
        //CMemoryMgr_Free(reinterpret_cast<void*>(pColTriangles));
        pVertices = newPVertices;
        return numVertices + 1;
    }

    std::map<ushort, CompressedVector> getAllVertices()
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
    ushort getNumVertices()
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

    inline bool checkVector(CVector &vecVector)
    {
        return ((128 < vecVector.fX > -128) && (128 < vecVector.fY > -128) && (128 < vecVector.fZ > -128));
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
    CColModelSA(void);
    CColModelSA(CColModelSAInterface* pInterface);
    ~CColModelSA(void);

    CColModelSAInterface* GetInterface(void) { return m_pInterface; }
    void                  Destroy(void) { delete this; }

private:
    CColModelSAInterface* m_pInterface;
    bool                  m_bDestroyInterface;
};

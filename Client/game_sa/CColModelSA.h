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
} CSphereSA;

typedef struct : CSphereSA
{
    unsigned char m_nMaterial;
    unsigned char m_nFlags;
    unsigned char m_nLighting;
    unsigned char m_nLight;
} CColSphereSA;

typedef struct
{
    CVector min;
    CVector max;
} CBoxSA;

typedef struct : CBoxSA
{
    unsigned char m_nMaterial;
    unsigned char m_nFlags;
    unsigned char m_nLighting;
    unsigned char m_nLight;
} CColBoxSA;

typedef struct
{
    unsigned short vertexIndex1;
    unsigned short vertexIndex2;
    unsigned short vertexIndex3;
    EColSurface    material;
    CColLighting   lighting;
} CColTriangleSA;

#pragma pack(push, 1)
typedef struct
{
public:
    short x, y, z;
} CCompressedVectorSA;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    CCompressedVectorSA m_normal;
    unsigned short m_nDistance;
    unsigned char m_nOrientation;
    char          padding;
} CColTrianglePlaneSA;
#pragma pack(pop)


typedef struct
{
public:
    CVector m_vecStart;
    float m_fStartRadius; // unused
    CVector m_vecEnd;
    float m_fEndRadius; // unused
} CColLineSA;


typedef struct
{
public:
    CVector m_vecCenter;
    float m_fOutsideRadiuss;
    unsigned char m_nMaterial;
    unsigned char m_nPiece;
    unsigned char m_nLighting;
private:
    char _pad13;
public:
    CVector m_vecRelPoint;
    float m_fInnerRadius;
} CColDiskSA;

typedef struct
{
    char  version[4];
    DWORD size;
    char  name[0x16];
    unsigned short modelId; // 0-19999
} ColModelFileHeader;

typedef struct
{
    unsigned short       numColSpheres;
    unsigned short       numColBoxes;
    unsigned short       numColTriangles;
    unsigned char        numLinesOrDisks;
    union
    {
        unsigned char m_nFlags;
        struct {
            unsigned char   bUsesDisks : 1;
            unsigned char   bNotEmpty : 1;
            unsigned char   b03 : 1;
            unsigned char   bHasFaceGroups : 1;
            unsigned char   bHasShadow : 1;
        } Flags;
    };
    CColSphereSA*        pColSpheres;
    CColBoxSA*           pColBoxes;
    union
    {
        CColLineSA*      m_pSuspensionLines;
        CColDiskSA*      m_pDisks;
    };
    CCompressedVectorSA* m_pVertices;
    CColTriangleSA*      pColTriangles;
    CColTrianglePlaneSA* pColTrianglePlanes;
    unsigned int         m_nNumShadowTriangles;
    unsigned int         m_nNumShadowVertices;
    CCompressedVectorSA  *m_pShadowVertices;
    CColTriangleSA       *m_pShadowTriangles;
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

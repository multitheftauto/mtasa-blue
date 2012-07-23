/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.h
*  PURPOSE:     Header file for collision model entity class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COLMODELSA
#define __CGAMESA_COLMODELSA

#include <windows.h>
#include <game/CColModel.h>

#define FUNC_CColModel_Constructor      0x40FB60
#define FUNC_CColModel_Destructor       0x40F700

typedef struct
{
    CVector     vecMin;
    CVector     vecMax;
    CVector     vecOffset;
    FLOAT       fRadius;
} CBoundingBoxSA;


typedef struct
{
    CVector     vecCenter;
    float       fRadius;
} CColSphereSA;


typedef struct
{
    CVector     min;
    CVector     max;
} CColBoxSA;


typedef struct
{
    unsigned short  v1;
    unsigned short  v2;
    unsigned short  v3;
    EColSurface     material;
    CColLighting    lighting;
} CColTriangleSA;


typedef struct
{
    BYTE pad0 [ 12 ];
} CColTrianglePlaneSA;


typedef struct
{
    char version[4];
    DWORD size;
    char name[0x18];
} ColModelFileHeader;

typedef struct
{
    WORD                            numColSpheres;
    WORD                            numColBoxes;
    WORD                            numColTriangles;
    BYTE                            ucNumWheels;
    BYTE                            pad3;
    CColSphereSA*                   pColSpheres;
    CColBoxSA*                      pColBoxes;
    void*                           pSuspensionLines;
    void*                           pUnknown;
    CColTriangleSA*                 pColTriangles;
    CColTrianglePlaneSA*            pColTrianglePlanes;
} CColDataSA;

class CColModelSAInterface
{
public:
    CBoundingBoxSA                  boundingBox;
    BYTE                            level;
    BYTE                            unknownFlags;
    BYTE                            pad [ 2 ];
    CColDataSA*                     pColData;
};

class CColModelSA : public CColModel
{
public:
                                    CColModelSA     ( void );
                                    CColModelSA     ( CColModelSAInterface * pInterface );
                                    ~CColModelSA    ( void );

    inline CColModelSAInterface *   GetInterface    ( void ) { return m_pInterface; }
    inline void                     Destroy         ( void ) { delete this; }

private:
    CColModelSAInterface *          m_pInterface;
    bool                            m_bDestroyInterface;
};

#endif

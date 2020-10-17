/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPhysicalSA.h
 *  PURPOSE:     Header file for physical object entity base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CPhysical.h>
#include "CEntitySA.h"
#include <CVector.h>

class CShadowDataSAInterface
{
public:
    class CPhysicalSAInterface* pParent;
    uint8                       pad1;
    uint8                       ucIntensity;
    uint8                       pad2;
    uint8                       pad3;
    RwCamera*                   pCamera;
    RwTexture*                  pTexture;
    uint8                       bIsBlurred;
    uint8                       pad4;
    uint8                       pad5;
    uint8                       pad6;
    RwCamera*                   pCameraBlurred;
    RwTexture*                  pTextureBlurred;
    uint32                      pad7;
    uint8                       pad8;
    uint8                       pad9;
    uint8                       pad10;
    uint8                       pad11;
    uint32                      ObjectType;
    RpLight*                    pLight;
    RwSphere                    Sphere;
    CVector                     vecSunPosNormalized;
    uint32                      pad12;
};

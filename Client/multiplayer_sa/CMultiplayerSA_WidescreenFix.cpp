/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_WidescreenFix.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

struct RsGlobal
{
    char* AppName;
    int   width;
    int   height;
    int   frameLimit;
    int   quit;
    int   ps;
};

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

float fFrontendWidth[41], fFrontendHeight[41];
float fMiscWidth[30], fMiscHeight[30];
float fRadarWidth[23], fRadarHeight[23];
float fHUDWidth[120], fHUDHeight[120];
float fCameraWidth[2], fCameraHeight[2];

float fWideScreenWidthScale = 1.0f;
float fWideScreenHeightScale = 1.0f;

static const float fHudHeightScale = 1.0f;
static const float fHudWidthScale = 1.0f;
static const float fRadarWidthScale = 1.0f;
static const float fRadarHeightScale = 1.0f;

float  fWideScreenWidthProperScale;
float  fWideScreenHeightProperScale;
float  fDefaultWidth;
float  fDefaultCoords;
float* pfScreenAspectRatio;
float* pfScreenFieldOfView;

float fWideScreenWidthScaleDown, fWideScreenHeightScaleDown;

RsGlobal* pRsGlobal;

float AdjustFOV(float, float);

void  CalculateAspectRatio();

void GetMemoryAddresses();

void InstallAspectRatioFixes();
void InstallFrontendFixes();
void InstallMiscFixes();
void InstallHUDFixes();

void UpdateFrontendFixes();
void UpdateMiscFixes();
void UpdateHUDFixes();
void UpdateScriptFixes();

float AdjustFOV(float f, float ar)
{
    return std::round((2.0f * atan(((ar) / (4.0f / 3.0f)) * tan(f / 2.0f * ((float)M_PI / 180.0f)))) * (180.0f / (float)M_PI) * 100.0f) / 100.0f;
}

void __declspec(naked) CalculateAimingPoint()
{
    _asm
    {
        fstp    st
        mov     edx, [pfScreenAspectRatio]
        fmul[edx]
        mov     edi, [esp + 1Ch + 14h]
        mov     edx, edi
        mov     ebx, 5149A6h
        jmp     ebx
    }
}

void CalculateAspectRatio()
{
    fWideScreenWidthScaleDown = (1.0f / 640.0f) / (*pfScreenAspectRatio / (4.0f / 3.0f));
}

void UpdateFrontendFixes()
{
    fFrontendWidth[0] = 0.0020312499f * fWideScreenWidthScale;
    fFrontendWidth[1] = 0.00065625005f * fWideScreenWidthScale;
    fFrontendWidth[2] = 0.00109375f * fWideScreenWidthScale;
    fFrontendWidth[3] = 0.00109375f * fWideScreenWidthScale;
    fFrontendWidth[4] = 0.00087500003f * fWideScreenWidthScale;
    fFrontendWidth[5] = 0.000546875f * fWideScreenWidthScale;
    fFrontendWidth[6] = 0.00076562498f * fWideScreenWidthScale;
    fFrontendWidth[7] = 0.00076562498f * fWideScreenWidthScale;
    fFrontendWidth[8] = 0.00046875002f * fWideScreenWidthScale;
    fFrontendWidth[9] = 0.00093750004f * fWideScreenWidthScale;
    fFrontendWidth[10] = 0.0625f;
    fFrontendWidth[11] = 0.09375f;
    fFrontendWidth[12] = 0.0015625f;
    fFrontendWidth[13] = 0.0625f;
    fFrontendWidth[14] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[15] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[16] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[17] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[18] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[19] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[20] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[21] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[22] = 0.00046875002f * fWideScreenWidthScale;
    fFrontendWidth[23] = 0.00125f * fWideScreenWidthScale;
    fFrontendWidth[24] = 0.78125f * fWideScreenWidthScale;
    fFrontendWidth[25] = 0.78125f * fWideScreenWidthScale;
    fFrontendWidth[26] = 0.78125f * fWideScreenWidthScale;
    fFrontendWidth[27] = 0.78125f * fWideScreenWidthScale;
    fFrontendWidth[28] = 0.78125f * fWideScreenWidthScale;
    fFrontendWidth[29] = 0.0046875002f * fWideScreenWidthScale;
    fFrontendWidth[30] = 0.15625f * fWideScreenWidthScale;
    fFrontendWidth[31] = 0.0046875002f * fWideScreenWidthScale;
    fFrontendWidth[32] = 0.15625f * fWideScreenWidthScale;
    fFrontendWidth[33] = 0.0046875002f * fWideScreenWidthScale;
    fFrontendWidth[34] = 0.15625f * fWideScreenWidthScale;
    fFrontendWidth[35] = 0.0046875002f * fWideScreenWidthScale;
    fFrontendWidth[36] = 0.15625f * fWideScreenWidthScale;
    fFrontendWidth[37] = 0.0046875002f * fWideScreenWidthScale;
    fFrontendWidth[38] = 0.15625f * fWideScreenWidthScale;
    fFrontendWidth[39] = 0.0015625f * fWideScreenWidthScale;
    fFrontendWidth[40] = 0.00087500003f * fWideScreenWidthScale;

    fFrontendHeight[0] = 0.0046874997f * fWideScreenHeightScale;
    fFrontendHeight[1] = 0.0021205356f * fWideScreenHeightScale;
    fFrontendHeight[2] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[3] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[4] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[5] = 0.0021205356f * fWideScreenHeightScale;
    fFrontendHeight[6] = 0.0026785715f * fWideScreenHeightScale;
    fFrontendHeight[7] = 0.0015625f * fWideScreenHeightScale;
    fFrontendHeight[8] = 0.0016741072f * fWideScreenHeightScale;
    fFrontendHeight[9] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[10] = 0.0625f * fWideScreenHeightScale;
    fFrontendHeight[11] = 0.21651785f * fWideScreenHeightScale;
    fFrontendHeight[12] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[13] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[14] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[15] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[16] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[17] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[18] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[19] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[20] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[21] = 0.0012276786f * fWideScreenHeightScale;
    fFrontendHeight[22] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[23] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[24] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[25] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[26] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[27] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[28] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[29] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[30] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[31] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[32] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[33] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[34] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[35] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[36] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[37] = 0.0017857143f * fWideScreenHeightScale;
    fFrontendHeight[38] = 0.002232143f * fWideScreenHeightScale;
    fFrontendHeight[39] = 0.002232143f * fWideScreenHeightScale;
}

void UpdateMiscFixes()
{
    fCameraWidth[0] = 0.01403292f;
    fCameraHeight[0] = 0.0f;

    fMiscWidth[0] = 0.0015625f * fWideScreenWidthScale;            // StretchX
    fMiscWidth[1] = 0.0546875f * fWideScreenWidthScale;
    fMiscWidth[2] = 0.0015625f;
    fMiscWidth[3] = 0.09375f * fWideScreenWidthScale;
    fMiscWidth[4] = 0.0015625f;
    fMiscWidth[5] = 0.078125f * fWideScreenWidthScale;
    fMiscWidth[6] = 0.0703125f * fWideScreenWidthScale;
    fMiscWidth[7] = 0.0859375f * fWideScreenWidthScale;
    fMiscWidth[8] = 0.078125f * fWideScreenWidthScale;
    fMiscWidth[9] = 0.078125f * fWideScreenWidthScale;
    fMiscWidth[10] = 0.0703125f * fWideScreenWidthScale;
    fMiscWidth[11] = 0.0859375f * fWideScreenWidthScale;
    fMiscWidth[12] = 0.078125f * fWideScreenWidthScale;
    fMiscWidth[13] = 0.625f;
    fMiscWidth[14] = 0.0015625f * fWideScreenWidthScale;
    fMiscWidth[15] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[16] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[17] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[18] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[19] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[20] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[21] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[22] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[23] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[24] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[25] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[26] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[27] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fMiscWidth[28] = 0.0015625f * fWideScreenWidthScale;

    fMiscHeight[0] = 0.002232143f * fWideScreenHeightScale;            // StretchY
    fMiscHeight[1] = 0.078125f * fWideScreenHeightScale;
    fMiscHeight[2] = 0.66964287f * fWideScreenHeightScale;
    fMiscHeight[3] = 0.13392857f * fWideScreenHeightScale;
    fMiscHeight[4] = 0.6473214f * fWideScreenHeightScale;
    fMiscHeight[5] = 0.22321428f * fWideScreenHeightScale;
    fMiscHeight[6] = 0.24553572f * fWideScreenHeightScale;
    fMiscHeight[7] = 0.24553572f * fWideScreenHeightScale;
    fMiscHeight[8] = 0.22321428f * fWideScreenHeightScale;
    fMiscHeight[9] = 0.77678573f * fWideScreenHeightScale;
    fMiscHeight[10] = 0.75446427f * fWideScreenHeightScale;
    fMiscHeight[11] = 0.75446427f * fWideScreenHeightScale;
    fMiscHeight[12] = 0.77678573f * fWideScreenHeightScale;
    fMiscHeight[13] = 0.037946429f * fWideScreenHeightScale;
    fMiscHeight[14] = 0.002232143f * fWideScreenHeightScale;
    fMiscHeight[15] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[16] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[17] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[18] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[19] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[20] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[21] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[22] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[23] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[24] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[25] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[26] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[27] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fMiscHeight[28] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
}

void UpdateScriptFixes()
{
    float w;
    if (*pfScreenAspectRatio < fDefaultWidth)
        w = static_cast<float>(pRsGlobal->width);
    else
        w = pRsGlobal->width * fDefaultWidth / *pfScreenAspectRatio;

    fDefaultCoords = 0.5f * (pRsGlobal->width - w);
}

void UpdateHUDFixes()
{
    fRadarWidth[0] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[1] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[2] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[3] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[4] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[5] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[6] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[7] = 0.0015625f * fWideScreenWidthScale * fRadarWidthScale;
    fRadarWidth[8] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[9] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[10] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[11] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[12] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[13] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[14] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[15] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[16] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[17] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[18] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[19] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[20] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[21] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fRadarWidth[22] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;

    fRadarHeight[0] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[1] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[2] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[3] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[4] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[5] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[6] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[7] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[8] = 0.002232143f * fWideScreenHeightScale * fRadarHeightScale;
    fRadarHeight[9] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[10] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[11] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[12] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[13] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[14] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[15] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[16] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[17] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[18] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[19] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[20] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[21] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fRadarHeight[22] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;

    fHUDWidth[0] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[1] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[2] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[3] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[4] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[5] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[6] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[7] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[8] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[9] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[10] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[11] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[12] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[13] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[14] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[15] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[16] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[17] = 0.17343046f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[18] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[19] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[20] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[21] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[22] = 0.17343046f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[23] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[24] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[25] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[26] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[27] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[28] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[29] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[30] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[31] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[32] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[33] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[34] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[35] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[36] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[37] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[38] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[39] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[40] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[41] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[42] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[43] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[44] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[45] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[46] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[47] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[48] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[49] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[50] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[51] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[52] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[53] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[54] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[55] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[56] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[57] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[58] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[59] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[60] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[61] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[62] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[63] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[64] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[65] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[66] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[67] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[68] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[69] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[70] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[71] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[72] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[73] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[74] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[75] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[76] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[77] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[78] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[79] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[80] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[81] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[82] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[83] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[84] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[85] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[86] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[87] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[88] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[89] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[90] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[91] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[92] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[93] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[94] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[95] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[96] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[97] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[98] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[99] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[100] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[101] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[102] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[103] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[104] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;
    fHUDWidth[105] = 0.0015625f * fWideScreenWidthScale * fHudWidthScale;

    fHUDWidth[110] = 1920.0f * 0.17343046f * fWideScreenWidthScale * fHudWidthScale;

    fHUDHeight[0] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[1] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[2] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[3] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[4] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[5] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[6] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[7] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[8] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[9] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[10] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[11] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[12] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[13] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[14] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[15] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[16] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[17] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[18] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[19] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[20] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[21] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[22] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[23] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[24] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[25] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[26] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[27] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[28] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[29] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[30] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[31] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[32] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[33] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[34] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[35] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[36] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[37] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[38] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[39] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[40] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[41] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[42] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[43] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[44] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[45] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[46] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[47] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[48] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[49] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[50] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[51] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[52] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[53] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[54] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[55] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[56] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[57] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[58] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[59] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[60] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[61] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[62] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[63] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[64] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[65] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[66] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[67] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[68] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[69] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[70] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[71] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[72] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[73] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[74] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[75] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[76] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[77] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[78] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[79] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[80] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[81] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[82] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[83] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[84] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[85] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[86] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[87] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[88] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[89] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[90] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[91] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[92] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[93] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[94] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[95] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[96] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[97] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[98] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[99] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[100] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[101] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[102] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[103] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[104] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;
    fHUDHeight[105] = 0.002232143f * fWideScreenHeightScale * fHudHeightScale;

    fHUDHeight[14] /= 1.125f;
    fHUDHeight[15] /= 1.125f;
    fHUDHeight[16] /= 1.125f;
}

void UpdateScreenAspectRatio()
{
    fWideScreenWidthScale = 640.0f / (*(pfScreenAspectRatio) * 448.0f);
    fWideScreenHeightScale = 448.0 / 448.0f;
    fWideScreenWidthProperScale = static_cast<float>(pRsGlobal->width) / (*pfScreenAspectRatio * 448.0f);
    fWideScreenHeightProperScale = static_cast<float>(pRsGlobal->height) / 448.0f;
    fDefaultWidth = (4.0f / 3.0f);

    UpdateFrontendFixes();
    UpdateMiscFixes();
    UpdateHUDFixes();
    UpdateScriptFixes();
}

void InstallAspectRatioFixes()
{
    MemPut<BYTE>(0x745BD1, 0x7D);
    MemPut<BYTE>(0x745BD9, 0x7C);

    // Proportional coronas
    MemSet((void*)0x6FB2C9, 0x90, 4);

    MemPut<BYTE>(0x6FB2BD, 0x6C);
    MemPut<BYTE>(0x6FB2DC, 0x78);
    MemPut<BYTE>(0x713BE5, 0x20);
    MemPut<BYTE>(0x713B6D, 0x38);
    MemPut<BYTE>(0x713CFB, 0x38);
    MemPut<BYTE>(0x713EFC, 0x30);
    MemPut<BYTE>(0x714004, 0x38);

    HookInstall(0x6FF420, (DWORD)CalculateAspectRatio, 5);

    HookInstallCall(0x0053D695 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053D7B2 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053D967 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053E771 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053EB1A - 0x1, (DWORD)UpdateScreenAspectRatio);
}

void GetMemoryAddresses()
{
    pRsGlobal = (RsGlobal*)0xC17040;
    pfScreenAspectRatio = (float*)0xC3EFA4;
}

float __stdcall StretchXHook(float fValue)
{
    if (pRsGlobal->width == 640)
        return fValue;
    else
        return (float)pRsGlobal->width * fValue * 0.0015625f;
}

float __stdcall StretchYHook(float fValue)
{
    if (pRsGlobal->height == 448)
        return fValue;
    else
        return (float)pRsGlobal->height * fValue * 0.002232143f;
}

void InstallFrontendFixes()
{
    // Font Scales
    int m_dwFrontendWidth[] = {
        0x5795CF,            // 0 Menu text
        0x579981,            // 1 Menu text
        0x579A01,            // 2 Menu text
        0x57A36E,            // 3 Menu text
        0x57A319,            // 4 Menu text
        0x57A2A0,            // 5 Menu text
        0x579718,            // 6 Menu text
        0x5763C0,            // 7 Menu text
        0x5749A5,            // 8 Menu text
        0x57500F,            // 9 Menu text
        0x579665,            // 10 Menu text
        0x579863,            // 11 Menu text
        0x57A1E3,            // 12 Menu text
        0x57A3EB,            // 13 Menu text
        0x582DDB,            // 14 Legend
        0x582E0D,            // 15 Legend
        0x582EB0,            // 16 Legend
        0x583019,            // 17 Legend
        0x58309C,            // 18 Legend
        0x582F82,            // 19 Legend
        0x583128,            // 20 Legend
        0x5831AD,            // 21 Legend
        0x57613B,            // 22 Legend
        0x575EDF,            // 23 Map zones
        0x57A88D,            // 24 Brightness slider posn
        0x57AA81,            // 25 Radio slider posn
        0x57ACBE,            // 26 Sfx slider posn
        0x57AEB0,            // 27 Draw slider posn
        0x57B0E4,            // 28 Mouse slider posn
        0x57A807,            // 29 Brightness slider
        0x57A811,            // 30 Brightness slider
        0x57A9FE,            // 31 Radio slider
        0x57AA08,            // 32 Radio slider
        0x57AC2F,            // 33 Sfx slider
        0x57AC39,            // 34 Sfx slider
        0x57AE36,            // 35 Draw distance slider
        0x57AE40,            // 36 Draw distance slider
        0x57B064,            // 37 Mouse acc slider
        0x57B06E,            // 38 Mouse acc slider
        0x584A12,            // 39 DrawYouAreHereSprite
        0x5740BC,            // 40 Message screen
    };

    int m_dwFrontendHeight[] = {
        0x5795AD,            // 0 Menu text
        0x579958,            // 1 Menu text
        0x5799D5,            // 2 Menu text
        0x57A345,            // 3 Menu text
        0x57A2DB,            // 4 Menu text
        0x57A25F,            // 5 Menu text
        0x5796F6,            // 6 Menu text
        0x576398,            // 7 Menu text
        0x57497D,            // 8 Menu text
        0x574FED,            // 9 Menu text
        0x579643,            // 10 Menu text
        0x579841,            // 11 Menu text
        0x57A1AE,            // 12 Menu text
        0x57A3C5,            // 13 Menu text
        0x582DC1,            // 14 Legend
        0x582ED3,            // 15 Legend
        0x582FA3,            // 16 Legend
        0x582FF5,            // 17 Legend
        0x583079,            // 18 Legend
        0x5830F6,            // 19 Legend
        0x58317B,            // 20 Legend
        0x576119,            // 21 Legend
        0x575EB7,            // 22 Map zones
        NULL,                // 23
        NULL,                // 24
        NULL,                // 25
        NULL,                // 26
        NULL,                // 27
        NULL,                // 28
        NULL,                // 29
        NULL,                // 30
        NULL,                // 31
        NULL,                // 32
        NULL,                // 33
        NULL,                // 34
        NULL,                // 35
        NULL,                // 36
        NULL,                // 37
        0x5849FA,            // 38 DrawYouAreHereSprite
        0x574096,            // 39 Message screen
    };

    for (int i = 0; i < sizeof(m_dwFrontendWidth) / sizeof(const void*); i++)
    {
        if (m_dwFrontendWidth[i] != NULL)
            MemPut<const void*>(m_dwFrontendWidth[i] + 0x2, &fFrontendWidth[i]);
    }

    for (int i = 0; i < sizeof(m_dwFrontendHeight) / sizeof(const void*); i++)
    {
        if (m_dwFrontendHeight[i] != NULL)
            MemPut<const void*>(m_dwFrontendHeight[i] + 0x2, &fFrontendHeight[i]);
    }

    // StretchXY Restoration
    int m_dwCallWidth[] = {
        0x57E3A5, 0x577CB7, 0x577CD0, 0x577D6F, 0x577D99, 0x577DAB, 0x577FA6, 0x578031, 0x578052, 0x5780E1, 0x5788DF, 0x5788FB,
    };

    int m_dwCallHeight[] = {
        0x57E391, 0x577C7B, 0x577C94, 0x577DEC, 0x577F6F, 0x577F81, 0x578106, 0x578199, 0x5781BA, 0x57824D, 0x5788ED,

    };

    for (int i = 0; i < sizeof(m_dwCallWidth) / sizeof(const void*); i++)
        HookInstallCall(m_dwCallWidth[i], (DWORD)StretchXHook);

    for (int i = 0; i < sizeof(m_dwCallHeight) / sizeof(const void*); i++)
        HookInstallCall(m_dwCallHeight[i], (DWORD)StretchYHook);
}

void InstallMiscFixes()
{
    // Misc
    int m_dwMiscWidth[] = {
        0x5733FD,            // 0 StretchX
        0x574761,            // 1 Radio Icons
        0x5747A6,            // 2 Radio Icons
        0x574857,            // 3 Radio Icons
        0x5748AA,            // 4 Radio Icons
        0x5765C0,            // 5 2d Brief triangles
        0x576603,            // 6 2d Brief triangles
        0x576646,            // 7 2d Brief triangles
        0x576689,            // 8 2d Brief triangles
        0x57672F,            // 9 2d Brief triangles
        0x576772,            // 10 2d Brief triangles
        0x5767B5,            // 11 2d Brief triangles
        0x5767F8,            // 12 2d Brief triangles
        0x574ECC,            // 13 Stats bars
        0x574F30,            // 14 Stats bars
        0x719D2D,            // 15 Font fixes
        0x719D94,            // 16 Font fixes
        0x719DD1,            // 17 Font fixes
        0x719E0E,            // 18 Font fixes
        0x719E4B,            // 19 Font fixes
        0x719E6F,            // 20 Font fixes
        0x719E97,            // 21 Font fixes
        0x719C0D,            // 22 Font fixes
        0x719C6E,            // 23 Font fixes
        0x7288F5,            // 24 BarChart fixes
        0x728941,            // 25 BarChart fixes
        0x573F93,            // 26 DrawWindow header
        0x573FF0,            // 27 DrawWindow header
    };

    int m_dwMiscHeight[] = {
        0x57342D,            // 0 StretchY
        0x57473B,            // 1 Radio Icons
        0x574783,            // 2 Radio Icons
        0x57482F,            // 3 Radio Icons
        0x574879,            // 4 Radio Icons
        0x57659A,            // 5 2d Brief triangles
        0x5765E2,            // 6 2d Brief triangles
        0x576625,            // 7 2d Brief triangles
        0x576668,            // 8 2d Brief triangles
        0x576709,            // 9 2d Brief triangles
        0x576751,            // 10 2d Brief triangles
        0x576794,            // 11 2d Brief triangles
        0x5767D7,            // 12 2d Brief triangles
        0x574EAA,            // 13 Stats bars
        0x574F0C,            // 14 Stats bars
        0x719D47,            // 15 Font fixes
        0x719D7C,            // 16 Font fixes
        0x719DB5,            // 17 Font fixes
        0x719DF2,            // 18 Font fixes
        0x719E2F,            // 19 Font fixes
        0x719EBF,            // 20 Font fixes
        0x719EE3,            // 21 Font fixes
        0x719C27,            // 22 Font fixes
        0x719C58,            // 23 Font fixes
        0x728864,            // 24 BarChart fixes
        0x7288A9,            // 25 BarChart fixes
        0x573F7D,            // 26 DrawWindow header
        0x573FD6,            // 27 DrawWindow header
    };

    for (int i = 0; i < sizeof(m_dwMiscWidth) / sizeof(const void*); i++)
    {
        if (m_dwMiscWidth[i] != NULL)
            MemPut<const void*>(m_dwMiscWidth[i] + 0x2, &fMiscWidth[i]);
    }

    for (int i = 0; i < sizeof(m_dwMiscHeight) / sizeof(const void*); i++)
    {
        if (m_dwMiscHeight[i] != NULL)
            MemPut<const void*>(m_dwMiscHeight[i] + 0x2, &fMiscHeight[i]);
    }
}

void InstallHUDFixes()
{
    int m_dwCrosshairWidth[] = {
        0x58E7CE, 0x58E7F8, 0x58E2FA, 0x58E4ED, 0x58E75B, 0x58E28B, 0x58E2AC, 0x58E2BA, 0x53E472, 0x53E4AE,
    };

    int m_dwCrosshairHeight[] = {
        0x58E7E4, 0x58E80E, 0x58E319, 0x58E527, 0x58E2C8, 0x53E3E7, 0x53E409, NULL, NULL, NULL,
    };

    for (int i = 0; i < sizeof(m_dwCrosshairWidth) / sizeof(const void*); i++)
    {
        if (m_dwCrosshairWidth[i] != NULL)
            MemPut<const void*>(m_dwCrosshairWidth[i] + 0x2, &fMiscWidth[0]);
    }

    for (int i = 0; i < sizeof(m_dwCrosshairHeight) / sizeof(const void*); i++)
    {
        if (m_dwCrosshairHeight[i] != NULL)
            MemPut<const void*>(m_dwCrosshairHeight[i] + 0x2, &fMiscHeight[0]);
    }

    int m_dwRadarWidth[] = {
        0x58A441,            // Radar plane
        0x58A791,            // Radar disc
        0x58A82E,            // Radar disc
        0x58A8DF,            // Radar disc
        0x58A982,            // Radar disc
        0x58A5D8, 0x58A6DE,
        0x5834BA,            // Radar point
        0x58603F,            // Radar point
        0x5886CC,            // Radar centre
        0x58439C,            // Radar Trace 0
        0x584434,            // Radar Trace 0
        0x58410B,            // Radar Trace 2
        0x584190,            // Radar Trace 2
        0x584249,            // Radar Trace 1
        0x5842E6,            // Radar Trace 1
        0x5876D4, 0x58774B, 0x58780A, 0x58788F, 0x58792E, 0x587A1A, 0x587AAA,
    };

    int m_dwRadarHeight[] = {
        0x58A473,            // Radar plane
        0x58A600,            // Radar disc
        0x58A69E,            // Radar disc
        0x58A704,            // Radar disc
        0x58A7B9,            // Radar disc
        0x58A85A, 0x58A909,
        0x58A9BD,            // Radar point
        0x5834EC,            // Radar point
        0x586058,            // Radar centre
        0x584346,            // Radar Trace 0
        0x58440C,            // Radar Trace 0
        0x58412B,            // Radar Trace 2
        0x5841B0,            // Radar Trace 2
        0x584207,            // Radar Trace 1
        0x5842C6,            // Radar Trace 1
        0x5876BC, 0x587733, 0x587916, 0x587A02, 0x587A92, NULL, NULL,
    };

    for (int i = 0; i < sizeof(m_dwRadarWidth) / sizeof(const void*); i++)
    {
        if (m_dwRadarWidth[i] != NULL)
            MemPut<const void*>(m_dwRadarWidth[i] + 0x2, &fRadarWidth[i]);
    }

    for (int i = 0; i < sizeof(m_dwRadarHeight) / sizeof(const void*); i++)
    {
        if (m_dwRadarHeight[i] != NULL)
            MemPut<const void*>(m_dwRadarHeight[i] + 0x2, &fRadarHeight[i]);
    }

    int m_dwHUDWidth[] = {
        0x58EB3F,            // 0 Clock
        0x58EC0C,            // 1 Clock
        0x58F55C,            // 2 Money
        0x58F5F4,            // 3 Money
        0x5892CA,            // 4 Info bars
        0x58937E,            // 5 Info bars
        0x58EE7E,            // 6 Info bars
        0x58EEF4,            // 7 Info bars
        0x589155,            // 8 Info bars
        0x58EF50,            // 9 Info bars
        0x58EFC5,            // 10 Info bars
        0x58922D,            // 11 Info bars
        0x58F116,            // 12 Info bars
        0x58F194,            // 13 Info bars
        0x58D92D,            // 14 Weapon icons
        0x58D8C3,            // 15 Weapon icons
        0x58F91C,            // 16 Weapon icons
        0x58F92D,            // 17 Weapon icons
        0x5894C5,            // 18 Ammo
        0x5894E9,            // 19 Ammo
        0x58F9D0,            // 20 Ammo
        0x58FA5D,            // 21 Ammo
        0x58F9F5,            // 22 Ammo
        0x58FA8E,            // 23 Ammo
        0x58DCB8,            // 24 Wanted
        0x58DD00,            // 25 Wanted
        0x58DD7E,            // 26 Wanted
        0x58DF71,            // 27 Wanted
        0x58DFE5,            // 28 Wanted
        NULL,                // 29
        0x58B09F,            // 30 Vehicle names
        0x58B13F,            // 31 Vehicle names
        0x58AD3A,            // 32 Area names
        0x58AD65,            // 33 Area names
        0x58AE4A,            // 34 Area names
        0x58C395,            // 35 Subs
        0x58C41D,            // 36 Subs
        0x58C4DC,            // 37 Subs
        0x5896D8,            // 38 Stats box
        0x589703,            // 39 Stats box
        0x58990C,            // 40 Stats box
        0x58986D,            // 41 Stats box
        0x5897C3,            // 42 Stats box
        0x589A16,            // 43 Stats box
        0x589B2D,            // 44 Stats box
        0x589C73,            // 45 Stats box
        0x589D61,            // 46 Stats box
        0x589E49,            // 47 Stats box
        0x589F31,            // 48 Stats box
        0x58A013,            // 49 Stats box
        0x58A090,            // 50 Stats box
        0x58A134,            // 51 Stats box
        NULL,                // 52
        NULL,                // 53
        NULL,                // 54
        NULL,                // 55
        NULL,                // 56
        NULL,                // 57
        NULL,                // 58
        NULL,                // 59
        NULL,                // 60
        NULL,                // 61
        0x58C863,            // 62 SuccessFailed text
        0x58D2DB,            // 63 MissionTitle text
        0x58D459,            // 64 MissionTitle text
        0x58CBC1,            // 65 WastedBusted text
        0x58B273,            // 66 Timers
        0x58B2A4,            // 67 Timers
        0x58B3AF,            // 68 Timers
        0x58B3FC,            // 69 Timers
        0x58B56A,            // 70 Timers
        0x58B5EE,            // 71 Timers
        0x58B67E,            // 72 Timers
        0x58B76F,            // 73 Helptext
        0x58B7D6,            // 74 Helptext
        0x58BA62,            // 75 Helptext
        0x58BAC6,            // 76 Helptext
        0x58BBDB,            // 77 Helptext
        0x58BCB0,            // 78 Helptext
        0x58BD58,            // 79 Helptext
        0x58BE8D,            // 80 Helptext
        0x58BF7E,            // 81 Helptext
        0x58BFFC,            // 82 Helptext
        0x580F16,            // 83 Menu system
        0x580F95,            // 84
        0x5810EF,            // 85
        0x581158,            // 86
        0x5811CD,            // 87
        0x58148A,            // 88
        0x5814F7,            // 89
        0x5815B1,            // 90
        0x5815EB,            // 91
        0x581633,            // 92
        0x47AD2A,            // 93
        0x5818CF,            // 94
        0x58CCDB,            // 95 OddJob
        0x58CDE6,            // 96 OddJob
        0x58CEE2,            // 97 OddJob
        0x58D15C,            // 98 OddJob
        0x58A178,            // 99 TripSkip
        0x58A21D,            // 100 TripSkip
        0x58A2C0,            // 101 TripSkip
        0x4E9F30,            // 102 RadioStation
        0x43CF57,            // 103 CDarkel
        0x4477CD,            // 104 CGarages
        0x4477F7,            // 105 CGarages
    };

    int m_dwHUDHeight[] = {
        0x58EB29,            // 0 Clock
        0x58EBF9,            // 1 Clock
        0x58F546,            // 2 Money
        0x58F5CE,            // 3 Money
        0x589346,            // 4 Info bars
        0x58EE60,            // 5 Info bars
        0x588B9C,            // 6 Info bars
        0x58EEC8,            // 7 Info bars
        0x58913E,            // 8 Info bars
        0x58EF32,            // 9 Info bars
        0x58EF99,            // 10 Info bars
        0x589216,            // 11 Info bars
        0x58F0F8,            // 12 Info bars
        0x58F168,            // 13 Info bars
        0x58D945,            // 14 Weapon icons
        0x58D882,            // 15 Weapon icons
        0x58F90B,            // 16 Weapon icons
        NULL,                // 17
        0x5894AF,            // 18 Ammo
        NULL,                // 19
        0x58F9C0,            // 20 Ammo
        0x58FA4A,            // 21 Ammo
        NULL,                // 22
        NULL,                // 23
        0x58DCA2,            // 24 Wanted
        0x58DD68,            // 25 Wanted
        0x58DDF4,            // 26 Wanted
        0x58DF55,            // 27 Wanted
        0x58DF9B,            // 28 Wanted
        0x58DEE4,            // 29 Wanted
        0x58B089,            // 30 Vehicle names
        0x58B12D,            // 31 Vehicle names
        0x58AD24,            // 32 Area names
        0x58AE0D,            // 33 Area names
        NULL,                // 34
        0x58C37F,            // 35 Subs
        0x58C407,            // 36 Subs
        0x58C4C6,            // 37 Subs
        // 0x58C53B, // 38 Subs
        // 0x58C611, // 39 Subs
        0x5898F6,            // 40 Stats box text
        // 0x58C46E, // 41 Subs
        NULL,                // 42
        0x589735,            // 43 Stats box
        0x58978B,            // 44 Stats box
        0x589813,            // 45 Stats box
        0x58983F,            // 46 Stats box
        0x5898BD,            // 47 Stats box
        0x5899FF,            // 48 Stats box
        0x589A4B,            // 49 Stats box
        0x589B16,            // 50 Stats box
        0x589C5C,            // 51 Stats box
        0x589CA8,            // 52 Stats box
        0x589D4A,            // 53 Stats box
        0x589D92,            // 54 Stats box
        0x589E32,            // 55 Stats box
        0x589E7A,            // 56 Stats box
        0x589F1A,            // 57 Stats box
        0x589F62,            // 58 Stats box
        0x589FFC,            // 59 Stats box
        0x58A040,            // 60 Stats box
        0x58A07A,            // 61 Stats box
        0x58C84D,            // 62 SuccessFailed text
        0x58D2C5,            // 63 MissionTitle text
        NULL,                // 0x58D447, // 64 MissionTitle text
        0x58CBAB,            // 65 WastedBusted text
        NULL,                // 0x58B1B7, // 66 Timers
        0x58B263,            // 67 Timers
        NULL,                // 0x58B435, // 68 Timers
        NULL,                // 0x58B536, // 69 Timers
        0x58B5DE,            // 70 Timers
        NULL,                // 71
        NULL,                // 72
        0x58B7BD,            // 73 Help text
        0x58BA4C,            // 74 Help text
        0x58BBA7,            // 75 Help text
        0x58BD19,            // 76 Help text
        0x58BE2B,            // 77 Help text
        0x58BF1C,            // 78 Help text
        0x58BFCB,            // 79 Help text
        NULL,                // 80
        NULL,                // 81
        NULL,                // 82
        0x580E11,            // 83 Menu system
        0x580F85,            // 84
        0x5810CC,            // 85
        0x581132,            // 86
        0x5811A1,            // 87
        0x58147A,            // 88
        0x5814E7,            // 89
        0x581699,            // 90
        NULL,                // 91
        NULL,                // 92
        NULL,                // 93
        0x581889,            // 94
        0x58CCC5,            // 95 OddJob
        0x58CDD0,            // 96 OddJob
        0x58CECC,            // 97 OddJob
        0x58D146,            // 98 OddJob
        NULL,                // 0x58A199, // 99 TripSkip
        NULL,                // 0x58A207, // 100 TripSkip
        NULL,                // 0x58A2B0, // 101 TripSkip
        0x4E9F1A,            // 102 RadioStation
        0x43CF47,            // 103 CDarkel
        0x4477B7,            // 104 CGarages
        0x4478AC,            // 105 CGarages
    };

    for (int i = 0; i < sizeof(m_dwHUDWidth) / sizeof(const void*); i++)
    {
        if (m_dwHUDWidth[i] != NULL)
            MemPut<const void*>(m_dwHUDWidth[i] + 0x2, &fHUDWidth[i]);
    }

    for (int i = 0; i < sizeof(m_dwHUDHeight) / sizeof(const void*); i++)
    {
        if (m_dwHUDHeight[i] != NULL)
            MemPut<const void*>(m_dwHUDHeight[i] + 0x2, &fHUDHeight[i]);
    }

    /*
    // Help text bar chart offset
    static float fBarChartOffsetY = 160.0f;
    MemPut<const void*>(0x58BE9F + 0x2, &fBarChartOffsetY);

    // Lock Subtitles Width
    static float fSubtitlesMult = 1.0f;
    MemPut<const void*>(0x58C4E8 + 0x2, &fSubtitlesMult);
    */

    // Second player fix.
    MemPut<const void*>(0x58F9A0 + 0x2, &fHUDWidth[110]);            // Weapon icon X
    MemPut<const void*>(0x58F993 + 0x2, &fHUDWidth[16]);             // Weapon icon X
    MemPut<const void*>(0x58F972 + 0x2, &fHUDHeight[16]);            // Weapon icon Y
    MemPut<const void*>(0x58FA8E + 0x2, &fHUDWidth[17]);             // Ammo x
}

void CMultiplayerSA::InitHooks_WidescreenFix()
{
    GetMemoryAddresses();

    InstallAspectRatioFixes();
    InstallFrontendFixes();
    InstallMiscFixes();
    InstallHUDFixes();
}

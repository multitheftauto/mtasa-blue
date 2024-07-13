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
    int width;
    int height;
};

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

float MiscWidth[30], MiscHeight[30];
float RadarWidth[23], RadarHeight[23];
float HUDWidth[120], HUDHeight[120];
float CameraWidth[2], CameraHeight[2];

float                  ScreenWidthScale = 1.0f;
static constexpr float ScreenHeightScale = 1.0f;
static constexpr float HudWidthScale = 1.0f;
static constexpr float HudHeightScale = 1.0f;
static constexpr float RadarWidthScale = 1.0f;
static constexpr float RadarHeightScale = 1.0f;

constexpr float WidthMult = 0.0015625f;
constexpr float HeightMult = 0.002232143f;

float* ScreenAspectRatio;
float* ScreenFieldOfView;

RsGlobal* rsGlobal;

void UpdateMiscFixes()
{
    CameraWidth[0] = 0.01403292f;
    CameraHeight[0] = 0.0f;

    MiscWidth[0] = WidthMult * ScreenWidthScale;            // StretchX
    MiscWidth[1] = 0.0546875f * ScreenWidthScale;
    MiscWidth[2] = WidthMult;
    MiscWidth[3] = 0.09375f * ScreenWidthScale;
    MiscWidth[4] = WidthMult;
    MiscWidth[5] = 0.078125f * ScreenWidthScale;
    MiscWidth[6] = 0.0703125f * ScreenWidthScale;
    MiscWidth[7] = 0.0859375f * ScreenWidthScale;
    MiscWidth[8] = 0.078125f * ScreenWidthScale;
    MiscWidth[9] = 0.078125f * ScreenWidthScale;
    MiscWidth[10] = 0.0703125f * ScreenWidthScale;
    MiscWidth[11] = 0.0859375f * ScreenWidthScale;
    MiscWidth[12] = 0.078125f * ScreenWidthScale;
    MiscWidth[13] = 0.625f;
    MiscWidth[14] = WidthMult * ScreenWidthScale;
    MiscWidth[15] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[16] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[17] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[18] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[19] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[20] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[21] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[22] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[23] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[24] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[25] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[26] = WidthMult * ScreenWidthScale * HudWidthScale;
    MiscWidth[27] = WidthMult * ScreenWidthScale * HudWidthScale;

    MiscHeight[0] = HeightMult * ScreenHeightScale;            // StretchY
    MiscHeight[1] = 0.078125f * ScreenHeightScale;
    MiscHeight[2] = 0.66964287f * ScreenHeightScale;
    MiscHeight[3] = 0.13392857f * ScreenHeightScale;
    MiscHeight[4] = 0.6473214f * ScreenHeightScale;
    MiscHeight[5] = 0.22321428f * ScreenHeightScale;
    MiscHeight[6] = 0.24553572f * ScreenHeightScale;
    MiscHeight[7] = 0.24553572f * ScreenHeightScale;
    MiscHeight[8] = 0.22321428f * ScreenHeightScale;
    MiscHeight[9] = 0.77678573f * ScreenHeightScale;
    MiscHeight[10] = 0.75446427f * ScreenHeightScale;
    MiscHeight[11] = 0.75446427f * ScreenHeightScale;
    MiscHeight[12] = 0.77678573f * ScreenHeightScale;
    MiscHeight[13] = 0.037946429f * ScreenHeightScale;
    MiscHeight[14] = HeightMult * ScreenHeightScale;
    MiscHeight[15] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[16] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[17] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[18] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[19] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[20] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[21] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[22] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[23] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[24] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[25] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[26] = HeightMult * ScreenHeightScale * HudHeightScale;
    MiscHeight[27] = HeightMult * ScreenHeightScale * HudHeightScale;
}

void UpdateHUDFixes()
{
    RadarWidth[0] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[1] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[2] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[3] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[4] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[5] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[6] = 0.0015000f * ScreenWidthScale * RadarWidthScale;
    RadarWidth[7] = WidthMult * ScreenWidthScale * RadarWidthScale;
    RadarWidth[8] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[9] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[10] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[11] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[12] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[13] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[14] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[15] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[16] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[17] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[18] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[19] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[20] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[21] = WidthMult * ScreenWidthScale * HudWidthScale;
    RadarWidth[22] = WidthMult * ScreenWidthScale * HudWidthScale;

    RadarHeight[0] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[1] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[2] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[3] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[4] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[5] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[6] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[7] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[8] = HeightMult * ScreenHeightScale * RadarHeightScale;
    RadarHeight[9] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[10] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[11] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[12] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[13] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[14] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[15] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[16] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[17] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[18] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[19] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[20] = HeightMult * ScreenHeightScale * HudHeightScale;
    RadarHeight[21] = HeightMult * ScreenHeightScale * HudHeightScale;

    HUDWidth[0] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[1] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[2] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[3] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[4] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[5] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[6] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[7] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[8] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[9] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[10] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[11] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[12] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[13] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[14] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[15] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[16] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[17] = 0.17343046f * ScreenWidthScale * HudWidthScale;
    HUDWidth[18] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[19] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[20] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[21] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[22] = 0.17343046f * ScreenWidthScale * HudWidthScale;
    HUDWidth[23] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[24] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[25] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[26] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[27] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[28] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[29] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[30] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[31] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[32] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[33] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[34] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[35] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[36] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[37] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[38] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[39] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[40] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[41] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[42] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[43] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[44] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[45] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[46] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[47] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[48] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[49] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[50] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[51] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[52] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[53] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[54] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[55] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[56] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[57] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[58] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[59] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[60] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[61] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[62] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[63] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[64] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[65] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[66] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[67] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[68] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[69] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[70] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[71] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[72] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[73] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[74] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[75] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[76] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[77] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[78] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[79] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[80] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[81] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[82] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[83] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[84] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[85] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[86] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[87] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[88] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[89] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[90] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[91] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[92] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[93] = WidthMult * ScreenWidthScale * HudWidthScale;
    HUDWidth[94] = WidthMult * ScreenWidthScale * HudWidthScale;

    HUDWidth[110] = 1920.0f * 0.17343046f * ScreenWidthScale * HudWidthScale;

    HUDHeight[0] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[1] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[2] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[3] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[4] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[5] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[6] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[7] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[8] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[9] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[10] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[11] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[12] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[13] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[14] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[15] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[16] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[17] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[18] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[19] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[20] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[21] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[22] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[23] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[24] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[25] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[26] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[27] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[28] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[29] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[30] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[31] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[32] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[33] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[34] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[35] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[36] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[37] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[38] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[39] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[40] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[41] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[42] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[43] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[44] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[45] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[46] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[47] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[48] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[49] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[50] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[51] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[52] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[53] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[54] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[55] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[56] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[57] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[58] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[59] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[60] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[61] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[62] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[63] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[64] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[65] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[66] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[67] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[68] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[69] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[70] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[71] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[72] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[73] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[74] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[75] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[76] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[77] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[78] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[79] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[80] = HeightMult * ScreenHeightScale * HudHeightScale;
    HUDHeight[81] = HeightMult * ScreenHeightScale * HudHeightScale;

    HUDHeight[14] /= 1.125f;
    HUDHeight[15] /= 1.125f;
    HUDHeight[16] /= 1.125f;
}

void UpdateScreenAspectRatio()
{
    ScreenWidthScale = 640.0f / (*(ScreenAspectRatio) * 448.0f);

    UpdateMiscFixes();
    UpdateHUDFixes();
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

    HookInstallCall(0x0053D695 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053D7B2 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053D967 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053E771 - 0x1, (DWORD)UpdateScreenAspectRatio);
    HookInstallCall(0x0053EB1A - 0x1, (DWORD)UpdateScreenAspectRatio);
}

void InstallMiscFixes()
{
    // Misc
    uintptr_t miscWidthAddresses[28] = {
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

    uintptr_t miscHeightAddresses[28] = {
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

    for (size_t i = 0; i < std::size(miscWidthAddresses); i++)
    {
        MemPut<float*>(miscWidthAddresses[i] + 0x2, &MiscWidth[i]);
    }

    for (size_t i = 0; i < std::size(miscHeightAddresses); i++)
    {
        MemPut<float*>(miscHeightAddresses[i] + 0x2, &MiscHeight[i]);
    }
}

void InstallHUDFixes()
{
    uintptr_t crosshairWidthAddresses[10] = {
        0x58E7CE, 0x58E7F8, 0x58E2FA, 0x58E4ED, 0x58E75B, 0x58E28B, 0x58E2AC, 0x58E2BA, 0x53E472, 0x53E4AE,
    };

    uintptr_t crosshairHeightAddresses[7] = {
        0x58E7E4, 0x58E80E, 0x58E319, 0x58E527, 0x58E2C8, 0x53E3E7, 0x53E409,
    };

    for (size_t i = 0; i < std::size(crosshairWidthAddresses); i++)
    {
        MemPut<float*>(crosshairWidthAddresses[i] + 0x2, &MiscWidth[0]);
    }

    for (size_t i = 0; i < std::size(crosshairHeightAddresses); i++)
    {
        MemPut<float*>(crosshairHeightAddresses[i] + 0x2, &MiscHeight[0]);
    }

    uintptr_t radarWidthAddresses[23] = {
        0x58A441,                      // Radar plane
        0x58A791,                      // Radar disc
        0x58A82E,                      // Radar disc
        0x58A8DF,                      // Radar disc
        0x58A982,                      // Radar disc
        0x58A5D8, 0x58A6DE,            // Radar altimeter
        0x5834BA,                      // Radar point
        0x58603F,                      // Radar point
        0x5886CC,                      // Radar centre
        0x58439C,                      // Radar Trace 0
        0x584434,                      // Radar Trace 0
        0x58410B,                      // Radar Trace 2
        0x584190,                      // Radar Trace 2
        0x584249,                      // Radar Trace 1
        0x5842E6,                      // Radar Trace 1
        0x5876D4, 0x58774B, 0x58780A, 0x58788F, 0x58792E, 0x587A1A, 0x587AAA,
    };

    uintptr_t radarHeightAddresses[21] = {
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
        0x5876BC, 0x587733, 0x587916, 0x587A02, 0x587A92,
    };

    for (size_t i = 0; i < std::size(radarWidthAddresses); i++)
    {
        MemPut<float*>(radarWidthAddresses[i] + 0x2, &RadarWidth[i]);
    }

    for (size_t i = 0; i < std::size(radarHeightAddresses); i++)
    {
        MemPut<float*>(radarHeightAddresses[i] + 0x2, &RadarHeight[i]);
    }

    uintptr_t hudWidthAddresses[95] = {
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
        0x58B09F,            // 29 Vehicle names
        0x58B13F,            // 30 Vehicle names
        0x58AD3A,            // 31 Area names
        0x58AD65,            // 32 Area names
        0x58AE4A,            // 33 Area names
        0x58C395,            // 34 Subs
        0x58C41D,            // 35 Subs
        0x58C4DC,            // 36 Subs
        0x5896D8,            // 37 Stats box
        0x589703,            // 38 Stats box
        0x58990C,            // 39 Stats box
        0x58986D,            // 40 Stats box
        0x5897C3,            // 41 Stats box
        0x589A16,            // 42 Stats box
        0x589B2D,            // 43 Stats box
        0x589C73,            // 44 Stats box
        0x589D61,            // 45 Stats box
        0x589E49,            // 46 Stats box
        0x589F31,            // 47 Stats box
        0x58A013,            // 48 Stats box
        0x58A090,            // 49 Stats box
        0x58A134,            // 50 Stats box
        0x58C863,            // 51 SuccessFailed text
        0x58D2DB,            // 52 MissionTitle text
        0x58D459,            // 53 MissionTitle text
        0x58CBC1,            // 54 WastedBusted text
        0x58B273,            // 55 Timers
        0x58B2A4,            // 56 Timers
        0x58B3AF,            // 57 Timers
        0x58B3FC,            // 58 Timers
        0x58B56A,            // 59 Timers
        0x58B5EE,            // 60 Timers
        0x58B67E,            // 61 Timers
        0x58B76F,            // 62 Helptext
        0x58B7D6,            // 63 Helptext
        0x58BA62,            // 64 Helptext
        0x58BAC6,            // 65 Helptext
        0x58BBDB,            // 66 Helptext
        0x58BCB0,            // 67 Helptext
        0x58BD58,            // 68 Helptext
        0x58BE8D,            // 69 Helptext
        0x58BF7E,            // 70 Helptext
        0x58BFFC,            // 71 Helptext
        0x580F16,            // 72 Menu system
        0x580F95,            // 73
        0x5810EF,            // 74
        0x581158,            // 75
        0x5811CD,            // 76
        0x58148A,            // 77
        0x5814F7,            // 78
        0x5815B1,            // 79
        0x5815EB,            // 80
        0x581633,            // 81
        0x47AD2A,            // 82
        0x5818CF,            // 83
        0x58CCDB,            // 84 OddJob
        0x58CDE6,            // 85 OddJob
        0x58CEE2,            // 86 OddJob
        0x58D15C,            // 87 OddJob
        0x58A178,            // 88 TripSkip
        0x58A21D,            // 89 TripSkip
        0x58A2C0,            // 90 TripSkip
        0x4E9F30,            // 91 RadioStation
        0x43CF57,            // 92 CDarkel
        0x4477CD,            // 93 CGarages
        0x4477F7,            // 94 CGarages
    };

    uintptr_t hudHeightAddresses[82] = {
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
        0x5894AF,            // 17 Ammo
        0x58F9C0,            // 18 Ammo
        0x58FA4A,            // 19 Ammo
        0x58DCA2,            // 20 Wanted
        0x58DD68,            // 21 Wanted
        0x58DDF4,            // 22 Wanted
        0x58DF55,            // 23 Wanted
        0x58DF9B,            // 24 Wanted
        0x58DEE4,            // 25 Wanted
        0x58B089,            // 26 Vehicle names
        0x58B12D,            // 27 Vehicle names
        0x58AD24,            // 28 Area names
        0x58AE0D,            // 29 Area names
        0x58C37F,            // 30 Subs
        0x58C407,            // 31 Subs
        0x58C4C6,            // 32 Subs
        0x5898F6,            // 33 Stats box text
        0x589735,            // 34 Stats box
        0x58978B,            // 35 Stats box
        0x589813,            // 36 Stats box
        0x58983F,            // 37 Stats box
        0x5898BD,            // 38 Stats box
        0x5899FF,            // 39 Stats box
        0x589A4B,            // 40 Stats box
        0x589B16,            // 41 Stats box
        0x589C5C,            // 42 Stats box
        0x589CA8,            // 43 Stats box
        0x589D4A,            // 44 Stats box
        0x589D92,            // 45 Stats box
        0x589E32,            // 46 Stats box
        0x589E7A,            // 47 Stats box
        0x589F1A,            // 48 Stats box
        0x589F62,            // 49 Stats box
        0x589FFC,            // 50 Stats box
        0x58A040,            // 51 Stats box
        0x58A07A,            // 52 Stats box
        0x58C84D,            // 53 SuccessFailed text
        0x58D2C5,            // 54 MissionTitle text
        0x58CBAB,            // 55 WastedBusted text
        0x58B263,            // 56 Timers
        0x58B5DE,            // 57 Timers
        0x58B7BD,            // 58 Help text
        0x58BA4C,            // 59 Help text
        0x58BBA7,            // 60 Help text
        0x58BD19,            // 61 Help text
        0x58BE2B,            // 62 Help text
        0x58BF1C,            // 63 Help text
        0x58BFCB,            // 64 Help text
        0x580E11,            // 65 Menu system
        0x580F85,            // 66
        0x5810CC,            // 67
        0x581132,            // 68
        0x5811A1,            // 69
        0x58147A,            // 70
        0x5814E7,            // 71
        0x581699,            // 72
        0x581889,            // 73
        0x58CCC5,            // 74 OddJob
        0x58CDD0,            // 75 OddJob
        0x58CECC,            // 76 OddJob
        0x58D146,            // 77 OddJob
        0x4E9F1A,            // 78 RadioStation
        0x43CF47,            // 79 CDarkel
        0x4477B7,            // 80 CGarages
        0x4478AC,            // 81 CGarages
    };

    for (size_t i = 0; i < std::size(hudWidthAddresses); i++)
    {
        MemPut<float*>(hudWidthAddresses[i] + 0x2, &HUDWidth[i]);
    }

    for (size_t i = 0; i < std::size(hudHeightAddresses); i++)
    {
        MemPut<float*>(hudHeightAddresses[i] + 0x2, &HUDHeight[i]);
    }

    // Second player fix.
    MemPut<float*>(0x58F9A0 + 0x2, &HUDWidth[110]);            // Weapon icon X
    MemPut<float*>(0x58F993 + 0x2, &HUDWidth[16]);             // Weapon icon X
    MemPut<float*>(0x58F972 + 0x2, &HUDHeight[16]);            // Weapon icon Y
    MemPut<float*>(0x58FA8E + 0x2, &HUDWidth[17]);             // Ammo x
}

void GetMemoryAddresses()
{
    rsGlobal = (RsGlobal*)0xC17040;
    ScreenAspectRatio = (float*)0xC3EFA4;
}

void CMultiplayerSA::InitHooks_WidescreenFix()
{
    GetMemoryAddresses();

    InstallAspectRatioFixes();
    InstallMiscFixes();
    InstallHUDFixes();
}

/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTimeCycleSA.h
 *  PURPOSE:     Header file for time cycle class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#ifndef __CGAMESA_TIMECYCLE
#define __CGAMESA_TIMECYCLE

#include <game/CTimeCycle.h>
#include "Common.h"

#define VAR_CTimeCyc_AMBRedPool                 0xB7C3C8
#define VAR_CTimeCyc_AMBGreenPool               0xB7C310
#define VAR_CTimeCyc_AMBBluePool                0xB7C258
#define VAR_CTimeCyc_AMB_OBJRedPool             0xB7C1A0
#define VAR_CTimeCyc_AMB_OBJGreenPool           0xB7C0E8
#define VAR_CTimeCyc_AMB_OBJBluePool            0xB7C030
#define VAR_CTimeCyc_SkyTopRedPool              0xB7BF78
#define VAR_CTimeCyc_SkyTopGreenPool            0xB7BEC0
#define VAR_CTimeCyc_SkyTopBluePool             0xB7BE08
#define VAR_CTimeCyc_SkyBotRedPool              0xB7BD50
#define VAR_CTimeCyc_SkyBotGreenPool            0xB7BC98
#define VAR_CTimeCyc_SkyBotBluePool             0xB7BBE0
#define VAR_CTimeCyc_SunCoreRedPool             0xB7BB28
#define VAR_CTimeCyc_SunCoreGreenPool           0xB7BA70
#define VAR_CTimeCyc_SunCoreBluePool            0xB7B9B8
#define VAR_CTimeCyc_SunCoronaRedPool           0xB7B900
#define VAR_CTimeCyc_SunCoronaGreenPool         0xB7B848
#define VAR_CTimeCyc_SunCoronaBluePool          0xB7B790
#define VAR_CTimeCyc_SunszPool                  0xB7B6D8
#define VAR_CTimeCyc_SprszPool                  0xB7B620
#define VAR_CTimeCyc_SprzbghtPool               0xB7B568
#define VAR_CTimeCyc_ShdwPool                   0xB7B4B0
#define VAR_CTimeCyc_LightShdPool               0xB7B3F8
#define VAR_CTimeCyc_PoleShdPool                0xB7B340
#define VAR_CTimeCyc_FarClpPool                 0xB7B1D0
#define VAR_CTimeCyc_FogStPool                  0xB7B060
#define VAR_CTimeCyc_LightOnGroundPool          0xB7AFA8
#define VAR_CTimeCyc_LowCloudsRedPool           0xB7AEF0
#define VAR_CTimeCyc_LowCloudsGreenPool         0xB7AE38
#define VAR_CTimeCyc_LowCloudsBluePool          0xB7AD80
#define VAR_CTimeCyc_BottomCloudsRedPool        0xB7ACC8
#define VAR_CTimeCyc_BottomCloudsGreenPool      0xB7AC10
#define VAR_CTimeCyc_BottomCloudsBluePool       0xB7AB58
#define VAR_CTimeCyc_WaterRedPool               0xB7AAA0
#define VAR_CTimeCyc_WaterGreenPool             0xB7A9E8
#define VAR_CTimeCyc_WaterBluePool              0xB7A930
#define VAR_CTimeCyc_WaterAlphaPool             0xB7A878
#define VAR_CTimeCyc_ColourCorrectRedPool       0xB7A7C0
#define VAR_CTimeCyc_ColourCorrectGreenPool     0xB7A708
#define VAR_CTimeCyc_ColourCorrectBluePool      0xB7A650
#define VAR_CTimeCyc_ColourCorrectAlphaPool     0xB7A598
#define VAR_CTimeCyc_ColourAddRedPool           0xB7A4E0
#define VAR_CTimeCyc_ColourAddGreenPool         0xB7A428
#define VAR_CTimeCyc_ColourAddBluePool          0xB7A370
#define VAR_CTimeCyc_ColourAddAlphaPool         0xB7A2B8
#define VAR_CTimeCyc_LowCloudsAlphaPool         0xB7A200
#define VAR_CTimeCyc_UpperCloudsAlphaPool       0xB7A148
#define VAR_CTimeCyc_IlluminationPool           0xB79FD8

#define TIMECYCLE_AMBIENT_RED                   "AmbientRed"
#define TIMECYCLE_AMBIENT_GREEN                 "AmbientGreen"
#define TIMECYCLE_AMBIENT_BLUE                  "AmbientBlue"
#define TIMECYCLE_AMBIENT_OBJ_RED               "AmbientObjectRed"
#define TIMECYCLE_AMBIENT_OBJ_GREEN             "AmbientObjectGreen"
#define TIMECYCLE_AMBIENT_OBJ_BLUE              "AmbientObjectBlue"
#define TIMECYCLE_SKY_TOP_RED                   "SkyTopRed"
#define TIMECYCLE_SKY_TOP_GREEN                 "SkyTopGreen"
#define TIMECYCLE_SKY_TOP_BLUE                  "SkyTopBlue"
#define TIMECYCLE_SKY_BOTTOM_RED                "SkyBottomRed"
#define TIMECYCLE_SKY_BOTTOM_GREEN              "SkyBottomGreen"
#define TIMECYCLE_SKY_BOTTOM_BLUE               "SkyBottomBlue"
#define TIMECYCLE_SUN_CORE_RED                  "SunCoreRed"
#define TIMECYCLE_SUN_CORE_GREEN                "SunCoreGreen"
#define TIMECYCLE_SUN_CORE_BLUE                 "SunCoreBlue"
#define TIMECYCLE_SUN_CORONA_RED                "SunCoronaRed"
#define TIMECYCLE_SUN_CORONA_GREEN              "SunCoronaGreen"
#define TIMECYCLE_SUN_CORONA_BLUE               "SunCoronaBlue"
#define TIMECYCLE_SUN_SIZE                      "SunSize"
#define TIMECYCLE_SPRITE_SIZE                   "SpriteSize"
#define TIMECYCLE_SPRITE_BRIGHTNESS             "SpriteBrightness"
#define TIMECYCLE_SHADOW_STRENGTH               "ShadowStrength"
#define TIMECYCLE_LIGHT_SHADOW_STRENGTH         "LightShadowStrength"
#define TIMECYCLE_POLE_SHADOW_STRENGTH          "PoleShadowStrength"
#define TIMECYCLE_FAR_CLIP                      "FarClip"
#define TIMECYCLE_FOG_START                     "FogStart"
#define TIMECYCLE_LIGHTS_ON_GROUND              "LightsOnGroundBrightness"
#define TIMECYCLE_LOW_CLOUDS_RED                "LowCloudsRed"
#define TIMECYCLE_LOW_CLOUDS_GREEN              "LowCloudsGreen"
#define TIMECYCLE_LOW_CLOUDS_BLUE               "LowCloudsBlue"
#define TIMECYCLE_BOTTOM_CLOUDS_RED             "BottomCloudsRed"
#define TIMECYCLE_BOTTOM_CLOUDS_GREEN           "BottomCloudsGreen"
#define TIMECYCLE_BOTTOM_CLOUDS_BLUE            "BottomCloudsBlue"
#define TIMECYCLE_WATER_RED                     "WaterRed"
#define TIMECYCLE_WATER_GREEN                   "WaterGreen"
#define TIMECYCLE_WATER_BLUE                    "WaterBlue"
#define TIMECYCLE_WATER_ALPHA                   "WaterAlpha"
#define TIMECYCLE_COLOUR_CORRECT_RED            "ColourCorrectRed"
#define TIMECYCLE_COLOUR_CORRECT_GREEN          "ColourCorrectGreen"
#define TIMECYCLE_COLOUR_CORRECT_BLUE           "ColourCorrectBlue"
#define TIMECYCLE_COLOUR_CORRECT_ALPHA          "ColourCorrectAlpha"
#define TIMECYCLE_COLOUR_ADD_RED                "ColourAddRed"
#define TIMECYCLE_COLOUR_ADD_GREEN              "ColourAddGreen"
#define TIMECYCLE_COLOUR_ADD_BLUE               "ColourAddBlue"
#define TIMECYCLE_COLOUR_ADD_ALPHA              "ColourAddAlpha"
#define TIMECYCLE_LOW_CLOUDS_ALPHA              "LowCloudsAlpha"
#define TIMECYCLE_UPPER_CLOUDS_ALPHA            "UpperCloudsAlpha"
#define TIMECYCLE_ILLUMINATION                  "Illumination"

class CTimeCycleSA : public CTimeCycle
{
public:
    // constructor
    CTimeCycleSA();
    ~CTimeCycleSA(void);

    bool    ResetTimeCycle();
    bool    SetTimeCycle(const char* szPropertyName, float fValue, int iWeatherID, int iCycleNum);
    float   GetTimeCycle(int iWeatherID, int iCycleNum, const char* strPropertyName);
    void    GetTimeCycle(int iWeatherID, int iCycleNum, std::map<const char*, float>& m_Properties);
    void    GetTimeCycle(int iWeatherID, std::map<int, std::map<const char*, float>>& m_Properties);
    float   GetOriginalTimeCycle(int iWeatherID, int iCycleNum, const char* strPropertyName);
    void    GetOriginalTimeCycle(int iWeatherID, int iCycleNum, std::map<const char*, float>& m_Properties);
    void    GetOriginalTimeCycle(int iWeatherID, std::map<int, std::map<const char*, float>>& m_Properties);
private:
    std::map<std::string, CTimeCyclePropertyInterface*> m_TimeCycleProps;
};

#endif

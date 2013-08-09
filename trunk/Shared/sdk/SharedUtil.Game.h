/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Game.h
*  PURPOSE:     Shared stuff which is game oriented
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{

    namespace TrafficLight
    {
        enum EColor
        {
            GREEN,
            YELLOW,
            RED
        };
    
        enum EState
        {
            AUTO,
            DISABLED,
        };
    }

    unsigned char   GetTrafficLightStateFromColors ( TrafficLight::EColor eColorNS, TrafficLight::EColor eColorEW );

    class CVehicleColor
    {
    public:
                        CVehicleColor               ( void );

        void            SetRGBColors                ( SColor color1, SColor color2, SColor color3, SColor color4 );
        void            SetPaletteColors            ( uchar ucColor1, uchar ucColor2, uchar ucColor3, uchar ucColor4 );

        void            SetRGBColor                 ( uint uiSlot, SColor color );
        void            SetPaletteColor             ( uint uiSlot, uchar ucColor );

        SColor          GetRGBColor                 ( uint uiSlot );
        uchar           GetPaletteColor             ( uint uiSlot );

        int             GetNumColorsUsed            ( void );

        static uchar    GetPaletteIndexFromRGB      ( SColor color );
        static SColor   GetRGBFromPaletteIndex      ( uchar ucColor );

    protected:
        void            InvalidatePaletteColors     ( void );
        void            ValidatePaletteColors       ( void );
        void            InvalidateRGBColors         ( void );
        void            ValidateRGBColors           ( void );

        SColor          m_RGBColors[4];
        uchar           m_ucPaletteColors[4];
        bool            m_bPaletteColorsWrong;
        bool            m_bRGBColorsWrong;
    };


    struct SHeatHazeSettings
    {
        SHeatHazeSettings ( void )
            : ucIntensity ( 0 )
            , ucRandomShift ( 0 )
            , usSpeedMin ( 1 )
            , usSpeedMax ( 1 )
            , sScanSizeX ( 1 )
            , sScanSizeY ( 1 )
            , usRenderSizeX ( 1 )
            , usRenderSizeY ( 1 )
            , bInsideBuilding ( false )
        {}

        uchar       ucIntensity;        //     0 to 255
        uchar       ucRandomShift;      //     0 to 255
        ushort      usSpeedMin;         //     0 to 1000
        ushort      usSpeedMax;         //     0 to 1000
        short       sScanSizeX;         // -1000 to 1000
        short       sScanSizeY;         // -1000 to 1000
        ushort      usRenderSizeX;      //     0 to 1000
        ushort      usRenderSizeY;      //     0 to 1000
        bool        bInsideBuilding;
    };


    namespace EDiagnosticDebug
    {
        enum EDiagnosticDebugType
        {
            NONE,
            GRAPHICS_6734,
            BIDI_6778,
            D3D_6732,
            LOG_TIMING_0000,
            JOYSTICK_0000,
            MAX,
        };
    };

    using EDiagnosticDebug::EDiagnosticDebugType;


    // Common strings for SetApplicationSetting
    #define WD_SECTION_PRELOAD_UPGRADES         "preload-upgrades"
    #define DIAG_PRELOAD_UPGRADES_SLOW          "diagnostics", "preloading-upgrades-slow"
    #define DIAG_PRELOAD_UPGRADE_ATTEMPT_ID     "diagnostics", "preloading-upgrade-attempt-id"
    #define DIAG_PRELOAD_UPGRADES_LOWEST_UNSAFE "diagnostics", "preloading-upgrades-lowest-unsafe"
    #define DIAG_PRELOAD_UPGRADES_HISCORE       "diagnostics", "preloading-upgrades-hiscore"
    #define DIAG_CRASH_EXTRA_MSG                "diagnostics", "last-crash-reason"
    #define DIAG_MINIDUMP_DETECTED_COUNT        "diagnostics", "num-minidump-detected"
    #define DIAG_MINIDUMP_CONFIRMED_COUNT       "diagnostics", "num-minidump-confirmed"

}

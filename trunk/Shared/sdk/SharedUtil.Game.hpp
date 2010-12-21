/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Game.hpp
*  PURPOSE:     Shared stuff which is game oriented
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    bool GetTrafficLightStateFromStrings ( const char* szColorNS, const char* szColorEW,
                                           unsigned char& ucOutput )
    {
        enum ETrafficColor
        {
            UNKNOWN,
            GREEN,
            YELLOW,
            RED
        };
        ETrafficColor eColorNS = UNKNOWN;
        ETrafficColor eColorEW = UNKNOWN;

        // Transform the colors from strings to a numeric value.
        if ( ! stricmp ( szColorNS, "green" ) ) eColorNS = GREEN;
        else if ( ! stricmp ( szColorNS, "yellow" ) ) eColorNS = YELLOW;
        else if ( ! stricmp ( szColorNS, "amber" ) ) eColorNS = YELLOW;
        else if ( ! stricmp ( szColorNS, "red" ) ) eColorNS = RED;
        if ( ! stricmp ( szColorEW, "green" ) ) eColorEW = GREEN;
        else if ( ! stricmp ( szColorEW, "yellow" ) ) eColorEW = YELLOW;
        else if ( ! stricmp ( szColorEW, "amber" ) ) eColorEW = YELLOW;
        else if ( ! stricmp ( szColorEW, "red" ) ) eColorEW = RED;

        if ( eColorNS != UNKNOWN && eColorEW != UNKNOWN )
        {
            // Get the state number
            unsigned char ucState = 0;
            switch ( eColorEW )
            {
                case RED:
                    switch ( eColorNS ) {
                        case GREEN: ucState = 0; break;
                        case YELLOW: ucState = 1; break;
                        case RED: ucState = 2; break;
                        default: break;
                    }
                    break;
                case YELLOW:
                    switch ( eColorNS ) {
                        case GREEN: ucState = 8; break;
                        case YELLOW: ucState = 6; break;
                        case RED: ucState = 4; break;
                        default: break;
                    }
                    break;
                case GREEN:
                    switch ( eColorNS ) {
                        case GREEN: ucState = 5; break;
                        case YELLOW: ucState = 7; break;
                        case RED: ucState = 3; break;
                        default: break;
                    }
                    break;
                default:
                    break;
            }

            ucOutput = ucState;
            return true;
        }
        else
            return false;
    }

    CVehicleColor::CVehicleColor ( void )
    {
        // Init
        m_ucPaletteColors[0] = 0; // Palette color 0 is black
        m_ucPaletteColors[1] = 0;
        m_ucPaletteColors[2] = 0;
        m_ucPaletteColors[3] = 0;
        m_RGBColors[0] = 0;
        m_RGBColors[1] = 0;
        m_RGBColors[2] = 0;
        m_RGBColors[3] = 0;
        m_bPaletteColorsWrong = false;
        m_bRGBColorsWrong = false;
    }

    // Use black for colours that are not used (bandwidth saving)
    void CVehicleColor::SetRGBColors ( SColor color1, SColor color2, SColor color3, SColor color4 )
    {
        if (
            m_RGBColors[0] != color1 ||
            m_RGBColors[1] != color2 ||
            m_RGBColors[2] != color3 ||
            m_RGBColors[3] != color4
            )
        {
            m_RGBColors[0] = color1;
            m_RGBColors[1] = color2;
            m_RGBColors[2] = color3;
            m_RGBColors[3] = color4;
            InvalidatePaletteColors ();
        }
    }

    void CVehicleColor::SetPaletteColors ( unsigned char ucColor1, unsigned char ucColor2, unsigned char ucColor3, unsigned char ucColor4 )
    {
        if ( m_ucPaletteColors[0] != ucColor1 ||
             m_ucPaletteColors[1] != ucColor2 ||
             m_ucPaletteColors[2] != ucColor3 ||
             m_ucPaletteColors[3] != ucColor4 )
        {
            m_ucPaletteColors[0] = ucColor1;
            m_ucPaletteColors[1] = ucColor2;
            m_ucPaletteColors[2] = ucColor3;
            m_ucPaletteColors[3] = ucColor4;
            InvalidateRGBColors ();
        }
    }

    void CVehicleColor::SetRGBColor ( uint uiSlot, SColor color )
    {
        ValidateRGBColors ();
        uiSlot = Min ( uiSlot, NUMELMS( m_RGBColors ) );
        if ( m_RGBColors [ uiSlot ] != color )
        {
            m_RGBColors [ uiSlot ] = color;
            InvalidatePaletteColors ();
        }
    }

    void CVehicleColor::SetPaletteColor ( uint uiSlot, uchar ucColor )
    {
        ValidatePaletteColors ();
        uiSlot = Min ( uiSlot, NUMELMS( m_ucPaletteColors ) );
        if ( m_ucPaletteColors [ uiSlot ] != ucColor )
        {
            m_ucPaletteColors [ uiSlot ] = ucColor;
            InvalidateRGBColors ();
        }
    }

    // Get a slot colour as a palette index
    uchar CVehicleColor::GetPaletteColor ( uint uiSlot )
    {
        ValidatePaletteColors ();
        uiSlot = Min ( uiSlot, NUMELMS( m_ucPaletteColors ) );
        return m_ucPaletteColors [ uiSlot ];
    }

    // Get a slot colour as an RGB colour
    SColor CVehicleColor::GetRGBColor ( uint uiSlot )
    {
        ValidateRGBColors ();
        uiSlot = Min ( uiSlot, NUMELMS( m_RGBColors ) );
        return m_RGBColors [ uiSlot ];
    }

    //  Can return: 1,2,3, or 4
    int CVehicleColor::GetNumColorsUsed ( void )
    {
        // Find last unblack
        int i;
        for ( i = NUMELMS( m_RGBColors ) ; i > 1 ; i-- )
        {
            if ( GetRGBColor ( i - 1 ) )
                break;
        }
        assert ( i >= 1 && i <= 4 );
        return i;
    }


    // Switching to RGB mode
    void CVehicleColor::InvalidatePaletteColors ( void )
    {
        m_bRGBColorsWrong = false;
        m_bPaletteColorsWrong = true;
    }

    // Switching to palette mode
    void CVehicleColor::InvalidateRGBColors ( void )
    {
        m_bPaletteColorsWrong = false;
        m_bRGBColorsWrong = true;
    }

    // Ensure switched
    void CVehicleColor::ValidateRGBColors ( void )
    {
        if ( m_bRGBColorsWrong )
        {
            m_bRGBColorsWrong = false;
            for ( uint i = 0 ; i < NUMELMS( m_RGBColors ) ; i++ )
                m_RGBColors[i] = GetRGBFromPaletteIndex ( m_ucPaletteColors[i] );
        }
    }

    // Ensure switched
    void CVehicleColor::ValidatePaletteColors ( void )
    {
        if ( m_bPaletteColorsWrong )
        {
            m_bPaletteColorsWrong = false;
            for ( uint i = 0 ; i < NUMELMS( m_ucPaletteColors ) ; i++ )
                m_ucPaletteColors[i] = GetPaletteIndexFromRGB ( m_RGBColors[i] );
        }
    }


    static const uchar paletteColorTable8[] = {
                0x00, 0x00, 0x00, 0xff, 0xf5, 0xf5, 0xf5, 0xff, 0x2a, 0x77, 0xa1, 0xff, 0x84, 0x04, 0x10, 0xff,
                0x26, 0x37, 0x39, 0xff, 0x86, 0x44, 0x6e, 0xff, 0xd7, 0x8e, 0x10, 0xff, 0x4c, 0x75, 0xb7, 0xff,
                0xbd, 0xbe, 0xc6, 0xff, 0x5e, 0x70, 0x72, 0xff, 0x46, 0x59, 0x7a, 0xff, 0x65, 0x6a, 0x79, 0xff,
                0x5d, 0x7e, 0x8d, 0xff, 0x58, 0x59, 0x5a, 0xff, 0xd6, 0xda, 0xd6, 0xff, 0x9c, 0xa1, 0xa3, 0xff,
                0x33, 0x5f, 0x3f, 0xff, 0x73, 0x0e, 0x1a, 0xff, 0x7b, 0x0a, 0x2a, 0xff, 0x9f, 0x9d, 0x94, 0xff,
                0x3b, 0x4e, 0x78, 0xff, 0x73, 0x2e, 0x3e, 0xff, 0x69, 0x1e, 0x3b, 0xff, 0x96, 0x91, 0x8c, 0xff,
                0x51, 0x54, 0x59, 0xff, 0x3f, 0x3e, 0x45, 0xff, 0xa5, 0xa9, 0xa7, 0xff, 0x63, 0x5c, 0x5a, 0xff,
                0x3d, 0x4a, 0x68, 0xff, 0x97, 0x95, 0x92, 0xff, 0x42, 0x1f, 0x21, 0xff, 0x5f, 0x27, 0x2b, 0xff,
                0x84, 0x94, 0xab, 0xff, 0x76, 0x7b, 0x7c, 0xff, 0x64, 0x64, 0x64, 0xff, 0x5a, 0x57, 0x52, 0xff,
                0x25, 0x25, 0x27, 0xff, 0x2d, 0x3a, 0x35, 0xff, 0x93, 0xa3, 0x96, 0xff, 0x6d, 0x7a, 0x88, 0xff,
                0x22, 0x19, 0x18, 0xff, 0x6f, 0x67, 0x5f, 0xff, 0x7c, 0x1c, 0x2a, 0xff, 0x5f, 0x0a, 0x15, 0xff,
                0x19, 0x38, 0x26, 0xff, 0x5d, 0x1b, 0x20, 0xff, 0x9d, 0x98, 0x72, 0xff, 0x7a, 0x75, 0x60, 0xff,
                0x98, 0x95, 0x86, 0xff, 0xad, 0xb0, 0xb0, 0xff, 0x84, 0x89, 0x88, 0xff, 0x30, 0x4f, 0x45, 0xff,
                0x4d, 0x62, 0x68, 0xff, 0x16, 0x22, 0x48, 0xff, 0x27, 0x2f, 0x4b, 0xff, 0x7d, 0x62, 0x56, 0xff,
                0x9e, 0xa4, 0xab, 0xff, 0x9c, 0x8d, 0x71, 0xff, 0x6d, 0x18, 0x22, 0xff, 0x4e, 0x68, 0x81, 0xff,
                0x9c, 0x9c, 0x98, 0xff, 0x91, 0x73, 0x47, 0xff, 0x66, 0x1c, 0x26, 0xff, 0x94, 0x9d, 0x9f, 0xff,
                0xa4, 0xa7, 0xa5, 0xff, 0x8e, 0x8c, 0x46, 0xff, 0x34, 0x1a, 0x1e, 0xff, 0x6a, 0x7a, 0x8c, 0xff,
                0xaa, 0xad, 0x8e, 0xff, 0xab, 0x98, 0x8f, 0xff, 0x85, 0x1f, 0x2e, 0xff, 0x6f, 0x82, 0x97, 0xff,
                0x58, 0x58, 0x53, 0xff, 0x9a, 0xa7, 0x90, 0xff, 0x60, 0x1a, 0x23, 0xff, 0x20, 0x20, 0x2c, 0xff,
                0xa4, 0xa0, 0x96, 0xff, 0xaa, 0x9d, 0x84, 0xff, 0x78, 0x22, 0x2b, 0xff, 0x0e, 0x31, 0x6d, 0xff,
                0x72, 0x2a, 0x3f, 0xff, 0x7b, 0x71, 0x5e, 0xff, 0x74, 0x1d, 0x28, 0xff, 0x1e, 0x2e, 0x32, 0xff,
                0x4d, 0x32, 0x2f, 0xff, 0x7c, 0x1b, 0x44, 0xff, 0x2e, 0x5b, 0x20, 0xff, 0x39, 0x5a, 0x83, 0xff,
                0x6d, 0x28, 0x37, 0xff, 0xa7, 0xa2, 0x8f, 0xff, 0xaf, 0xb1, 0xb1, 0xff, 0x36, 0x41, 0x55, 0xff,
                0x6d, 0x6c, 0x6e, 0xff, 0x0f, 0x6a, 0x89, 0xff, 0x20, 0x4b, 0x6b, 0xff, 0x2b, 0x3e, 0x57, 0xff,
                0x9b, 0x9f, 0x9d, 0xff, 0x6c, 0x84, 0x95, 0xff, 0x4d, 0x84, 0x95, 0xff, 0xae, 0x9b, 0x7f, 0xff,
                0x40, 0x6c, 0x8f, 0xff, 0x1f, 0x25, 0x3b, 0xff, 0xab, 0x92, 0x76, 0xff, 0x13, 0x45, 0x73, 0xff,
                0x96, 0x81, 0x6c, 0xff, 0x64, 0x68, 0x6a, 0xff, 0x10, 0x50, 0x82, 0xff, 0xa1, 0x99, 0x83, 0xff,
                0x38, 0x56, 0x94, 0xff, 0x52, 0x56, 0x61, 0xff, 0x7f, 0x69, 0x56, 0xff, 0x8c, 0x92, 0x9a, 0xff,
                0x59, 0x6e, 0x87, 0xff, 0x47, 0x35, 0x32, 0xff, 0x44, 0x62, 0x4f, 0xff, 0x73, 0x0a, 0x27, 0xff,
                0x22, 0x34, 0x57, 0xff, 0x64, 0x0d, 0x1b, 0xff, 0xa3, 0xad, 0xc6, 0xff, 0x69, 0x58, 0x53, 0xff,
                0x9b, 0x8b, 0x80, 0xff, 0x62, 0x0b, 0x1c, 0xff, 0x5b, 0x5d, 0x5e, 0xff, 0x62, 0x44, 0x28, 0xff,
                0x73, 0x18, 0x27, 0xff, 0x1b, 0x37, 0x6d, 0xff, 0xec, 0x6a, 0xae, 0xff,
            };


    uchar CVehicleColor::GetPaletteIndexFromRGB ( SColor color )
    {
        ulong ulBestDist = 0xFFFFFFFF;
        uchar ucBestMatch = 0;
        for ( uint i = 0 ; i < NUMELMS( paletteColorTable8 ) / 4 ; i++ )
        {
            int r = paletteColorTable8[ i * 4 + 0 ] - color.R;
            int g = paletteColorTable8[ i * 4 + 1 ] - color.G;
            int b = paletteColorTable8[ i * 4 + 2 ] - color.B;
            ulong ulDist = r * r + g * g + b * b;
            if ( ulDist < ulBestDist )
            {
                ulBestDist = ulDist;
                ucBestMatch = i;
            }
        }
        return ucBestMatch;
    }

    SColor CVehicleColor::GetRGBFromPaletteIndex ( uchar ucColor )
    {
        ucColor = Min < uchar > ( ucColor, NUMELMS( paletteColorTable8 ) / 4 );
        uchar r = paletteColorTable8[ ucColor * 4 ];
        uchar g = paletteColorTable8[ ucColor * 4 + 1 ];
        uchar b = paletteColorTable8[ ucColor * 4 + 2 ];
        return SColorRGBA ( r, g, b, 0 );
    }

}

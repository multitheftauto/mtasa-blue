#include "svgcolor.h"
#include "svgdocumentimpl.h"
#include "svgpaintelement.h"
#include "rendercontext.h"

#include <map>

namespace lunasvg {

static const std::map<std::string, Rgb> colormap = {
    {"aliceblue", 0xF0F8FFFF},
    {"antiquewhite", 0xFAEBD7FF},
    {"aqua", 0x00FFFFFF},
    {"aquamarine", 0x7FFFD4FF},
    {"azure", 0xF0FFFFFF},
    {"beige", 0xF5F5DCFF},
    {"bisque", 0xFFE4C4FF},
    {"black", 0x000000FF},
    {"blanchedalmond", 0xFFEBCDFF},
    {"blue", 0x0000FFFF},
    {"blueviolet", 0x8A2BE2FF},
    {"brown", 0xA52A2AFF},
    {"burlywood", 0xDEB887FF},
    {"cadetblue", 0x5F9EA0FF},
    {"chartreuse", 0x7FFF00FF},
    {"chocolate", 0xD2691EFF},
    {"coral", 0xFF7F50FF},
    {"cornflowerblue", 0x6495EDFF},
    {"cornsilk", 0xFFF8DCFF},
    {"crimson", 0xDC143CFF},
    {"cyan", 0x00FFFFFF},
    {"darkblue", 0x00008BFF},
    {"darkcyan", 0x008B8BFF},
    {"darkgoldenrod", 0xB8860BFF},
    {"darkgray", 0xA9A9A9FF},
    {"darkgreen", 0x006400FF},
    {"darkgrey", 0xA9A9A9FF},
    {"darkkhaki", 0xBDB76BFF},
    {"darkmagenta", 0x8B008BFF},
    {"darkolivegreen", 0x556B2FFF},
    {"darkorange", 0xFF8C00FF},
    {"darkorchid", 0x9932CCFF},
    {"darkred", 0x8B0000FF},
    {"darksalmon", 0xE9967AFF},
    {"darkseagreen", 0x8FBC8FFF},
    {"darkslateblue", 0x483D8BFF},
    {"darkslategray", 0x2F4F4FFF},
    {"darkslategrey", 0x2F4F4FFF},
    {"darkturquoise", 0x00CED1FF},
    {"darkviolet", 0x9400D3FF},
    {"deeppink", 0xFF1493FF},
    {"deepskyblue", 0x00BFFFFF},
    {"dimgray", 0x696969FF},
    {"dimgrey", 0x696969FF},
    {"dodgerblue", 0x1E90FFFF},
    {"firebrick", 0xB22222FF},
    {"floralwhite", 0xFFFAF0FF},
    {"forestgreen", 0x228B22FF},
    {"fuchsia", 0xFF00FFFF},
    {"gainsboro", 0xDCDCDCFF},
    {"ghostwhite", 0xF8F8FFFF},
    {"gold", 0xFFD700FF},
    {"goldenrod", 0xDAA520FF},
    {"gray", 0x808080FF},
    {"green", 0x008000FF},
    {"greenyellow", 0xADFF2FFF},
    {"grey", 0x808080FF},
    {"honeydew", 0xF0FFF0FF},
    {"hotpink", 0xFF69B4FF},
    {"indianred", 0xCD5C5CFF},
    {"indigo", 0x4B0082FF},
    {"ivory", 0xFFFFF0FF},
    {"khaki", 0xF0E68CFF},
    {"lavender", 0xE6E6FAFF},
    {"lavenderblush", 0xFFF0F5FF},
    {"lawngreen", 0x7CFC00FF},
    {"lemonchiffon", 0xFFFACDFF},
    {"lightblue", 0xADD8E6FF},
    {"lightcoral", 0xF08080FF},
    {"lightcyan", 0xE0FFFFFF},
    {"lightgoldenrodyellow", 0xFAFAD2FF},
    {"lightgray", 0xD3D3D3FF},
    {"lightgreen", 0x90EE90FF},
    {"lightgrey", 0xD3D3D3FF},
    {"lightpink", 0xFFB6C1FF},
    {"lightsalmon", 0xFFA07AFF},
    {"lightseagreen", 0x20B2AAFF},
    {"lightskyblue", 0x87CEFAFF},
    {"lightslategray", 0x778899FF},
    {"lightslategrey", 0x778899FF},
    {"lightsteelblue", 0xB0C4DEFF},
    {"lightyellow", 0xFFFFE0FF},
    {"lime", 0x00FF00FF},
    {"limegreen", 0x32CD32FF},
    {"linen", 0xFAF0E6FF},
    {"magenta", 0xFF00FFFF},
    {"maroon", 0x800000FF},
    {"mediumaquamarine", 0x66CDAAFF},
    {"mediumblue", 0x0000CDFF},
    {"mediumorchid", 0xBA55D3FF},
    {"mediumpurple", 0x9370DBFF},
    {"mediumseagreen", 0x3CB371FF},
    {"mediumslateblue", 0x7B68EEFF},
    {"mediumspringgreen", 0x00FA9AFF},
    {"mediumturquoise", 0x48D1CCFF},
    {"mediumvioletred", 0xC71585FF},
    {"midnightblue", 0x191970FF},
    {"mintcream", 0xF5FFFAFF},
    {"mistyrose", 0xFFE4E1FF},
    {"moccasin", 0xFFE4B5FF},
    {"navajowhite", 0xFFDEADFF},
    {"navy", 0x000080FF},
    {"oldlace", 0xFDF5E6FF},
    {"olive", 0x808000FF},
    {"olivedrab", 0x6B8E23FF},
    {"orange", 0xFFA500FF},
    {"orangered", 0xFF4500FF},
    {"orchid", 0xDA70D6FF},
    {"palegoldenrod", 0xEEE8AAFF},
    {"palegreen", 0x98FB98FF},
    {"paleturquoise", 0xAFEEEEFF},
    {"palevioletred", 0xDB7093FF},
    {"papayawhip", 0xFFEFD5FF},
    {"peachpuff", 0xFFDAB9FF},
    {"peru", 0xCD853FFF},
    {"pink", 0xFFC0CBFF},
    {"plum", 0xDDA0DDFF},
    {"powderblue", 0xB0E0E6FF},
    {"purple", 0x800080FF},
    {"rebeccapurple", 0x663399FF},
    {"red", 0xFF0000FF},
    {"rosybrown", 0xBC8F8FFF},
    {"royalblue", 0x4169E1FF},
    {"saddlebrown", 0x8B4513FF},
    {"salmon", 0xFA8072FF},
    {"sandybrown", 0xF4A460FF},
    {"seagreen", 0x2E8B57FF},
    {"seashell", 0xFFF5EEFF},
    {"sienna", 0xA0522DFF},
    {"silver", 0xC0C0C0FF},
    {"skyblue", 0x87CEEBFF},
    {"slateblue", 0x6A5ACDFF},
    {"slategray", 0x708090FF},
    {"slategrey", 0x708090FF},
    {"snow", 0xFFFAFAFF},
    {"springgreen", 0x00FF7FFF},
    {"steelblue", 0x4682B4FF},
    {"tan", 0xD2B48CFF},
    {"teal", 0x008080FF},
    {"thistle", 0xD8BFD8FF},
    {"tomato", 0xFF6347FF},
    {"turquoise", 0x40E0D0FF},
    {"violet", 0xEE82EEFF},
    {"wheat", 0xF5DEB3FF},
    {"white", 0xFFFFFFFF},
    {"whitesmoke", 0xF5F5F5FF},
    {"yellow", 0xFFFF00FF},
    {"yellowgreen", 0x9ACD32FF}
};

SVGColor::SVGColor()
    : m_colorType(ColorTypeRgb)
{
}

Paint SVGColor::getPaint(const RenderState& state) const
{
    switch(m_colorType)
    {
    case ColorTypeNone:
        return Paint();
    case ColorTypeRgb:
        return m_value;
    case ColorTypeCurrentColor:
        return state.color;
    default:
        break;
    }

    return Paint();
}

bool SVGColor::parseColorComponent(const char*& ptr, double& value)
{
    if(!Utils::parseNumber(ptr, value))
        return false;

    if(Utils::skipDesc(ptr, "%", 1))
        value *= 2.56;

    value = (value < 0.0) ? 0.0 : (value > 255.0) ? 255.0 : std::round(value);
    return true;
}

void SVGColor::setValueAsString(const std::string& value)
{
    m_colorType = ColorTypeRgb;
    m_value = KRgbBlack;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(Utils::skipDesc(ptr, "#", 1))
    {
        const char* start = ptr;
        while(*ptr && Utils::isIntegralDigit(*ptr, 16))
            ++ptr;
        std::ptrdiff_t pos = ptr - start;
        unsigned int hex = 0;
        if(pos == 3)
        {
            std::string hex_str;
            hex_str.reserve(6);
            hex_str.append(2, start[0]);
            hex_str.append(2, start[1]);
            hex_str.append(2, start[2]);
            const char* s = hex_str.c_str();
            Utils::parseInteger(s, hex, 16);
        }
        else if(pos == 6)
        {
            Utils::parseInteger(start, hex, 16);
        }
        else
        {
            return;
        }

        m_colorType = ColorTypeRgb;
        m_value.r = (hex&0xff0000)>>16;
        m_value.g = (hex&0x00ff00)>>8;
        m_value.b = (hex&0x0000ff)>>0;
        m_value.a = 255;
    }
    else if(Utils::skipDesc(ptr, "none", 4))
    {
        m_colorType = ColorTypeNone;
        m_value.r = 0;
        m_value.g = 0;
        m_value.b = 0;
        m_value.a = 0;
    }
    else if(Utils::skipDesc(ptr, "currentColor", 12))
    {
        m_colorType = ColorTypeCurrentColor;
        m_value.r = 0;
        m_value.g = 0;
        m_value.b = 0;
        m_value.a = 255;
    }
    else if(Utils::skipDesc(ptr, "rgb(", 4))
    {
        double r, g, b;
        if(!Utils::skipWs(ptr)
            || !parseColorComponent(ptr, r)
            || !Utils::skipWsComma(ptr)
            || !parseColorComponent(ptr, g)
            || !Utils::skipWsComma(ptr)
            || !parseColorComponent(ptr, b)
            || !Utils::skipWs(ptr)
            || !Utils::skipDesc(ptr, ")", 1))
            return;

        m_colorType = ColorTypeRgb;
        m_value.r = std::uint8_t(r);
        m_value.g = std::uint8_t(g);
        m_value.b = std::uint8_t(b);
        m_value.a = 255;
    }
    else
    {
        std::map<std::string, Rgb>::const_iterator it = colormap.find(value);
        m_colorType = ColorTypeRgb;
        m_value = it != colormap.end() ? it->second : KRgbBlack;
    }
}

std::string SVGColor::valueAsString() const
{
    if(m_colorType == ColorTypeNone)
        return std::string("none");
    if(m_colorType == ColorTypeCurrentColor)
        return  std::string("currentColor");

    char buf[16];
    snprintf(buf, 16, "#%02x%02x%02x", int(m_value.r), int(m_value.g), int(m_value.b));
    return buf;
}

SVGPropertyBase* SVGColor::clone() const
{
    SVGColor* property = new SVGColor();
    property->m_colorType = m_colorType;
    property->m_value = m_value;

    return property;
}

SVGPaint::SVGPaint()
{
}

Paint SVGPaint::getPaint(const RenderState& state) const
{
    if(m_colorType == ColorTypeUrl)
    {
        SVGElementImpl* ref = state.element->document()->impl()->resolveIRI(m_url);
        if(ref && ref->isSVGPaintElement())
            return to<SVGPaintElement>(ref)->getPaint(state);
        return Paint();
    }

    return SVGColor::getPaint(state);
}

void SVGPaint::setValueAsString(const std::string& value)
{
    m_url.clear();
    m_colorType = ColorTypeRgb;
    m_value = KRgbBlack;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(Utils::skipDesc(ptr, "url(", 4))
    {
        const char* closeBracket = strchr(ptr, ')');
        if(!closeBracket)
            return;

        m_colorType = ColorTypeUrl;
        m_url.assign(ptr, closeBracket);
        return;
    }

    SVGColor::setValueAsString(value);
}

std::string SVGPaint::valueAsString() const
{
    return  m_colorType == ColorTypeUrl ? "url(" + m_url + ")" : SVGColor::valueAsString();
}

SVGPropertyBase* SVGPaint::clone() const
{
    SVGPaint* property = new SVGPaint();
    property->m_url = m_url;
    property->m_colorType = m_colorType;
    property->m_value = m_value;

    return property;
}

} // namespace lunasvg

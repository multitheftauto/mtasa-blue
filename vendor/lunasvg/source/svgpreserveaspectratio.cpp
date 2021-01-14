#include "svgpreserveaspectratio.h"
#include "affinetransform.h"
#include "rect.h"

namespace lunasvg {

SVGPreserveAspectRatio::SVGPreserveAspectRatio()
    : m_align(xMidYMid),
      m_scale(Meet)
{
}

AffineTransform SVGPreserveAspectRatio::getMatrix(const Rect& viewPort, const Rect& viewBox) const
{
    AffineTransform matrix;

    matrix.translate(viewPort.x, viewPort.y);
    if(viewBox.width == 0.0 || viewBox.height == 0.0)
        return matrix;

    double scaleX = viewPort.width  / viewBox.width;
    double scaleY = viewPort.height  / viewBox.height;

    if(scaleX == 0.0 || scaleY == 0.0)
        return matrix;

    double transX = -viewBox.x;
    double transY = -viewBox.y;

    if(m_align == None)
    {
        matrix.scale(scaleX, scaleY);
        matrix.translate(transX, transY);
        return matrix;
    }

    double scale = (m_scale == Meet) ? std::min(scaleX, scaleY) : std::max(scaleX, scaleY);

    double viewW = viewPort.width / scale;
    double viewH = viewPort.height / scale;

    switch(m_align)
    {
    case xMidYMin:
    case xMidYMid:
    case xMidYMax:
        transX -= (viewBox.width - viewW) * 0.5;
        break;
    case xMaxYMin:
    case xMaxYMid:
    case xMaxYMax:
        transX -= (viewBox.width - viewW);
        break;
    default:
        break;
    }

    switch(m_align)
    {
    case xMinYMid:
    case xMidYMid:
    case xMaxYMid:
        transY -= (viewBox.height - viewH) * 0.5;
        break;
    case xMinYMax:
    case xMidYMax:
    case xMaxYMax:
        transY -= (viewBox.height - viewH);
        break;
    default:
        break;
    }

    matrix.scale(scale, scale);
    matrix.translate(transX, transY);

    return matrix;
}

const SVGPreserveAspectRatio* SVGPreserveAspectRatio::defaultValue()
{
    static SVGPreserveAspectRatio value;
    return &value;
}

void SVGPreserveAspectRatio::setValueAsString(const std::string& value)
{
    m_align = xMidYMid;
    m_scale = Meet;
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(Utils::skipDesc(ptr, "none", 4))
        m_align = None;
    else if(Utils::skipDesc(ptr, "xMinYMin", 8))
        m_align = xMinYMin;
    else if(Utils::skipDesc(ptr, "xMidYMin", 8))
        m_align = xMidYMin;
    else if(Utils::skipDesc(ptr, "xMaxYMin", 8))
        m_align = xMaxYMin;
    else if(Utils::skipDesc(ptr, "xMinYMid", 8))
        m_align = xMinYMid;
    else if(Utils::skipDesc(ptr, "xMidYMid", 8))
        m_align = xMidYMid;
    else if(Utils::skipDesc(ptr, "xMaxYMid", 8))
        m_align = xMaxYMid;
    else if(Utils::skipDesc(ptr, "xMinYMax", 8))
        m_align = xMinYMax;
    else if(Utils::skipDesc(ptr, "xMidYMax", 8))
        m_align = xMidYMax;
    else if(Utils::skipDesc(ptr, "xMaxYMax", 8))
        m_align = xMaxYMax;
    else
        return;

    if(m_align != None)
    {
        Utils::skipWs(ptr);
        if(Utils::skipDesc(ptr, "meet", 4))
            m_scale = Meet;
        if(Utils::skipDesc(ptr, "slice", 5))
            m_scale = Slice;
    }
}

std::string SVGPreserveAspectRatio::valueAsString() const
{
    std::string out;

    switch(m_align)
    {
    case None:
        out += "none";
        break;
    case xMinYMin:
        out += "xMinYMin";
        break;
    case xMidYMin:
        out += "xMidYMin";
        break;
    case xMaxYMin:
        out += "xMaxYMin";
        break;
    case xMinYMid:
        out += "xMinYMid";
        break;
    case xMidYMid:
        out += "xMidYMid";
        break;
    case xMaxYMid:
        out += "xMaxYMid";
        break;
    case xMinYMax:
        out += "xMinYMax";
        break;
    case xMidYMax:
        out += "xMidYMax";
        break;
    case xMaxYMax:
        out += "xMaxYMax";
        break;
    }

    if(m_align != None)
    {
        out += ' ';
        if(m_scale == Meet)
            out += "meet";
        else
            out += "slice";
    }

    return out;
}

SVGPropertyBase* SVGPreserveAspectRatio::clone() const
{
    SVGPreserveAspectRatio* property = new SVGPreserveAspectRatio();
    property->m_align = m_align;
    property->m_scale = m_scale;

    return property;
}

} // namespace lunasvg

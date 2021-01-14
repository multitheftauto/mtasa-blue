#ifndef SVGCOLOR_H
#define SVGCOLOR_H

#include "svgproperty.h"
#include "paint.h"

namespace lunasvg {

enum ColorType
{
    ColorTypeNone,
    ColorTypeRgb,
    ColorTypeCurrentColor,
    ColorTypeUrl
};

class RenderState;

class SVGColor : public SVGPropertyBase
{
public:
    SVGColor();

    void setValue(const Rgb& value) { m_value = value; }
    void setColorType(ColorType type) { m_colorType = type; }
    const Rgb& value() const { return m_value; }
    ColorType colorType() const { return m_colorType; }

    static bool parseColorComponent(const char*& ptr, double& value);
    virtual Paint getPaint(const RenderState& state) const;
    virtual void setValueAsString(const std::string& value);
    virtual std::string valueAsString() const;
    virtual SVGPropertyBase* clone() const;

protected:
    ColorType m_colorType;
    Rgb m_value;
};

class SVGPaint : public SVGColor
{
public:
    SVGPaint();

    void setUrl(const std::string& url) { m_url = url; }
    const std::string& url() { return  m_url; }

    Paint getPaint(const RenderState& state) const;
    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    std::string m_url;
};

} // namespace lunasvg

#endif // SVGCOLOR_H

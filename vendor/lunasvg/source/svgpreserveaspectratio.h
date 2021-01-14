#ifndef SVGPRESERVEASPECTRATIO_H
#define SVGPRESERVEASPECTRATIO_H

#include "svgproperty.h"

namespace lunasvg {

class AffineTransform;
class Rect;

class SVGPreserveAspectRatio : public SVGPropertyBase
{
public:
    enum Alignment
    {
        None,
        xMinYMin,
        xMidYMin,
        xMaxYMin,
        xMinYMid,
        xMidYMid,
        xMaxYMid,
        xMinYMax,
        xMidYMax,
        xMaxYMax
    };

    enum Scale
    {
        Meet,
        Slice
    };

    SVGPreserveAspectRatio();
    void setAlign(Alignment align) { m_align = align; }
    void setScale(Scale scale) { m_scale = scale; }
    Alignment align() const { return  m_align; }
    Scale scale() const { return  m_scale; }
    AffineTransform getMatrix(const Rect& viewPort, const Rect& viewBox) const;

    static const SVGPreserveAspectRatio* defaultValue();
    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    Alignment m_align;
    Scale m_scale;
};

typedef DOMSVGProperty<SVGPreserveAspectRatio> DOMSVGPreserveAspectRatio;

} // namespace lunasvg

#endif // SVGPRESERVEASPECTRATIO_H

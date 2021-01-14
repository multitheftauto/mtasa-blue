#ifndef SVGTRANSFORM_H
#define SVGTRANSFORM_H

#include "svgproperty.h"
#include "affinetransform.h"
#include "point.h"

namespace lunasvg {

enum TransformType
{
    TransformTypeUnknown,
    TransformTypeMatrix,
    TransformTypeRotate,
    TransformTypeScale,
    TransformTypeSkewX,
    TransformTypeSkewY,
    TransformTypeTranslate
};

class SVGTransform : public SVGPropertyBase
{
public:
    SVGTransform();

    void setValue(const AffineTransform& value) { m_value = value; }
    const AffineTransform& value() const { return  m_value; }

    static bool parseTransform(const char*& ptr, TransformType& type, double* values, int& count);
    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    AffineTransform m_value;
};

typedef DOMSVGProperty<SVGTransform> DOMSVGTransform;

} // namespace lunasvg

#endif // SVGTRANSFORM_H

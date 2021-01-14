#ifndef SVGPATH_H
#define SVGPATH_H

#include "svgproperty.h"
#include "path.h"

namespace lunasvg {

class SVGPath : public SVGPropertyBase
{
public:
    SVGPath();

    void setValue(const Path& value) { m_value = value; }
    const Path& value() const { return  m_value; }

    static bool parseArcFlag(const char*& ptr, bool& flag);
    static bool parseCoordinate(const char*& ptr, double* coords, int length);
    void setValueAsString(const std::string& value);
    std::string valueAsString() const;
    SVGPropertyBase* clone() const;

private:
    Path m_value;
};

typedef DOMSVGProperty<SVGPath> DOMSVGPath;

} // namespace lunasvg

#endif // SVGPATH_H

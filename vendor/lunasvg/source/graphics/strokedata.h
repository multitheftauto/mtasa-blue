#ifndef STROKEDATA_H
#define STROKEDATA_H

#include "svgenumeration.h"

namespace lunasvg {

typedef std::vector<double> DashArray;

class StrokeData
{
public:
    StrokeData();

    void setWidth(double width) { m_width = width; }
    void setMiterLimit(double miterLimit) { m_miterLimit = miterLimit; }
    void setCap(LineCap cap) { m_cap = cap; }
    void setJoin(LineJoin join) { m_join = join; }
    void setDash(const DashArray& dash);
    void setDashOffset(double dashOffset) { m_dashOffset = dashOffset; }
    double width() const { return m_width; }
    double miterLimit() const { return m_miterLimit; }
    LineCap cap() const { return m_cap; }
    LineJoin join() const { return m_join; }
    const DashArray& dash() const { return m_dash; }
    double dashOffset() const { return m_dashOffset; }

private:
    double m_width;
    double m_miterLimit;
    LineCap m_cap;
    LineJoin m_join;
    DashArray m_dash;
    double m_dashOffset;
};

} // namespace lunasvg

#endif // STROKEDATA_H

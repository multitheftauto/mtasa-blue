#include "strokedata.h"
#include <numeric>

namespace lunasvg {

StrokeData::StrokeData()
    : m_width(1.0), m_miterLimit(4.0), m_cap(LineCapButt), m_join(LineJoinMiter), m_dashOffset(0.0)
{
}

void StrokeData::setDash(const DashArray& dash)
{
    m_dash.clear();
    if(std::accumulate(dash.begin(), dash.end(), 0.0) == 0.0)
        return;

    std::size_t num_dash = dash.size() % 2 == 0 ? dash.size() : dash.size() * 2;
    m_dash.resize(num_dash);
    for(unsigned int i = 0;i < num_dash;i++)
        m_dash[i] = dash[i % dash.size()];
}

} // namespace lunasvg

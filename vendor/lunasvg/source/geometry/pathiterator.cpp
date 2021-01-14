#include "pathiterator.h"
#include "point.h"

namespace lunasvg {

PathIterator::PathIterator(const Path& path)
    : m_segments(path.segments()),
      m_coordinates(path.coordinates().data()),
      m_index(0)
{
}

SegType PathIterator::currentSegment(double* coords) const
{
    SegType segType = m_segments[m_index];
    switch(segType)
    {
    case SegTypeMoveTo:
        coords[0] = m_coordinates[0].x;
        coords[1] = m_coordinates[0].y;
        m_startPoint.x = coords[0];
        m_startPoint.y = coords[1];
        break;
    case SegTypeLineTo:
        coords[0] = m_coordinates[0].x;
        coords[1] = m_coordinates[0].y;
        break;
    case SegTypeQuadTo:
        coords[0] = m_coordinates[0].x;
        coords[1] = m_coordinates[0].y;
        coords[2] = m_coordinates[1].x;
        coords[3] = m_coordinates[1].y;
        break;
    case SegTypeCubicTo:
        coords[0] = m_coordinates[0].x;
        coords[1] = m_coordinates[0].y;
        coords[2] = m_coordinates[1].x;
        coords[3] = m_coordinates[1].y;
        coords[4] = m_coordinates[2].x;
        coords[5] = m_coordinates[2].y;
        break;
    case SegTypeClose:
        coords[0] = m_startPoint.x;
        coords[1] = m_startPoint.y;
        break;
    }

    return segType;
}

bool PathIterator::isDone() const
{
    return (m_index >= m_segments.size());
}

void PathIterator::next()
{
    switch(m_segments[m_index])
    {
    case SegTypeMoveTo:
        m_coordinates += 1;
        break;
    case SegTypeLineTo:
        m_coordinates += 1;
        break;
    case SegTypeQuadTo:
        m_coordinates += 2;
        break;
    case SegTypeCubicTo:
        m_coordinates += 3;
        break;
    default:
        break;
    }

    m_index += 1;
}

} // namespace lunasvg

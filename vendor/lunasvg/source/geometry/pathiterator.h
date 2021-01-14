#ifndef PATHITERATOR_H
#define PATHITERATOR_H

#include "path.h"

namespace lunasvg {

class PathIterator
{
public:
   PathIterator(const Path& path);

   SegType currentSegment(double* coords) const;
   bool isDone() const;
   void next();

private:
   mutable Point m_startPoint;
   const std::vector<SegType>& m_segments;
   const Point* m_coordinates;
   unsigned int m_index;
};

} // namespace lunasvg

#endif // PATHITERATOR_H

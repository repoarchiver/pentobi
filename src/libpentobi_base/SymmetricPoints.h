//-----------------------------------------------------------------------------
/** @file libpentobi_base/SymmetricPoints.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_BASE_SYMMETRIC_POINTS_H
#define LIBPENTOBI_BASE_SYMMETRIC_POINTS_H

#include "Geometry.h"
#include "Grid.h"
#include "libboardgame_base/PointTransform.h"

namespace libpentobi_base {

using libboardgame_base::PointTransform;

//-----------------------------------------------------------------------------

/** Lookup table to quickly get points that are symmetric with respect to the
    center of the board. */
class SymmetricPoints
{
public:
    void init(const Geometry& geo, const PointTransform<Point>& transform);

    Point operator[](Point p) const;

private:
    Grid<Point> m_symmetric_point;
};

inline Point SymmetricPoints::operator[](Point p) const
{
    return m_symmetric_point[p];
}

//-----------------------------------------------------------------------------

} // namespace libpentobi_base

#endif // LIBPENTOBI_BASE_SYMMETRIC_POINTS_H

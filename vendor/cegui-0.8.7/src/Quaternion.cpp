/***********************************************************************
    created:    3/1/2011
    author:     Martin Preisler

    purpose:    Implements the Quaternion class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/

#include "CEGUI/Quaternion.h"
#include "CEGUI/Vector.h"
#include "CEGUI/String.h"
#include "CEGUI/PropertyHelper.h"
#include "CEGUI/Exceptions.h"
#include <limits>

// Start of CEGUI namespace section
namespace CEGUI
{

const Quaternion Quaternion::ZERO(0, 0, 0, 0);
const Quaternion Quaternion::IDENTITY(1, 0, 0, 0);

//----------------------------------------------------------------------------//
Quaternion Quaternion::eulerAnglesRadians(const float x, const float y, const float z)
{
    // the order of rotation:
    // 1) around Z
    // 2) around Y
    // 3) around X
    // even though it's passed in reverse...

    const float sin_z_2 = sinf(0.5f * z);
	const float sin_y_2 = sinf(0.5f * y);
	const float sin_x_2 = sinf(0.5f * x);

	const float cos_z_2 = cosf(0.5f * z);
	const float cos_y_2 = cosf(0.5f * y);
	const float cos_x_2 = cosf(0.5f * x);

    return Quaternion(
        cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2,
	    cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2,
	    cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2,
	    sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2
    );
}

//----------------------------------------------------------------------------//
Quaternion Quaternion::eulerAnglesDegrees(const float x, const float y, const float z)
{
    static const float d2r = (4.0f * std::atan2(1.0f, 1.0f)) / 180.0f;

    return eulerAnglesRadians(x * d2r, y * d2r, z * d2r);
}

//----------------------------------------------------------------------------//
Quaternion Quaternion::axisAngleRadians(const Vector3f& axis, const float rotation)
{
    const float halfRotation = 0.5f * rotation;
    const float halfSin = sinf(halfRotation);

    return Quaternion(cosf(halfRotation),
        halfSin * axis.d_x, halfSin * axis.d_y, halfSin * axis.d_z);
}

//----------------------------------------------------------------------------//
Quaternion Quaternion::axisAngleDegrees(const Vector3f& axis, const float rotation)
{
    static const float d2r = (4.0f * std::atan2(1.0f, 1.0f)) / 180.0f;

    return axisAngleRadians(axis, rotation * d2r);
}

//----------------------------------------------------------------------------//
Quaternion Quaternion::slerp(const Quaternion& left, const Quaternion& right,
    float position, const bool shortestPath)
{
    // Geometric Tools, LLC
    // Copyright (c) 1998-2010
    // Distributed under the Boost Software License, Version 1.0.
    // http://www.boost.org/LICENSE_1_0.txt
    // http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt

    float vcos = left.dot(right);
    Quaternion _right;

    // Do we need to invert rotation?
    if (vcos < 0.0f && shortestPath)
    {
        vcos = -vcos;
        _right = -right;
    }
    else
    {
        _right = right;
    }

    if (fabs(vcos) < 1 - std::numeric_limits<float>::epsilon())
    {
        // Standard case (slerp)
        const float vsin = sqrtf(1.0f - vcos * vcos);
        const float angle = atan2(vsin, vcos);
        const float invSin = 1.0f / vsin;
        const float coeff0 = sin((1.0f - position) * angle) * invSin;
        const float coeff1 = sin((position) * angle) * invSin;

        return coeff0 * left + coeff1 * _right;
    }
    else
    {
        // There are two situations:
        // 1. "left" and "right" are very close (cos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "left" and "right" are almost inverse of each other (cos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.

        Quaternion ret = (1.0f - position) * left + position * _right;
        // taking the complement requires renormalisation
        ret.normalise();
        return ret;
    }
}

//----------------------------------------------------------------------------//
const String& QuaternionSlerpInterpolator::getType() const
{
    static String type("QuaternionSlerp");

    return type;
}

//----------------------------------------------------------------------------//
String QuaternionSlerpInterpolator::interpolateAbsolute(const String& value1,
                                    const String& value2,
                                    float position)
{
    Helper::return_type val1 = Helper::fromString(value1);
    Helper::return_type val2 = Helper::fromString(value2);

    return Helper::toString(Quaternion::slerp(val1, val2, position));
}

//----------------------------------------------------------------------------//
String QuaternionSlerpInterpolator::interpolateRelative(const String& base,
                                    const String& value1,
                                    const String& value2,
                                    float position)
{
    Helper::return_type bas = Helper::fromString(base);
    Helper::return_type val1 = Helper::fromString(value1);
    Helper::return_type val2 = Helper::fromString(value2);

    return Helper::toString(bas * Quaternion::slerp(val1, val2, position));
}

//----------------------------------------------------------------------------//
String QuaternionSlerpInterpolator::interpolateRelativeMultiply(
                                            const String& /*base*/,
                                            const String& /*value1*/,
                                            const String& /*value2*/,
                                            float /*position*/)
{
    CEGUI_THROW(InvalidRequestException("AM_RelativeMultiply doesn't make sense "
        "with Quaternions! Please use absolute or relative application method."));

    return Helper::toString(Quaternion::IDENTITY);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

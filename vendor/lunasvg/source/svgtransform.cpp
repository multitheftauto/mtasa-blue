#include "svgtransform.h"

namespace lunasvg {

#define PI 3.14159265358979323846

SVGTransform::SVGTransform()
{
}

bool SVGTransform::parseTransform(const char*& ptr, TransformType& type, double* values, int& count)
{
    int required = 0;
    int optional = 0;
    if(Utils::skipDesc(ptr, "matrix", 6))
    {
        type = TransformTypeMatrix;
        required = 6;
        optional = 0;
    }
    else if(Utils::skipDesc(ptr, "rotate", 6))
    {
        type = TransformTypeRotate;
        required = 1;
        optional = 2;
    }
    else if(Utils::skipDesc(ptr, "scale", 5))
    {
        type = TransformTypeScale;
        required = 1;
        optional = 1;
    }
    else if(Utils::skipDesc(ptr, "skewX", 5))
    {
        type = TransformTypeSkewX;
        required = 1;
        optional = 0;
    }
    else if(Utils::skipDesc(ptr, "skewY", 5))
    {
        type = TransformTypeSkewY;
        required = 1;
        optional = 0;
    }
    else if(Utils::skipDesc(ptr, "translate", 9))
    {
        type = TransformTypeTranslate;
        required = 1;
        optional = 1;
    }
    else
    {
        return false;
    }

    Utils::skipWs(ptr);
    if(*ptr!='(')
        return false;
    ++ptr;

    int maxCount = required + optional;
    count = 0;
    Utils::skipWs(ptr);
    while(count < maxCount)
    {
        if(!Utils::parseNumber(ptr, values[count]))
            break;
        ++count;
        Utils::skipWsComma(ptr);
    }

    if(*ptr!=')' || !(count==required || count==maxCount))
        return false;
    ++ptr;

    return true;
}

void SVGTransform::setValueAsString(const std::string& value)
{
    m_value.makeIdentity();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    TransformType type;
    double values[6];
    int count;
    while(*ptr)
    {
        if(!parseTransform(ptr, type, values, count))
            return;
        Utils::skipWsComma(ptr);
        switch(type)
        {
        case TransformTypeMatrix:
            m_value.multiply(AffineTransform(values[0], values[1], values[2], values[3], values[4], values[5]));
            break;
        case TransformTypeRotate:
            if(count == 1)
                m_value.rotate(values[0]*PI/180.0, 0, 0);
            else
                m_value.rotate(values[0]*PI/180.0, values[1], values[2]);
            break;
        case TransformTypeScale:
            if(count == 1)
                m_value.scale(values[0], values[0]);
            else
                m_value.scale(values[0], values[1]);
            break;
        case TransformTypeSkewX:
            m_value.shear(values[0]*PI/180.0, 0);
            break;
        case TransformTypeSkewY:
            m_value.shear(0, values[0]*PI/180.0);
            break;
        case TransformTypeTranslate:
            if(count == 1)
                m_value.translate(values[0], 0);
            else
                m_value.translate(values[0], values[1]);
            break;
        default:
            break;
        }
    }
}

std::string SVGTransform::valueAsString() const
{
    std::string out;

    const double* m = m_value.getMatrix();
    out += "matrix(";
    out += Utils::toString(m[0]);
    out += ' ';
    out += Utils::toString(m[1]);
    out += ' ';
    out += Utils::toString(m[2]);
    out += ' ';
    out += Utils::toString(m[3]);
    out += ' ';
    out += Utils::toString(m[4]);
    out += ' ';
    out += Utils::toString(m[5]);
    out += ')';

    return out;
}

SVGPropertyBase* SVGTransform::clone() const
{
    SVGTransform* property = new SVGTransform();
    property->m_value = m_value;

    return property;
}

} // namespace lunasvg

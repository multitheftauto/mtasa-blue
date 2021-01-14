#include "svgpath.h"
#include "pathiterator.h"

namespace lunasvg {

SVGPath::SVGPath()
{
}

bool SVGPath::parseArcFlag(const char*& ptr, bool& flag)
{
    if(*ptr == '0')
        flag = false;
    else if(*ptr == '1')
        flag = true;
    else
        return false;

    ++ptr;
    Utils::skipWsComma(ptr);
    return true;
}

bool SVGPath::parseCoordinate(const char*& ptr, double* coords, int length)
{
    for(int i = 0;i < length;i++)
    {
        if(!Utils::parseNumber(ptr, coords[i]))
            return false;
        Utils::skipWsComma(ptr);
    }

    return true;
}

void SVGPath::setValueAsString(const std::string& value)
{
    m_value.reset();
    if(value.empty())
        return;

    const char* ptr = value.c_str();
    Utils::skipWs(ptr);
    if(!*ptr || !(*ptr=='M' || *ptr=='m'))
        return;
    char pathCommand = *ptr++;
    double c[6];
    bool f[2];
    while(true)
    {
        Utils::skipWs(ptr);
        switch(pathCommand)
        {
        case 'M':
        case 'm':
            if(!parseCoordinate(ptr, c, 2))
                return;
            m_value.moveTo(c[0], c[1], pathCommand == 'm');
            pathCommand = pathCommand == 'm' ? 'l' : 'L';
            break;
        case 'L':
        case 'l':
            if(!parseCoordinate(ptr, c, 2))
                return;
            m_value.lineTo(c[0], c[1], pathCommand == 'l');
            break;
        case 'Q':
        case 'q':
            if(!parseCoordinate(ptr, c, 4))
                return;
            m_value.quadTo(c[0], c[1], c[2], c[3], pathCommand == 'q');
            break;
        case 'C':
        case 'c':
            if(!parseCoordinate(ptr, c, 6))
                return;
            m_value.cubicTo(c[0], c[1], c[2], c[3], c[4], c[5], pathCommand == 'c');
            break;
        case 'T':
        case 't':
            if(!parseCoordinate(ptr, c, 2))
                return;
            m_value.smoothQuadTo(c[0], c[1], pathCommand == 't');
            break;
        case 'S':
        case 's':
            if(!parseCoordinate(ptr, c, 4))
                return;
            m_value.smoothCubicTo(c[0], c[1], c[2], c[3], pathCommand == 's');
            break;
        case 'H':
        case 'h':
            if(!parseCoordinate(ptr, c, 1))
                return;
            m_value.horizontalTo(c[0], pathCommand == 'h');
            break;
        case 'V':
        case 'v':
            if(!parseCoordinate(ptr, c, 1))
                return;
            m_value.verticalTo(c[0], pathCommand == 'v');
            break;
        case 'A':
        case 'a':
            if(!parseCoordinate(ptr, c, 3)
                    || !parseArcFlag(ptr, f[0])
                    || !parseArcFlag(ptr, f[1])
                    || !parseCoordinate(ptr, c + 3, 2))
                return;
            m_value.arcTo(c[0], c[1], c[2], f[0], f[1], c[3], c[4], pathCommand == 'a');
            break;
        case 'Z':
        case 'z':
            m_value.closePath();
            break;
        default:
            return;
        }

        if(!Utils::skipWs(ptr))
            break;

        if(IS_ALPHA(*ptr))
            pathCommand = *ptr++;
    }
}

std::string SVGPath::valueAsString() const
{
    std::string out;

    PathIterator it(m_value);
    double c[6];
    SegType lastSeg = SegTypeMoveTo;
    while(!it.isDone())
    {
        SegType seg = it.currentSegment(c);
        if(seg!=lastSeg)
            out += ' ';
        switch(seg)
        {
        case SegTypeMoveTo:
            out += 'M';
            out += Utils::toString(c[0]);
            out += ' ';
            out += Utils::toString(c[1]);
            break;
        case SegTypeLineTo:
            out += (lastSeg == SegTypeLineTo ? ' ' : 'L');
            out += Utils::toString(c[0]);
            out += ' ';
            out += Utils::toString(c[1]);
            break;
        case SegTypeQuadTo:
            out += (lastSeg == SegTypeQuadTo ? ' ' : 'Q');
            out += Utils::toString(c[0]);
            out += ' ';
            out += Utils::toString(c[1]);
            out += ' ';
            out += Utils::toString(c[2]);
            out += ' ';
            out += Utils::toString(c[3]);
            break;
        case SegTypeCubicTo:
            out += (lastSeg == SegTypeCubicTo ? ' ' : 'C');
            out += Utils::toString(c[0]);
            out += ' ';
            out += Utils::toString(c[1]);
            out += ' ';
            out += Utils::toString(c[2]);
            out += ' ';
            out += Utils::toString(c[3]);
            out += ' ';
            out += Utils::toString(c[4]);
            out += ' ';
            out += Utils::toString(c[5]);
            break;
        case SegTypeClose:
            out += 'Z';
            break;
        }
        lastSeg = seg;
        it.next();
    }

    return out;
}

SVGPropertyBase* SVGPath::clone() const
{
    SVGPath* property = new SVGPath();
    property->m_value = m_value;

    return property;
}

} // namespace lunasvg

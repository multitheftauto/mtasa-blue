#ifndef SVGPARSERUTILS_H
#define SVGPARSERUTILS_H

#include <cstring>
#include <cmath>
#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

namespace lunasvg {

#define IS_ALPHA(c) (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
#define IS_NUM(c) (c >= '0' && c <= '9')
#define IS_WS(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r')

namespace Utils {

inline const char* rtrim(const char* start, const char* end)
{
    while(end > start && IS_WS(end[-1]))
        --end;

    return end;
}

inline const char* ltrim(const char* start, const char* end)
{
    while(start < end && IS_WS(*start))
        ++start;

    return start;
}

inline std::pair<const char*, const char*> trim(const char* start, const char* end)
{
    return std::make_pair(ltrim(start, end), rtrim(start, end));
}

inline bool skipDesc(const char*& ptr, const char* str, int length)
{
    for(int i = 0;i < length;i++)
    {
        if(ptr[i] != str[i])
            return false;
    }

    ptr += length;
    return true;
}

inline bool skipWs(const char*& ptr)
{
    while(*ptr && IS_WS(*ptr))
       ++ptr;

    return *ptr;
}

inline bool skipWsDelimiter(const char*& ptr, char delimiter)
{
    if(*ptr && !IS_WS(*ptr) && *ptr!=delimiter)
        return false;

    if(skipWs(ptr))
    {
        if(*ptr && *ptr == delimiter)
        {
            ++ptr;
            skipWs(ptr);
        }
    }

    return *ptr;
}

inline bool skipWsComma(const char*& ptr)
{
    return skipWsDelimiter(ptr, ',');
}

template<typename T>
inline std::string toString(const T& value)
{
    std::ostringstream ss;
    ss << value;

    return ss.str();
}

inline bool isIntegralDigit(char ch, int base)
{
    if(IS_NUM(ch))
        return ch - '0' < base;

    if(IS_ALPHA(ch))
        return (ch>='a'&&ch<'a'+std::min(base, 36)-10) || (ch>='A'&&ch<'A'+std::min(base, 36)-10);

    return false;
}

template<typename T>
inline bool parseInteger(const char*& ptr, T& integer, int base = 10)
{
    bool isNegative = 0;
    T value = 0;

    static const T intMax = std::numeric_limits<T>::max();
    static const bool isSigned = std::numeric_limits<T>::is_signed;
    using signed_t = typename std::make_signed<T>::type;
    const T maxMultiplier = intMax / static_cast<T>(base);

    if(isSigned && *ptr == '-')
    {
        ++ptr;
        isNegative = true;
    }
    else if(*ptr == '+')
        ++ptr;

    if(!*ptr || !isIntegralDigit(*ptr, base))
        return false;

    int digitValue;
    while(*ptr && isIntegralDigit(*ptr, base))
    {
        const char ch = *ptr++;
        if(IS_NUM(ch))
            digitValue = ch - '0';
        else if(ch >= 'a')
            digitValue = ch - 'a' + 10;
        else
            digitValue = ch - 'A' + 10;
        if(value > maxMultiplier || (value == maxMultiplier && static_cast<T>(digitValue) > (intMax % static_cast<T>(base)) + isNegative))
            return false;

        value = static_cast<T>(base) * value + static_cast<T>(digitValue);
    }

    if(isNegative)
        integer = -static_cast<signed_t>(value);
    else
        integer = value;

    return true;
}

template<typename T>
inline bool parseNumber(const char*& ptr, T& number)
{
    T integer, fraction;
    int sign, expsign, exponent;

    static const T numberMax = std::numeric_limits<T>::max();
    fraction = 0;
    integer = 0;
    exponent = 0;
    sign = 1;
    expsign = 1;

    if(*ptr == '+')
        ++ptr;
    else if(*ptr == '-')
    {
        ++ptr;
        sign = -1;
    }

    if(!*ptr || (!IS_NUM(*ptr) && *ptr != '.'))
        return false;

    if(*ptr != '.')
    {
        while(*ptr && IS_NUM(*ptr))
            integer = static_cast<T>(10) * integer + (*ptr++ - '0');
    }

    if(*ptr == '.')
    {
        ++ptr;
        if(!*ptr || !IS_NUM(*ptr))
            return false;
        T div = 1;
        while(*ptr && IS_NUM(*ptr))
        {
            fraction = static_cast<T>(10) * fraction + (*ptr++ - '0');
            div *= static_cast<T>(10);
        }
        fraction /= div;
    }

    if(*ptr && (*ptr == 'e' || *ptr == 'E')
       && (ptr[1] != 'x' && ptr[1] != 'm'))
    {
        ++ptr;
        if(*ptr == '+')
            ++ptr;
        else if(*ptr == '-')
        {
            ++ptr;
            expsign = -1;
        }

        if(!*ptr || !IS_NUM(*ptr))
            return false;

        while(*ptr && IS_NUM(*ptr))
            exponent = 10 * exponent + (*ptr++ - '0');
    }

    number = sign * (integer + fraction);
    if(exponent)
        number *= static_cast<T>(pow(10.0, expsign*exponent));

    return number >= -numberMax && number <= numberMax;
}

} // namespace Utils

} // namespace lunasvg

#endif // SVGPARSERUTILS_H

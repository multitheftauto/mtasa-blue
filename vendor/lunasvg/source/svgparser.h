#ifndef SVGPARSER_H
#define SVGPARSER_H

#include "svgparserutils.h"
#include "svgcontentutils.h"

#include <vector>
#include <stack>
#include <utility>

namespace lunasvg {

class SVGDocument;
class SVGElementImpl;
class SVGElementHead;

class SVGParser
{
public:
    static SVGElementImpl* parse(const std::string& source, SVGDocument* document, SVGElementHead* parent);

private:
    typedef std::pair<std::string, std::string> Attribute;
    typedef std::vector<Attribute> AttributeList;
    static bool enumTag(const char*& ptr, int& tagType, std::string& tagName, std::string& content, AttributeList& attributes);
};

} // namespace lunasvg

#endif // SVGPARSER_H

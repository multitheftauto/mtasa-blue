#ifndef SVGELEMENTITER_H
#define SVGELEMENTITER_H

#include <string>

namespace lunasvg {

class SVGElement;
class SVGElementImpl;

class SVGElementIter
{
public:
    SVGElementIter(const SVGElement* element, const std::string& id = std::string(), const std::string& tagName = std::string());
    SVGElement* currentElement() const;
    bool next();

private:
    const SVGElementImpl* m_element;
    SVGElementImpl* m_currentElement;
    std::string m_id;
    unsigned short m_elementId;
};

} // namespace lunasvg

#endif // SVGELEMENTITER_H

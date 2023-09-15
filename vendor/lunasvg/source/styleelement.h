#ifndef STYLEELEMENT_H
#define STYLEELEMENT_H

#include "element.h"

namespace lunasvg {

class StyleElement : public Element {
public:
    StyleElement();

    std::unique_ptr<Node> clone() const;
};

} // namespace lunasvg

#endif // STYLEELEMENT_H

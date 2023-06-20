#ifndef SVGELEMENT_H
#define SVGELEMENT_H

#include "graphicselement.h"

namespace lunasvg {

class TreeBuilder;
class LayoutSymbol;

class SVGElement : public GraphicsElement {
public:
    SVGElement();

    Length x() const;
    Length y() const;
    Length width() const;
    Length height() const;

    Rect viewBox() const;
    PreserveAspectRatio preserveAspectRatio() const;
    std::unique_ptr<LayoutSymbol> build(const TreeBuilder* builder) const;

    void layout(LayoutContext* context, LayoutContainer* current) const;
    std::unique_ptr<Node> clone() const;
};

} // namespace lunasvg

#endif // SVGELEMENT_H

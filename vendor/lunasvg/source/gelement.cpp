#include "gelement.h"
#include "layoutcontext.h"

namespace lunasvg {

GElement::GElement()
    : GraphicsElement(ElementID::G)
{
}

void GElement::layout(LayoutContext* context, LayoutContainer* current) const
{
    if(isDisplayNone())
        return;

    auto group = std::make_unique<LayoutGroup>();
    group->transform = transform();
    group->opacity = opacity();
    group->masker = context->getMasker(mask());
    group->clipper = context->getClipper(clip_path());
    layoutChildren(context, group.get());
    current->addChildIfNotEmpty(std::move(group));
}

std::unique_ptr<Node> GElement::clone() const
{
    return cloneElement<GElement>();
}

} // namespace lunasvg

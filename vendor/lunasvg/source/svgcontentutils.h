#ifndef SVGCONTENTUTILS_H
#define SVGCONTENTUTILS_H

#include <string>

namespace lunasvg {

enum DOMElementID
{
    DOMElementIdUnknown = 0,
    DOMElementIdCircle,
    DOMElementIdClipPath,
    DOMElementIdDefs,
    DOMElementIdEllipse,
    DOMElementIdG,
    DOMElementIdLine,
    DOMElementIdLinearGradient,
    DOMElementIdMarker,
    DOMElementIdMask,
    DOMElementIdPath,
    DOMElementIdPattern,
    DOMElementIdPolygon,
    DOMElementIdPolyline,
    DOMElementIdRadialGradient,
    DOMElementIdRect,
    DOMElementIdSolidColor,
    DOMElementIdStop,
    DOMElementIdSvg,
    DOMElementIdSymbol,
    DOMElementIdText,
    DOMElementIdUse,
    DOMElementLastId
};

enum DOMPropertyID
{
    DOMPropertyIdUnknown = 0,
    DOMPropertyIdClipPathUnits,
    DOMPropertyIdCx,
    DOMPropertyIdCy,
    DOMPropertyIdD,
    DOMPropertyIdFx,
    DOMPropertyIdFy,
    DOMPropertyIdGradientTransform,
    DOMPropertyIdGradientUnits,
    DOMPropertyIdHeight,
    DOMPropertyIdHref,
    DOMPropertyIdId,
    DOMPropertyIdMarkerHeight,
    DOMPropertyIdMarkerUnits,
    DOMPropertyIdMarkerWidth,
    DOMPropertyIdMaskContentUnits,
    DOMPropertyIdMaskUnits,
    DOMPropertyIdOffset,
    DOMPropertyIdOrient,
    DOMPropertyIdPatternContentUnits,
    DOMPropertyIdPatternTransform,
    DOMPropertyIdPatternUnits,
    DOMPropertyIdPoints,
    DOMPropertyIdPreserveAspectRatio,
    DOMPropertyIdR,
    DOMPropertyIdRefX,
    DOMPropertyIdRefY,
    DOMPropertyIdRx,
    DOMPropertyIdRy,
    DOMPropertyIdSpreadMethod,
    DOMPropertyIdStyle,
    DOMPropertyIdTransform,
    DOMPropertyIdViewBox,
    DOMPropertyIdWidth,
    DOMPropertyIdX,
    DOMPropertyIdX1,
    DOMPropertyIdX2,
    DOMPropertyIdY,
    DOMPropertyIdY1,
    DOMPropertyIdY2,
    DOMPropertyLastId
};

enum CSSPropertyID
{
    CSSPropertyIdUnknown = 0,
    CSSPropertyIdClip_Path,
    CSSPropertyIdClip_Rule,
    CSSPropertyIdColor,
    CSSPropertyIdDisplay,
    CSSPropertyIdFill,
    CSSPropertyIdFill_Opacity,
    CSSPropertyIdFill_Rule,
    CSSPropertyIdFont_Size,
    CSSPropertyIdMarker_End,
    CSSPropertyIdMarker_Mid,
    CSSPropertyIdMarker_Start,
    CSSPropertyIdMask,
    CSSPropertyIdOpacity,
    CSSPropertyIdSolid_Color,
    CSSPropertyIdSolid_Opacity,
    CSSPropertyIdStop_Color,
    CSSPropertyIdStop_Opacity,
    CSSPropertyIdStroke,
    CSSPropertyIdStroke_Dasharray,
    CSSPropertyIdStroke_Dashoffset,
    CSSPropertyIdStroke_Linecap,
    CSSPropertyIdStroke_Linejoin,
    CSSPropertyIdStroke_Miterlimit,
    CSSPropertyIdStroke_Opacity,
    CSSPropertyIdStroke_Width,
    CSSPropertyIdText_Anchor,
    CSSPropertyIdVisibility,
    CSSPropertyLastId
};

class SVGElementHead;
class SVGDocument;

namespace Utils {

const std::string& domElementName(DOMElementID nameId);
DOMElementID domElementId(const std::string& name);
bool isElementPermitted(DOMElementID parentId, DOMElementID childId);
SVGElementHead* createElement(DOMElementID elementId, SVGDocument* document);
const std::string& domPropertyName(DOMPropertyID nameId);
DOMPropertyID domPropertyId(const std::string& name);
const std::string& cssPropertyName(CSSPropertyID nameId);
CSSPropertyID cssPropertyId(const std::string& name);

} // namespace Utils

} // namespace lunasvg

#endif // SVGCONTENTUTILS_H

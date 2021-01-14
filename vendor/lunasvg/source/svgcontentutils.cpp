#include "svgcontentutils.h"
#include "svgsvgelement.h"
#include "svggelement.h"
#include "svgdefselement.h"
#include "svgpaintelement.h"
#include "svgstopelement.h"
#include "svgsymbolelement.h"
#include "svguseelement.h"
#include "svgmaskelement.h"
#include "svgclippathelement.h"
#include "svgmarkerelement.h"
#include "svggeometryelement.h"
#include "svgtextelement.h"

#include <map>

namespace lunasvg {

namespace Utils {

#define KElementIdsGradient \
    DOMElementIdLinearGradient, \
    DOMElementIdRadialGradient

#define KElementIdsStructural \
    DOMElementIdDefs, \
    DOMElementIdG, \
    DOMElementIdSvg, \
    DOMElementIdSymbol, \
    DOMElementIdUse

#define KElementIdsShape \
    DOMElementIdCircle, \
    DOMElementIdEllipse, \
    DOMElementIdLine, \
    DOMElementIdPath, \
    DOMElementIdPolygon, \
    DOMElementIdPolyline, \
    DOMElementIdRect

static const DOMElementID contentmodel_1[] = {
    KElementIdsShape,
    KElementIdsStructural,
    KElementIdsGradient,
    DOMElementIdClipPath,
    DOMElementIdMarker,
    DOMElementIdMask,
    DOMElementIdPattern,
    DOMElementIdSolidColor,
    DOMElementIdText,
    DOMElementIdUnknown
};

static const DOMElementID contentmodel_2[] = {
    KElementIdsGradient,
    DOMElementIdSolidColor,
    DOMElementIdPattern,
    DOMElementIdUnknown
};

static const DOMElementID contentmodel_3[] = {
    KElementIdsShape,
    DOMElementIdText,
    DOMElementIdUse,
    DOMElementIdUnknown
};

static const DOMElementID contentmodel_4[] = {
    DOMElementIdStop,
    DOMElementIdUnknown
};

static const struct
{
    DOMElementID id;
    const DOMElementID* content;
} contentmodelmap[] = {
    {DOMElementIdUnknown, nullptr},
    {DOMElementIdCircle, nullptr},
    {DOMElementIdClipPath, contentmodel_3},
    {DOMElementIdDefs, contentmodel_1},
    {DOMElementIdEllipse, nullptr},
    {DOMElementIdG, contentmodel_1},
    {DOMElementIdLine, nullptr},
    {DOMElementIdLinearGradient, contentmodel_4},
    {DOMElementIdMarker, contentmodel_1},
    {DOMElementIdMask, contentmodel_1},
    {DOMElementIdPath, nullptr},
    {DOMElementIdPattern, contentmodel_1},
    {DOMElementIdPolygon, nullptr},
    {DOMElementIdPolyline, nullptr},
    {DOMElementIdRadialGradient, contentmodel_4},
    {DOMElementIdRect, nullptr},
    {DOMElementIdSolidColor, contentmodel_2},
    {DOMElementIdStop, nullptr},
    {DOMElementIdSvg, contentmodel_1},
    {DOMElementIdSymbol, contentmodel_1},
    {DOMElementIdText, nullptr},
    {DOMElementIdUse, nullptr},
    {DOMElementLastId, nullptr}
};

static const std::map<std::string, DOMElementID> domelementmap = {
    {"circle", DOMElementIdCircle},
    {"clipPath", DOMElementIdClipPath},
    {"defs", DOMElementIdDefs},
    {"ellipse", DOMElementIdEllipse},
    {"g", DOMElementIdG},
    {"line", DOMElementIdLine},
    {"linearGradient", DOMElementIdLinearGradient},
    {"marker", DOMElementIdMarker},
    {"mask", DOMElementIdMask},
    {"path", DOMElementIdPath},
    {"pattern", DOMElementIdPattern},
    {"polygon", DOMElementIdPolygon},
    {"polyline", DOMElementIdPolyline},
    {"radialGradient", DOMElementIdRadialGradient},
    {"rect", DOMElementIdRect},
    {"stop", DOMElementIdStop},
    {"solidColor", DOMElementIdSolidColor},
    {"svg", DOMElementIdSvg},
    {"symbol", DOMElementIdSymbol},
    {"text", DOMElementIdText},
    {"use", DOMElementIdUse}
};

static const std::map<std::string, DOMPropertyID> dompropertymap = {
    {"clipPathUnits", DOMPropertyIdClipPathUnits},
    {"cx", DOMPropertyIdCx},
    {"cy", DOMPropertyIdCy},
    {"d", DOMPropertyIdD},
    {"fx", DOMPropertyIdFx},
    {"fy", DOMPropertyIdFy},
    {"gradientTransform", DOMPropertyIdGradientTransform},
    {"gradientUnits", DOMPropertyIdGradientUnits},
    {"height", DOMPropertyIdHeight},
    {"id", DOMPropertyIdId},
    {"markerHeight", DOMPropertyIdMarkerHeight},
    {"markerUnits", DOMPropertyIdMarkerUnits},
    {"markerWidth", DOMPropertyIdMarkerWidth},
    {"maskContentUnits", DOMPropertyIdMaskContentUnits},
    {"maskUnits", DOMPropertyIdMaskUnits},
    {"offset", DOMPropertyIdOffset},
    {"orient", DOMPropertyIdOrient},
    {"patternContentUnits", DOMPropertyIdPatternContentUnits},
    {"patternTransform", DOMPropertyIdPatternTransform},
    {"patternUnits", DOMPropertyIdPatternUnits},
    {"points", DOMPropertyIdPoints},
    {"preserveAspectRatio", DOMPropertyIdPreserveAspectRatio},
    {"r", DOMPropertyIdR},
    {"refX", DOMPropertyIdRefX},
    {"refY", DOMPropertyIdRefY},
    {"rx", DOMPropertyIdRx},
    {"ry", DOMPropertyIdRy},
    {"spreadMethod", DOMPropertyIdSpreadMethod},
    {"style", DOMPropertyIdStyle},
    {"transform", DOMPropertyIdTransform},
    {"viewBox", DOMPropertyIdViewBox},
    {"width", DOMPropertyIdWidth},
    {"x", DOMPropertyIdX},
    {"x1", DOMPropertyIdX1},
    {"x2", DOMPropertyIdX2},
    {"xlink:href", DOMPropertyIdHref},
    {"y", DOMPropertyIdY},
    {"y1", DOMPropertyIdY1},
    {"y2", DOMPropertyIdY2}
};

static const std::map<std::string, CSSPropertyID> csspropertymap = {
    {"clip-path", CSSPropertyIdClip_Path},
    {"clip-rule", CSSPropertyIdClip_Rule},
    {"color", CSSPropertyIdColor},
    {"display", CSSPropertyIdDisplay},
    {"fill", CSSPropertyIdFill},
    {"fill-opacity", CSSPropertyIdFill_Opacity},
    {"fill-rule", CSSPropertyIdFill_Rule},
    {"font-size", CSSPropertyIdFont_Size},
    {"marker-end", CSSPropertyIdMarker_End},
    {"marker-mid", CSSPropertyIdMarker_Mid},
    {"marker-start", CSSPropertyIdMarker_Start},
    {"mask", CSSPropertyIdMask},
    {"opacity", CSSPropertyIdOpacity},
    {"solid-color", CSSPropertyIdSolid_Color},
    {"solid-opacity", CSSPropertyIdSolid_Opacity},
    {"stop-color", CSSPropertyIdStop_Color},
    {"stop-opacity", CSSPropertyIdStop_Opacity},
    {"stroke", CSSPropertyIdStroke},
    {"stroke-dasharray", CSSPropertyIdStroke_Dasharray},
    {"stroke-dashoffset", CSSPropertyIdStroke_Dashoffset},
    {"stroke-linecap", CSSPropertyIdStroke_Linecap},
    {"stroke-linejoin", CSSPropertyIdStroke_Linejoin},
    {"stroke-miterlimit", CSSPropertyIdStroke_Miterlimit},
    {"stroke-opacity", CSSPropertyIdStroke_Opacity},
    {"stroke-width", CSSPropertyIdStroke_Width},
    {"text-anchor", CSSPropertyIdText_Anchor},
    {"visibility", CSSPropertyIdVisibility}
};

bool isElementPermitted(DOMElementID parentId, DOMElementID childId)
{
    if(const DOMElementID* content = contentmodelmap[parentId].content)
    {
        for(int i = 0;content[i] != DOMElementIdUnknown;i++)
            if(content[i] == childId)
                return true;
    }

    return false;
}

SVGElementHead* createElement(DOMElementID elementId, SVGDocument* document)
{
    switch(elementId)
    {
    case DOMElementIdSvg:
        return new SVGSVGElement(document);
    case DOMElementIdPath:
        return new SVGPathElement(document);
    case DOMElementIdG:
        return new SVGGElement(document);
    case DOMElementIdRect:
        return new SVGRectElement(document);
    case DOMElementIdCircle:
        return new SVGCircleElement(document);
    case DOMElementIdEllipse:
        return new SVGEllipseElement(document);
    case DOMElementIdLine:
        return new SVGLineElement(document);
    case DOMElementIdDefs:
        return new SVGDefsElement(document);
    case DOMElementIdPolygon:
        return new SVGPolygonElement(document);
    case DOMElementIdPolyline:
        return new SVGPolylineElement(document);
    case DOMElementIdStop:
        return new SVGStopElement(document);
    case DOMElementIdLinearGradient:
        return new SVGLinearGradientElement(document);
    case DOMElementIdRadialGradient:
        return new SVGRadialGradientElement(document);
    case DOMElementIdSymbol:
        return new SVGSymbolElement(document);
    case DOMElementIdUse:
        return new SVGUseElement(document);
    case DOMElementIdPattern:
        return new SVGPatternElement(document);
    case DOMElementIdMask:
        return new SVGMaskElement(document);
    case DOMElementIdClipPath:
        return new SVGClipPathElement(document);
    case DOMElementIdSolidColor:
        return new SVGSolidColorElement(document);
    case DOMElementIdMarker:
        return new SVGMarkerElement(document);
    case DOMElementIdText:
        return new SVGTextElement(document);
    default:
        break;
    }

    return nullptr;
}

const std::string& domElementName(DOMElementID nameId)
{
    std::map<std::string, DOMElementID>::const_iterator it = domelementmap.begin();
    std::map<std::string, DOMElementID>::const_iterator end = domelementmap.end();

    for(;it != end;++it)
        if(it->second == nameId)
            return it->first;

    return KEmptyString;
}

DOMElementID domElementId(const std::string& name)
{
    std::map<std::string, DOMElementID>::const_iterator it = domelementmap.find(name);
    return it != domelementmap.end() ? it->second : DOMElementIdUnknown;
}

const std::string& domPropertyName(DOMPropertyID nameId)
{
    std::map<std::string, DOMPropertyID>::const_iterator it = dompropertymap.begin();
    std::map<std::string, DOMPropertyID>::const_iterator end = dompropertymap.end();

    for(;it != end;++it)
        if(it->second == nameId)
            return it->first;

    return KEmptyString;
}

DOMPropertyID domPropertyId(const std::string& name)
{
    std::map<std::string, DOMPropertyID>::const_iterator it = dompropertymap.find(name);
    return it != dompropertymap.end() ? it->second : DOMPropertyIdUnknown;
}

const std::string& cssPropertyName(CSSPropertyID nameId)
{
    std::map<std::string, CSSPropertyID>::const_iterator it = csspropertymap.begin();
    std::map<std::string, CSSPropertyID>::const_iterator end = csspropertymap.end();

    for(;it != end;++it)
        if(it->second == nameId)
            return it->first;

    return KEmptyString;
}

CSSPropertyID cssPropertyId(const std::string& name)
{
    std::map<std::string, CSSPropertyID>::const_iterator it = csspropertymap.find(name);
    return it != csspropertymap.end() ? it->second : CSSPropertyIdUnknown;
}

} // namespace Utils

} // namespace lunasvg

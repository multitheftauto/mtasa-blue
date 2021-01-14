#include "canvas.h"
#include "canvasimpl.h"

namespace lunasvg {

Canvas::Canvas()
{
}

Canvas::Canvas(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride)
    : m_impl(new CanvasImpl(data, width, height, stride))
{
}

Canvas::Canvas(unsigned int width, unsigned int height)
    : m_impl(new CanvasImpl(width, height))
{
}

void Canvas::reset(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride)
{
    m_impl.reset(new CanvasImpl(data, width, height, stride));
}

void Canvas::reset(unsigned int width, unsigned int height)
{
    m_impl.reset(new CanvasImpl(width, height));
}

void Canvas::clear(const Rgb& color)
{
    m_impl->clear(color);
}

void Canvas::blend(const Canvas& source, BlendMode mode, double opacity, double dx, double dy)
{
    m_impl->blend(source, mode, opacity, dx, dy);
}

void Canvas::draw(const Path& path, const AffineTransform& matrix, WindRule fillRule, const Paint& fillPaint, const Paint& strokePaint, const StrokeData& strokeData)
{
    m_impl->draw(path, matrix, fillRule, fillPaint, strokePaint, strokeData);
}

void Canvas::updateLuminance()
{
    m_impl->updateLuminance();
}

void Canvas::convertToRGBA()
{
    m_impl->convertToRGBA();
}

unsigned char* Canvas::data() const
{
    return m_impl->data();
}

unsigned int Canvas::width() const
{
    return m_impl->width();
}

unsigned int Canvas::height() const
{
    return m_impl->height();
}

unsigned int Canvas::stride() const
{
    return m_impl->stride();
}

} // namespace lunasvg

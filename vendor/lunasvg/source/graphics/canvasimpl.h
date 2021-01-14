#ifndef CANVASIMPL_H
#define CANVASIMPL_H

#include "canvas.h"

#include "plutovg.h"

namespace lunasvg {

class CanvasImpl
{
public:
    ~CanvasImpl();
    CanvasImpl(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride);
    CanvasImpl(unsigned int width, unsigned int height);

    void clear(const Rgb& color);
    void blend(const Canvas& source, BlendMode mode, double opacity, double dx, double dy);
    void draw(const Path& path, const AffineTransform& matrix, WindRule fillRule, const Paint& fillPaint, const Paint& strokePaint, const StrokeData& strokeData);
    void updateLuminance();
    void convertToRGBA();

    unsigned char* data() const;
    unsigned int width() const;
    unsigned int height() const;
    unsigned int stride() const;

    plutovg_surface_t* surface() const { return m_surface; }
    plutovg_t* pluto() const { return m_pluto; }

private:
    plutovg_surface_t* m_surface;
    plutovg_t* m_pluto;
};

} // namespace lunasvg

#endif // CANVASIMPL_H

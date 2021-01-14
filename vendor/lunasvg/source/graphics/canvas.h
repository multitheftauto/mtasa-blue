#ifndef CANVAS_H
#define CANVAS_H

#include "svgenumeration.h"

#include <memory>

namespace lunasvg {

enum BlendMode
{
    BlendModeSrc,
    BlendModeSrc_Over,
    BlendModeDst_In,
    BlendModeDst_Out
};

class Path;
class Rgb;
class Paint;
class StrokeData;
class AffineTransform;
class CanvasImpl;

class Canvas
{
public:
    Canvas();
    Canvas(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride);
    Canvas(unsigned int width, unsigned int height);

    void reset(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride);
    void reset(unsigned int width, unsigned int height);

    void clear(const Rgb& color);
    void blend(const Canvas& source, BlendMode mode, double opacity, double dx, double dy);
    void draw(const Path& path, const AffineTransform& matrix, WindRule fillRule, const Paint& fillPaint, const Paint& strokePaint, const StrokeData& strokeData);
    void updateLuminance();
    void convertToRGBA();

    unsigned char* data() const;
    unsigned int width() const;
    unsigned int height() const;
    unsigned int stride() const;

    CanvasImpl* impl() const { return m_impl.get(); }
    bool valid() const { return !!m_impl; }

private:
    std::shared_ptr<CanvasImpl> m_impl;
};

} // namespace lunasvg

#endif // CANVAS_H

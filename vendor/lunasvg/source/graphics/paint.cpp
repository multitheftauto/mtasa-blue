#include "paint.h"

namespace lunasvg {

Rgb::Rgb(unsigned int value)
    : r((value&0xff000000)>>24),
      g((value&0x00ff0000)>>16),
      b((value&0x0000ff00)>>8),
      a((value&0x000000ff)>>0)
{
}

Rgb::Rgb(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    : r(red), g(green), b(blue), a(alpha)
{
}

unsigned int Rgb::value() const
{
    return (r<<24) | (g<<16) | (b<<8) | a;
}

bool Rgb::operator==(const Rgb& rgb) const
{
    return (rgb.r == r)
            && (rgb.g == g)
            && (rgb.b == b)
            && (rgb.a == a);
}

bool Rgb::operator!=(const Rgb& rgb) const
{
    return !operator==(rgb);
}

LinearGradient::LinearGradient(double x1, double y1, double x2, double y2)
    : x1(x1), y1(y1), x2(x2), y2(y2)
{
}

RadialGradient::RadialGradient(double cx, double cy, double r, double fx, double fy)
    : cx(cx), cy(cy), r(r), fx(fx), fy(fy)
{
}

Gradient::Gradient(const LinearGradient& linear)
    : type(GradientTypeLinear)
{
    values[0] = linear.x1;
    values[1] = linear.y1;
    values[2] = linear.x2;
    values[3] = linear.y2;
}

Gradient::Gradient(const RadialGradient& radial)
    : type(GradientTypeRadial)
{
    values[0] = radial.cx;
    values[1] = radial.cy;
    values[2] = radial.r;
    values[3] = radial.fx;
    values[4] = radial.fy;
}

LinearGradient Gradient::linear() const
{
    return LinearGradient(values[0], values[1], values[2], values[3]);
}

RadialGradient Gradient::radial() const
{
    return RadialGradient(values[0], values[1], values[2], values[3], values[4]);
}

Texture::Texture(const Canvas& canvas)
    : type(TextureTypePlain),
      canvas(canvas)
{
}

Paint::Paint(const Rgb& color)
    : type(PaintTypeColor),
      color(color)
{
}

Paint::Paint(const Gradient& gradient)
    : type(PaintTypeGradient),
      gradient(gradient)
{
}

Paint::Paint(const Texture& texture)
    : type(PaintTypeTexture),
      texture(texture)
{
}

} // namespace lunasvg

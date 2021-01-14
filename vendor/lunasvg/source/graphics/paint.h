#ifndef PAINT_H
#define PAINT_H

#include "svgenumeration.h"
#include "affinetransform.h"
#include "canvas.h"

#include <array>

namespace lunasvg {

class Rgb
{
public:
    Rgb() = default;
    Rgb(unsigned int value);
    Rgb(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);

    bool operator==(const Rgb& rgb) const;
    bool operator!=(const Rgb& rgb) const;
    unsigned int value() const;

public:
    unsigned char r{0};
    unsigned char g{0};
    unsigned char b{0};
    unsigned char a{255};
};

#define KRgbBlack Rgb(0, 0, 0)
#define KRgbWhite Rgb(255, 255, 255)
#define KRgbRed Rgb(255, 0, 0)
#define KRgbGreen Rgb(0, 255, 0)
#define KRgbBlue Rgb(0, 0, 255)
#define KRgbYellow Rgb(255, 255, 0)
#define KRgbTransparent Rgb(0, 0, 0, 0)

enum GradientType
{
   GradientTypeLinear,
   GradientTypeRadial
};

typedef std::pair<double, Rgb> GradientStop;
typedef std::vector<GradientStop> GradientStops;

class LinearGradient
{
public:
   LinearGradient() = default;
   LinearGradient(double x1, double y1, double x2, double y2);

public:
   double x1{0.0};
   double y1{0.0};
   double x2{1.0};
   double y2{1.0};
};

class RadialGradient
{
public:
   RadialGradient() = default;
   RadialGradient(double cx, double cy, double r, double fx, double fy);

public:
   double cx{0.0};
   double cy{0.0};
   double r{1.0};
   double fx{0.0};
   double fy{0.0};
};

class Gradient
{
public:
    Gradient() = default;
    Gradient(const LinearGradient& linear);
    Gradient(const RadialGradient& radial);

    LinearGradient linear() const;
    RadialGradient radial() const;

public:
    GradientType type{GradientTypeLinear};
    GradientStops stops;
    SpreadMethod spread{SpreadMethodPad};
    AffineTransform matrix;
    std::array<double, 5> values;
};

enum TextureType
{
    TextureTypePlain,
    TextureTypeTiled
};

class Texture
{
public:
    Texture() = default;
    Texture(const Canvas& canvas);

public:
    TextureType type{TextureTypePlain};
    Canvas canvas;
    AffineTransform matrix;
};

enum PaintType
{
    PaintTypeNone,
    PaintTypeColor,
    PaintTypeGradient,
    PaintTypeTexture
};

class Paint
{
public:
    Paint() = default;
    Paint(const Rgb& color);
    Paint(const Gradient& gradient);
    Paint(const Texture& texture);

    bool isNone() const { return type == PaintTypeNone; }

public:
    PaintType type{PaintTypeNone};
    double opacity{1.0};
    Rgb color;
    Gradient gradient;
    Texture texture;
};

} // namespace lunasvg

#endif // PAINT_H

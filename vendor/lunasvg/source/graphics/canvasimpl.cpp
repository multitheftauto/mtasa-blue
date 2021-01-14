#include "canvasimpl.h"
#include "affinetransform.h"
#include "pathiterator.h"
#include "strokedata.h"
#include "paint.h"

namespace lunasvg {

static plutovg_paint_t* to_plutovg_paint(const Paint& paint);
static plutovg_matrix_t to_plutovg_matrix(const AffineTransform& transform);
static plutovg_fill_rule_t to_plutovg_fill_rule(WindRule fillRule);
static plutovg_operator_t to_plutovg_operator(BlendMode mode);
static plutovg_line_cap_t to_plutovg_line_cap(LineCap cap);
static plutovg_line_join_t to_plutovg_line_join(LineJoin join);

CanvasImpl::~CanvasImpl()
{
    plutovg_surface_destroy(m_surface);
    plutovg_destroy(m_pluto);
}

CanvasImpl::CanvasImpl(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride)
    : m_surface(plutovg_surface_create_for_data(data, int(width), int(height), int(stride))),
      m_pluto(plutovg_create(m_surface))
{
}

CanvasImpl::CanvasImpl(unsigned int width, unsigned int height)
    : m_surface(plutovg_surface_create(int(width), int(height))),
      m_pluto(plutovg_create(m_surface))
{
}

void CanvasImpl::clear(const Rgb& color)
{
    std::uint32_t width = this->width();
    std::uint32_t height = this->height();
    std::uint32_t stride = this->stride();
    std::uint8_t* data = this->data();
    for(std::uint32_t y = 0;y < height;y++)
    {
        std::uint8_t* row = data + stride * y;
        for(std::uint32_t x = 0;x < width;x++)
        {
            row[0] = (color.b * color.a) / 255;
            row[1] = (color.g * color.a) / 255;
            row[2] = (color.r * color.a) / 255;
            row[3] = color.a;

            row += 4;
        }
    }
}

void CanvasImpl::blend(const Canvas& source, BlendMode mode, double opacity, double dx, double dy)
{
    plutovg_set_source_surface(m_pluto, source.impl()->surface(), dx, dy);
    plutovg_set_operator(m_pluto, to_plutovg_operator(mode));
    plutovg_set_opacity(m_pluto, opacity);
    plutovg_identity_matrix(m_pluto);
    plutovg_paint(m_pluto);
}

void CanvasImpl::draw(const Path& path, const AffineTransform& matrix, WindRule fillRule, const Paint& fillPaint, const Paint& strokePaint, const StrokeData& strokeData)
{
    if(fillPaint.isNone() && strokePaint.isNone())
        return;

    plutovg_matrix_t m = to_plutovg_matrix(matrix);
    plutovg_set_matrix(m_pluto, &m);
    plutovg_set_operator(m_pluto, plutovg_operator_src_over);
    plutovg_set_opacity(m_pluto, 1.0);

    PathIterator it(path);
    double c[6];
    while(!it.isDone())
    {
        switch(it.currentSegment(c))
        {
        case SegTypeMoveTo:
            plutovg_move_to(m_pluto, c[0], c[1]);
            break;
        case SegTypeLineTo:
            plutovg_line_to(m_pluto, c[0], c[1]);
            break;
        case SegTypeQuadTo:
            plutovg_quad_to(m_pluto, c[0], c[1], c[2], c[3]);
            break;
        case SegTypeCubicTo:
            plutovg_cubic_to(m_pluto, c[0], c[1], c[2], c[3], c[4], c[5]);
            break;
        case SegTypeClose:
            plutovg_close_path(m_pluto);
            break;
        }

        it.next();
    }

    if(!fillPaint.isNone())
    {
        plutovg_paint_t* paint = to_plutovg_paint(fillPaint);
        plutovg_set_fill_rule(m_pluto, to_plutovg_fill_rule(fillRule));
        plutovg_set_source(m_pluto, paint);
        plutovg_fill_preserve(m_pluto);
        plutovg_paint_destroy(paint);
    }

    if(!strokePaint.isNone())
    {
        plutovg_paint_t* paint = to_plutovg_paint(strokePaint);
        plutovg_set_miter_limit(m_pluto, strokeData.miterLimit());
        plutovg_set_line_cap(m_pluto, to_plutovg_line_cap(strokeData.cap()));
        plutovg_set_line_join(m_pluto, to_plutovg_line_join(strokeData.join()));
        plutovg_set_line_width(m_pluto, strokeData.width());
        plutovg_set_dash(m_pluto, strokeData.dashOffset(), strokeData.dash().data(), int(strokeData.dash().size()));
        plutovg_set_source(m_pluto, paint);
        plutovg_stroke_preserve(m_pluto);
        plutovg_paint_destroy(paint);
    }

    plutovg_new_path(m_pluto);
}

void CanvasImpl::updateLuminance()
{
    std::uint32_t width = this->width();
    std::uint32_t height = this->height();
    std::uint32_t stride = this->stride();
    std::uint8_t* data = this->data();
    for(std::uint32_t y = 0;y < height;y++)
    {
        std::uint8_t* row = data + stride * y;
        for(std::uint32_t x = 0;x < width;x++)
        {
            std::uint8_t b = row[0];
            std::uint8_t g = row[1];
            std::uint8_t r = row[2];

            row[3] = (2*r + 3*g + b) / 6;

            row += 4;
        }
    }
}

void CanvasImpl::convertToRGBA()
{
    std::uint32_t width = this->width();
    std::uint32_t height = this->height();
    std::uint32_t stride = this->stride();
    std::uint8_t* data = this->data();
    for(std::uint32_t y = 0;y < height;y++)
    {
        std::uint8_t* row = data + stride * y;
        for(std::uint32_t x = 0;x < width;x++)
        {
            std::uint8_t a = row[3];
            if(a != 0)
            {
                std::uint8_t r = row[2];
                std::uint8_t g = row[1];
                std::uint8_t b = row[0];

                row[0] = (r * 255) / a;
                row[1] = (g * 255) / a;
                row[2] = (b * 255) / a;
                row[3] = a;
            }
            else
            {
                row[0] = 0;
                row[1] = 0;
                row[2] = 0;
                row[3] = 0;
            }

            row += 4;
        }
    }
}

unsigned char* CanvasImpl::data() const
{
    return plutovg_surface_get_data(m_surface);
}

unsigned int CanvasImpl::width() const
{
    return std::uint32_t(plutovg_surface_get_width(m_surface));
}

unsigned int CanvasImpl::height() const
{
    return std::uint32_t(plutovg_surface_get_height(m_surface));
}

unsigned int CanvasImpl::stride() const
{
    return std::uint32_t(plutovg_surface_get_stride(m_surface));
}

plutovg_matrix_t to_plutovg_matrix(const AffineTransform& transform)
{
    const double* m = transform.getMatrix();
    plutovg_matrix_t matrix;
    plutovg_matrix_init(&matrix, m[0], m[1], m[2], m[3], m[4], m[5]);
    return matrix;
}

plutovg_paint_t* to_plutovg_paint(const Paint& paint)
{
    if(paint.type == PaintTypeColor)
    {
        const Rgb& c = paint.color;
        return plutovg_paint_create_rgba(c.r/255.0, c.g/255.0, c.b/255.0, (c.a/255.0)*paint.opacity);
    }

    if(paint.type == PaintTypeGradient)
    {
        plutovg_paint_t* source;
        if(paint.gradient.type == GradientTypeLinear)
        {
            LinearGradient linear = paint.gradient.linear();
            source = plutovg_paint_create_linear(linear.x1, linear.y1, linear.x2, linear.y2);
        }
        else
        {
            RadialGradient radial = paint.gradient.radial();
            source = plutovg_paint_create_radial(radial.cx, radial.cy, radial.r, radial.fx, radial.fy, 0);
        }

        plutovg_gradient_t* gradient = plutovg_paint_get_gradient(source);
        if(paint.gradient.spread == SpreadMethodReflect)
        {
            plutovg_gradient_set_spread(gradient, plutovg_spread_method_reflect);
        }
        else if(paint.gradient.spread == SpreadMethodRepeat)
        {
            plutovg_gradient_set_spread(gradient, plutovg_spread_method_repeat);
        }
        else
        {
            plutovg_gradient_set_spread(gradient, plutovg_spread_method_pad);
        }

        const GradientStops& stops = paint.gradient.stops;
        for(unsigned int i = 0;i < stops.size();i++)
        {
            double offset = stops[i].first;
            const Rgb& c = stops[i].second;
            plutovg_gradient_add_stop_rgba(gradient, offset, c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0);
        }

        plutovg_gradient_set_opacity(gradient, paint.opacity);
        plutovg_matrix_t matrix = to_plutovg_matrix(paint.gradient.matrix);
        plutovg_gradient_set_matrix(gradient, &matrix);
        return source;
    }

    if(paint.type == PaintTypeTexture)
    {
        plutovg_paint_t* source = plutovg_paint_create_for_surface(paint.texture.canvas.impl()->surface());
        plutovg_texture_t* texture = plutovg_paint_get_texture(source);
        if(paint.texture.type == TextureTypePlain)
        {
            plutovg_texture_set_type(texture, plutovg_texture_type_plain);
        }
        else
        {
            plutovg_texture_set_type(texture, plutovg_texture_type_tiled);
        }

        plutovg_texture_set_opacity(texture, paint.opacity);
        plutovg_matrix_t matrix = to_plutovg_matrix(paint.texture.matrix);
        plutovg_texture_set_matrix(texture, &matrix);
        return source;
    }

    return nullptr;
}

plutovg_fill_rule_t to_plutovg_fill_rule(WindRule fillRule)
{
    return fillRule==WindRuleEvenOdd?plutovg_fill_rule_even_odd:plutovg_fill_rule_non_zero;
}

plutovg_operator_t to_plutovg_operator(BlendMode mode)
{
    return mode==BlendModeDst_In?plutovg_operator_dst_in:plutovg_operator_src_over;
}

plutovg_line_cap_t to_plutovg_line_cap(LineCap cap)
{
    return cap==LineCapButt?plutovg_line_cap_butt:cap==LineCapRound?plutovg_line_cap_round:plutovg_line_cap_square;
}

plutovg_line_join_t to_plutovg_line_join(LineJoin join)
{
    return join==LineJoinMiter?plutovg_line_join_miter:join==LineJoinRound?plutovg_line_join_round:plutovg_line_join_bevel;
}

} // namespace lunasvg

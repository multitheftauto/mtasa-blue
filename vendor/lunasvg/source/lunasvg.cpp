#include "lunasvg.h"
#include "layoutcontext.h"
#include "parser.h"

#include <fstream>
#include <cstring>
#include <cmath>

namespace lunasvg {

struct Bitmap::Impl {
    Impl(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
    Impl(std::uint32_t width, std::uint32_t height);

    std::unique_ptr<std::uint8_t[]> ownData;
    std::uint8_t* data;
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t stride;
};

Bitmap::Impl::Impl(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
    : data(data), width(width), height(height), stride(stride)
{
}

Bitmap::Impl::Impl(std::uint32_t width, std::uint32_t height)
    : ownData(new std::uint8_t[width*height*4]), data(nullptr), width(width), height(height), stride(width * 4)
{
}

Bitmap::Bitmap()
{
}

Bitmap::Bitmap(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
    : m_impl(new Impl(data, width, height, stride))
{
}

Bitmap::Bitmap(std::uint32_t width, std::uint32_t height)
    : m_impl(new Impl(width, height))
{
}

void Bitmap::reset(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
{
    m_impl.reset(new Impl(data, width, height, stride));
}

void Bitmap::reset(std::uint32_t width, std::uint32_t height)
{
    m_impl.reset(new Impl(width, height));
}

std::uint8_t* Bitmap::data() const
{
    if(m_impl == nullptr)
        return nullptr;
    if(m_impl->data == nullptr)
        return m_impl->ownData.get();
    return m_impl->data;
}

std::uint32_t Bitmap::width() const
{
    return m_impl ? m_impl->width : 0;
}

std::uint32_t Bitmap::height() const
{
    return m_impl ? m_impl->height : 0;
}

std::uint32_t Bitmap::stride() const
{
    return m_impl ? m_impl->stride : 0;
}

void Bitmap::clear(std::uint32_t color)
{
    auto r = (color >> 24) & 0xFF;
    auto g = (color >> 16) & 0xFF;
    auto b = (color >> 8) & 0xFF;
    auto a = (color >> 0) & 0xFF;

    auto pr = (r * a) / 255;
    auto pg = (g * a) / 255;
    auto pb = (b * a) / 255;

    auto width = this->width();
    auto height = this->height();
    auto stride = this->stride();
    auto rowData = this->data();

    for(std::uint32_t y = 0; y < height; y++) {
        auto data = rowData;
        for(std::uint32_t x = 0; x < width; x++) {
            data[0] = pb;
            data[1] = pg;
            data[2] = pr;
            data[3] = a;
            data += 4;
        }

        rowData += stride;
    }
}

void Bitmap::convert(int ri, int gi, int bi, int ai, bool unpremultiply)
{
    auto width = this->width();
    auto height = this->height();
    auto stride = this->stride();
    auto rowData = this->data();

    for(std::uint32_t y = 0; y < height; y++) {
        auto data = rowData;
        for(std::uint32_t x = 0; x < width; x++) {
            auto b = data[0];
            auto g = data[1];
            auto r = data[2];
            auto a = data[3];

            if(unpremultiply && a != 0) {
                r = (r * 255) / a;
                g = (g * 255) / a;
                b = (b * 255) / a;
            }

            data[ri] = r;
            data[gi] = g;
            data[bi] = b;
            data[ai] = a;
            data += 4;
        }

        rowData += stride;
    }
}

Box::Box(double x, double y, double w, double h)
    : x(x), y(y), w(w), h(h)
{
}

Box::Box(const Rect& rect)
    : x(rect.x), y(rect.y), w(rect.w), h(rect.h)
{
}

Box& Box::transform(const Matrix &matrix)
{
    *this = transformed(matrix);
    return *this;
}

Box Box::transformed(const Matrix& matrix) const
{
    return Transform(matrix).map(*this);
}

Matrix::Matrix(double a, double b, double c, double d, double e, double f)
    : a(a), b(b), c(c), d(d), e(e), f(f)
{
}

Matrix::Matrix(const Transform& transform)
    : a(transform.m00), b(transform.m10), c(transform.m01), d(transform.m11), e(transform.m02), f(transform.m12)
{
}

Matrix& Matrix::rotate(double angle)
{
    *this = rotated(angle) * *this;
    return *this;
}

Matrix& Matrix::rotate(double angle, double cx, double cy)
{
    *this = rotated(angle, cx, cy) * *this;
    return *this;
}

Matrix& Matrix::scale(double sx, double sy)
{
    *this = scaled(sx, sy) * *this;
    return *this;
}

Matrix& Matrix::shear(double shx, double shy)
{
    *this = sheared(shx, shy) * *this;
    return *this;
}

Matrix& Matrix::translate(double tx, double ty)
{
   *this = translated(tx, ty) * *this;
    return *this;
}

Matrix& Matrix::transform(double a, double b, double c, double d, double e, double f)
{
    *this = Matrix{a, b, c, d, e, f} * *this;
    return *this;
}

Matrix& Matrix::identity()
{
    *this = Matrix{1, 0, 0, 1, 0, 0};
    return *this;
}

Matrix& Matrix::invert()
{
    *this = inverted();
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& matrix)
{
    *this = *this * matrix;
    return *this; 
}

Matrix& Matrix::premultiply(const Matrix& matrix)
{
    *this = matrix * *this;
    return *this; 
}

Matrix& Matrix::postmultiply(const Matrix& matrix)
{
    *this = *this * matrix;
    return *this; 
}

Matrix Matrix::inverted() const
{
    return Transform(*this).inverted();
}

Matrix Matrix::operator*(const Matrix& matrix) const
{
    return Transform(*this) * Transform(matrix);
}

Matrix Matrix::rotated(double angle)
{
    return Transform::rotated(angle);
}

Matrix Matrix::rotated(double angle, double cx, double cy)
{
    return Transform::rotated(angle, cx, cy);
}

Matrix Matrix::scaled(double sx, double sy)
{
    return Transform::scaled(sx, sy);
}

Matrix Matrix::sheared(double shx, double shy)
{
    return Transform::sheared(shx, shy);
}

Matrix Matrix::translated(double tx, double ty)
{
    return Transform::translated(tx, ty);
}

std::unique_ptr<Document> Document::loadFromFile(const std::string& filename)
{
    std::ifstream fs;
    fs.open(filename);
    if(!fs.is_open())
        return nullptr;

    std::string content;
    std::getline(fs, content, '\0');
    fs.close();

    return loadFromData(content);
}

std::unique_ptr<Document> Document::loadFromData(const std::string& string)
{
    return loadFromData(string.data(), string.size());
}

std::unique_ptr<Document> Document::loadFromData(const char* data, std::size_t size)
{
    TreeBuilder builder;
    if(!builder.parse(data, size))
        return nullptr;

    auto root = builder.build();
    if(root == nullptr)
        return nullptr;

    std::unique_ptr<Document> document(new Document);
    document->root = std::move(root);
    return document;
}

std::unique_ptr<Document> Document::loadFromData(const char* data)
{
    return loadFromData(data, std::strlen(data));
}

void Document::setMatrix(const Matrix& matrix)
{
    root->transform = Transform(matrix);
}

Matrix Document::matrix() const
{
    return root->transform;
}

Box Document::box() const
{
    return root->map(root->strokeBoundingBox());
}

double Document::width() const
{
    return root->width;
}

double Document::height() const
{
    return root->height;
}

void Document::render(Bitmap bitmap, const Matrix& matrix) const
{
    RenderState state(nullptr, RenderMode::Display);
    state.canvas = Canvas::create(bitmap.data(), bitmap.width(), bitmap.height(), bitmap.stride());
    state.transform = Transform(matrix);
    root->render(state);
}

Bitmap Document::renderToBitmap(std::uint32_t width, std::uint32_t height, std::uint32_t backgroundColor) const
{
    if(root->width == 0.0 || root->height == 0.0)
        return Bitmap{};

    if(width == 0 && height == 0) {
        width = static_cast<std::uint32_t>(std::ceil(root->width));
        height = static_cast<std::uint32_t>(std::ceil(root->height));
    } else if(width != 0 && height == 0) {
        height = static_cast<std::uint32_t>(std::ceil(width * root->height / root->width));
    } else if(height != 0 && width == 0) {
        width = static_cast<std::uint32_t>(std::ceil(height * root->width / root->height));
    }

    Matrix matrix(width / root->width, 0, 0, height / root->height, 0, 0);
    Bitmap bitmap(width, height);
    bitmap.clear(backgroundColor);
    render(bitmap, matrix);
    return bitmap;
}

Document::Document() = default;

Document::~Document() = default;

} // namespace lunasvg

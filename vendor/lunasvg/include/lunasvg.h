/*
 * Copyright (c) 2020 Nwutobo Samuel Ugochukwu <sammycageagle@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef LUNASVG_H
#define LUNASVG_H

#include <memory>
#include <string>

#if defined(_MSC_VER) && defined(LUNASVG_SHARED)
#ifdef LUNASVG_EXPORT
#define LUNASVG_API __declspec(dllexport)
#else
#define LUNASVG_API __declspec(dllimport)
#endif
#else
#define LUNASVG_API
#endif

namespace lunasvg {

class Box
{
public:
    Box() = default;
    Box(double x, double y, double w, double h)
        : x(x), y(y), w(w), h(h)
    {}

public:
    double x{0};
    double y{0};
    double w{0};
    double h{0};
};

class Matrix
{
public:
    Matrix() = default;
    Matrix(double a, double b, double c, double d, double e, double f)
        : a(a), b(b), c(c), d(d), e(e), f(f)
    {}

public:
    double a{1};
    double b{0};
    double c{0};
    double d{1};
    double e{0};
    double f{0};
};

class LUNASVG_API Bitmap
{
public:
    /**
     * @note Default bitmap format is RGBA (non-premultiplied).
     */
    Bitmap();
    Bitmap(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
    Bitmap(std::uint32_t width, std::uint32_t height);

    void reset(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
    void reset(std::uint32_t width, std::uint32_t height);

    std::uint8_t* data() const;
    std::uint32_t width() const;
    std::uint32_t height() const;
    std::uint32_t stride() const;
    bool valid() const;

private:
    struct Impl;
    std::shared_ptr<Impl> m_impl;
};

class LayoutSymbol;

class LUNASVG_API Document
{
public:
    /**
     * @brief Creates a document from a file
     * @param filename - file to load
     * @return pointer to document on success, otherwise nullptr
     */
    static std::unique_ptr<Document> loadFromFile(const std::string& filename);

    /**
     * @brief Creates a document from a string
     * @param string - string to load
     * @return pointer to document on success, otherwise nullptr
     */
    static std::unique_ptr<Document> loadFromData(const std::string& string);

    /**
     * @brief Creates a document from a string data and size
     * @param data - string data to load
     * @param size - size of the data to load, in bytes
     * @return pointer to document on success, otherwise nullptr
     */
    static std::unique_ptr<Document> loadFromData(const char* data, std::size_t size);

    /**
     * @brief Creates a document from a null terminated string data
     * @param data - null terminated string data to load
     * @return pointer to document on success, otherwise nullptr
     */
    static std::unique_ptr<Document> loadFromData(const char* data);

    /**
     * @brief Rotates the document matrix clockwise around the current origin
     * @param angle - rotation angle, in degrees
     * @return this
     */
    Document* rotate(double angle);

    /**
     * @brief Rotates the document matrix clockwise around the given point
     * @param angle - rotation angle, in degrees
     * @param cx - horizontal translation
     * @param cy - vertical translation
     * @return this
     */
    Document* rotate(double angle, double cx, double cy);

    /**
     * @brief Scales the document matrix by sx horizontally and sy vertically
     * @param sx - horizontal scale factor
     * @param sy - vertical scale factor
     * @return this
     */
    Document* scale(double sx, double sy);

    /**
     * @brief Shears the document matrix by shx horizontally and shy vertically
     * @param shx - horizontal skew factor, in degree
     * @param shy - vertical skew factor, in degree
     * @return this
     */
    Document* shear(double shx, double shy);

    /**
     * @brief Translates the document matrix by tx horizontally and ty vertically
     * @param tx - horizontal translation
     * @param ty - vertical translation
     * @return this
     */
    Document* translate(double tx, double ty);

    /**
     * @brief Multiplies the document matrix by Matrix(a, b, c, d, e, f)
     * @param a - horizontal scale factor
     * @param b - horizontal skew factor
     * @param c - vertical skew factor
     * @param d - vertical scale factor
     * @param e - horizontal translation
     * @param f - vertical translation
     * @return this
     */
    Document* transform(double a, double b, double c, double d, double e, double f);

    /**
     * @brief Resets the document matrix to identity
     * @return this
     */
    Document* identity();

    /**
     * @brief Returns the current transformation matrix of the document
     * @return the current transformation matrix
     */
    Matrix matrix() const;

    /**
     * @brief Returns the smallest rectangle in which the document fits
     * @return the smallest rectangle in which the document fits
     */
    Box box() const;

    /**
     * @brief Returns width of the document
     * @return the width of the document in pixels
     */
    double width() const;

    /**
     * @brief Returns the height of the document
     * @return the height of the document in pixels
     */
    double height() const;

    /**
     * @brief Renders the document to a bitmap
     * @param matrix - the current transformation matrix
     * @param bitmap - target image on which the content will be drawn
     * @param backgroundColor - background color in 0xRRGGBBAA format
     */
    void render(Bitmap bitmap, const Matrix& matrix = Matrix{}, std::uint32_t backgroundColor = 0x00000000) const;

    /**
     * @brief Renders the document to a bitmap
     * @param width - maximum width, in pixels
     * @param height - maximum height, in pixels
     * @param backgroundColor - background color in 0xRRGGBBAA format
     * @return the raster representation of the document
     */
    Bitmap renderToBitmap(std::uint32_t width = 0, std::uint32_t height = 0, std::uint32_t backgroundColor = 0x00000000) const;

    ~Document();
private:
    Document();

    std::unique_ptr<LayoutSymbol> root;
};

} //namespace lunasvg

#endif // LUNASVG_H

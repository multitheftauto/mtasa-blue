#ifndef FONT_H
#define FONT_H

#include "path.h"

#include <memory>
#include <string>

namespace lunasvg {

struct Glyph
{
    double advance;
    Path data;
};

class FontImpl;

class Font
{
public:
    static std::unique_ptr<Font> loadFromFile(const std::string& filename);

    const Glyph& getGlyph(std::uint32_t codepoint) const;
    double getKerning(std::uint32_t first, std::uint32_t second) const;
    double getScale() const;

    ~Font();
private:
    Font();

    std::unique_ptr<FontImpl> d;
};

} // namespace lunasvg

#endif // FONT_H

[![Releases](https://img.shields.io/badge/Version-2.2.0-orange.svg)](https://github.com/sammycage/lunasvg/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/sammycage/lunasvg/blob/master/LICENSE)
[![Build Status](https://github.com/sammycage/lunasvg/actions/workflows/ci.yml/badge.svg)](https://github.com/sammycage/lunasvg/actions)

# LunaSVG - SVG rendering library in C++

![LunaSVG](https://github.com/sammycage/lunasvg/blob/master/luna.png)

## Example

```cpp
#include <lunasvg/document.h>

using namespace lunasvg;

int main()
{
    auto document = Document::loadFromFile("tiger.svg");
    auto bitmap = document->renderToBitmap();

    // do something useful with the bitmap here.

    return 0;
}

```

## Features

- Basic Shapes
- Document Structures
- Coordinate Systems, Transformations and Units
- SolidColors
- Gradients
- Patterns
- Masks
- ClipPaths
- Markers
- StyleSheet

## TODO

- Texts
- Filters
- Images

## Build

```
git clone https://github.com/sammycage/lunasvg.git
cd lunasvg
mkdir build
cd build
cmake ..
make -j 2
```

To install lunasvg library.

```
make install
```

## Demo

While building lunasvg example it generates a simple SVG to PNG converter which can be used to convert SVG file to PNG file.

Run Demo.
```
svg2png [filename] [resolution] [bgColor]
```

## Projects Using LunaSVG

- [OpenSiv3D](https://github.com/Siv3D/OpenSiv3D)
- [PICsimLab](https://github.com/lcgamboa/picsimlab)
- [MoneyManagerEx](https://github.com/moneymanagerex/moneymanagerex)
- [RmlUi](https://github.com/mikke89/RmlUi)
- [EKA2L1](https://github.com/EKA2L1/EKA2L1)

## Support Me

If you like the work lunasvg is doing please consider a small donation :

[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/sammycage)
[![Donate](https://img.shields.io/badge/Donate-BuyMeACoffee-yellow.svg)](https://www.buymeacoffee.com/sammycage)
[![Sponsor](https://img.shields.io/badge/Sponsor-Patreon-orange.svg)](https://patreon.com/sammycage)

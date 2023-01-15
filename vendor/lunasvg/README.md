[![Releases](https://img.shields.io/badge/Version-2.3.5-orange.svg)](https://github.com/sammycage/lunasvg/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/sammycage/lunasvg/blob/master/LICENSE)
[![Build Status](https://github.com/sammycage/lunasvg/actions/workflows/ci.yml/badge.svg)](https://github.com/sammycage/lunasvg/actions)

> If you like the work lunasvg is doing please consider a small donation : [![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.me/sammycage)

# LunaSVG - SVG rendering library in C++

![LunaSVG](https://github.com/sammycage/lunasvg/blob/master/luna.png)

## Example

```cpp
#include <lunasvg.h>

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
- [EKA2L1](https://github.com/EKA2L/EKA2L1)
- [ObEngine](https://github.com/ObEngine/ObEngine)
- [OTTO](https://github.com/bitfieldaudio/OTTO)
- [EmulationStation-DE](https://gitlab.com/es-de/emulationstation-de)

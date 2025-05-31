[![Releases](https://img.shields.io/badge/Version-3.2.0-orange.svg)](https://github.com/sammycage/lunasvg/releases)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/sammycage/lunasvg/blob/master/LICENSE)
[![Build Status](https://github.com/sammycage/lunasvg/actions/workflows/main.yml/badge.svg)](https://github.com/sammycage/lunasvg/actions)

# LunaSVG

LunaSVG is an SVG rendering library in C++, designed to be lightweight and portable, offering efficient rendering and manipulation of Scalable Vector Graphics (SVG) files.

## Basic Usage

```cpp
#include <lunasvg.h>

using namespace lunasvg;

int main()
{
    auto document = Document::loadFromFile("tiger.svg");
    if(document == nullptr)
        return -1;
    auto bitmap = document->renderToBitmap();
    if(bitmap.isNull())
        return -1;
    bitmap.writeToPng("tiger.png");
    return 0;
}

```

![tiger.png](https://github.com/user-attachments/assets/b87bbf92-6dd1-4b29-a890-99cfffce66b8)

---

## Dynamic Styling

```cpp
#include <lunasvg.h>

using namespace lunasvg;

static const char kLandspaceContent[] = R"SVG(
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 800 600" width="800" height="600">
  <!-- Background (Sky) -->
  <rect width="800" height="600" class="sky"/>

  <!-- Sun -->
  <circle cx="650" cy="150" r="80" class="sun" />

  <!-- Clouds -->
  <ellipse cx="200" cy="150" rx="100" ry="40" class="cloud" />
  <ellipse cx="250" cy="200" rx="120" ry="50" class="cloud" />
  <ellipse cx="500" cy="80" rx="150" ry="60" class="cloud" />
  <ellipse cx="550" cy="120" rx="120" ry="50" class="cloud" />

  <!-- Mountains -->
  <polygon points="0,450 200,200 400,450" class="mountain" />
  <polygon points="200,450 400,100 600,450" class="mountain" />
  <polygon points="400,450 600,250 800,450" class="mountain" />

  <!-- Foreground (Ground) -->
  <rect y="450" width="800" height="150" class="ground" />
</svg>
)SVG";

static const char kSummerStyle[] = R"CSS(
.sky { fill: #4A90E2; }
.sun { fill: #FF7F00; }
.mountain { fill: #2E3A59; }
.cloud { fill: #FFFFFF; opacity: 0.8; }
.ground { fill: #2E8B57; }
)CSS";

static const char kWinterStyle[] = R"CSS(
.sky { fill: #87CEEB; }
.sun { fill: #ADD8E6; }
.mountain { fill: #2F4F4F; }
.cloud { fill: #FFFFFF; opacity: 0.8; }
.ground { fill: #FFFAFA; }
)CSS";

int main()
{
    auto document = Document::loadFromData(kLandspaceContent);

    document->applyStyleSheet(kSummerStyle);
    document->renderToBitmap().writeToPng("summer.png");

    document->applyStyleSheet(kWinterStyle);
    document->renderToBitmap().writeToPng("winter.png");
    return 0;
}
```

| `summer.png` | `winter.png` |
| --- | --- |
| ![summer.png](https://github.com/user-attachments/assets/c7f16780-23f8-4acd-906a-2242f2d0d33b) | ![winter.png](https://github.com/user-attachments/assets/fdd65288-11c7-4e16-bb5a-2bf28de57145) |

## Features

LunaSVG supports nearly all graphical features outlined in the SVG 1.1 and SVG 1.2 Tiny specifications. The primary exceptions are animation, filters, and scripts. As LunaSVG is designed for static rendering, animation is unlikely to be supported in the future. However, support for filters may be added. It currently handles a wide variety of elements, including:

`<a>` `<circle>` `<clipPath>` `<defs>` `<ellipse>` `<g>` `<image>` `<line>` `<linearGradient>` `<marker>` `<mask>` `<path>` `<pattern>` `<polygon>` `<polyline>` `<radialGradient>` `<rect>` `<stop>` `<style>` `<svg>` `<symbol>` `<text>` `<tspan>` `<use>`

## Installation

Follow the steps below to install LunaSVG using either [CMake](https://cmake.org/) or [Meson](https://mesonbuild.com/).

### Using CMake

```bash
git clone --recursive https://github.com/sammycage/lunasvg.git
cd lunasvg
cmake -B build .
cmake --build build
cmake --install build
```

After installing LunaSVG, you can include the library in your CMake projects using `find_package`:

```cmake
find_package(lunasvg REQUIRED)

# Link LunaSVG to your target
target_link_libraries(your_target_name PRIVATE lunasvg::lunasvg)
```

Alternatively, you can use CMake's `FetchContent` to include LunaSVG directly in your project without needing to install it first:

```cmake
include(FetchContent)
FetchContent_Declare(
    lunasvg
    GIT_REPOSITORY https://github.com/sammycage/lunasvg.git
    GIT_TAG master  # Specify the desired branch or tag
)
FetchContent_MakeAvailable(lunasvg)

# Link LunaSVG to your target
target_link_libraries(your_target_name PRIVATE lunasvg::lunasvg)
```

Replace `your_target_name` with the name of your executable or library target.

### Using Meson

```bash
git clone https://github.com/sammycage/lunasvg.git
cd lunasvg
meson setup build
meson compile -C build
meson install -C build
```

After installing LunaSVG, you can include the library in your Meson projects using the `dependency` function:

```meson
lunasvg_dep = dependency('lunasvg', required: true)
```

Alternatively, add `lunasvg.wrap` to your `subprojects` directory to include LunaSVG directly in your project without needing to install it first. Create a file named `lunasvg.wrap` with the following content:

```ini
[wrap-git]
url = https://github.com/sammycage/lunasvg.git
revision = head
depth = 1

[provide]
lunasvg = lunasvg_dep
```

You can retrieve the dependency from the wrap fallback with:

```meson
lunasvg_dep = dependency('lunasvg', fallback: ['lunasvg', 'lunasvg_dep'])
```

## Demo

LunaSVG provides a command-line tool `svg2png` for converting SVG files to PNG format.

### Usage:
```bash
svg2png [filename] [resolution] [bgColor]
```

### Examples:
```bash
$ svg2png input.svg
$ svg2png input.svg 512x512
$ svg2png input.svg 512x512 0xff00ffff
```

## Projects Using LunaSVG

- [OpenSiv3D](https://github.com/Siv3D/OpenSiv3D)
- [PICsimLab](https://github.com/lcgamboa/picsimlab)
- [MoneyManagerEx](https://github.com/moneymanagerex/moneymanagerex)
- [RmlUi](https://github.com/mikke89/RmlUi)
- [EKA2L1](https://github.com/EKA2L1/EKA2L1)
- [ObEngine](https://github.com/ObEngine/ObEngine)
- [OTTO](https://github.com/bitfieldaudio/OTTO)
- [EmulationStation-DE](https://gitlab.com/es-de/emulationstation-de)
- [SvgBooga](https://github.com/etodanik/SvgBooga/tree/main)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [Multi Theft Auto: San Andreas](https://github.com/multitheftauto/mtasa-blue)
- [eScada Solutions](https://www.escadasolutions.com)
- [CARLA Simulator](https://carla.org/)
- [AUI Framework](https://github.com/aui-framework/aui)

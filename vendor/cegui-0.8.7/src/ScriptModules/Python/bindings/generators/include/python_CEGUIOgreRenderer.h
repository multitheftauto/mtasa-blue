#include "CEGUI/String.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/Texture.h"
#include "CEGUI/TextureTarget.h"
#include "CEGUI/GUIContext.h"

#include "CEGUI/RendererModules/Ogre/Renderer.h"
#include "CEGUI/RendererModules/Ogre/ResourceProvider.h"
#include "CEGUI/RendererModules/Ogre/ImageCodec.h"

namespace Ogre
{
    class Root{};
    class RenderSystem{};
    class RenderTarget{};
    #if (CEGUI_OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
        class TexturePtr{};
    #else
        template<typename T> class SharedPtr{};
        class Texture{};
        typedef SharedPtr<Texture> TexturePtr;
    #endif
    class Matrix4{};
}

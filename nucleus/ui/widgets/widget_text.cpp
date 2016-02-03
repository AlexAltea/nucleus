/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_text.h"
#include "nucleus/logger/logger.h"
#include "nucleus/ui/ui.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "externals/stb/stb_truetype.h"

namespace ui {

void WidgetText::update(const Font* fontFamily, const Length& fontSize, const std::string& text) {
    const auto* fontInfo = &fontFamily->fontinfo;
    auto lineHeight = getCoordY(fontSize) * manager->surface.getHeight();

    // Font properties
    int ascent, descent, lineGap;
    float scale = stbtt_ScaleForPixelHeight(fontInfo, lineHeight);
    stbtt_GetFontVMetrics(fontInfo, &ascent, &descent, &lineGap);
    ascent *= scale;
    descent *= scale;

    // Compute texture size
    txWidth = 0;
    txHeight = ascent - descent;
    for (Size i = 0; i < text.length(); i++) {
        int dx;
        int dy = (ascent - descent) + lineGap;
        int kern = stbtt_GetCodepointKernAdvance(fontInfo, text[i], text[i+1]);
        stbtt_GetCodepointHMetrics(fontInfo, text[i], &dx, 0);
        txWidth += (dx + kern) * scale;
    }

    // Render characters
    Size x = 0;
    Size y = 0;
    txBuffer.clear();
    txBuffer.resize(txWidth * txHeight);
    for (Size i = 0; i < text.length(); i++) {
        int dx;
        int dy = (ascent - descent) + lineGap;
        int kern = stbtt_GetCodepointKernAdvance(fontInfo, text[i], text[i+1]);
        stbtt_GetCodepointHMetrics(fontInfo, text[i], &dx, 0);

        int x1, y1, x2, y2;
        stbtt_GetCodepointBitmapBox(fontInfo, text[i], scale, scale, &x1, &y1, &x2, &y2);

        Size offset = x + ((y + ascent + y1) * txWidth);
        stbtt_MakeCodepointBitmap(fontInfo, &txBuffer[offset], x2 - x1, y2 - y1, txWidth, scale, scale, text[i]);
        x += (dx + kern) * scale;
    }

    // Create texture
    gfx::TextureDesc textureDesc = {};
    textureDesc.width = txWidth;
    textureDesc.height = txHeight;
    textureDesc.mipmapLevels = 1;
    textureDesc.format = gfx::FORMAT_R8G8B8A8_UNORM;
    textureDesc.data = txBuffer.data();
    textureDesc.size = txBuffer.size();

    texture = manager->graphics->createTexture(textureDesc);
}

void WidgetText::dimensionalize() {
    vertWidth = 0.0;
    vertHeight = 0.0;

    if (style.width.type == Length::TYPE_UNDEFINED) {
        vertWidth = getCoordX(Length{ double(txWidth), Length::TYPE_PX });
    } else {
        vertWidth = getCoordX(style.width);
    }
    if (style.height.type == Length::TYPE_UNDEFINED) {
        vertHeight = getCoordY(Length{ double(txHeight), Length::TYPE_PX });
    } else {
        vertHeight = getCoordY(style.height);
    }
}

void WidgetText::render() {
    auto offsetTop = getOffsetTop() + getCoordY(style.margin.top);
    auto offsetLeft = getOffsetLeft() + getCoordX(style.margin.left);

    // Preparing vertex input
    auto x1 = -1.0 + 2 * (offsetLeft);
    auto x2 = -1.0 + 2 * (offsetLeft + getPaddingWidth());
    auto y1 = +1.0 - 2 * (offsetTop + getPaddingHeight());
    auto y2 = +1.0 - 2 * (offsetTop);

    /**
     * Screen
     * ======
     *  (0,1)                              (1,1)
     *  +--------------------------------------+
     *  |   V1 = (x1,y2)       V3 = (x2,y2)    |
     *  |     + - - - - - - - +                |
     *  |     : \             :                |
     *  |     :    \          :                |
     *  |     :       \       : h              |
     *  |     :          \    :                |
     *  |     :       w     \ :                |
     *  |     + - - - - - - - +                |
     *  |   V0 = (x1,y1)       V2 = (x2,y1)    |
     *  +--------------------------------------+
     *  (0,0)                              (1,0)
     */
    auto& V0 = input.vertex[0];
    auto& V1 = input.vertex[1];
    auto& V2 = input.vertex[2];
    auto& V3 = input.vertex[3];

    V0.position[0] = V1.position[0] = x1;
    V2.position[0] = V3.position[0] = x2;
    V0.position[1] = V2.position[1] = y1;
    V1.position[1] = V3.position[1] = y2;
    V0.position[2] = V1.position[2] = V2.position[2] = V3.position[2] = 0.0;
    V0.position[3] = V1.position[3] = V2.position[3] = V3.position[3] = 1.0;
    V0.background = style.background;
    V1.background = style.background;
    V2.background = style.background;
    V3.background = style.background;

    manager->renderWidget(input);
}

}  // namespace ui

/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "font.h"
#include "nucleus/logger/logger.h"

namespace ui {

Font::Font(core::ResourceName resName) : resource(resName) {
    if (!stbtt_InitFont(&fontinfo, resource.data, 0)) {
        logger.error(LOG_UI, "Could not load the specified font resource");
    }
}

Font::~Font() {

}

}  // namespace ui

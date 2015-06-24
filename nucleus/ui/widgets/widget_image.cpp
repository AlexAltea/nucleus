/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_image.h"
#include "nucleus/common.h"
#include "nucleus/emulator.h"

#include "externals/stb/stb_image.h"

WidgetImage::~WidgetImage()
{
    delete image.buffer;
}

void WidgetImage::init(const std::string& pngfile)
{
    // Get file contents
    FILE* file = fopen(pngfile.c_str(), "rb");
    if (!file) {
        nucleus.log.error(LOG_UI, "Image not found: %s", pngfile.c_str());
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    auto* pngbuffer = new unsigned char[size + 1];
    fseek(file, 0, SEEK_SET);
    fread(pngbuffer, size, 1, file);
    fclose(file);

    init(pngbuffer, size);
}

void WidgetImage::init(const unsigned char* pngbuffer, size_t size)
{
    /**
     * NOTE: STB generates stores the image rows in reverse order with respect to the format OpenGL expects.
     * Vertical quad coordinates are swapped on rendering to make sure the image shows up properly.
     */
    image.buffer = stbi_load_from_memory(pngbuffer, size, &image.width, &image.height, &image.components, 4);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void WidgetImage::render()
{
    Length width = style.width;
    Length height = style.height;

    if (style.sizeMode == PROPORTION_AUTOWIDTH) {
        width = height * float(image.width) / float(image.height);
    }
    if (style.sizeMode == PROPORTION_AUTOHEIGHT) {
        height = width * float(image.height) / float(image.width);
    }
    height = Widget::correctHeight(height);
    
    GLfloat x1 = Widget::getCoordinateX(style.left);
    GLfloat x2 = Widget::getCoordinateX(style.left + width);
    GLfloat y1 = Widget::getCoordinateY(style.top);
    GLfloat y2 = Widget::getCoordinateY(style.top + height);

    // Draw a textured quad
    glBindTexture(GL_TEXTURE_2D, textureId);
    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, style.opacity);
    glTexCoord2f(0, 0); glVertex3f(x1, y1, 0);
    glTexCoord2f(0, 1); glVertex3f(x1, y2, 0);
    glTexCoord2f(1, 1); glVertex3f(x2, y2, 0);
    glTexCoord2f(1, 0); glVertex3f(x2, y1, 0);
    glEnd();
}

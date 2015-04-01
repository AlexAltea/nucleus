/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_texture.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

void WidgetTexture::render()
{
    Length width = style.width;
    Length height = style.height;
    
    GLfloat x1 = Widget::getCoordinateX(style.left);
    GLfloat x2 = Widget::getCoordinateX(style.left + width);
    GLfloat y1 = Widget::getCoordinateY(style.top);
    GLfloat y2 = Widget::getCoordinateY(style.top + height);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, style.opacity);
    glTexCoord2f(0, 1); glVertex2f(x1, y1);
    glTexCoord2f(0, 0); glVertex2f(x1, y2);
    glTexCoord2f(1, 0); glVertex2f(x2, y2);
    glTexCoord2f(1, 1); glVertex2f(x2, y1);
    glEnd();
}

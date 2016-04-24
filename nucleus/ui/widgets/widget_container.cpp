/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_container.h"
#include "nucleus/assert.h"
#include "nucleus/core/resource.h"
#include "nucleus/graphics/frontend/shader_parser.h"
#include "nucleus/ui/ui.h"

#include <algorithm>

namespace ui {

gfx::Pipeline* WidgetContainer::createPipeline(gfx::IBackend& backend) {
    gfx::ShaderDesc vertDesc = {};
    gfx::ShaderDesc fragDesc = {};
    core::Resource resVS(core::RES_SHADER_UI_WIDGET_CONTAINER_VS);
    core::Resource resPS(core::RES_SHADER_UI_WIDGET_CONTAINER_PS);
    vertDesc.type = gfx::SHADER_TYPE_VERTEX;
    vertDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resVS.data), resVS.size);
    fragDesc.type = gfx::SHADER_TYPE_PIXEL;
    fragDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resPS.data), resPS.size);

    gfx::PipelineDesc pipelineDesc = {};
    pipelineDesc.numCBVs = 0;
    pipelineDesc.numSRVs = 0;
    pipelineDesc.vs = backend.createShader(vertDesc);
    pipelineDesc.ps = backend.createShader(fragDesc);
    pipelineDesc.iaState.topology = gfx::TOPOLOGY_TRIANGLE_STRIP;
    pipelineDesc.iaState.inputLayout = {
        { 0, gfx::FORMAT_R32G32_FLOAT,       0,  0, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Position
        { 1, gfx::FORMAT_R32G32B32A32_FLOAT, 0,  8, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Color
        { 2, gfx::FORMAT_R32_FLOAT,          0, 24, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Z-Index
        { 3, gfx::FORMAT_R32_FLOAT,          0, 28, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 }, // Opacity
    };
    pipelineDesc.cbState.colorTarget[0] = {
        true, false,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::LOGIC_OP_NOOP,
        gfx::COLOR_WRITE_ENABLE_ALL
    };
    return backend.createPipeline(pipelineDesc);
}

bool WidgetContainer::addElement(Widget* widget) {
    assert_true(widget->parent == nullptr, "Widget already has a parent");
    widget->parent = this;
    widget->manager = manager;
    children.push_back(widget);
    return true;
}

bool WidgetContainer::removeElement(const std::string& target) {
    bool found = false;
    auto it = children.begin();
    while (it != children.end()) {
        Widget* child = *it;
        if (child->id == target) {
            it = children.erase(it);
            found = true;
        } else {
            it++;
        }
    }
    return found;
}

Widget* WidgetContainer::find(const std::string& query) {
    if (id == query) {
        return this;
    }
    for (auto& child : children) {
        Widget* result = child->find(query);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

void WidgetContainer::dimensionalize() {
    compWidth = 0.0;
    compWidth += getCoordX(style.padding.left);
    compWidth += getCoordX(style.padding.right);
    compWidth += getCoordX(style.border.left);
    compWidth += getCoordX(style.border.right);
    compHeight = 0.0;
    compHeight += getCoordY(style.padding.top);
    compHeight += getCoordY(style.padding.bottom);
    compHeight += getCoordY(style.border.top);
    compHeight += getCoordY(style.border.bottom);

    if (layout == LAYOUT_VERTICAL) {
        for (auto& child : children) {
            child->dimensionalize();
            auto cw = child->getMarginWidth();
            auto ch = child->getMarginHeight();
            compWidth = std::max(cw, compWidth);
            compHeight += ch;
        }
    } else {
        for (auto& child : children) {
            child->dimensionalize();
            auto cw = child->getMarginWidth();
            auto ch = child->getMarginHeight();
            compWidth = std::max(ch, compWidth);
            vertWidth += cw;
        }
    }

    // Set final dimensions
    if (style.width.type != Length::TYPE_UNDEFINED) {
        vertWidth = getCoordX(style.width);
        scrollH = compWidth > vertWidth;
    } else {
        vertWidth = compWidth;
    }
    if (style.height.type != Length::TYPE_UNDEFINED) {
        vertHeight = getCoordY(style.height);
        scrollV = compHeight > vertHeight;
    } else {
        vertHeight = compHeight;
    }
}

void WidgetContainer::render() {
    auto offsetTop = getOffsetTop() + getCoordY(style.margin.top);
    auto offsetLeft = getOffsetLeft() + getCoordX(style.margin.left);

    // Preparing vertex input
    auto x1 = -1.0 + 2 * (offsetLeft);
    auto x2 = -1.0 + 2 * (offsetLeft + getPaddingWidth());
    auto y1 = +1.0 - 2 * (offsetTop + getPaddingHeight());
    auto y2 = +1.0 - 2 * (offsetTop);

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

    manager->pushWidgetContainer(input);

    auto childOffsetTop = offsetTop + getCoordY(style.padding.top);
    auto childOffsetLeft = offsetLeft + getCoordX(style.padding.left);

    // Render children widgets vertically
    if (layout == LAYOUT_VERTICAL) {
        if (style.alignV == ALIGN_VERTICAL_CENTER) {
            childOffsetTop += (vertHeight - compHeight) / 2.0;
        }
        if (style.alignV == ALIGN_VERTICAL_BOTTOM) {
            childOffsetTop += (vertHeight - compHeight);
        }

        for (auto& child : children) {
            // Correct horizontal alignment
            switch (style.alignH) {
            case ALIGN_HORIZONTAL_LEFT:
                child->setOffsetLeft(childOffsetLeft); break;
            case ALIGN_HORIZONTAL_CENTER:
                child->setOffsetLeft(childOffsetLeft + (getContentWidth() - child->getBorderWidth()) / 2.0); break;
            case ALIGN_HORIZONTAL_RIGHT:
                child->setOffsetLeft(childOffsetLeft + (getContentWidth() - child->getBorderWidth())); break;
            }
            child->setOffsetTop(childOffsetTop);
            child->render();
            childOffsetTop += child->getMarginHeight();
        }
    }

    // Render children widgets horizontally
    if (layout == LAYOUT_HORIZONTAL) {
        if (style.alignH == ALIGN_HORIZONTAL_CENTER) {
            childOffsetLeft += (vertWidth - compWidth) / 2.0;
        }
        if (style.alignH == ALIGN_HORIZONTAL_RIGHT) {
            childOffsetLeft += (vertWidth - compWidth);
        }

        for (auto& child : children) {
            // Correct vertical alignment
            switch (style.alignH) {
            case ALIGN_HORIZONTAL_LEFT:
                child->setOffsetTop(childOffsetTop); break;
            case ALIGN_HORIZONTAL_CENTER:
                child->setOffsetTop(childOffsetTop + (getContentHeight() - child->getBorderHeight()) / 2.0); break;
            case ALIGN_HORIZONTAL_RIGHT:
                child->setOffsetTop(childOffsetTop + (getContentHeight() - child->getBorderHeight())); break;
            }
            child->setOffsetLeft(childOffsetLeft);
            child->render();
            childOffsetLeft += child->getMarginWidth();
        }
    }
}

}  // namespace ui

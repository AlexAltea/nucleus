/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "direct3d12_debug.h"
#include "nucleus/graphics/backend/direct3d12/direct3d12.h"
#include "nucleus/logger/logger.h"

#include <vector>

namespace gfx {
namespace direct3d12 {

#ifdef NUCLEUS_BUILD_DEBUG

bool Direct3D12Debug::enable() {
    HRESULT hr = _D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController));
    if (FAILED(hr)) {
        return false;
    }
    d3dDebugController->EnableDebugLayer();
    return true;
}

bool Direct3D12Debug::initialize(ID3D12Device* device) {
    HRESULT hr;
    hr = device->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue));
#if defined(NUCLEUS_PLATFORM_WINDOWS)
    hr = _DXGIGetDebugInterface(IID_PPV_ARGS(&dxgiDebugController));
    hr = _DXGIGetDebugInterface(IID_PPV_ARGS(&dxgiInfoQueue));
#elif defined(NUCLEUS_PLATFORM_UWP)
    hr = _DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugController));
    hr = _DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue));
#endif
    return true;
}

void Direct3D12Debug::print(D3D12_MESSAGE* msg) {
    const char* categoryName = "(Unknown)";
    switch (msg->Category) {
    case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:
        categoryName = "Application defined"; break;
    case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:
        categoryName = "Miscellaneous"; break;
    case D3D12_MESSAGE_CATEGORY_INITIALIZATION:
        categoryName = "Initialization"; break;
    case D3D12_MESSAGE_CATEGORY_CLEANUP:
        categoryName = "Cleanup"; break;
    case D3D12_MESSAGE_CATEGORY_COMPILATION:
        categoryName = "Compilation"; break;
    case D3D12_MESSAGE_CATEGORY_STATE_CREATION:
        categoryName = "State creation"; break;
    case D3D12_MESSAGE_CATEGORY_STATE_SETTING:
        categoryName = "State setting"; break;
    case D3D12_MESSAGE_CATEGORY_STATE_GETTING:
        categoryName = "State getting"; break;
    case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
        categoryName = "Resource manipulation"; break;
    case D3D12_MESSAGE_CATEGORY_EXECUTION:
        categoryName = "Execution"; break;
    case D3D12_MESSAGE_CATEGORY_SHADER:
        categoryName = "Shader"; break;
    }

    const char* severityName = "(Unknown)";
    switch (msg->Severity) {
    case D3D12_MESSAGE_SEVERITY_CORRUPTION:
        severityName = "Corruption"; break;
    case D3D12_MESSAGE_SEVERITY_ERROR:
        severityName = "Error"; break;
    case D3D12_MESSAGE_SEVERITY_WARNING:
        severityName = "Warning"; break;
    case D3D12_MESSAGE_SEVERITY_INFO:
        severityName = "Info"; break;
    case D3D12_MESSAGE_SEVERITY_MESSAGE:
        severityName = "Message"; break;
    }

    logger.error(LOG_GRAPHICS, "Direct3D12 Debug Message (Category: %s, Severity: %s, ID: %d):\n%s",
        categoryName, severityName, msg->ID, msg->pDescription);
}

void Direct3D12Debug::print(DXGI_INFO_QUEUE_MESSAGE* msg) {
    const char* categoryName = "(Unknown)";
    switch (msg->Category) {
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_MISCELLANEOUS:
        categoryName = "Miscellaneous"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION:
        categoryName = "Initialization"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP:
        categoryName = "Cleanup"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_COMPILATION:
        categoryName = "Compilation"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_CREATION:
        categoryName = "State creation"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_SETTING:
        categoryName = "State setting"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_STATE_GETTING:
        categoryName = "State getting"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
        categoryName = "Resource manipulation"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION:
        categoryName = "Execution"; break;
    case DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER:
        categoryName = "Shader"; break;
    }

    const char* severityName = "(Unknown)";
    switch (msg->Severity) {
    case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
        severityName = "Corruption"; break;
    case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
        severityName = "Error"; break;
    case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
        severityName = "Warning"; break;
    case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
        severityName = "Info"; break;
    case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
        severityName = "Message"; break;
    }

    OLECHAR* producerStr;
    StringFromCLSID(msg->Producer, &producerStr);
    logger.error(LOG_GRAPHICS, "DXGI Debug Message (Category: %s, Severity: %s, Producer: {%s}, ID: %d):\n%s",
        categoryName, severityName, producerStr, msg->ID, msg->pDescription);
    CoTaskMemFree(producerStr);
}

void Direct3D12Debug::printMessages() {
    HRESULT hr;
    if (!d3dInfoQueue) {
        return;
    }

    // Direct3D messages
    UINT64 d3dCount = d3dInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
    for (UINT64 i = 0; i < d3dCount; i++) {
        SIZE_T msgSize = 0;
        hr = d3dInfoQueue->GetMessage(i, nullptr, &msgSize);

        std::vector<byte> buffer(msgSize);
        auto* msg = reinterpret_cast<D3D12_MESSAGE*>(buffer.data());
        hr = d3dInfoQueue->GetMessage(i, msg, &msgSize);

        if (msg->ID == 820 || // TODO: SRV resource should be cleared on creation, but CreateCommittedResource resource fails if we do.
            msg->ID == 821) { // TODO: DSV resource should be cleared on creation, but CreateCommittedResource resource fails if we do.
            continue;
        }
        print(msg);
    }
    d3dInfoQueue->ClearStoredMessages();

    // DXGI messages
    UINT64 dxgiCount = dxgiInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilters(DXGI_DEBUG_ALL);
    for (UINT64 i = 0; i < dxgiCount; i++) {
        SIZE_T msgSize = 0;
        hr = dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &msgSize);

        std::vector<byte> buffer(msgSize);
        auto* msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(buffer.data());
        hr = dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, msg, &msgSize);
        print(msg);
    }
    dxgiInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
}

#endif

}  // namespace direct3d12
}  // namespace gfx

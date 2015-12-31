/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Forward declarations
struct D3D12_MESSAGE;
struct DXGI_INFO_QUEUE_MESSAGE;
struct ID3D12Device;
struct IDXGIDebug;
struct IDXGIInfoQueue;
struct ID3D12InfoQueue;
struct ID3D12Debug;

namespace gfx {
namespace direct3d12 {

class Direct3D12Debug {
private:
    ID3D12InfoQueue* d3dInfoQueue;
    IDXGIInfoQueue* dxgiInfoQueue;
    ID3D12Debug* d3dDebugController;
    IDXGIDebug* dxgiDebugController;

    /**
     * Prints a Direct3D12 debug message
     * @param[in]  msg  Debug message to print
     */
    void print(D3D12_MESSAGE* msg);

    /**
     * Prints a DXGI debug message
     * @param[in]  msg  Debug message to print
     */
    void print(DXGI_INFO_QUEUE_MESSAGE* msg);

public:
    /**
     * Enable Direct3D12 debug layer. Has to be called before the device is created.
     */
    bool enable();

    /**
     * Initializes the info queues
     * @param[in]  device  Direc3D12 device
     */
    bool initialize(ID3D12Device* device);

    void printMessages();
};

}  // namespace direct3d12
}  // namespace gfx

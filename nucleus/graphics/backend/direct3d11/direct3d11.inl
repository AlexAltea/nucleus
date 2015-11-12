/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Functions: d3d11.dll
FUNCTION(PFN_D3D11_CREATE_DEVICE, d3d11, D3D11CreateDevice);

// Functions: dxgi.dll
FUNCTION(PFN_CreateDXGIFactory, dxgi, CreateDXGIFactory);
FUNCTION(PFN_CreateDXGIFactory1, dxgi, CreateDXGIFactory1);
FUNCTION(PFN_CreateDXGIFactory2, dxgi, CreateDXGIFactory2);

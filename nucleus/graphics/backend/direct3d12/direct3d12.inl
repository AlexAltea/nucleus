/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Functions: d3d12.dll
FUNCTION(PFN_D3D12_CREATE_DEVICE, d3d12, D3D12CreateDevice);
FUNCTION(PFN_D3D12_GET_DEBUG_INTERFACE, d3d12, D3D12GetDebugInterface);
FUNCTION(PFN_D3D12_SERIALIZE_ROOT_SIGNATURE, d3d12, D3D12SerializeRootSignature);

// Functions: dxgi.dll
FUNCTION(PFN_CreateDXGIFactory, dxgi, CreateDXGIFactory);
FUNCTION(PFN_CreateDXGIFactory1, dxgi, CreateDXGIFactory1);
FUNCTION(PFN_CreateDXGIFactory2, dxgi, CreateDXGIFactory2);

// Functions: d3dcompiler_47.dll
FUNCTION(pD3DCompile, d3dcompiler_47, D3DCompile);
FUNCTION(pD3DDisassemble, d3dcompiler_47, D3DDisassemble);
FUNCTION(pD3DPreprocess, d3dcompiler_47, D3DPreprocess);

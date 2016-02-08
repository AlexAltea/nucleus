/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "drop_target.h"

// Nucleus
#include "nucleus/nucleus.h"

// IUnknown methods
HRESULT DropTarget::QueryInterface(REFIID riid, void** ppvObject) {
    return E_NOTIMPL;
}

ULONG DropTarget::AddRef() {
    return E_NOTIMPL;
}

ULONG DropTarget::Release() {
    return E_NOTIMPL;
}

// IDropTarget methods
HRESULT DropTarget::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    return E_NOTIMPL;
}

HRESULT DropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    return E_NOTIMPL;
}

HRESULT DropTarget::DragLeave() {
    return E_NOTIMPL;
}

HRESULT DropTarget::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) {
    return E_NOTIMPL;
}

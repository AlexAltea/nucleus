/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <Windows.h>

struct DropTarget : IDropTarget {
    // IUnknown methods
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
         /* [in] */ REFIID riid,
         /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override;
    
     virtual ULONG STDMETHODCALLTYPE AddRef( void) override;
    
     virtual ULONG STDMETHODCALLTYPE Release( void) override;

    // IDropTarget methods
    virtual HRESULT STDMETHODCALLTYPE DragEnter( 
        /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect) override;
    
    virtual HRESULT STDMETHODCALLTYPE DragOver( 
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect) override;
    
    virtual HRESULT STDMETHODCALLTYPE DragLeave( void) override;
    
    virtual HRESULT STDMETHODCALLTYPE Drop( 
        /* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
        /* [in] */ DWORD grfKeyState,
        /* [in] */ POINTL pt,
        /* [out][in] */ __RPC__inout DWORD *pdwEffect) override;
};

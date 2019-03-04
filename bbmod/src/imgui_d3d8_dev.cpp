#include "imgui_d3d8_dev.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui_impl_d3d8.h"
#include "log.h"
#include "luastate.h"
#include "lua_hooks.h"
#include "lua_psolib.h"

STDMETHODIMP ImguiD3D8Device::QueryInterface(REFIID riid, void ** ppvObj)
{
    return device->QueryInterface(riid, ppvObj);
}

STDMETHODIMP_(ULONG) ImguiD3D8Device::AddRef(void)
{
    return device->AddRef();
}

STDMETHODIMP_(ULONG) ImguiD3D8Device::Release(void)
{
    return device->Release();
}

STDMETHODIMP ImguiD3D8Device::TestCooperativeLevel(void)
{
    return device->TestCooperativeLevel();
}

STDMETHODIMP_(UINT) ImguiD3D8Device::GetAvailableTextureMem(void)
{
    return device->GetAvailableTextureMem();
}

STDMETHODIMP ImguiD3D8Device::ResourceManagerDiscardBytes(DWORD Bytes)
{
    return device->ResourceManagerDiscardBytes(Bytes);
}

STDMETHODIMP ImguiD3D8Device::GetDirect3D(IDirect3D8 ** ppD3D8)
{
    return device->GetDirect3D(ppD3D8);
}

STDMETHODIMP ImguiD3D8Device::GetDeviceCaps(D3DCAPS8 * pCaps)
{
    return device->GetDeviceCaps(pCaps);
}

STDMETHODIMP ImguiD3D8Device::GetDisplayMode(D3DDISPLAYMODE * pMode)
{
    return device->GetDisplayMode(pMode);
}

STDMETHODIMP ImguiD3D8Device::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS * pParameters)
{
    return device->GetCreationParameters(pParameters);
}

STDMETHODIMP ImguiD3D8Device::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8 * pCursorBitmap)
{
    return device->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

STDMETHODIMP_(void) ImguiD3D8Device::SetCursorPosition(int X, int Y, DWORD Flags)
{
    device->SetCursorPosition(X, Y, Flags);
}

STDMETHODIMP_(BOOL) ImguiD3D8Device::ShowCursor(BOOL bShow)
{
    return device->ShowCursor(bShow);
}

STDMETHODIMP ImguiD3D8Device::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS * pPresentationParameters, IDirect3DSwapChain8 ** pSwapChain)
{
    return device->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

STDMETHODIMP ImguiD3D8Device::Reset(D3DPRESENT_PARAMETERS * pPresentationParameters)
{
    return device->Reset(pPresentationParameters);
}

STDMETHODIMP ImguiD3D8Device::Present(CONST RECT * pSourceRect, CONST RECT * pDestRect, HWND hDestWindowOverride, CONST RGNDATA * pDirtyRegion)
{
    FPUSTATE fpustate;
    psolua_store_fpu_state(fpustate);

    psolua_process_key_events();

    psoluah_Present();

    psolua_restore_fpu_state(fpustate);
    if (device->BeginScene() >= 0) {

        // Prevent imgui from asserting.
        while (GImGui->CurrentWindowStack.Size > 1) {
            g_log << "[assert avoided] Match your imgui.Begin's with imgui.End's!" << std::endl;
            ImGui::End();
        }

        ImGui::Render();

        device->EndScene();

    }

    ImGui_BetweenFrameChanges();

    auto ret = device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    ImGui_ImplD3D8_NewFrame();
    if (psolua_initialize_on_next_frame) {
        psolua_store_fpu_state(fpustate);
        psolua_initialize_state();
        psolua_restore_fpu_state(fpustate);
    }
    return ret;
}

STDMETHODIMP ImguiD3D8Device::GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8 ** ppBackBuffer)
{
    return device->GetBackBuffer(BackBuffer, Type, ppBackBuffer);
}

STDMETHODIMP ImguiD3D8Device::GetRasterStatus(D3DRASTER_STATUS * pRasterStatus)
{
    return device->GetRasterStatus(pRasterStatus);
}

STDMETHODIMP_(void) ImguiD3D8Device::SetGammaRamp(DWORD Flags, CONST D3DGAMMARAMP * pRamp)
{
    device->SetGammaRamp(Flags, pRamp);
}

STDMETHODIMP_(void) ImguiD3D8Device::GetGammaRamp(D3DGAMMARAMP * pRamp)
{
    device->GetGammaRamp(pRamp);
}

STDMETHODIMP ImguiD3D8Device::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8 ** ppTexture)
{
    return device->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture);
}

STDMETHODIMP ImguiD3D8Device::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8 ** ppVolumeTexture)
{
    return device->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture);
}

STDMETHODIMP ImguiD3D8Device::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8 ** ppCubeTexture)
{
    return device->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture);
}

STDMETHODIMP ImguiD3D8Device::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8 ** ppVertexBuffer)
{
    return device->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer);
}

STDMETHODIMP ImguiD3D8Device::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8 ** ppIndexBuffer)
{
    return device->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer);
}

STDMETHODIMP ImguiD3D8Device::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8 ** ppSurface)
{
    return device->CreateRenderTarget(Width, Height, Format, MultiSample, Lockable, ppSurface);
}

STDMETHODIMP ImguiD3D8Device::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8 ** ppSurface)
{
    return device->CreateDepthStencilSurface(Width, Height, Format, MultiSample, ppSurface);
}

STDMETHODIMP ImguiD3D8Device::CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8 ** ppSurface)
{
    return device->CreateImageSurface(Width, Height, Format, ppSurface);
}

STDMETHODIMP ImguiD3D8Device::CopyRects(IDirect3DSurface8 * pSourceSurface, CONST RECT * pSourceRectsArray, UINT cRects, IDirect3DSurface8 * pDestinationSurface, CONST POINT * pDestPointsArray)
{
    return device->CopyRects(pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray);
}

STDMETHODIMP ImguiD3D8Device::UpdateTexture(IDirect3DBaseTexture8 * pSourceTexture, IDirect3DBaseTexture8 * pDestinationTexture)
{
    return device->UpdateTexture(pSourceTexture, pDestinationTexture);
}

STDMETHODIMP ImguiD3D8Device::GetFrontBuffer(IDirect3DSurface8 * pDestSurface)
{
    return device->GetFrontBuffer(pDestSurface);
}

STDMETHODIMP ImguiD3D8Device::SetRenderTarget(IDirect3DSurface8 * pRenderTarget, IDirect3DSurface8 * pNewZStencil)
{
    return device->SetRenderTarget(pRenderTarget, pNewZStencil);
}

STDMETHODIMP ImguiD3D8Device::GetRenderTarget(IDirect3DSurface8 ** ppRenderTarget)
{
    return device->GetRenderTarget(ppRenderTarget);
}

STDMETHODIMP ImguiD3D8Device::GetDepthStencilSurface(IDirect3DSurface8 ** ppZStencilSurface)
{
    return device->GetDepthStencilSurface(ppZStencilSurface);
}

STDMETHODIMP ImguiD3D8Device::BeginScene(void)
{
    return device->BeginScene();
}

STDMETHODIMP ImguiD3D8Device::EndScene(void)
{
    return device->EndScene();
}

STDMETHODIMP ImguiD3D8Device::Clear(DWORD Count, CONST D3DRECT * pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    return device->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

STDMETHODIMP ImguiD3D8Device::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX * pMatrix)
{
    return device->SetTransform(State, pMatrix);
}

STDMETHODIMP ImguiD3D8Device::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX * pMatrix)
{
    return device->GetTransform(State, pMatrix);
}

STDMETHODIMP ImguiD3D8Device::MultiplyTransform(D3DTRANSFORMSTATETYPE TransformStateType, CONST D3DMATRIX * pMatrix)
{
    return device->MultiplyTransform(TransformStateType, pMatrix);
}

STDMETHODIMP ImguiD3D8Device::SetViewport(CONST D3DVIEWPORT8 * pViewport)
{
    return device->SetViewport(pViewport);
}

STDMETHODIMP ImguiD3D8Device::GetViewport(D3DVIEWPORT8 * pViewport)
{
    return device->GetViewport(pViewport);
}

STDMETHODIMP ImguiD3D8Device::SetMaterial(CONST D3DMATERIAL8 * pMaterial)
{
    return device->SetMaterial(pMaterial);
}

STDMETHODIMP ImguiD3D8Device::GetMaterial(D3DMATERIAL8 * pMaterial)
{
    return device->GetMaterial(pMaterial);
}

STDMETHODIMP ImguiD3D8Device::SetLight(DWORD Index, CONST D3DLIGHT8 * pLight)
{
    return device->SetLight(Index, pLight);
}

STDMETHODIMP ImguiD3D8Device::GetLight(DWORD Index, D3DLIGHT8 * pLight)
{
    return device->GetLight(Index, pLight);
}

STDMETHODIMP ImguiD3D8Device::LightEnable(DWORD Index, BOOL Enable)
{
    return device->LightEnable(Index, Enable);
}

STDMETHODIMP ImguiD3D8Device::GetLightEnable(DWORD Index, BOOL * pEnable)
{
    return device->GetLightEnable(Index, pEnable);
}

STDMETHODIMP ImguiD3D8Device::SetClipPlane(DWORD Index, CONST float * pPlane)
{
    return device->SetClipPlane(Index, pPlane);
}

STDMETHODIMP ImguiD3D8Device::GetClipPlane(DWORD Index, float * pPlane)
{
    return device->GetClipPlane(Index, pPlane);
}

STDMETHODIMP ImguiD3D8Device::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    return device->SetRenderState(State, Value);
}

STDMETHODIMP ImguiD3D8Device::GetRenderState(D3DRENDERSTATETYPE State, DWORD * pValue)
{
    return device->GetRenderState(State, pValue);
}

STDMETHODIMP ImguiD3D8Device::BeginStateBlock(void)
{
    return device->BeginStateBlock();
}

STDMETHODIMP ImguiD3D8Device::EndStateBlock(DWORD * pToken)
{
    return device->EndStateBlock(pToken);
}

STDMETHODIMP ImguiD3D8Device::ApplyStateBlock(DWORD Token)
{
    return device->ApplyStateBlock(Token);
}

STDMETHODIMP ImguiD3D8Device::CaptureStateBlock(DWORD Token)
{
    return device->CaptureStateBlock(Token);
}

STDMETHODIMP ImguiD3D8Device::DeleteStateBlock(DWORD Token)
{
    return device->DeleteStateBlock(Token);
}

STDMETHODIMP ImguiD3D8Device::CreateStateBlock(D3DSTATEBLOCKTYPE Type, DWORD * pToken)
{
    return device->CreateStateBlock(Type, pToken);
}

STDMETHODIMP ImguiD3D8Device::SetClipStatus(CONST D3DCLIPSTATUS8 * pClipStatus)
{
    return device->SetClipStatus(pClipStatus);
}

STDMETHODIMP ImguiD3D8Device::GetClipStatus(D3DCLIPSTATUS8 * pClipStatus)
{
    return device->GetClipStatus(pClipStatus);
}

STDMETHODIMP ImguiD3D8Device::GetTexture(DWORD Stage, IDirect3DBaseTexture8 ** ppTexture)
{
    return device->GetTexture(Stage, ppTexture);
}

STDMETHODIMP ImguiD3D8Device::SetTexture(DWORD Stage, IDirect3DBaseTexture8 * pTexture)
{
    return device->SetTexture(Stage, pTexture);
}

STDMETHODIMP ImguiD3D8Device::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD * pValue)
{
    return device->GetTextureStageState(Stage, Type, pValue);
}

STDMETHODIMP ImguiD3D8Device::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    return device->SetTextureStageState(Stage, Type, Value);
}

STDMETHODIMP ImguiD3D8Device::ValidateDevice(DWORD * pNumPasses)
{
    return device->ValidateDevice(pNumPasses);
}

STDMETHODIMP ImguiD3D8Device::GetInfo(DWORD DevInfoID, void * pDevInfoStruct, DWORD DevInfoStructSize)
{
    return device->GetInfo(DevInfoID, pDevInfoStruct, DevInfoStructSize);
}

STDMETHODIMP ImguiD3D8Device::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY * pEntries)
{
    return device->SetPaletteEntries(PaletteNumber, pEntries);
}

STDMETHODIMP ImguiD3D8Device::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY * pEntries)
{
    return device->GetPaletteEntries(PaletteNumber, pEntries);
}

STDMETHODIMP ImguiD3D8Device::SetCurrentTexturePalette(UINT PaletteNumber)
{
    return device->SetCurrentTexturePalette(PaletteNumber);
}

STDMETHODIMP ImguiD3D8Device::GetCurrentTexturePalette(UINT * PaletteNumber)
{
    return device->GetCurrentTexturePalette(PaletteNumber);
}

STDMETHODIMP ImguiD3D8Device::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    return device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

STDMETHODIMP ImguiD3D8Device::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    return device->DrawIndexedPrimitive(Type, minIndex, NumVertices, startIndex, primCount);
}

STDMETHODIMP ImguiD3D8Device::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

STDMETHODIMP ImguiD3D8Device::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return device->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertexIndices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

STDMETHODIMP ImguiD3D8Device::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8 * pDestBuffer, DWORD Flags)
{
    return device->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, Flags);
}

STDMETHODIMP ImguiD3D8Device::CreateVertexShader(CONST DWORD * pDeclaration, CONST DWORD * pFunction, DWORD * pHandle, DWORD Usage)
{
    return device->CreateVertexShader(pDeclaration, pFunction, pHandle, Usage);
}

STDMETHODIMP ImguiD3D8Device::SetVertexShader(DWORD Handle)
{
    return device->SetVertexShader(Handle);
}

STDMETHODIMP ImguiD3D8Device::GetVertexShader(DWORD * pHandle)
{
    return device->GetVertexShader(pHandle);
}

STDMETHODIMP ImguiD3D8Device::DeleteVertexShader(DWORD Handle)
{
    return device->DeleteVertexShader(Handle);
}

STDMETHODIMP ImguiD3D8Device::SetVertexShaderConstant(DWORD Register, CONST void * pConstantData, DWORD ConstantCount)
{
    return device->SetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

STDMETHODIMP ImguiD3D8Device::GetVertexShaderConstant(DWORD Register, void * pConstantData, DWORD ConstantCount)
{
    return device->GetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

STDMETHODIMP ImguiD3D8Device::GetVertexShaderDeclaration(DWORD Handle, void * pData, DWORD * pSizeOfData)
{
    return device->GetVertexShaderDeclaration(Handle, pData, pSizeOfData);
}

STDMETHODIMP ImguiD3D8Device::GetVertexShaderFunction(DWORD Handle, void * pData, DWORD * pSizeOfData)
{
    return device->GetVertexShaderFunction(Handle, pData, pSizeOfData);
}

STDMETHODIMP ImguiD3D8Device::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 * pStreamData, UINT Stride)
{
    return device->SetStreamSource(StreamNumber, pStreamData, Stride);
}

STDMETHODIMP ImguiD3D8Device::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 ** ppStreamData, UINT * pStride)
{
    return device->GetStreamSource(StreamNumber, ppStreamData, pStride);
}

STDMETHODIMP ImguiD3D8Device::SetIndices(IDirect3DIndexBuffer8 * pIndexData, UINT BaseVertexIndex)
{
    return device->SetIndices(pIndexData, BaseVertexIndex);
}

STDMETHODIMP ImguiD3D8Device::GetIndices(IDirect3DIndexBuffer8 ** ppIndexData, UINT * pBaseVertexIndex)
{
    return device->GetIndices(ppIndexData, pBaseVertexIndex);
}

STDMETHODIMP ImguiD3D8Device::CreatePixelShader(CONST DWORD * pFunction, DWORD * pHandle)
{
    return device->CreatePixelShader(pFunction, pHandle);
}

STDMETHODIMP ImguiD3D8Device::SetPixelShader(DWORD Handle)
{
    return device->SetPixelShader(Handle);
}

STDMETHODIMP ImguiD3D8Device::GetPixelShader(DWORD * pHandle)
{
    return device->GetPixelShader(pHandle);
}

STDMETHODIMP ImguiD3D8Device::DeletePixelShader(DWORD Handle)
{
    return device->DeletePixelShader(Handle);
}

STDMETHODIMP ImguiD3D8Device::SetPixelShaderConstant(DWORD Register, CONST void * pConstantData, DWORD ConstantCount)
{
    return device->SetPixelShaderConstant(Register, pConstantData, ConstantCount);
}

STDMETHODIMP ImguiD3D8Device::GetPixelShaderConstant(DWORD Register, void * pConstantData, DWORD ConstantCount)
{
    return device->GetPixelShaderConstant(Register, pConstantData, ConstantCount);
}

STDMETHODIMP ImguiD3D8Device::GetPixelShaderFunction(DWORD Handle, void * pData, DWORD * pSizeOfData)
{
    return device->GetPixelShaderFunction(Handle, pData, pSizeOfData);
}

STDMETHODIMP ImguiD3D8Device::DrawRectPatch(UINT Handle, CONST float * pNumSegs, CONST D3DRECTPATCH_INFO * pRectPatchInfo)
{
    return device->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

STDMETHODIMP ImguiD3D8Device::DrawTriPatch(UINT Handle, CONST float * pNumSegs, CONST D3DTRIPATCH_INFO * pTriPatchInfo)
{
    return device->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

STDMETHODIMP ImguiD3D8Device::DeletePatch(UINT Handle)
{
    return device->DeletePatch(Handle);
}

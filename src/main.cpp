#include <xtl.h>
#include "ui/StudioScene.h"
#include <cstdio>
using namespace soundstudio;
static void Trace(const char* name, HRESULT value) {
    char text[128]; sprintf_s(text, sizeof(text), "Coda: %s = %08X\n", name, value); OutputDebugString(text);
}
int __cdecl main() {
    MountSystemFont();
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return 1;
    D3DPRESENT_PARAMETERS pp; ZeroMemory(&pp, sizeof(pp));
    pp.BackBufferWidth = 1280; pp.BackBufferHeight = 720; pp.BackBufferFormat = D3DFMT_A8R8G8B8;
    pp.BackBufferCount = 1; pp.SwapEffect = D3DSWAPEFFECT_DISCARD; pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    pp.EnableAutoDepthStencil = TRUE; pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    IDirect3DDevice9* device = NULL;
    HRESULT hr = d3d->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device);
    if (FAILED(hr)) { d3d->Release(); return 2; }
    StudioApp app;
    hr = app.InitShared(device, &pp, XuiD3DXTextureLoader);
    Trace("InitShared", hr);
    if (SUCCEEDED(hr)) {
        // A bundled font can override the console font (also useful in an emulator).
        const wchar_t* font = GetFileAttributes("GAME:\\Media\\font.ttf") != (DWORD)-1
            ? L"file://GAME:/Media/font.ttf" : L"file://flash:/XenonJKLatin.xtt";
        hr = app.RegisterDefaultTypeface(L"Tahoma", font);
        Trace("RegisterDefaultTypeface", hr);
        if (SUCCEEDED(hr)) hr = app.LoadSkin(L"file://GAME:/Media/skin.xur");
        Trace("LoadSkin", hr);
        if (SUCCEEDED(hr)) hr = app.LoadFirstScene(L"", L"file://GAME:/Media/scene.xur", NULL);
        Trace("LoadFirstScene", hr);
        if (SUCCEEDED(hr)) {
            while (!exitRequested) {
                device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 13, 20, 28), 1.0f, 0);
                app.RunFrame(); app.Render(); XuiTimersRun(); device->Present(NULL, NULL, NULL, NULL);
            }
        }
        app.Uninit();
    }
    device->Release(); d3d->Release(); return FAILED(hr) ? 3 : 0;
}

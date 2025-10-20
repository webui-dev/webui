/*
  WebUI Library
  Win32 WebView2 C++ Implementation
  https://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2025 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#ifdef _WIN32

#include "win32_wv2.hpp"
#include "WebView2.h"
#include <cstdio>

#ifdef _MSC_VER
    // MSVC: Use Windows Runtime Library
    #include <wrl.h>
    using namespace Microsoft::WRL;
#else
    // MinGW: Define our own ComPtr template
    template<typename T>
    class ComPtr {
        T* ptr;
    public:
        ComPtr() : ptr(nullptr) {}
        ~ComPtr() { Reset(); }
        ComPtr(const ComPtr&) = delete;
        ComPtr& operator=(const ComPtr&) = delete;

        void Reset() {
            if (ptr) {
                ptr->Release();
                ptr = nullptr;
            }
        }

        T* Get() const { return ptr; }
        T** GetAddressOf() { Reset(); return &ptr; }
        T* operator->() const { return ptr; }
        operator bool() const { return ptr != nullptr; }

        ComPtr& operator=(T* p) {
            Reset();
            ptr = p;
            if (ptr) ptr->AddRef();
            return *this;
        }
    };
#endif

class WebView2Instance {
public:
    ComPtr<ICoreWebView2Environment> webviewEnvironment;
    ComPtr<ICoreWebView2Controller> webviewController;
    ComPtr<ICoreWebView2> webviewWindow;
    HWND hwnd;
    wchar_t* url;
    bool navigate, size, position, stop;
    int width, height, x, y;

    WebView2Instance()
        : hwnd(nullptr), url(nullptr), navigate(false), size(false)
        , position(false), stop(false), width(800), height(600), x(0), y(0) {}

    ~WebView2Instance() {
        if (url) free(url);
    }
};

#ifdef _MSC_VER
    // MSVC: Use WRL RuntimeClass
    class TitleChangedHandler : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ICoreWebView2DocumentTitleChangedEventHandler> {
        WebView2Instance* instance;
    public:
        TitleChangedHandler(WebView2Instance* inst) : instance(inst) {}
        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, IUnknown* args) override {
            (void)args;
            if (sender && instance->hwnd) {
                LPWSTR newTitle = nullptr;
                sender->get_DocumentTitle(&newTitle);
                if (newTitle) {
                    SetWindowTextW(instance->hwnd, newTitle);
                    CoTaskMemFree(newTitle);
                }
            }
            return S_OK;
        }
    };
#else
    // MinGW: Manual COM implementation
    class TitleChangedHandler : public ICoreWebView2DocumentTitleChangedEventHandler {
        ULONG refCount;
        WebView2Instance* instance;
    public:
        TitleChangedHandler(WebView2Instance* inst) : refCount(1), instance(inst) {}
        virtual ~TitleChangedHandler() = default;

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
            if (!ppvObject) return E_POINTER;
            *ppvObject = nullptr;
            if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ICoreWebView2DocumentTitleChangedEventHandler)) {
                *ppvObject = static_cast<ICoreWebView2DocumentTitleChangedEventHandler*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE AddRef() override {
            return ++refCount;
        }

        ULONG STDMETHODCALLTYPE Release() override {
            ULONG count = --refCount;
            if (count == 0) delete this;
            return count;
        }

        HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2* sender, IUnknown* args) override {
            (void)args;
            if (sender && instance->hwnd) {
                LPWSTR newTitle = nullptr;
                sender->get_DocumentTitle(&newTitle);
                if (newTitle) {
                    SetWindowTextW(instance->hwnd, newTitle);
                    CoTaskMemFree(newTitle);
                }
            }
            return S_OK;
        }
    };
#endif

#ifdef _MSC_VER
    class ControllerCompletedHandler : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ICoreWebView2CreateCoreWebView2ControllerCompletedHandler> {
        WebView2Instance* instance;
    public:
        ControllerCompletedHandler(WebView2Instance* inst) : instance(inst) {}
        HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Controller* controller) override {
            if (SUCCEEDED(result) && controller) {
                instance->webviewController = controller;
                controller->get_CoreWebView2(&instance->webviewWindow);

                ComPtr<ICoreWebView2Settings> settings;
                if (SUCCEEDED(instance->webviewWindow->get_Settings(&settings))) {
                    settings->put_IsScriptEnabled(TRUE);
                    settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                    settings->put_IsWebMessageEnabled(TRUE);
                    #ifdef WEBUI_LOG
                    settings->put_AreDevToolsEnabled(TRUE);
                    #else
                    settings->put_AreDevToolsEnabled(FALSE);
                    #endif
                }

                RECT bounds = {0, 0, instance->width, instance->height};
                controller->put_Bounds(bounds);

                auto titleHandler = Make<TitleChangedHandler>(instance);
                EventRegistrationToken token;
                instance->webviewWindow->add_DocumentTitleChanged(titleHandler.Get(), &token);

                if (instance->url) {
                    instance->webviewWindow->Navigate(instance->url);
                }
            }
            return S_OK;
        }
    };
#else
    class ControllerCompletedHandler : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
        ULONG refCount;
        WebView2Instance* instance;
    public:
        ControllerCompletedHandler(WebView2Instance* inst) : refCount(1), instance(inst) {}
        virtual ~ControllerCompletedHandler() = default;

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
            if (!ppvObject) return E_POINTER;
            *ppvObject = nullptr;
            if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ICoreWebView2CreateCoreWebView2ControllerCompletedHandler)) {
                *ppvObject = static_cast<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE AddRef() override {
            return ++refCount;
        }

        ULONG STDMETHODCALLTYPE Release() override {
            ULONG count = --refCount;
            if (count == 0) delete this;
            return count;
        }

        HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Controller* controller) override {
            if (SUCCEEDED(result) && controller) {
                instance->webviewController = controller;
                controller->get_CoreWebView2(instance->webviewWindow.GetAddressOf());

                ICoreWebView2Settings* settings = nullptr;
                if (SUCCEEDED(instance->webviewWindow->get_Settings(&settings)) && settings) {
                    settings->put_IsScriptEnabled(TRUE);
                    settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                    settings->put_IsWebMessageEnabled(TRUE);
                    #ifdef WEBUI_LOG
                    settings->put_AreDevToolsEnabled(TRUE);
                    #else
                    settings->put_AreDevToolsEnabled(FALSE);
                    #endif
                    settings->Release();
                }

                RECT bounds = {0, 0, instance->width, instance->height};
                controller->put_Bounds(bounds);

                TitleChangedHandler* titleHandler = new TitleChangedHandler(instance);
                EventRegistrationToken token;
                instance->webviewWindow->add_DocumentTitleChanged(titleHandler, &token);

                if (instance->url) {
                    instance->webviewWindow->Navigate(instance->url);
                }
            }
            return S_OK;
        }
    };
#endif

#ifdef _MSC_VER
    class EnvironmentCompletedHandler : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler> {
        WebView2Instance* instance;
    public:
        EnvironmentCompletedHandler(WebView2Instance* inst) : instance(inst) {}
        HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Environment* env) override {
            if (SUCCEEDED(result) && env) {
                instance->webviewEnvironment = env;
                auto controllerHandler = Make<ControllerCompletedHandler>(instance);
                env->CreateCoreWebView2Controller(instance->hwnd, controllerHandler.Get());
            }
            return S_OK;
        }
    };
#else
    class EnvironmentCompletedHandler : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
        ULONG refCount;
        WebView2Instance* instance;
    public:
        EnvironmentCompletedHandler(WebView2Instance* inst) : refCount(1), instance(inst) {}
        virtual ~EnvironmentCompletedHandler() = default;

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override {
            if (!ppvObject) return E_POINTER;
            *ppvObject = nullptr;
            if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler)) {
                *ppvObject = static_cast<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        ULONG STDMETHODCALLTYPE AddRef() override {
            return ++refCount;
        }

        ULONG STDMETHODCALLTYPE Release() override {
            ULONG count = --refCount;
            if (count == 0) delete this;
            return count;
        }

        HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Environment* env) override {
            if (SUCCEEDED(result) && env) {
                instance->webviewEnvironment = env;
                ControllerCompletedHandler* controllerHandler = new ControllerCompletedHandler(instance);
                env->CreateCoreWebView2Controller(instance->hwnd, controllerHandler);
            }
            return S_OK;
        }
    };
#endif

extern "C" {

_webui_win32_wv2_handle _webui_win32_wv2_create(void) {
    return static_cast<_webui_win32_wv2_handle>(new WebView2Instance());
}

void _webui_win32_wv2_free(_webui_win32_wv2_handle handle) {
    if (!handle) return;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    if (instance->webviewWindow) instance->webviewWindow.Reset();
    if (instance->webviewController) instance->webviewController.Reset();
    if (instance->webviewEnvironment) instance->webviewEnvironment.Reset();
    delete instance;
}

bool _webui_win32_wv2_navigate(_webui_win32_wv2_handle handle, wchar_t* url) {
    if (!handle) return false;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    if (instance->webviewWindow && url) {
        return SUCCEEDED(instance->webviewWindow->Navigate(url));
    }
    return false;
}

bool _webui_win32_wv2_set_position(_webui_win32_wv2_handle handle, int x, int y) {
    if (!handle) return false;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    return instance->hwnd && SetWindowPos(instance->hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

bool _webui_win32_wv2_set_size(_webui_win32_wv2_handle handle, int width, int height) {
    if (!handle) return false;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    return instance->hwnd && SetWindowPos(instance->hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOREPOSITION);
}

bool _webui_win32_wv2_maximize(_webui_win32_wv2_handle handle) {
    if (!handle) return false;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    return instance->hwnd && ShowWindow(instance->hwnd, SW_MAXIMIZE);
}

bool _webui_win32_wv2_minimize(_webui_win32_wv2_handle handle) {
    if (!handle) return false;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    return instance->hwnd && ShowWindow(instance->hwnd, SW_MINIMIZE);
}

HWND _webui_win32_wv2_get_hwnd(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->hwnd : nullptr;
}

void _webui_win32_wv2_set_hwnd(_webui_win32_wv2_handle handle, HWND hwnd) {
    if (handle) static_cast<WebView2Instance*>(handle)->hwnd = hwnd;
}

void _webui_win32_wv2_set_stop(_webui_win32_wv2_handle handle, bool stop) {
    if (handle) static_cast<WebView2Instance*>(handle)->stop = stop;
}

bool _webui_win32_wv2_get_stop(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->stop : false;
}

void _webui_win32_wv2_set_navigate_flag(_webui_win32_wv2_handle handle, bool navigate) {
    if (handle) static_cast<WebView2Instance*>(handle)->navigate = navigate;
}

void _webui_win32_wv2_set_size_flag(_webui_win32_wv2_handle handle, bool size) {
    if (handle) static_cast<WebView2Instance*>(handle)->size = size;
}

void _webui_win32_wv2_set_position_flag(_webui_win32_wv2_handle handle, bool position) {
    if (handle) static_cast<WebView2Instance*>(handle)->position = position;
}

bool _webui_win32_wv2_get_navigate_flag(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->navigate : false;
}

bool _webui_win32_wv2_get_size_flag(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->size : false;
}

bool _webui_win32_wv2_get_position_flag(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->position : false;
}

void _webui_win32_wv2_set_url(_webui_win32_wv2_handle handle, wchar_t* url) {
    if (!handle) return;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    if (instance->url) free(instance->url);
    if (url) {
        size_t len = wcslen(url) + 1;
        instance->url = (wchar_t*)malloc(len * sizeof(wchar_t));
        if (instance->url) wcscpy_s(instance->url, len, url);
    } else {
        instance->url = nullptr;
    }
}

wchar_t* _webui_win32_wv2_get_url(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->url : nullptr;
}

void _webui_win32_wv2_set_dimensions(_webui_win32_wv2_handle handle, int x, int y, int width, int height) {
    if (!handle) return;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    instance->x = x;
    instance->y = y;
    instance->width = width;
    instance->height = height;
}

void _webui_win32_wv2_get_dimensions(_webui_win32_wv2_handle handle, int* x, int* y, int* width, int* height) {
    if (!handle) return;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);
    if (x) *x = instance->x;
    if (y) *y = instance->y;
    if (width) *width = instance->width;
    if (height) *height = instance->height;
}

bool _webui_win32_wv2_create_environment(_webui_win32_wv2_handle handle, wchar_t* cacheFolder) {
    if (!handle) return false;
    WebView2Instance* instance = static_cast<WebView2Instance*>(handle);

    _wputenv(L"WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS=--enable-features=msWebView2EnableDraggableRegions");

    typedef HRESULT (__stdcall *CreateCoreWebView2EnvironmentWithOptionsFunc)(
        PCWSTR, PCWSTR, ICoreWebView2EnvironmentOptions*,
        ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*);

    HMODULE webviewLib = LoadLibraryA("WebView2Loader.dll");
    if (!webviewLib) return false;

    CreateCoreWebView2EnvironmentWithOptionsFunc createEnv =
        (CreateCoreWebView2EnvironmentWithOptionsFunc)GetProcAddress(webviewLib, "CreateCoreWebView2EnvironmentWithOptions");
    if (!createEnv) {
        FreeLibrary(webviewLib);
        return false;
    }

    #ifdef _MSC_VER
        auto environmentHandler = Make<EnvironmentCompletedHandler>(instance);
        if (!environmentHandler) {
            FreeLibrary(webviewLib);
            return false;
        }
        HRESULT hr = createEnv(NULL, cacheFolder, NULL, environmentHandler.Get());
    #else
        EnvironmentCompletedHandler* environmentHandler = new EnvironmentCompletedHandler(instance);
        if (!environmentHandler) {
            FreeLibrary(webviewLib);
            return false;
        }
        HRESULT hr = createEnv(NULL, cacheFolder, NULL, environmentHandler);
    #endif

    return SUCCEEDED(hr);
}

void* _webui_win32_wv2_get_controller(_webui_win32_wv2_handle handle) {
    return handle ? static_cast<WebView2Instance*>(handle)->webviewController.Get() : nullptr;
}

}

#endif

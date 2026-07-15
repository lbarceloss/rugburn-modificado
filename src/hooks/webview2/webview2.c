
#include "webview2.h"


typedef struct ICoreWebView2 ICoreWebView2;
typedef struct ICoreWebView2Controller ICoreWebView2Controller;
typedef struct ICoreWebView2Environment ICoreWebView2Environment;
typedef struct EnvHandler EnvHandler;
typedef struct CtrlHandler CtrlHandler;

typedef struct ICoreWebView2Vtbl {
    HRESULT(STDCALL *QueryInterface)(ICoreWebView2 *, REFIID, void **);
    ULONG(STDCALL *AddRef)(ICoreWebView2 *);
    ULONG(STDCALL *Release)(ICoreWebView2 *);
    PVOID get_Settings;
    PVOID get_Source;
    HRESULT(STDCALL *Navigate)(ICoreWebView2 *, LPCWSTR);
} ICoreWebView2Vtbl;
struct ICoreWebView2 {
    CONST_VTBL ICoreWebView2Vtbl *lpVtbl;
};

typedef struct ICoreWebView2ControllerVtbl {
    HRESULT(STDCALL *QueryInterface)(ICoreWebView2Controller *, REFIID, void **);
    ULONG(STDCALL *AddRef)(ICoreWebView2Controller *);
    ULONG(STDCALL *Release)(ICoreWebView2Controller *);
    PVOID get_IsVisible;
    HRESULT(STDCALL *put_IsVisible)(ICoreWebView2Controller *, BOOL);
    PVOID get_Bounds;
    HRESULT(STDCALL *put_Bounds)(ICoreWebView2Controller *, RECT);
    PVOID get_ZoomFactor;
    PVOID put_ZoomFactor;
    PVOID add_ZoomFactorChanged;
    PVOID remove_ZoomFactorChanged;
    PVOID SetBoundsAndZoomFactor;
    PVOID MoveFocus;
    PVOID add_MoveFocusRequested;
    PVOID remove_MoveFocusRequested;
    PVOID add_GotFocus;
    PVOID remove_GotFocus;
    PVOID add_LostFocus;
    PVOID remove_LostFocus;
    PVOID add_AcceleratorKeyPressed;
    PVOID remove_AcceleratorKeyPressed;
    PVOID get_ParentWindow;
    HRESULT(STDCALL *put_ParentWindow)(ICoreWebView2Controller *, HWND);
    HRESULT(STDCALL *NotifyParentWindowPositionChanged)(ICoreWebView2Controller *);
    HRESULT(STDCALL *Close)(ICoreWebView2Controller *);
    HRESULT(STDCALL *get_CoreWebView2)(ICoreWebView2Controller *, ICoreWebView2 **);
} ICoreWebView2ControllerVtbl;
struct ICoreWebView2Controller {
    CONST_VTBL ICoreWebView2ControllerVtbl *lpVtbl;
};

typedef struct ICoreWebView2EnvironmentVtbl {
    HRESULT(STDCALL *QueryInterface)(ICoreWebView2Environment *, REFIID, void **);
    ULONG(STDCALL *AddRef)(ICoreWebView2Environment *);
    ULONG(STDCALL *Release)(ICoreWebView2Environment *);
    HRESULT(STDCALL *CreateCoreWebView2Controller)(ICoreWebView2Environment *, HWND,
                                                   CtrlHandler *);
} ICoreWebView2EnvironmentVtbl;
struct ICoreWebView2Environment {
    CONST_VTBL ICoreWebView2EnvironmentVtbl *lpVtbl;
};

typedef struct EnvHandlerVtbl {
    HRESULT(STDCALL *QueryInterface)(EnvHandler *, REFIID, void **);
    ULONG(STDCALL *AddRef)(EnvHandler *);
    ULONG(STDCALL *Release)(EnvHandler *);
    HRESULT(STDCALL *Invoke)(EnvHandler *, HRESULT, ICoreWebView2Environment *);
} EnvHandlerVtbl;
struct EnvHandler {
    CONST_VTBL EnvHandlerVtbl *lpVtbl;
};

typedef struct CtrlHandlerVtbl {
    HRESULT(STDCALL *QueryInterface)(CtrlHandler *, REFIID, void **);
    ULONG(STDCALL *AddRef)(CtrlHandler *);
    ULONG(STDCALL *Release)(CtrlHandler *);
    HRESULT(STDCALL *Invoke)(CtrlHandler *, HRESULT, ICoreWebView2Controller *);
} CtrlHandlerVtbl;
struct CtrlHandler {
    CONST_VTBL CtrlHandlerVtbl *lpVtbl;
};

typedef HRESULT(STDCALL *PFNCREATEWEBVIEW2ENV)(PCWSTR browserExecutableFolder,
                                               PCWSTR userDataFolder, PVOID environmentOptions,
                                               EnvHandler *environmentCreatedHandler);


static PFNCREATEWEBVIEW2ENV pCreateEnv = NULL;
static BOOL bInitTried = FALSE;

static EnvHandlerVtbl g_envVtbl;
static CtrlHandlerVtbl g_ctrlVtbl;
static EnvHandler g_envHandler;
static CtrlHandler g_ctrlHandler;

static ICoreWebView2Controller *g_controller = NULL;
static ICoreWebView2 *g_webview = NULL;
static HWND g_host = NULL;
static BOOL g_creating = FALSE;
static WCHAR g_url[2048];
static WCHAR g_dataFolder[MAX_PATH];

static HWND g_ownWnd = NULL;
static HWND g_targetWnd = NULL;
static HWND g_pangyaTop = NULL;
static ATOM g_wndClass = 0;
static BOOL g_escWasPressed = FALSE;
static BOOL g_active = FALSE;
static DWORD g_graceUntil = 0;
static DWORD g_suppressOpenUntil = 0;
static volatile BOOL g_wantClose = FALSE;
static HWND g_closeBtn = NULL;
static HWND g_editWnd = NULL;
static HWND g_goWnd = NULL;
static WNDPROC g_oldEditProc = NULL;
static volatile LONG g_suppress = 0;
static volatile HWND g_suppressWnd = NULL;


#define WV2_W 1600
#define WV2_H 900
#define WV2_BAR_H 40
#define WV2_ID_EDIT 1001
#define WV2_ID_GO   1002

static VOID CenteredRect(RECT *rc) {
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    int x = (sw - WV2_W) / 2;
    int y = (sh - WV2_H) / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    rc->left = x;
    rc->top = y;
    rc->right = x + WV2_W;
    rc->bottom = y + WV2_H;
}

static VOID ApplyBounds(VOID) {
    RECT rc, cr;
    if (g_ownWnd == NULL) {
        return;
    }
    CenteredRect(&rc);
    SetWindowPos(g_ownWnd, HWND_TOPMOST, rc.left, rc.top, WV2_W, WV2_H, SWP_NOACTIVATE);
    if (g_controller != NULL && GetClientRect(g_ownWnd, &cr)) {
        cr.top += WV2_BAR_H;
        g_controller->lpVtbl->put_Bounds(g_controller, cr);
    }
}

static BOOL StrHasSchemeW(LPCWSTR s) {
    int i;
    for (i = 0; s[i]; ++i) {
        if (s[i] == L' ') return FALSE;
        if (s[i] == L':' && s[i + 1] == L'/' && s[i + 2] == L'/') return TRUE;
    }
    return FALSE;
}

static VOID NavigateFromEdit(VOID) {
    WCHAR buf[2040];
    WCHAR url[2060];
    if (g_editWnd == NULL || g_webview == NULL) {
        return;
    }
    buf[0] = 0;
    if (GetWindowTextW(g_editWnd, buf, 2040) == 0 || buf[0] == 0) {
        return;
    }
    if (StrHasSchemeW(buf)) {
        Log("[webview2] barra -> %S\r\n", buf);
        g_webview->lpVtbl->Navigate(g_webview, buf);
    } else {
        lstrcpyW(url, L"https://");
        lstrcatW(url, buf);
        Log("[webview2] barra -> %S\r\n", url);
        g_webview->lpVtbl->Navigate(g_webview, url);
    }
}

static LRESULT CALLBACK EditProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if ((msg == WM_KEYDOWN || msg == WM_CHAR) && wp == VK_RETURN) {
        if (msg == WM_KEYDOWN) NavigateFromEdit();
        return 0;
    }
    return CallWindowProcW(g_oldEditProc, h, msg, wp, lp);
}

static LRESULT CALLBACK WV2WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CLOSE) {
        g_wantClose = TRUE;
        return 0;
    }
    if (msg == WM_COMMAND && LOWORD(wp) == WV2_ID_GO && HIWORD(wp) == BN_CLICKED) {
        NavigateFromEdit();
        return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static BOOL EnsureOwnWindow(VOID) {
    WNDCLASSEXW wc;
    RECT rc;
    HINSTANCE hInst = GetModuleHandleW(NULL);

    if (g_ownWnd != NULL) {
        return TRUE;
    }
    if (g_wndClass == 0) {
        wc.cbSize = sizeof(wc);
        wc.style = 0;
        wc.lpfnWndProc = WV2WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInst;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursorW(NULL, (LPCWSTR)0x7F00 );
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"RugburnWV2Host";
        wc.hIconSm = NULL;
        g_wndClass = RegisterClassExW(&wc);
        if (g_wndClass == 0) {
            Log("[webview2] RegisterClassExW falhou err=%u\r\n", GetLastError());
            return FALSE;
        }
    }
    CenteredRect(&rc);
    g_ownWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"RugburnWV2Host", L"",
                               WS_POPUP, rc.left, rc.top, WV2_W, WV2_H, NULL, NULL, hInst, NULL);
    if (g_ownWnd == NULL) {
        Log("[webview2] CreateWindowExW falhou err=%u\r\n", GetLastError());
        return FALSE;
    }
    g_host = g_ownWnd;

    g_editWnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", g_url,
                                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 4, 6, WV2_W - 80,
                                WV2_BAR_H - 12, g_ownWnd, (HMENU)WV2_ID_EDIT, hInst, NULL);
    g_goWnd = CreateWindowExW(0, L"BUTTON", L"Ir", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              WV2_W - 72, 4, 64, WV2_BAR_H - 8, g_ownWnd, (HMENU)WV2_ID_GO, hInst,
                              NULL);
    if (g_editWnd != NULL) {
        g_oldEditProc = (WNDPROC)(LONG_PTR)SetWindowLongPtrW(g_editWnd, GWLP_WNDPROC,
                                                             (LONG_PTR)EditProc);
    }

    Log("[webview2] janela propria criada: 0x%p (edit=0x%p go=0x%p)\r\n", g_ownWnd, g_editWnd,
        g_goWnd);
    return TRUE;
}

static VOID HideOverlay(VOID) {
    g_targetWnd = NULL;
    if (g_ownWnd != NULL && IsWindowVisible(g_ownWnd)) {
        ShowWindow(g_ownWnd, SW_HIDE);
    }
    if (g_controller != NULL) {
        g_controller->lpVtbl->put_IsVisible(g_controller, FALSE);
    }
    if (g_webview != NULL) {
        g_webview->lpVtbl->Navigate(g_webview, L"about:blank");
    }
    if (g_pangyaTop != NULL && IsWindow(g_pangyaTop)) {
        SetForegroundWindow(g_pangyaTop);
        SetFocus(g_pangyaTop);
    }
}

static VOID ClickGameClose(VOID) {
    if (g_closeBtn != NULL && IsWindow(g_closeBtn)) {
        Log("[watcher] clicando closebtn 0x%p (voltar ao jogo)\r\n", g_closeBtn);
        PostMessageW(g_closeBtn, WM_LBUTTONDOWN, MK_LBUTTON, 0);
        PostMessageW(g_closeBtn, WM_LBUTTONUP, 0, 0);
        SendMessageW(g_closeBtn, BM_CLICK, 0, 0);
    } else if (g_pangyaTop != NULL && IsWindow(g_pangyaTop)) {
        Log("[watcher] closebtn nao achado; fallback ESC pra janela do jogo\r\n");
        PostMessageW(g_pangyaTop, WM_KEYDOWN, VK_ESCAPE, 0);
        PostMessageW(g_pangyaTop, WM_KEYUP, VK_ESCAPE, 0);
    }
}

static VOID CloseOverlay(BOOL notifyGame) {
    g_active = FALSE;
    g_targetWnd = NULL;
    g_suppressOpenUntil = GetTickCount() + 1500;
    HideOverlay();
    if (g_suppress) {
        g_suppress = 0;
        g_suppressWnd = NULL;
    } else if (notifyGame) {
        ClickGameClose();
    }
}


static HRESULT STDCALL EnvQI(EnvHandler *This, REFIID riid, void **ppv) {
    (void)riid;
    *ppv = This;
    return S_OK;
}
static ULONG STDCALL EnvAddRef(EnvHandler *This) {
    (void)This;
    return 1;
}
static ULONG STDCALL EnvRelease(EnvHandler *This) {
    (void)This;
    return 1;
}

static HRESULT STDCALL CtrlQI(CtrlHandler *This, REFIID riid, void **ppv) {
    (void)riid;
    *ppv = This;
    return S_OK;
}
static ULONG STDCALL CtrlAddRef(CtrlHandler *This) {
    (void)This;
    return 1;
}
static ULONG STDCALL CtrlRelease(CtrlHandler *This) {
    (void)This;
    return 1;
}

static HRESULT STDCALL CtrlInvoke(CtrlHandler *This, HRESULT code,
                                  ICoreWebView2Controller *controller) {
    (void)This;
    g_creating = FALSE;

    if (code < 0 || controller == NULL) {
        Log("[webview2] falha ao criar controller: 0x%08x\r\n", code);
        return S_OK;
    }

    g_controller = controller;
    controller->lpVtbl->AddRef(controller);

    if (controller->lpVtbl->get_CoreWebView2(controller, &g_webview) < 0 || g_webview == NULL) {
        Log("[webview2] get_CoreWebView2 falhou\r\n");
        return S_OK;
    }

    ApplyBounds();
    g_controller->lpVtbl->put_IsVisible(g_controller, TRUE);
    Log("[webview2] navegando -> %S\r\n", g_url);
    g_webview->lpVtbl->Navigate(g_webview, g_url);
    return S_OK;
}

static HRESULT STDCALL EnvInvoke(EnvHandler *This, HRESULT code, ICoreWebView2Environment *env) {
    HRESULT hr;
    (void)This;

    if (code < 0 || env == NULL) {
        Log("[webview2] falha ao criar ambiente: 0x%08x\r\n", code);
        g_creating = FALSE;
        return S_OK;
    }

    hr = env->lpVtbl->CreateCoreWebView2Controller(env, g_host, &g_ctrlHandler);
    if (hr < 0) {
        Log("[webview2] CreateCoreWebView2Controller falhou: 0x%08x\r\n", hr);
        g_creating = FALSE;
    }
    return S_OK;
}

static BOOL EnsureLoader(VOID) {
    HMODULE hLoader;
    DWORD n;

    if (bInitTried) {
        return pCreateEnv != NULL;
    }
    bInitTried = TRUE;

    hLoader = LoadLib("WebView2Loader.dll");
    if (hLoader == NULL) {
        Log("[webview2] WebView2Loader.dll nao encontrado (copie ao lado do ijl15.dll)\r\n");
        return FALSE;
    }
    pCreateEnv =
        (PFNCREATEWEBVIEW2ENV)GetProc(hLoader, "CreateCoreWebView2EnvironmentWithOptions");
    if (pCreateEnv == NULL) {
        Log("[webview2] export CreateCoreWebView2EnvironmentWithOptions ausente\r\n");
        return FALSE;
    }

    g_envVtbl.QueryInterface = EnvQI;
    g_envVtbl.AddRef = EnvAddRef;
    g_envVtbl.Release = EnvRelease;
    g_envVtbl.Invoke = EnvInvoke;
    g_envHandler.lpVtbl = &g_envVtbl;

    g_ctrlVtbl.QueryInterface = CtrlQI;
    g_ctrlVtbl.AddRef = CtrlAddRef;
    g_ctrlVtbl.Release = CtrlRelease;
    g_ctrlVtbl.Invoke = CtrlInvoke;
    g_ctrlHandler.lpVtbl = &g_ctrlVtbl;

    n = GetEnvironmentVariableW(L"LOCALAPPDATA", g_dataFolder, MAX_PATH);
    if (n == 0 || n >= MAX_PATH - 20) {
        g_dataFolder[0] = 0;
    } else {
        lstrcatW(g_dataFolder, L"\\RugburnWebView2");
    }

    Log("[webview2] loader OK\r\n");
    return TRUE;
}


BOOL OpenWebView2(HWND targetWindow, LPCWSTR urlW) {
    HRESULT hr;

    if (targetWindow == NULL || urlW == NULL) {
        return FALSE;
    }

    lstrcpynW(g_url, urlW, 2048);
    g_targetWnd = targetWindow;

    if (!EnsureOwnWindow()) {
        return FALSE;
    }

    ShowWindow(g_ownWnd, SW_SHOW);
    SetForegroundWindow(g_ownWnd);
    SetFocus(g_ownWnd);

    if (g_controller != NULL) {
        ApplyBounds();
        g_controller->lpVtbl->put_IsVisible(g_controller, TRUE);
        if (g_webview != NULL) {
            g_webview->lpVtbl->Navigate(g_webview, g_url);
        }
        return TRUE;
    }

    if (g_creating) {
        return TRUE;
    }
    if (!EnsureLoader()) {
        return FALSE;
    }

    ApplyBounds();
    g_creating = TRUE;
    hr = pCreateEnv(NULL, g_dataFolder[0] ? g_dataFolder : NULL, NULL, &g_envHandler);
    if (hr < 0) {
        Log("[webview2] CreateCoreWebView2EnvironmentWithOptions falhou: 0x%08x\r\n", hr);
        g_creating = FALSE;
        return FALSE;
    }

    Log("[webview2] criando ambiente (janela propria=0x%p, alvo=0x%p)...\r\n", g_ownWnd,
        targetWindow);
    return TRUE;
}


#define WV2_WATCH_URL L"https://www.youtube.com/"

#define WV2_WATCH_MAX_KNOWN 64
static HWND g_knownTops[WV2_WATCH_MAX_KNOWN];
static int  g_knownTopsCount = 0;
static BOOL g_serverFoundPass = FALSE;
static int  g_emptyPasses = 0;
static DWORD g_pid = 0;

VOID Wv2BeginCookieSuppressed(HWND gameWnd) {
    g_suppressWnd = gameWnd;
    if (gameWnd != NULL) {
        g_pangyaTop = gameWnd;
    }
    g_suppress = 1;
}
BOOL Wv2CookieSuppressActive(VOID) {
    return g_suppress != 0;
}

static BOOL IsBrowserClass(LPCSTR className) {
    if (lstrcmpiA(className, "Internet Explorer_Server") == 0) return TRUE;
    if (lstrcmpiA(className, "Shell DocObject View") == 0) return TRUE;
    if (lstrcmpiA(className, "Shell Embedding") == 0) return TRUE;
    return FALSE;
}

static BOOL IsHideableBrowserClass(LPCSTR className) {
    if (IsBrowserClass(className)) return TRUE;
    if (lstrcmpiA(className, "Internet Explorer_Hidden") == 0) return TRUE;
    return FALSE;
}

static HWND g_visBrowser = NULL;
static BOOL g_anyBrowser = FALSE;

static VOID HideBrowserRegion(HWND h) {
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    SetWindowRgn(h, rgn, TRUE);
}

static BOOL CALLBACK EnumChildSearchProc(HWND hChild, LPARAM lParam) {
    CHAR cls[64];
    (void)lParam;
    cls[0] = 0;
    GetClassNameA(hChild, cls, sizeof(cls));
    if (IsBrowserClass(cls)) {
        RECT rc;
        int w = 0, h = 0;
        if (GetWindowRect(hChild, &rc)) { w = rc.right - rc.left; h = rc.bottom - rc.top; }
        if (IsWindowVisible(hChild)) {
            g_anyBrowser = TRUE;
            if (g_visBrowser == NULL && w > 100 && h > 100) {
                g_visBrowser = hChild;
            }
        }
    }
    if (IsHideableBrowserClass(cls)) {
        HideBrowserRegion(hChild);
    }
    if (g_closeBtn == NULL && IsWindowVisible(hChild) && lstrcmpiA(cls, "Button") == 0) {
        CHAR title[64];
        title[0] = 0;
        GetWindowTextA(hChild, title, sizeof(title));
        if (lstrcmpiA(title, "closebtn") == 0) {
            g_closeBtn = hChild;
        }
    }
    return TRUE;
}


static BOOL IsKnown(HWND h) {
    int i;
    for (i = 0; i < g_knownTopsCount; ++i) {
        if (g_knownTops[i] == h) return TRUE;
    }
    return FALSE;
}

static VOID Remember(HWND h) {
    if (g_knownTopsCount < WV2_WATCH_MAX_KNOWN) {
        g_knownTops[g_knownTopsCount++] = h;
    }
}

static BOOL CALLBACK EnumTopProc(HWND hTop, LPARAM lParam) {
    DWORD pid = 0;
    CHAR cls[64];
    (void)lParam;

    GetWindowThreadProcessId(hTop, &pid);
    if (pid != g_pid) return TRUE;

    cls[0] = 0;
    GetClassNameA(hTop, cls, sizeof(cls));

    if (g_pangyaTop == NULL && lstrcmpiA(cls, "PangYa") == 0) {
        g_pangyaTop = hTop;
    }

    if (g_ownWnd != NULL && hTop == g_ownWnd) {
        return TRUE;
    }

    if (IsBrowserClass(cls)) {
        RECT rc;
        int w = 0, h = 0;
        if (GetWindowRect(hTop, &rc)) { w = rc.right - rc.left; h = rc.bottom - rc.top; }
        if (IsWindowVisible(hTop)) {
            g_anyBrowser = TRUE;
            if (g_visBrowser == NULL && w > 100 && h > 100) {
                g_visBrowser = hTop;
            }
        }
    }
    if (IsHideableBrowserClass(cls)) {
        HideBrowserRegion(hTop);
    }
    EnumChildWindows(hTop, EnumChildSearchProc, 0);
    return TRUE;
}

typedef HRESULT(STDCALL *PFNCOINITEX)(LPVOID, DWORD);
#define WV2_COINIT_APARTMENTTHREADED 0x2u

static VOID EnsureComSTA(VOID) {
    HMODULE hOle = LoadLib("ole32");
    PFNCOINITEX pCoInit = hOle ? (PFNCOINITEX)GetProc(hOle, "CoInitializeEx") : NULL;
    if (pCoInit != NULL) {
        HRESULT hr = pCoInit(NULL, WV2_COINIT_APARTMENTTHREADED);
        Log("[webview2] CoInitializeEx(STA) na thread do watcher = 0x%08x\r\n", hr);
    } else {
        Log("[webview2] CoInitializeEx nao encontrado em ole32\r\n");
    }
}

static VOID PumpMessages(DWORD ms) {
    DWORD start = GetTickCount();
    MSG msg;
    do {
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        Sleep(10);
    } while (GetTickCount() - start < ms);
}

static DWORD WINAPI WatcherThread(LPVOID p) {
    (void)p;
    g_pid = GetCurrentProcessId();
    Log("[watcher] thread iniciada (pid=%u)\r\n", g_pid);

    EnsureComSTA();

    Sleep(5000);

    DWORD lastScan = 0;

    while (1) {
        BOOL escNow;
        DWORD now = GetTickCount();

        if (now - lastScan >= 500) {
            lastScan = now;

            g_visBrowser = NULL;
            g_anyBrowser = FALSE;
            g_closeBtn = NULL;
            EnumWindows(EnumTopProc, 0);

            if (g_active && (g_wantClose || (g_ownWnd != NULL && !IsWindow(g_ownWnd)))) {
                if (g_ownWnd != NULL && !IsWindow(g_ownWnd)) g_ownWnd = NULL;
                Log("[watcher] WM_CLOSE/ALT+F4 -> fechando overlay\r\n");
                g_wantClose = FALSE;
                CloseOverlay(TRUE);
            } else if (!g_active) {
                if (g_suppress && g_suppressWnd != NULL && now >= g_suppressOpenUntil) {
                    g_active = TRUE;
                    g_targetWnd = g_suppressWnd;
                    g_emptyPasses = 0;
                    g_graceUntil = now + 4000;
                    Log("[watcher] >>> cookies (IE suprimido) -> overlay 1600x900\r\n");
                    OpenWebView2(g_suppressWnd, WV2_WATCH_URL);
                } else if (g_visBrowser != NULL && now >= g_suppressOpenUntil) {
                    g_active = TRUE;
                    g_targetWnd = g_visBrowser;
                    g_emptyPasses = 0;
                    g_graceUntil = now + 4000;
                    Log("[watcher] >>> cookies aberto! host=0x%p -> overlay 1600x900\r\n",
                        g_targetWnd);
                    OpenWebView2(g_targetWnd, WV2_WATCH_URL);
                }
            } else {
                ApplyBounds();
                if (now >= g_graceUntil) {
                    BOOL alive = g_suppress ? TRUE : g_anyBrowser;
                    if (alive) {
                        g_emptyPasses = 0;
                    } else if (++g_emptyPasses >= 6) {
                        Log("[watcher] cookies fechado pelo jogo, escondendo overlay\r\n");
                        CloseOverlay(FALSE);
                        g_emptyPasses = 0;
                    }
                }
            }
        }

        escNow = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
        if (escNow && !g_escWasPressed && (g_active || g_suppress)) {
            Log("[watcher] ESC: fechando overlay e voltando pro jogo\r\n");
            CloseOverlay(TRUE);
            g_emptyPasses = 0;
        }
        g_escWasPressed = escNow;

        PumpMessages(20);
    }
    return 0;
}

VOID StartWebView2Watcher(VOID) {
    HANDLE h;
    Log("[watcher] StartWebView2Watcher chamado\r\n");
    h = CreateThread(NULL, 0, WatcherThread, NULL, 0, NULL);
    if (h == NULL) {
        Log("[watcher] CreateThread falhou err=%u\r\n", GetLastError());
        return;
    }
    CloseHandle(h);
}

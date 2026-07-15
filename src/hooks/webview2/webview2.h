#pragma once

#include "../../common.h"

BOOL OpenWebView2(HWND hostWindow, LPCWSTR urlW);

VOID StartWebView2Watcher(VOID);

VOID Wv2BeginCookieSuppressed(HWND gameWnd);
BOOL Wv2CookieSuppressActive(VOID);

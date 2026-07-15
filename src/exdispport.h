#pragma once

#ifdef __MINGW32__
#ifndef __IWebBrowser2_FWD_DEFINED__
#define __IWebBrowser2_FWD_DEFINED__
typedef struct IWebBrowser2 IWebBrowser2;

#endif

typedef
    enum tagREADYSTATE {
        READYSTATE_UNINITIALIZED = 0,
        READYSTATE_LOADING = 1,
        READYSTATE_LOADED = 2,
        READYSTATE_INTERACTIVE = 3,
        READYSTATE_COMPLETE = 4
    } READYSTATE;

typedef enum OLECMDF {
    OLECMDF_SUPPORTED = 0x1,
    OLECMDF_ENABLED = 0x2,
    OLECMDF_LATCHED = 0x4,
    OLECMDF_NINCHED = 0x8,
    OLECMDF_INVISIBLE = 0x10,
    OLECMDF_DEFHIDEONCTXTMENU = 0x20
} OLECMDF;

typedef enum OLECMDID {
    OLECMDID_OPEN = 1,
    OLECMDID_NEW = 2,
    OLECMDID_SAVE = 3,
    OLECMDID_SAVEAS = 4,
    OLECMDID_SAVECOPYAS = 5,
    OLECMDID_PRINT = 6,
    OLECMDID_PRINTPREVIEW = 7,
    OLECMDID_PAGESETUP = 8,
    OLECMDID_SPELL = 9,
    OLECMDID_PROPERTIES = 10,
    OLECMDID_CUT = 11,
    OLECMDID_COPY = 12,
    OLECMDID_PASTE = 13,
    OLECMDID_PASTESPECIAL = 14,
    OLECMDID_UNDO = 15,
    OLECMDID_REDO = 16,
    OLECMDID_SELECTALL = 17,
    OLECMDID_CLEARSELECTION = 18,
    OLECMDID_ZOOM = 19,
    OLECMDID_GETZOOMRANGE = 20,
    OLECMDID_UPDATECOMMANDS = 21,
    OLECMDID_REFRESH = 22,
    OLECMDID_STOP = 23,
    OLECMDID_HIDETOOLBARS = 24,
    OLECMDID_SETPROGRESSMAX = 25,
    OLECMDID_SETPROGRESSPOS = 26,
    OLECMDID_SETPROGRESSTEXT = 27,
    OLECMDID_SETTITLE = 28,
    OLECMDID_SETDOWNLOADSTATE = 29,
    OLECMDID_STOPDOWNLOAD = 30,
    OLECMDID_ONTOOLBARACTIVATED = 31,
    OLECMDID_FIND = 32,
    OLECMDID_DELETE = 33,
    OLECMDID_HTTPEQUIV = 34,
    OLECMDID_HTTPEQUIV_DONE = 35,
    OLECMDID_ENABLE_INTERACTION = 36,
    OLECMDID_ONUNLOAD = 37,
    OLECMDID_PROPERTYBAG2 = 38,
    OLECMDID_PREREFRESH = 39,
    OLECMDID_SHOWSCRIPTERROR = 40,
    OLECMDID_SHOWMESSAGE = 41,
    OLECMDID_SHOWFIND = 42,
    OLECMDID_SHOWPAGESETUP = 43,
    OLECMDID_SHOWPRINT = 44,
    OLECMDID_CLOSE = 45,
    OLECMDID_ALLOWUILESSSAVEAS = 46,
    OLECMDID_DONTDOWNLOADCSS = 47,
    OLECMDID_UPDATEPAGESTATUS = 48,
    OLECMDID_PRINT2 = 49,
    OLECMDID_PRINTPREVIEW2 = 50,
    OLECMDID_SETPRINTTEMPLATE = 51,
    OLECMDID_GETPRINTTEMPLATE = 52,
    OLECMDID_PAGEACTIONBLOCKED = 55,
    OLECMDID_PAGEACTIONUIQUERY = 56,
    OLECMDID_FOCUSVIEWCONTROLS = 57,
    OLECMDID_FOCUSVIEWCONTROLSQUERY = 58,
    OLECMDID_SHOWPAGEACTIONMENU = 59,
    OLECMDID_ADDTRAVELENTRY = 60,
    OLECMDID_UPDATETRAVELENTRY = 61,
    OLECMDID_UPDATEBACKFORWARDSTATE = 62,
    OLECMDID_OPTICAL_ZOOM = 63,
    OLECMDID_OPTICAL_GETZOOMRANGE = 64,
    OLECMDID_WINDOWSTATECHANGED = 65,
    OLECMDID_ACTIVEXINSTALLSCOPE = 66,
    OLECMDID_UPDATETRAVELENTRY_DATARECOVERY = 67,
    OLECMDID_SHOWTASKDLG = 68,
    OLECMDID_POPSTATEEVENT = 69,
    OLECMDID_VIEWPORT_MODE = 70,
    OLECMDID_LAYOUT_VIEWPORT_WIDTH = 71,
    OLECMDID_VISUAL_VIEWPORT_EXCLUDE_BOTTOM = 72,
    OLECMDID_USER_OPTICAL_ZOOM = 73,
    OLECMDID_PAGEAVAILABLE = 74,
    OLECMDID_GETUSERSCALABLE = 75,
    OLECMDID_UPDATE_CARET = 76,
    OLECMDID_ENABLE_VISIBILITY = 77,
    OLECMDID_MEDIA_PLAYBACK = 78,
    OLECMDID_SETFAVICON = 79,
    OLECMDID_SET_HOST_FULLSCREENMODE = 80,
    OLECMDID_EXITFULLSCREEN = 81,
    OLECMDID_SCROLLCOMPLETE = 82,
    OLECMDID_ONBEFOREUNLOAD = 83,
    OLECMDID_SHOWMESSAGE_BLOCKABLE = 84,
    OLECMDID_SHOWTASKDLG_BLOCKABLE = 85
} OLECMDID;

typedef enum OLECMDEXECOPT {
    OLECMDEXECOPT_DODEFAULT = 0,
    OLECMDEXECOPT_PROMPTUSER = 1,
    OLECMDEXECOPT_DONTPROMPTUSER = 2,
    OLECMDEXECOPT_SHOWHELP = 3
} OLECMDEXECOPT;

typedef struct IWebBrowser2Vtbl {

    BEGIN_INTERFACE

    HRESULT(STDCALL *QueryInterface)
    (__RPC__in IWebBrowser2 *This,
      __RPC__in REFIID riid,
     _COM_Outptr_ void **ppvObject);

    ULONG(STDCALL *AddRef)(__RPC__in IWebBrowser2 *This);

    ULONG(STDCALL *Release)(__RPC__in IWebBrowser2 *This);

    HRESULT(STDCALL *GetTypeInfoCount)
    (__RPC__in IWebBrowser2 *This,
      __RPC__out UINT *pctinfo);

    HRESULT(STDCALL *GetTypeInfo)
    (__RPC__in IWebBrowser2 *This,
      UINT iTInfo,
      LCID lcid,
      __RPC__deref_out_opt ITypeInfo **ppTInfo);

    HRESULT(STDCALL *GetIDsOfNames)
    (__RPC__in IWebBrowser2 *This,
      __RPC__in REFIID riid,
      __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
      __RPC__in_range(0, 16384) UINT cNames,
      LCID lcid,
      __RPC__out_ecount_full(cNames) DISPID *rgDispId);

     HRESULT(STDCALL *Invoke)(IWebBrowser2 *This,
                                           _In_ DISPID dispIdMember,
                                           _In_ REFIID riid,
                                           _In_ LCID lcid,
                                           _In_ WORD wFlags,
                                           _In_ DISPPARAMS *pDispParams,
                                           _Out_opt_ VARIANT *pVarResult,
                                           _Out_opt_ EXCEPINFO *pExcepInfo,
                                           _Out_opt_ UINT *puArgErr);

     HRESULT(STDCALL *GoBack)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *GoForward)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *GoHome)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *GoSearch)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *Navigate)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in BSTR URL,
         __RPC__in_opt VARIANT *Flags,
         __RPC__in_opt VARIANT *TargetFrameName,
         __RPC__in_opt VARIANT *PostData,
         __RPC__in_opt VARIANT *Headers);

     HRESULT(STDCALL *Refresh)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *Refresh2)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in_opt VARIANT *Level);

     HRESULT(STDCALL *Stop)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *get_Application)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt IDispatch **ppDisp);

     HRESULT(STDCALL *get_Parent)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt IDispatch **ppDisp);

     HRESULT(STDCALL *get_Container)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt IDispatch **ppDisp);

     HRESULT(STDCALL *get_Document)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt IDispatch **ppDisp);

     HRESULT(STDCALL *get_TopLevelContainer)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pBool);

     HRESULT(STDCALL *get_Type)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *Type);

     HRESULT(STDCALL *get_Left)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out long *pl);

     HRESULT(STDCALL *put_Left)(__RPC__in IWebBrowser2 *This,
                                                    long Left);

     HRESULT(STDCALL *get_Top)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out long *pl);

     HRESULT(STDCALL *put_Top)(__RPC__in IWebBrowser2 *This,
                                                   long Top);

     HRESULT(STDCALL *get_Width)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out long *pl);

     HRESULT(STDCALL *put_Width)(__RPC__in IWebBrowser2 *This,
                                                     long Width);

     HRESULT(STDCALL *get_Height)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out long *pl);

     HRESULT(STDCALL *put_Height)(__RPC__in IWebBrowser2 *This,
                                                      long Height);

     HRESULT(STDCALL *get_LocationName)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *LocationName);

     HRESULT(STDCALL *get_LocationURL)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *LocationURL);

     HRESULT(STDCALL *get_Busy)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pBool);

     HRESULT(STDCALL *Quit)(__RPC__in IWebBrowser2 *This);

     HRESULT(STDCALL *ClientToWindow)(
        __RPC__in IWebBrowser2 *This,
         __RPC__inout int *pcx,
         __RPC__inout int *pcy);

     HRESULT(STDCALL *PutProperty)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in BSTR Property,
         VARIANT vtValue);

     HRESULT(STDCALL *GetProperty)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in BSTR Property,
         __RPC__out VARIANT *pvtValue);

     HRESULT(STDCALL *get_Name)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *Name);

     HRESULT(STDCALL *get_HWND)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out SHANDLE_PTR *pHWND);

     HRESULT(STDCALL *get_FullName)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *FullName);

     HRESULT(STDCALL *get_Path)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *Path);

     HRESULT(STDCALL *get_Visible)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pBool);

     HRESULT(STDCALL *put_Visible)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL Value);

     HRESULT(STDCALL *get_StatusBar)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pBool);

     HRESULT(STDCALL *put_StatusBar)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL Value);

     HRESULT(STDCALL *get_StatusText)(
        __RPC__in IWebBrowser2 *This,
         __RPC__deref_out_opt BSTR *StatusText);

     HRESULT(STDCALL *put_StatusText)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in BSTR StatusText);

     HRESULT(STDCALL *get_ToolBar)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out int *Value);

     HRESULT(STDCALL *put_ToolBar)(
        __RPC__in IWebBrowser2 *This,
         int Value);

     HRESULT(STDCALL *get_MenuBar)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *Value);

     HRESULT(STDCALL *put_MenuBar)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL Value);

     HRESULT(STDCALL *get_FullScreen)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pbFullScreen);

     HRESULT(STDCALL *put_FullScreen)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL bFullScreen);

     HRESULT(STDCALL *Navigate2)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in VARIANT *URL,
         __RPC__in_opt VARIANT *Flags,
         __RPC__in_opt VARIANT *TargetFrameName,
         __RPC__in_opt VARIANT *PostData,
         __RPC__in_opt VARIANT *Headers);

     HRESULT(STDCALL *QueryStatusWB)(
        __RPC__in IWebBrowser2 *This,
         OLECMDID cmdID,
         __RPC__out OLECMDF *pcmdf);

     HRESULT(STDCALL *ExecWB)(
        __RPC__in IWebBrowser2 *This,
         OLECMDID cmdID,
         OLECMDEXECOPT cmdexecopt,
         __RPC__in_opt VARIANT *pvaIn,
         __RPC__inout_opt VARIANT *pvaOut);

     HRESULT(STDCALL *ShowBrowserBar)(
        __RPC__in IWebBrowser2 *This,
         __RPC__in VARIANT *pvaClsid,
         __RPC__in_opt VARIANT *pvarShow,
         __RPC__in_opt VARIANT *pvarSize);

     HRESULT(STDCALL *get_ReadyState)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out READYSTATE *plReadyState);

     HRESULT(STDCALL *get_Offline)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pbOffline);

     HRESULT(STDCALL *put_Offline)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL bOffline);

     HRESULT(STDCALL *get_Silent)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pbSilent);

     HRESULT(STDCALL *put_Silent)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL bSilent);

     HRESULT(STDCALL *get_RegisterAsBrowser)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pbRegister);

     HRESULT(STDCALL *put_RegisterAsBrowser)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL bRegister);

     HRESULT(STDCALL *get_RegisterAsDropTarget)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pbRegister);

     HRESULT(STDCALL *put_RegisterAsDropTarget)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL bRegister);

     HRESULT(STDCALL *get_TheaterMode)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *pbRegister);

     HRESULT(STDCALL *put_TheaterMode)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL bRegister);

     HRESULT(STDCALL *get_AddressBar)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *Value);

     HRESULT(STDCALL *put_AddressBar)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL Value);

     HRESULT(STDCALL *get_Resizable)(
        __RPC__in IWebBrowser2 *This,
         __RPC__out VARIANT_BOOL *Value);

     HRESULT(STDCALL *put_Resizable)(
        __RPC__in IWebBrowser2 *This,
         VARIANT_BOOL Value);

    END_INTERFACE
} IWebBrowser2Vtbl;

struct IWebBrowser2 {
    CONST_VTBL struct IWebBrowser2Vtbl *lpVtbl;
};
#else
#include <ExDisp.h>
#endif

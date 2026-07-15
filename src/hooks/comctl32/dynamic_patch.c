/**
 * Copyright 2018-2024 John Chadwick <john@jchw.io>
 * Copyright 2020 Luiz Lopes <luizinrc@hotmail.com>
 * Copyright 2020-2021 fumitti <fumitti@gmail.com>
 * Copyright 2024 Acrisio Filho
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include "dynamic_patch.h"
#include "../../config.h"
#include "../../hooks/projectg/us852/ranking.h"
#include "../../patch.h"

static HMODULE hComCtl32Module = NULL;
static HMODULE hKernel32Module = NULL;
static PFNINITCOMMONCONTROLSEXPROC pInitCommonControlsEx = NULL;
static PFNGETSTARTUPINFOAPROC pGetStartupInfoA = NULL;
static PFNGETSTARTUPINFOWPROC pGetStartupInfoW = NULL;

#define GETRETURNADDRESS(_FirstParameter) *(((DWORD *)&(_FirstParameter)) - 1)

static BOOL compare_virtual_memory(DWORD _dwAddress, DWORD _value) {
    MEMORY_BASIC_INFORMATION mbi;

    if (VirtualQuery((void *)_dwAddress, &mbi, sizeof(mbi)) == 0) {
        Log("[compare_virtual_memory] Address 0x%08lX failed in VirtualQuery, ErrorCode: %08lX\r\n",
            _dwAddress, LastErr());
        return FALSE;
    }

    if (mbi.Type != MEM_IMAGE) {
        Log("[compare_virtual_memory] 0x%08lX is not image memory.\r\n", _dwAddress);
        return FALSE;
    }

    if (*(DWORD *)_dwAddress != _value)
        return FALSE;

    return TRUE;
}

static BOOL isModuleAllocationBase(DWORD _dwAddress) {
    MEMORY_BASIC_INFORMATION mbi;

    if (VirtualQuery((void *)_dwAddress, &mbi, sizeof(mbi)) == 0) {
        Log("[isImageMemory] Address 0x%08lX failed in VirtualQuery, ErrorCode: %08lX\r\n",
            _dwAddress, LastErr());
        return FALSE;
    }

    if (mbi.Type != MEM_IMAGE) {
        Log("[isImageMemory] 0x%08lX is not image memory.\r\n", _dwAddress);
        return FALSE;
    }

    if ((DWORD)GetModuleHandleA(NULL) != (DWORD)mbi.AllocationBase)
        return FALSE;

    return TRUE;
}

static void PatchGG_US() {
    if (compare_virtual_memory(0x00A495E0, 0x8F143D83)) {
        Patch((LPVOID)0x00A495E0, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A49670, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A49690, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A496B0, "\x90\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A496E0, "\xC3", 1);
        Patch((LPVOID)0x00A49840, "\xC3", 1);
        Log("Patched GG check routines (US 852)\r\n");

        Patch((LPVOID)0x00A6ECC9, "\x30\xC0", 2);
        Log("Patched Cookie Point Item (US 852)\r\n");

        Patch((LPVOID)0x005FB990,
              "\x80\xB9\x40\x02\x00\x00\x00\x0F\x85\x0D\x00\x00\x00\x8B\x89\x8C\x01\x00\x00\x8B"
              "\x01\x8B\x50\x4C\xFF\xD2\xC2\x04\x00",
              29);
        Log("Patched Cookie Btn in onCallback that's disabled (US 852)\r\n");

        Patch((LPVOID)0x005FB9AD,
              "\xB3\x01\x31\xFF\x90\x53\xBA\xB4\x6A\xCE\x00\xE9\xC9\x31\x00\x00", 16);
        Patch((LPVOID)0x005FEB7E, "\xE9\x2A\xCE\xFF\xFF", 5);
        Patch((LPVOID)0x005FEB8D, "\x53", 1);
        Patch((LPVOID)0x005FEB9A, "\x53", 1);
        Patch((LPVOID)0x005FB9BD,
              "\x6A\x01\xBA\xB4\x6A\xCE\x00\x8B\xCE\xE8\xF5\x2C\x00\x00\x6A\x01\xBA\xB0\x69\xCE"
              "\x00\xE9\x29\x32\x00\x00",
              26);
        Patch((LPVOID)0x005FEBF9, "\xE9\xBF\xCD\xFF\xFF", 5);
        Log("Patched Btn Cookie, Gacha and Scratch disabled (US 852)\r\n");

        Patch((LPVOID)0x008BC729, "\x01", 1);
        Patch((LPVOID)0x008C1495, "\xEB\x0C", 2);
        Patch((LPVOID)0x008C14A3, "\xE8\xF8\xB2\xFF\xFF\x88\x86\xE4\x00\x00\x00\x5E\xC3", 13);
        Log("Patched Btn Change Nickname disabled (US 852)\r\n");

        InitUS852RankingHook();
        Log("Patched Ranking System disabled (US 852)\r\n");
    } else if (compare_virtual_memory(0x00A49580, 0x8F143D83)) {
        Patch((LPVOID)0x00A49580, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A49670, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A49690, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A496B0, "\x90\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A496E0, "\xC3", 1);
        Patch((LPVOID)0x00A49840, "\xC3", 1);
        Log("Patched GG check routines (US 824)\r\n");
    } else if (compare_virtual_memory(0x006E469B, 0xFF6450E8)) {
        Patch((LPVOID)0x006E469B, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (US S3 431)\r\n");
    } else if (compare_virtual_memory(0x00749230, 0xFECE2BE8)) {
        Patch((LPVOID)0x00749230, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (US S4 500a)\r\n");
    } else if (compare_virtual_memory(0x0078EC23, 0xFEA938E8)) {
        Patch((LPVOID)0x0078EC23, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (US S5 627)\r\n");
    } else if (compare_virtual_memory(0x0083D869, 0xFE8772E8)) {
        Patch((LPVOID)0x0083D869, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (US S6 633)\r\n");
    } else if (compare_virtual_memory(0x00908229, 0xFEA4B2E8)) {
        Patch((LPVOID)0x00908229, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (US S7 727)\r\n");
    } else if (compare_virtual_memory(0x00979A19, 0xFEA222E8)) {
        Patch((LPVOID)0x00979A19, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (US S8 806)\r\n");
    } else if (compare_virtual_memory(0x006119E8, 0xFF85A3E8)) {
        Patch((LPVOID)0x006119E8, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (Albatross18 S1 242a)\r\n");
    } else if (compare_virtual_memory(0x0066DB32, 0xFF6D89E8)) {
        Patch((LPVOID)0x0066DB32, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (Albatross18 S2 323a)\r\n");
    } else if (compare_virtual_memory(0x006AF52B, 0xFF61B0E8)) {
        Patch((LPVOID)0x006AF52B, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (Albatross18 S3 404)\r\n");
    }
}

#define BONGDARI_TOGGLE_KEY VK_F8
#define BONGDARI_VT0        0x00D3733Cu
#define BONGDARI_VT1        0x00D37330u
#define BONGDARI_VT2        0x00D37328u
#define BONGDARI_MODE_OFF   0x444u
#define BONGDARI_SETBIG     0x008B47D0u
#define BONGDARI_SETNORMAL  0x008B4780u

typedef void(STDCALL *PFNBONGDARISETMODE)(LPVOID _this);
typedef int(STDCALL *PFNDRAWMODE)(LPVOID _this);

#define BONGDARI_DRAWMODE 0x008B5DA0u

static PFNBONGDARISETMODE pBongdariSetBig = NULL;
static PFNBONGDARISETMODE pBongdariSetNormal = NULL;
static PFNDRAWMODE pBongdariDrawMode_Orig = NULL;

static int STDCALL BongdariDrawModeHook(LPVOID _this) {
    static BOOL wasDown = FALSE;
    static BOOL loggedLive = FALSE;
    BOOL down;

    if (!loggedLive) {
        loggedLive = TRUE;
        Log("[BigPapel] render poll live (thread %lu)\r\n", GetCurrentThreadId());
    }

    down = (GetAsyncKeyState(BONGDARI_TOGGLE_KEY) & 0x8000) != 0;

    if (down && !wasDown && _this != NULL && *(DWORD *)_this == BONGDARI_VT0) {
        if (*(DWORD *)((BYTE *)_this + BONGDARI_MODE_OFF) == 0) {
            if (pBongdariSetBig != NULL)
                pBongdariSetBig(_this);
            Log("[BigPapel] -> BIG (obj=%08lX)\r\n", (DWORD)_this);
        } else {
            if (pBongdariSetNormal != NULL)
                pBongdariSetNormal(_this);
            Log("[BigPapel] -> NORMAL (obj=%08lX)\r\n", (DWORD)_this);
        }
    }

    wasDown = down;

    return pBongdariDrawMode_Orig(_this);
}

static void InstallBigPapelHook_JP983(void) {
    LPVOID thunk;
    PVOID trampoline;

    pBongdariSetBig = (PFNBONGDARISETMODE)BuildStdcallToThiscallThunk((LPVOID)BONGDARI_SETBIG);
    pBongdariSetNormal = (PFNBONGDARISETMODE)BuildStdcallToThiscallThunk((LPVOID)BONGDARI_SETNORMAL);

    thunk = BuildThiscallToStdcallThunk((LPVOID)BongdariDrawModeHook);
    trampoline = HookFunc((PVOID)BONGDARI_DRAWMODE, thunk);
    pBongdariDrawMode_Orig = (PFNDRAWMODE)BuildStdcallToThiscallThunk(trampoline);

    Patch((LPVOID)0x008B3E62, "\x6A\x01", 2);

    Log("Installed BIG Papel Shop: native BIG button + F8 toggle (JP 983)\r\n");
}

static void PatchGG_JP() {
    if (compare_virtual_memory(0x00A5CD10, 0x1BA43D83)) {
        Patch((LPVOID)0x00A5CD10, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5CDA0, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5CDC0, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5CDE0, "\x90\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5CE10, "\xC3", 1);
        Patch((LPVOID)0x00A5CE40, "\xC3", 1);
        Log("Patched GG check routines (JP 972)\r\n");
    } else if (compare_virtual_memory(0x00A5CF80, 0x1BA43D83)) {
        Patch((LPVOID)0x00A5CF80, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5C010, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5C030, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5C050, "\x90\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5CE80, "\xC3", 1);
        Patch((LPVOID)0x00A5CEB0, "\xC3", 1);
        Log("Patched GG check routines (JP 974)\r\n");
    } else if (compare_virtual_memory(0x00A5CF80, 0x1C143D83)) {
        Patch((LPVOID)0x00A5CF80, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5C010, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5C030, "\xC3\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5C050, "\x90\x90\x90\x90\x90\x90\x90", 7);
        Patch((LPVOID)0x00A5CE80, "\xC3", 1);
        Patch((LPVOID)0x00A5CEB0, "\xC3", 1);
        Log("Patched GG check routines (JP 983)\r\n");

        InstallBigPapelHook_JP983();
    } else if (compare_virtual_memory(0x005E345A, 0xFF83F1E8)) {
        Patch((LPVOID)0x005E345A, "\xB8\x01\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (JP S1 2.11)\r\n");
    } else if (compare_virtual_memory(0x006208A8, 0xFF0923E8)) {
        Patch((LPVOID)0x006208A8, "\xB8\x00\x00\x00\x00", 5);
        Patch((LPVOID)0x006208AD, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (JP S1 2.25b)\r\n");
    } else if (compare_virtual_memory(0x00673DC3, 0xFF7C18E8)) {
        Patch((LPVOID)0x00673DC3, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (JP S3 4.00a)\r\n");
    } else if (compare_virtual_memory(0x0067BF95, 0xFF6A76E8)) {
        Patch((LPVOID)0x0067BF95, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (JP S3 4.01h1)\r\n");
    } else if (compare_virtual_memory(0x00752E8B, 0xFF2AA0E8)) {
        Patch((LPVOID)0x00752E8B, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (JP S4 585)\r\n");
    }
}

static void PatchGG_TW() {
    if (compare_virtual_memory(0x00643743, 0xFF6B98E8)) {
        Patch((LPVOID)0x00643743, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TW S2 3.00a)\r\n");
    } else if (compare_virtual_memory(0x006690D1, 0xFF7BEAE8)) {
        Patch((LPVOID)0x006690D1, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TW S3 4.00a)\r\n");
    }
}

static void PatchHS_ID() {
    if (compare_virtual_memory(0x005FC66D, 0xFF071EE8)) {
        Patch((LPVOID)0x005FC66D, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (INA S1 2.12a)\r\n");
    }
}

static void PatchHS_BR() {
    if (compare_virtual_memory(0x005FE093, 0xFF8858E8)) {
        Patch((LPVOID)0x005FE093, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S1 2.14a)\r\n");
    } else if (compare_virtual_memory(0x005FE0B3, 0xFF8858E8)) {
        Patch((LPVOID)0x005FE0B3, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S1 2.14b)\r\n");
    } else if (compare_virtual_memory(0x005FE1A3, 0xFF8858E8)) {
        Patch((LPVOID)0x005FE1A3, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S1 2.15)\r\n");
    } else if (compare_virtual_memory(0x005FE1B3, 0xFF8858E8)) {
        Patch((LPVOID)0x005FE1B3, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S1 2.15a)\r\n");
    } else if (compare_virtual_memory(0x005FE1D3, 0xFF8838E8)) {
        Patch((LPVOID)0x005FE1D3, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S1 2.16)\r\n");
    } else if (compare_virtual_memory(0x006020A3, 0xFF86F8E8)) {
        Patch((LPVOID)0x006020A3, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S1 2.20c)\r\n");
    } else if (compare_virtual_memory(0x0065B4B2, 0xFF6299E8)) {
        Patch((LPVOID)0x0065B4B2, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S2 3.00)\r\n");
    } else if (compare_virtual_memory(0x0065B852, 0xFF6269E8)) {
        Patch((LPVOID)0x0065B852, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched HSHIELD check routines (BR S2 3.05a)\r\n");
    }
}

static void PatchGG_KR() {
    if (compare_virtual_memory(0x00634EAD, 0xFF178EE8)) {
        Patch((LPVOID)0x00634EAD, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (KR S2 3.26a)\r\n");
    } else if (compare_virtual_memory(0x0075C670, 0xFECB7BE8)) {
        Patch((LPVOID)0x0075C670, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (KR S5 603)\r\n");
    } else if (compare_virtual_memory(0x00A70EE5, 0xFEA346E8)) {
        Patch((LPVOID)0x00A70EE5, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (KR S9 839)\r\n");
    }
}

static void PatchGG_TH() {
    if (compare_virtual_memory(0x005F678A, 0xFF5B21E8)) {
        Patch((LPVOID)0x005F678A, "\xB8\x01\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S1 217)\r\n");
    } else if (compare_virtual_memory(0x0064B60B, 0xFF77A0E8)) {
        Patch((LPVOID)0x0064B60B, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S2 300b1)\r\n");
    } else if (compare_virtual_memory(0x0066589B, 0xFF6E30E8)) {
        Patch((LPVOID)0x0066589B, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S2 312b)\r\n");
    } else if (compare_virtual_memory(0x0066C24B, 0xFF6E40E8)) {
        Patch((LPVOID)0x0066C24B, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S2 321a)\r\n");
    } else if (compare_virtual_memory(0x0076F4F4, 0xFEBED7E8)) {
        Patch((LPVOID)0x0076F4F4, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S4 580)\r\n");
    } else if (compare_virtual_memory(0x0084F49A, 0xFE8011E8)) {
        Patch((LPVOID)0x0084F49A, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S6 644)\r\n");
    } else if (compare_virtual_memory(0x008BD475, 0xFE9A36E8)) {
        Patch((LPVOID)0x008BD475, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S7 714c)\r\n");
    } else if (compare_virtual_memory(0x00A6B375, 0xFE9A16E8)) {
        Patch((LPVOID)0x00A6B375, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (TH S9 829c)\r\n");
    }
}

static void PatchGG_SEA() {
    if (compare_virtual_memory(0x00611CF8, 0xFF86D3E8)) {
        Patch((LPVOID)0x00611CF8, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (SEA S1 2.16a)\r\n");
    } else if (compare_virtual_memory(0x0066C727, 0xFF6B24E8)) {
        Patch((LPVOID)0x0066C727, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (SEA S2 3.20)\r\n");
    }
}

static void PatchGG_EU() {
    if (compare_virtual_memory(0x006160D1, 0xFF161AE8)) {
        Patch((LPVOID)0x006160D1, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (EU S2 3.01a)\r\n");
    } else if (compare_virtual_memory(0x006B5369, 0xFF6182E8)) {
        Patch((LPVOID)0x006B5369, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (EU S3 400a)\r\n");
    } else if (compare_virtual_memory(0x0075FB71, 0xFECECAE8)) {
        Patch((LPVOID)0x0075FB71, "\xE9\x00\x00\x00\x00", 5);
        Log("Patched GG check routines (EU S4 500)\r\n");
    }
}

static const float kWindCap15 = 15.0f;

static void PatchReticleYardDecimals_JP983(void) {
    if (!compare_virtual_memory(0x00D08C74, 0x66302E25)) {
        Log("[reticle] string \"%%.0fy\" nao confere em 0x00D08C74 (=%08lX), pulando\r\n",
            *(DWORD *)0x00D08C74);
        return;
    }

    if (*(unsigned char *)0x00D08C76 == 0x30) {
        unsigned char two = 0x32;
        Patch((LPVOID)0x00D08C76, &two, 1);
    }

    Log("Patched Yard Distance: \"%%.0fy\" -> \"%%.2fy\" (JP 983)\r\n");
}

static void PatchWorldTourUnlock_JP983(void) {
    if (!compare_virtual_memory(0x005965FB, 0x00001FB9)) {
        Log("[worldtour] 'mov ecx,1Fh' nao confere em 0x005965FB (=%08lX), pulando (versao != JP983?)\r\n",
            *(DWORD *)0x005965FB);
        return;
    }
    if (*(unsigned char *)0x00596600 != 0xE8) {
        Log("[worldtour] opcode call nao confere em 0x00596600 (=%02X), pulando\r\n",
            *(unsigned char *)0x00596600);
        return;
    }
    if (!compare_virtual_memory(0x00596605, 0x840FC084)) {
        Log("[worldtour] 'test al,al;jz' nao confere em 0x00596605 (=%08lX), pulando\r\n",
            *(DWORD *)0x00596605);
        return;
    }

    Patch((LPVOID)0x00596600, "\xB0\x01\x90\x90\x90", 5);

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);
    Log("Patched World Tour Event: gate bit 31 forcado (JP 983) -- requer evento #31 do server + botao cmd74\r\n");
}

#define WT_TOGGLE_KEY   VK_F9
#define WT_LOBBY_PTR    0x00E78CA8u
#define WT_CONTENT_OFF  0x18Cu
#define WT_ANCHOR_OFF   0x248u
#define WT_DLG_OFF      0x480u
#define WT_FORM_ROOT    0x00F02340u
#define WT_CALLBACK     0x00AB30C0u
#define WT_CREATE_FN    0x00595700u
#define WT_TICK_FN      0x00560D70u
#define WT_HIDEOVL_FN   0x00B10600u

typedef int(__fastcall *PFN_WT_CREATE)(int mgr, int anchor, const char *name, int zero);
typedef void(STDCALL *PFN_WT_PREP)(int content);
typedef int(STDCALL *PFN_WT_SETUP)(int dlg, int anchorData);
typedef int(STDCALL *PFN_WT_ACTIVATE)(int dlg, int cb, int a3, int a4, int a5, int flags);
typedef int(STDCALL *PFN_WT_TICK)(int thisPtr, float arg);
typedef int(STDCALL *PFN_WT_HIDEOVL)(int dlg);

static const float kWTCenterAnchor[4] = {0.0f, 0.0f, 800.0f, 600.0f};

static PFN_WT_CREATE pWTCreate = (PFN_WT_CREATE)WT_CREATE_FN;
static PFN_WT_PREP pWTPrep = NULL;
static PFN_WT_SETUP pWTSetup = NULL;
static PFN_WT_ACTIVATE pWTActivate = NULL;
static PFN_WT_TICK pWTTickOrig = NULL;
static PFN_WT_HIDEOVL pWTHideOverlays = NULL;

static void WT_ForceOpen(void) {
    DWORD lobby, content, anchorNode, mgrRoot, mgr, dlg;

    lobby = *(DWORD *)WT_LOBBY_PTR;
    if (!lobby) {
        Log("[worldtour] bail: lobby(0x00E78CA8) NULL (fora do lobby?)\r\n");
        return;
    }
    content = *(DWORD *)(lobby + WT_CONTENT_OFF);
    if (!content) {
        Log("[worldtour] bail: content(lobby+0x18C) NULL\r\n");
        return;
    }
    anchorNode = *(DWORD *)(content + WT_ANCHOR_OFF);
    if (*(DWORD *)(content + WT_DLG_OFF) != 0) {
        Log("[worldtour] bail: dialog ja aberto (content+0x480=%08lX)\r\n", *(DWORD *)(content + WT_DLG_OFF));
        return;
    }

    mgrRoot = *(DWORD *)WT_FORM_ROOT;
    if (!mgrRoot) {
        Log("[worldtour] bail: formRoot(0x00F02340) NULL\r\n");
        return;
    }
    mgr = *(DWORD *)(mgrRoot + 0x40);
    Log("[worldtour] abrindo: content=%08lX anchor=%08lX mgr=%08lX\r\n", content, anchorNode, mgr);

    if (pWTPrep)
        pWTPrep((int)content);

    dlg = (DWORD)pWTCreate((int)mgr, (int)(content + 0x18), "world_tour_event_dlg", 0);
    *(DWORD *)(content + WT_DLG_OFF) = dlg;
    if (dlg) {
        if (pWTSetup)
            pWTSetup((int)dlg, anchorNode ? (int)(anchorNode + 52) : (int)kWTCenterAnchor);
        if (pWTActivate)
            pWTActivate((int)dlg, (int)WT_CALLBACK, -24, 0, 0, 17);
        if (pWTHideOverlays)
            pWTHideOverlays((int)dlg);
        Log("[worldtour] force-open OK: world_tour_event_dlg criado+ativado (dlg=%08lX, setup=%s)\r\n",
            dlg, anchorNode ? "evento" : "sintetica(center)");
    } else {
        Log("[worldtour] force-open: sub_595700 retornou NULL (nome .xml nao existe no pak?)\r\n");
    }
}

#define WS_TOGGLE_KEY   VK_F7
#define WS_FORCE_KEY    VK_F6
#define WS_SENDREQ_FN   0x00B15360u
#define WS_E73E60_PTR   0x00E73E60u
#define WS_CREATE_FN    0x0082E350u
#define WS_SETDATA_FN   0x0082EDD0u
#define WS_CALLBACK     0x0082E330u
#define WS_FINDFORM_FN  0x00BE80E0u

typedef int(__fastcall *PFN_WS_CREATE)(int mgr, int anchor, const char *name, int zero);
typedef int(STDCALL *PFN_WS_PREP)(int dlg, int a);
typedef int(STDCALL *PFN_WS_SETDATA)(int dlg, int d0, int d1, int d2, int d3);
typedef int(STDCALL *PFN_WS_ACTIVATE)(int dlg, int cb, int a3, int a4, int a5, int flags);
typedef int(STDCALL *PFN_WS_FINDFORM)(int mgrThis, const char *name);

static PFN_WS_CREATE pWSCreate = (PFN_WS_CREATE)WS_CREATE_FN;
static PFN_WS_PREP pWSPrep = NULL;
static PFN_WS_SETDATA pWSSetData = NULL;
static PFN_WS_ACTIVATE pWSActivate = NULL;
static PFN_WS_FINDFORM pWSFindForm = NULL;

static void WS_ForceOpen(void) {
    DWORD mgrRoot, mgr, mgrThis, lobby, content, dlg;

    mgrRoot = *(DWORD *)WT_FORM_ROOT;
    if (!mgrRoot)
        return;
    mgr = *(DWORD *)(mgrRoot + 0x40);
    if (!mgr)
        return;
    mgrThis = *(DWORD *)(mgr + 8);
    if (pWSFindForm && mgrThis && pWSFindForm((int)mgrThis, "WorkShopEvent2013")) {
        Log("[workshop] dialog ja aberto\r\n");
        return;
    }
    lobby = *(DWORD *)WT_LOBBY_PTR;
    if (!lobby)
        return;
    content = *(DWORD *)(lobby + WT_CONTENT_OFF);
    if (!content)
        return;

    dlg = (DWORD)pWSCreate((int)mgr, (int)(content + 0x18), "WorkShopEvent2013", 0);
    if (dlg) {
        if (pWSPrep)
            pWSPrep((int)dlg, 0);
        if (pWSSetData)
            pWSSetData((int)dlg, 1, 1, 1, 1);
        if (pWSActivate)
            pWSActivate((int)dlg, (int)WS_CALLBACK, -32, 0, 0, 1);
        Log("[workshop] force-open OK: WorkShopEvent2013 criado+ativado (dlg=%08lX)\r\n", dlg);
    } else {
        Log("[workshop] sub_82E350 retornou NULL\r\n");
    }
}

static int STDCALL WT_TickHook(int thisPtr, float arg) {
    static BOOL wasDown = FALSE;
    static BOOL loggedLive = FALSE;
    BOOL down;

    if (!loggedLive) {
        loggedLive = TRUE;
        Log("[worldtour] tick hook LIVE (sub_560D70 chamado; thread %lu)\r\n", GetCurrentThreadId());
    }

    down = (GetAsyncKeyState(WT_TOGGLE_KEY) & 0x8000) != 0;
    if (down && !wasDown) {
        Log("[worldtour] F9 detectado -> WT_ForceOpen()\r\n");
        WT_ForceOpen();
    }
    wasDown = down;

    {
        static BOOL wasF7 = FALSE, wasF6 = FALSE;
        BOOL f7 = (GetAsyncKeyState(WS_TOGGLE_KEY) & 0x8000) != 0;
        BOOL f6 = (GetAsyncKeyState(WS_FORCE_KEY) & 0x8000) != 0;
        if (f7 && !wasF7) {
            if (*(DWORD *)WS_E73E60_PTR) {
                Log("[workshop] F7 -> envia request 0x172 (fluxo nativo)\r\n");
                ((void(STDCALL *)(void))WS_SENDREQ_FN)();
            } else {
                Log("[workshop] F7: dword_E73E60 NULL (fora do lobby?)\r\n");
            }
        }
        if (f6 && !wasF6) {
            Log("[workshop] F6 detectado -> WS_ForceOpen() (fallback)\r\n");
            WS_ForceOpen();
        }
        wasF7 = f7;
        wasF6 = f6;
    }

    if (pWTHideOverlays) {
        DWORD lobby2 = *(DWORD *)WT_LOBBY_PTR;
        if (lobby2) {
            DWORD content2 = *(DWORD *)(lobby2 + WT_CONTENT_OFF);
            if (content2) {
                DWORD dlg2 = *(DWORD *)(content2 + WT_DLG_OFF);
                if (dlg2)
                    pWTHideOverlays((int)dlg2);
            }
        }
    }

    return pWTTickOrig ? pWTTickOrig(thisPtr, arg) : 0;
}

static void InstallWorldTourForceOpen_JP983(void) {
    LPVOID thunk;
    PVOID trampoline;

    if (!compare_virtual_memory(WT_TICK_FN, 0x57EC8B55)) {
        Log("[worldtour] prologo sub_560D70 nao confere em 0x00560D70 (=%08lX), pulando hotkey F9\r\n",
            *(DWORD *)WT_TICK_FN);
        return;
    }

    pWTPrep = (PFN_WT_PREP)BuildStdcallToVirtualThiscallThunk(60);
    pWTSetup = (PFN_WT_SETUP)BuildStdcallToVirtualThiscallThunk(88);
    pWTActivate = (PFN_WT_ACTIVATE)BuildStdcallToVirtualThiscallThunk(100);
    pWTHideOverlays = (PFN_WT_HIDEOVL)BuildStdcallToThiscallThunk((LPVOID)WT_HIDEOVL_FN);

    pWSPrep = (PFN_WS_PREP)BuildStdcallToVirtualThiscallThunk(120);
    pWSActivate = (PFN_WS_ACTIVATE)BuildStdcallToVirtualThiscallThunk(100);
    pWSSetData = (PFN_WS_SETDATA)BuildStdcallToThiscallThunk((LPVOID)WS_SETDATA_FN);
    pWSFindForm = (PFN_WS_FINDFORM)BuildStdcallToThiscallThunk((LPVOID)WS_FINDFORM_FN);

    thunk = BuildThiscallToStdcallThunk((LPVOID)WT_TickHook);
    trampoline = HookFunc((PVOID)WT_TICK_FN, thunk);
    pWTTickOrig = (PFN_WT_TICK)BuildStdcallToThiscallThunk(trampoline);

    Log("Installed World Tour force-open hotkey (F9) (JP 983)\r\n");
}

static void PatchWindCap15_JP983(void) {
    const DWORD sites[3] = {0x006C9F10, 0x006CA05B, 0x006CA1BD};
    DWORD newAddr;
    int i;

    if (!compare_virtual_memory(0x006C9F0E, 0x21C805D9)) {
        Log("[windcap] fld nao confere em 0x006C9F0E, pulando (versao != JP983?)\r\n");
        return;
    }
    if (*(DWORD *)0x006C9F10 != 0x00D021C8) {
        Log("[windcap] operando != 0x00D021C8 (=%08lX), pulando\r\n", *(DWORD *)0x006C9F10);
        return;
    }

    newAddr = (DWORD)&kWindCap15;
    for (i = 0; i < 3; i++)
        Patch((LPVOID)sites[i], &newAddr, 4);

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);
    Log("Patched Wind Cap 9m -> 15m (JP 983), const 15.0f @ 0x%08lX\r\n", newAddr);
}

static void PatchCaliperTextGold_JP983(void) {
    const DWORD gold = 0xFFFFD700;

    if (!compare_virtual_memory(0x005340C9, 0x001C41C7)) {
        Log("[caliper] mov nao confere em 0x005340C9 (=%08lX), pulando (versao != JP983?)\r\n",
            *(DWORD *)0x005340C9);
        return;
    }
    if (*(DWORD *)0x005340CC != 0xFF1AFD00) {
        Log("[caliper] cor != 0xFF1AFD00 (=%08lX), pulando\r\n", *(DWORD *)0x005340CC);
        return;
    }

    Patch((LPVOID)0x005340CC, (LPVOID)&gold, 4);

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);
    Log("Patched Caliper yard text: verde 0xFF1AFD00 -> gold 0x%08lX (JP 983)\r\n", gold);
}

static void PatchShowNickChange_JP983(void) {
    if (!compare_virtual_memory(0x008C8BE3, 0x8E8B3B75)) {
        Log("[nickchange] jnz nao confere em 0x008C8BE3 (=%08lX), pulando (versao != JP983?)\r\n",
            *(DWORD *)0x008C8BE3);
        return;
    }

    Patch((LPVOID)0x008C8BE3, "\xEB", 1);

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);
    Log("Patched Show Nickname Change row in Game Settings (JP 983)\r\n");
}


typedef int(STDCALL *PFN_SCRATCH_MAKE)(void);
typedef int(STDCALL *PFN_SCRATCH_INIT)(int obj, int mgr, int anchor, const char *name, int zero);
typedef int(STDCALL *PFN_SCRATCH_FINDFORM)(int mgrThis, const char *name);
typedef int(STDCALL *PFN_SCRATCH_CLOSEPOPUP)(int lobby, int zero);
typedef void(STDCALL *PFN_SCRATCH_PREP)(int content);
typedef int(STDCALL *PFN_SCRATCH_ACTIVATE)(int obj, int cb, int a3, int a4, int a5, int flags);
typedef int(STDCALL *PFN_SCRATCH_CLOSECB)(int thisPtr, int a2, int a3);

static PFN_SCRATCH_MAKE pScratchMake = (PFN_SCRATCH_MAKE)0x008EC8B0;
static PFN_SCRATCH_INIT pScratchInit = NULL;
static PFN_SCRATCH_FINDFORM pScratchFindForm = NULL;
static PFN_SCRATCH_CLOSEPOPUP pScratchClosePopup = NULL;
static PFN_SCRATCH_PREP pScratchPrep = NULL;
static PFN_SCRATCH_ACTIVATE pScratchActivate = NULL;
static void *pScratchCloseCb = NULL;

static int STDCALL H_ScratchClose(int thisPtr, int a2, int a3) {
    (void)thisPtr;
    (void)a2;
    (void)a3;
    return 1;
}

static int STDCALL H_OnScratchButtonClicked(int a1) {
    DWORD lobby = *(DWORD *)0x00E78CA8;
    DWORD content, mgr, mgrThis, obj;

    (void)a1;
    if (!lobby)
        return 0;

    content = *(DWORD *)(lobby + 0x18C);
    if (content) {
        mgr = *(DWORD *)(*(DWORD *)0x00F02340 + 0x40);
        mgrThis = *(DWORD *)(mgr + 8);

        if (!pScratchFindForm(mgrThis, "scratchdlg")) {
            pScratchPrep(content);
            obj = (DWORD)pScratchMake();
            if (obj) {
                pScratchInit((int)obj, (int)mgr, (int)(content + 0x18), "scratchdlg", 0);
                if (pScratchActivate)
                    pScratchActivate((int)obj, (int)pScratchCloseCb, -24, 0, 0, 17);
            }
        }
    }

    pScratchClosePopup(lobby, 0);
    return 0;
}

static void PatchScratchButton_JP983(void) {
    if (!compare_virtual_memory(0x005D5DC0, 0x8BEC8B55)) {
        Log("[scratch] prologo nao confere em 0x005D5DC0 (=%08lX), pulando (versao != JP983?)\r\n",
            *(DWORD *)0x005D5DC0);
        return;
    }

    pScratchFindForm = (PFN_SCRATCH_FINDFORM)BuildStdcallToThiscallThunk((LPVOID)0x00BE80E0);
    pScratchClosePopup = (PFN_SCRATCH_CLOSEPOPUP)BuildStdcallToThiscallThunk((LPVOID)0x005FAFB0);
    pScratchInit = (PFN_SCRATCH_INIT)BuildStdcallToThiscallThunk((LPVOID)0x00BE6A20);
    pScratchPrep = (PFN_SCRATCH_PREP)BuildStdcallToVirtualThiscallThunk(60);
    pScratchActivate = (PFN_SCRATCH_ACTIVATE)BuildStdcallToVirtualThiscallThunk(100);
    pScratchCloseCb = BuildThiscallToStdcallThunk((LPVOID)H_ScratchClose);

    InstallHook((LPVOID)0x005D5DC0, (LPVOID)H_OnScratchButtonClicked);

    FlushInstructionCache(GetCurrentProcess(), NULL, 0);
    Log("Patched Scratchy Card: btn_scratch abre FrScratchDlg nativo (JP 983)\r\n");
}

#define FILLDEV_PTR   0x00F02460u
#define D3DRS_FILL    0x22u
#define FILL_WIRE     1u
#define FILL_SOLID    0u
#define CLUB_DRAW_FN  0x00750150u
#define WIND_DRAW_FN  0x006C83B0u

typedef void(__fastcall *PFN_SETSTATE)(LPVOID _this, int edx, DWORD state, DWORD value, DWORD z);

#define CLUB_CALL_1  0x0075992Fu
#define WIND_CALL_1  0x007508EBu

#define PETBODY_RENDER_SLOT  0x00D1C854u
#define PETBODY_RENDER_FN    0x0070BA80u
#define PETBODY_RENDER8_SLOT 0x00D1C858u
#define PETBODY_RENDER8_FN   0x0070BC20u

#define GROUNDITEM_DRAW_FN   0x0085FA00u
#define GROUNDITEM_VTBL_SLOT 0x00D32A10u
static DWORD g_groundRet;

static DWORD g_clubRet;
static DWORD g_windRet;

static BOOL MemReadable(DWORD p) {
    MEMORY_BASIC_INFORMATION mbi;
    if (p < 0x00010000u) return FALSE;
    if (VirtualQuery((LPCVOID)p, &mbi, sizeof(mbi)) == 0) return FALSE;
    if (mbi.State != MEM_COMMIT) return FALSE;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return FALSE;
    return TRUE;
}

static BOOL MemExecutable(DWORD p) {
    MEMORY_BASIC_INFORMATION mbi;
    if (p < 0x00010000u) return FALSE;
    if (VirtualQuery((LPCVOID)p, &mbi, sizeof(mbi)) == 0) return FALSE;
    if (mbi.State != MEM_COMMIT) return FALSE;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return FALSE;
    return (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                           PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
}

static void SetFillMode(DWORD mode) {
    static int logged = 0;
    DWORD A, B, C, fn;

    A = *(DWORD *)FILLDEV_PTR;
    B = MemReadable(A) ? *(DWORD *)A : 0;
    C = MemReadable(B) ? *(DWORD *)B : 0;
    fn = MemReadable(C + 0x24) ? *(DWORD *)(C + 0x24) : 0;

    if (!logged) {
        logged = 1;
        Log("[wireframe] chain F02460: A=%08lX B=%08lX C=%08lX fn=%08lX exec=%d\r\n",
            A, B, C, fn, (int)MemExecutable(fn));
    }

    if (MemReadable(B) && MemExecutable(fn))
        ((PFN_SETSTATE)fn)((LPVOID)B, 0, D3DRS_FILL, mode, 0);
}

static __declspec(naked) void ClubDrawPost(void) {
    __asm {
        pushad
        push FILL_SOLID
        call SetFillMode
        add  esp, 4
        popad
        push g_clubRet
        ret
    }
}
static __declspec(naked) void ClubDrawWrapper(void) {
    __asm {
        pushad
        push FILL_WIRE
        call SetFillMode
        add  esp, 4
        popad
        pop  eax
        mov  g_clubRet, eax
        mov  eax, offset ClubDrawPost
        push eax
        mov  eax, CLUB_DRAW_FN
        jmp  eax
    }
}

static __declspec(naked) void WindDrawPost(void) {
    __asm {
        pushad
        push FILL_SOLID
        call SetFillMode
        add  esp, 4
        popad
        push g_windRet
        ret
    }
}
static __declspec(naked) void WindDrawWrapper(void) {
    __asm {
        pushad
        push FILL_WIRE
        call SetFillMode
        add  esp, 4
        popad
        pop  eax
        mov  g_windRet, eax
        mov  eax, offset WindDrawPost
        push eax
        mov  eax, WIND_DRAW_FN
        jmp  eax
    }
}

static void PatchCallSite(DWORD callSite, DWORD target) {
    DWORD rel, old;
    rel = target - (callSite + 5);
    VirtualProtect((LPVOID)(callSite + 1), 4, PAGE_EXECUTE_READWRITE, &old);
    *(DWORD *)(callSite + 1) = rel;
    VirtualProtect((LPVOID)(callSite + 1), 4, old, &old);
    FlushInstructionCache(GetCurrentProcess(), (LPVOID)callSite, 5);
}

static void InstallWireframeToggle_JP983(void) {
    if (*(WORD *)0x007235E9 != 0x958D) {
        Log("[wireframe] assinatura do dispatcher nao confere @ 0x007235E9, pulando\r\n");
        return;
    }
    if (*(BYTE *)CLUB_CALL_1 != 0xE8 || *(BYTE *)WIND_CALL_1 != 0xE8) {
        Log("[wireframe] call site nao e E8 (club=%02X wind=%02X), pulando\r\n",
            *(BYTE *)CLUB_CALL_1, *(BYTE *)WIND_CALL_1);
        return;
    }

    PatchCallSite(CLUB_CALL_1, (DWORD)ClubDrawWrapper);
    PatchCallSite(WIND_CALL_1, (DWORD)WindDrawWrapper);

    Log("Installed Wireframe (call-site wrap): taco (%08X) + seta (%08X) (JP983)\r\n",
        CLUB_CALL_1, WIND_CALL_1);
}

#define PETCTOR_FN 0x0070D1D0u
typedef LPVOID(__fastcall *PFN_PETCTOR)(LPVOID _this, int edx, int a2, int a3);
static PFN_PETCTOR pPetCtor_Orig = NULL;

static int PetSeen(int a2) {
    static int seen[512];
    static int n = 0;
    int i;
    for (i = 0; i < n; i++)
        if (seen[i] == a2) return 1;
    if (n < 512) seen[n++] = a2;
    return 0;
}

static int StrContains(const char *hay, const char *needle) {
    const char *h, *n;
    if (!hay || !needle) return 0;
    for (; *hay; hay++) {
        for (h = hay, n = needle; *n && *h == *n; h++, n++) { }
        if (!*n) return 1;
    }
    return 0;
}

static DWORD g_coins[256];
static int   g_coinN = 0;
static void CoinAdd(DWORD p) {
    int i;
    for (i = 0; i < g_coinN; i++) if (g_coins[i] == p) return;
    if (g_coinN < 256) g_coins[g_coinN++] = p;
}
static void CoinDel(DWORD p) {
    int i;
    for (i = 0; i < g_coinN; i++)
        if (g_coins[i] == p) { g_coins[i] = g_coins[--g_coinN]; return; }
}
static int IsCoin(DWORD p) {
    int i;
    for (i = 0; i < g_coinN; i++) if (g_coins[i] == p) return 1;
    return 0;
}

static LPVOID __fastcall PetCtorHook(LPVOID _this, int edx, int a2, int a3) {
    (void)edx;
    if ((DWORD)a2 > 0x00010000u && MemReadable((DWORD)a2)) {
        if (!PetSeen(a2))
            Log("[petlog] %s\r\n", (const char *)a2);
        if (StrContains((const char *)a2, "coin"))
            CoinAdd((DWORD)_this);
        else
            CoinDel((DWORD)_this);
    }
    return pPetCtor_Orig(_this, 0, a2, a3);
}

static void InstallPetLogger_JP983(void) {
    if (*(DWORD *)PETCTOR_FN != 0x6AEC8B55u) {
        Log("[petlog] prologo inesperado @ 0x%08X, pulando\r\n", PETCTOR_FN);
        return;
    }
    pPetCtor_Orig = (PFN_PETCTOR)HookFunc((PVOID)PETCTOR_FN, (PVOID)PetCtorHook);
    Log("[petlog] instalado: hook em CPetBody::CPetBody (0x%08X)\r\n", PETCTOR_FN);
}

typedef int(__fastcall *PFN_PETRENDER)(LPVOID _this, int edx);
typedef int(__fastcall *PFN_PETRENDER8)(LPVOID _this, int edx, int a2);
static PFN_PETRENDER  pPetRender_Orig  = NULL;
static PFN_PETRENDER8 pPetRender8_Orig = NULL;

static int __fastcall PetRenderHook(LPVOID _this, int edx) {
    int r;
    (void)edx;
    if (IsCoin((DWORD)_this)) {
        SetFillMode(FILL_WIRE);
        r = pPetRender_Orig(_this, 0);
        SetFillMode(FILL_SOLID);
    } else {
        r = pPetRender_Orig(_this, 0);
    }
    return r;
}

static int __fastcall PetRenderHook8(LPVOID _this, int edx, int a2) {
    int r;
    (void)edx;
    if (IsCoin((DWORD)_this)) {
        SetFillMode(FILL_WIRE);
        r = pPetRender8_Orig(_this, 0, a2);
        SetFillMode(FILL_SOLID);
    } else {
        r = pPetRender8_Orig(_this, 0, a2);
    }
    return r;
}

typedef int(__fastcall *PFN_GITEMDRAW)(LPVOID _this, int edx, int a2, int a3, int a4,
                                       float a5, int a6);
static PFN_GITEMDRAW pGItemDraw_Orig = (PFN_GITEMDRAW)GROUNDITEM_DRAW_FN;

static int __fastcall GItemDrawHook(LPVOID _this, int edx, int a2, int a3, int a4,
                                    float a5, int a6) {
    static int logged = 0;
    int r;
    (void)edx;
    if (!logged) {
        logged = 1;
        Log("[gitem] draw chamado! sub_85FA00 e o draw do GroundItem (this=%08X)\r\n",
            (DWORD)_this);
    }
    SetFillMode(FILL_WIRE);
    r = pGItemDraw_Orig(_this, 0, a2, a3, a4, a5, a6);
    SetFillMode(FILL_SOLID);
    return r;
}

static int PatchVtblSlot(DWORD slot, DWORD expected, DWORD wrapper, const char *tag) {
    DWORD old;
    if (*(DWORD *)slot != expected) {
        Log("[coinwire] slot %s inesperado @ %08X (=%08lX), pulando\r\n",
            tag, slot, *(DWORD *)slot);
        return 0;
    }
    VirtualProtect((LPVOID)slot, 4, PAGE_READWRITE, &old);
    *(DWORD *)slot = wrapper;
    VirtualProtect((LPVOID)slot, 4, old, &old);
    FlushInstructionCache(GetCurrentProcess(), NULL, 0);
    Log("[coinwire] slot %s @ %08X: escrevi %08lX, releitura=%08lX %s\r\n",
        tag, slot, wrapper, *(DWORD *)slot,
        (*(DWORD *)slot == wrapper) ? "(OK)" : "(REVERTIDO!)");
    return 1;
}

static void InstallCoinWireframe_JP983(void) {
    pPetRender_Orig  = (PFN_PETRENDER)PETBODY_RENDER_FN;
    pPetRender8_Orig = (PFN_PETRENDER8)PETBODY_RENDER8_FN;
    PatchVtblSlot(PETBODY_RENDER_SLOT,  PETBODY_RENDER_FN,  (DWORD)PetRenderHook,  "render+4");
    PatchVtblSlot(PETBODY_RENDER8_SLOT, PETBODY_RENDER8_FN, (DWORD)PetRenderHook8, "render+8");
    PatchVtblSlot(GROUNDITEM_VTBL_SLOT, GROUNDITEM_DRAW_FN, (DWORD)GItemDrawHook,
                  "grounditem");
    Log("[coinwire] instalado: CPetBody render+4/+8 (moeda coletada) + GroundItem draw (moeda parada)\r\n");
}

static void oneExec_PatchDynamicAndGG() {
    static int one = 0;
    PANGYAVER pangyaVersion;

    if (one == 1)
        return;

    one = 1;

    pangyaVersion = DetectPangyaVersion();

    switch (pangyaVersion) {
    case PANGYA_KR:
        PatchGG_KR();
        break;
    case PANGYA_JP:
        PatchGG_JP();
        PatchWindCap15_JP983();
        PatchShowNickChange_JP983();
        PatchScratchButton_JP983();
        PatchReticleYardDecimals_JP983();
        PatchCaliperTextGold_JP983();
        InstallWireframeToggle_JP983();
        InstallPetLogger_JP983();
        InstallCoinWireframe_JP983();
        PatchWorldTourUnlock_JP983();
        InstallWorldTourForceOpen_JP983();
        break;
    case PANGYA_TH:
        PatchGG_TH();
        break;
    case PANGYA_ID:
        PatchHS_ID();
        break;
    case PANGYA_BR:
        PatchHS_BR();
        break;
    case PANGYA_TW:
        PatchGG_TW();
        break;
    case PANGYA_US:
        PatchGG_US();
        break;
    case PANGYA_EU:
        PatchGG_EU();
        break;
    case PANGYA_SEA:
        PatchGG_SEA();
        break;
    default:
        Warning("It looks like no patch exists for this version of PangYa™.\nThe game will likely "
                "exit a couple minutes after detecting GameGuard is not present.");
    }

    PatchAddress();
}

static BOOL STDCALL InitCommonControlsExHook(const INITCOMMONCONTROLSEX *picce) {
    BOOL ret = pInitCommonControlsEx(picce);

    oneExec_PatchDynamicAndGG();

    return ret;
}

static VOID STDCALL GetStartupInfoAHook(LPSTARTUPINFOA lpStartupInfo) {
    pGetStartupInfoA(lpStartupInfo);

    if (isModuleAllocationBase(GETRETURNADDRESS(lpStartupInfo)))
        oneExec_PatchDynamicAndGG();
}

static VOID STDCALL GetStartupInfoWHook(LPSTARTUPINFOW lpStartupInfo) {
    pGetStartupInfoW(lpStartupInfo);

    if (isModuleAllocationBase(GETRETURNADDRESS(lpStartupInfo)))
        oneExec_PatchDynamicAndGG();
}

VOID InitComCtl32Hook() {
    hComCtl32Module = LoadLib("comctl32");
    hKernel32Module = LoadLib("kernel32");
    pInitCommonControlsEx = (PFNINITCOMMONCONTROLSEXPROC)HookProc(
        hComCtl32Module, "InitCommonControlsEx", (PVOID)InitCommonControlsExHook);
    pGetStartupInfoA = (PFNGETSTARTUPINFOAPROC)HookProc(hKernel32Module, "GetStartupInfoA",
                                                        (PVOID)GetStartupInfoAHook);
    pGetStartupInfoW = (PFNGETSTARTUPINFOWPROC)HookProc(hKernel32Module, "GetStartupInfoW",
                                                        (PVOID)GetStartupInfoWHook);
}

/**
 * Copyright 2018-2024 John Chadwick <john@jchw.io>
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

#include "patch.h"
#include "ld.h"

VOID Patch(LPVOID dst, LPCVOID src, DWORD size) {
    DWORD OldProtection;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &OldProtection);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, OldProtection, &OldProtection);
}

VOID InstallHook(PVOID pfnProc, LPCVOID pfnTargetProc) {
    DWORD dwOldProtect;
    DWORD dwRelAddr;
    BYTE pbHook[6] = {0xE9, 0x00, 0x00, 0x00, 0x00, 0xC3};

    if (VirtualProtect(pfnProc, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect) == 0) {
        FatalError("Failed to install hook: VirtualProtect failed. (%08x)", LastErr());
    }

    dwRelAddr = ((DWORD)pfnTargetProc - (DWORD)pfnProc - 5);
    memcpy(&pbHook[1], &dwRelAddr, 4);
    if (!WriteProcessMemory(GetCurrentProcess(), pfnProc, pbHook, 6, NULL)) {
        FatalError("Failed to install hook: WriteProcessMemory failed. (%08x)", LastErr());
    }

    if (VirtualProtect(pfnProc, 6, dwOldProtect, &dwOldProtect) == 0) {
        FatalError("Failed to install hook: VirtualProtect failed. (%08x)", LastErr());
    }

    if (FlushInstructionCache(GetCurrentProcess(), pfnProc, 6) == 0) {
        FatalError("Failed to install hook: FlushInstructionCache failed. (%08x)", LastErr());
    }
}

DWORD CountOpcodeBytes(LPCVOID fn, DWORD minBytes) {
    PBYTE fndata = (PBYTE)fn;
    DWORD count = 0;

    while (count < minBytes) {
        count += LenDisasm(&fndata[count]);
    }

    return count;
}

PBYTE BuildTrampoline(DWORD fn, DWORD prefixLen) {
    DWORD trampolineLen;
    DWORD oldProtect;
    DWORD relAddr;
    PBYTE codeblock;

    trampolineLen = prefixLen + 6;
    codeblock = (PBYTE)AllocMem(trampolineLen);

    memcpy(codeblock, (void *)fn, prefixLen);

    relAddr = (DWORD)fn - (DWORD)codeblock - 5;

    codeblock[prefixLen] = 0xE9;
    memcpy(&codeblock[prefixLen + 1], &relAddr, 4);

    codeblock[prefixLen + 5] = 0xC3;

    VirtualProtect(codeblock, trampolineLen, PAGE_EXECUTE_READWRITE, &oldProtect);

    return codeblock;
}

PVOID HookFunc(PVOID pfnProc, PVOID pfnTargetProc) {
    DWORD prefixLen, trampoline;

    prefixLen = CountOpcodeBytes(pfnProc, 6);

    trampoline = (DWORD)BuildTrampoline((DWORD)pfnProc, prefixLen);

    InstallHook((PCHAR)pfnProc, pfnTargetProc);

    return (PVOID)trampoline;
}

PVOID HookProc(HMODULE hModule, LPCSTR szName, PVOID pfnTargetProc) {
    PVOID pfnLibraryProc, pfnTrampolineProc;

    pfnLibraryProc = GetProc(hModule, szName);

    pfnTrampolineProc = HookFunc(pfnLibraryProc, pfnTargetProc);

    return pfnTrampolineProc;
}

PVOID BuildThiscallToStdcallThunk(LPCVOID pfnProc) {
    DWORD thunkLen = 9;
    DWORD oldProtect;
    DWORD relAddr;
    PBYTE codeblock;

    codeblock = (PBYTE)AllocMem(thunkLen);

    relAddr = (DWORD)pfnProc - (DWORD)&codeblock[8];

    codeblock[0] = 0x58;
    codeblock[1] = 0x51;
    codeblock[2] = 0x50;
    codeblock[3] = 0xE9;
    memcpy(&codeblock[4], &relAddr, 4);

    codeblock[8] = 0xC3;

    VirtualProtect(codeblock, thunkLen, PAGE_EXECUTE_READWRITE, &oldProtect);

    return codeblock;
}

PVOID BuildStdcallToThiscallThunk(LPCVOID pfnProc) {
    DWORD thunkLen = 9;
    DWORD oldProtect;
    DWORD relAddr;
    PBYTE codeblock;

    codeblock = (PBYTE)AllocMem(thunkLen);

    relAddr = (DWORD)pfnProc - (DWORD)&codeblock[8];

    codeblock[0] = 0x58;
    codeblock[1] = 0x59;
    codeblock[2] = 0x50;
    codeblock[3] = 0xE9;
    memcpy(&codeblock[4], &relAddr, 4);

    codeblock[8] = 0xC3;

    VirtualProtect(codeblock, thunkLen, PAGE_EXECUTE_READWRITE, &oldProtect);

    return codeblock;
}

PVOID BuildStdcallToVirtualThiscallThunk(DWORD dwVtblOffset) {
    DWORD thunkLen = 12;
    DWORD oldProtect;
    PBYTE codeblock;

    codeblock = (PBYTE)AllocMem(thunkLen);

    codeblock[0] = 0x58;
    codeblock[1] = 0x59;
    codeblock[2] = 0x50;

    codeblock[3] = 0x8b;
    codeblock[4] = 0x01;

    codeblock[5] = 0xff;
    codeblock[6] = 0xa0;
    memcpy(&codeblock[7], &dwVtblOffset, 4);

    codeblock[11] = 0xC3;

    VirtualProtect(codeblock, thunkLen, PAGE_EXECUTE_READWRITE, &oldProtect);

    return codeblock;
}

PVOID BuildStdcallToFastcallThunk(LPCVOID pfnProc) {
    DWORD thunkLen = 10;
    DWORD oldProtect;
    DWORD relAddr;
    PBYTE codeblock;

    codeblock = (PBYTE)AllocMem(thunkLen);

    relAddr = (DWORD)pfnProc - (DWORD)&codeblock[9];

    codeblock[0] = 0x58;
    codeblock[1] = 0x59;
    codeblock[2] = 0x5A;
    codeblock[3] = 0x50;
    codeblock[4] = 0xE9;
    memcpy(&codeblock[5], &relAddr, 4);

    codeblock[9] = 0xC3;

    VirtualProtect(codeblock, thunkLen, PAGE_EXECUTE_READWRITE, &oldProtect);

    return codeblock;
}

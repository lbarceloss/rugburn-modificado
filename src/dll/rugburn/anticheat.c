#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "anticheat.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

static const char* SECRET_KEY = "MeuAntiCheatPangya_SuperSecreto2026";

static int g_bCheatDetected = 0;

static int EncryptDataToBuffer(const char* input, char** outBuffer, DWORD* outLen) {
    HCRYPTPROV hProv;
    HCRYPTKEY hKey;
    HCRYPTHASH hHash;
    DWORD dwDataLen = lstrlenA(input);
    DWORD dwBufLen = dwDataLen + 16;
    BYTE* pbData = NULL;

    *outBuffer = NULL;
    *outLen = 0;

    if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return 0;
    }
    
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return 0;
    }
    
    if (!CryptHashData(hHash, (BYTE*)SECRET_KEY, lstrlenA(SECRET_KEY), 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return 0;
    }
    
    if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return 0;
    }
    
    pbData = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufLen);
    if (!pbData) {
        CryptDestroyKey(hKey);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return 0;
    }

    memcpy(pbData, input, dwDataLen);
    
    if (CryptEncrypt(hKey, 0, TRUE, 0, pbData, &dwDataLen, dwBufLen)) {
        *outBuffer = (char*)pbData;
        *outLen = dwDataLen;
        CryptDestroyKey(hKey);
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return 1;
    }
    
    HeapFree(GetProcessHeap(), 0, pbData);
    CryptDestroyKey(hKey);
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    
    return 0;
}

static const int BASE_POINTER_OFFSET = 0x00B006E8;
static const int offsets[] = { 0x1C, 0xC, 0xC, 0x14, 0x18, 0x0, 0x0 };
static const int numOffsets = sizeof(offsets) / sizeof(offsets[0]);

static DWORD WINAPI AntiCheatMonitor(LPVOID lpParam) {
    uintptr_t baseAddr = (uintptr_t)GetModuleHandleA("ProjectG.exe");
    if (!baseAddr) {
        baseAddr = (uintptr_t)GetModuleHandleA(NULL); 
    }

    int warnedSemprePangya = 0;
    int warnedSemprePangya2 = 0;
    int warnedAlpha = 0;
    int warnedDiametro = 0;
    int warnedRaio = 0;
    int warnedDeslocX = 0;
    int warnedDeslocY = 0;
    int warnedShotType = 0;

    while (1) {
        Sleep(500); 

        int currentCheckStatus = 0;

        __try {
            uint8_t* spBytes = (uint8_t*)(baseAddr + 0x29B192);
            if (spBytes[0] != 0xD8 || spBytes[1] != 0x80 || spBytes[2] != 0xE8) {
                if (!warnedSemprePangya) {
                    MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Sempre Pangya alterada (Fora do padrao D8 80 E8)!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                    warnedSemprePangya = 1;
                }
            } else {
                warnedSemprePangya = 0;
            }

            uint8_t* spBytes2 = (uint8_t*)(baseAddr + 0x275198);
            if (spBytes2[0] != 0xD9 || spBytes2[1] != 0x98 || spBytes2[2] != 0xE4) {
                if (!warnedSemprePangya2) {
                    MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Sempre Pangya alterada (Fora do padrao D9 98 E4)!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                    warnedSemprePangya2 = 1;
                }
            } else {
                warnedSemprePangya2 = 0;
            }

            uintptr_t current = *(uintptr_t*)(baseAddr + BASE_POINTER_OFFSET);
            if (current != 0) {
                for (int i = 0; i < numOffsets; i++) {
                    uintptr_t nextPtr = current + offsets[i];
                    if (i < numOffsets - 1) {
                        current = *(uintptr_t*)nextPtr;
                        if (current == 0) break;
                    } else {
                        current = nextPtr;
                    }
                }

                if (current != 0) {
                    uintptr_t finalAddress = current;
                    
                    float alpha = *(float*)(finalAddress + 0x8C);
                    if (fabsf(alpha - 90.0f) > 0.01f) {
                        if (!warnedAlpha) {
                            MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Alpha alterada!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                            warnedAlpha = 1;
                        }
                    } else warnedAlpha = 0;

                    float diametro = *(float*)(finalAddress + 0x88);
                    if (fabsf(diametro - 100.0f) > 0.01f) {
                        if (!warnedDiametro) {
                            MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Diametro alterada!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                            warnedDiametro = 1;
                        }
                    } else warnedDiametro = 0;

                    float raio = *(float*)(finalAddress + 0x84);
                    if (fabsf(raio - 20.0f) > 0.01f) {
                        if (!warnedRaio) {
                            MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Circulo Raio alterada!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                            warnedRaio = 1;
                        }
                    } else warnedRaio = 0;

                    float deslocX = *(float*)(finalAddress + 0x78);
                    if (fabsf(deslocX) < 0.001f) { 
                        if (!warnedDeslocX) {
                            MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Deslocamento X alterada (Esta como 0.00)!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                            warnedDeslocX = 1;
                        }
                    } else warnedDeslocX = 0;

                    float deslocY = *(float*)(finalAddress + 0x70);
                    if (fabsf(deslocY) < 0.001f) {
                        if (!warnedDeslocY) {
                            MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do Deslocamento Y alterada (Esta como 0.00)!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                            warnedDeslocY = 1;
                        }
                    } else warnedDeslocY = 0;

                    uint8_t shotType = *(uint8_t*)(finalAddress + 0x4C);
                    if (shotType != 0) {
                        if (!warnedShotType) {
                            MessageBoxA(NULL, "Alerta Anti-Cheat: Memoria do ShotType alterada!", "Anti-Cheat (ProjectG)", MB_ICONWARNING | MB_OK | MB_TOPMOST);
                            warnedShotType = 1;
                        }
                    } else warnedShotType = 0;
                }
            }
            
            if (warnedSemprePangya || warnedSemprePangya2 || warnedAlpha || 
                warnedDiametro || warnedRaio || warnedDeslocX || 
                warnedDeslocY || warnedShotType) {
                currentCheckStatus = 1;
            }

        } __except(EXCEPTION_EXECUTE_HANDLER) {
            
        }
        g_bCheatDetected = currentCheckStatus;
    }

    return 0;
}

static DWORD WINAPI ServerCommunicationThread(LPVOID lpParam) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in clientService;
    
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        return 1;
    }

    while(1) {
        ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ConnectSocket == INVALID_SOCKET) {
            Sleep(5000);
            continue;
        }

        clientService.sin_family = AF_INET;
        clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
        clientService.sin_port = htons(8080);

        iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            Sleep(5000); 
            continue;
        }
        
        char recvbuf[512];
        int recvbuflen = 512;
        
        while(1) {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen - 1, 0);
            if (iResult > 0) {
                recvbuf[iResult] = '\0';
                
                int isHacking = g_bCheatDetected;
                if ((rand() % 100) < 20) {
                    isHacking = 1; 
                }

                char statusMsg[1024];
                wsprintfA(statusMsg, "%s:%s", isHacking ? "HACK_DETECTED" : "OK", recvbuf);
                
                char* encryptedBytes = NULL;
                DWORD encryptedLen = 0;
                
                if (EncryptDataToBuffer(statusMsg, &encryptedBytes, &encryptedLen)) {
                    send(ConnectSocket, encryptedBytes, encryptedLen, 0);
                    HeapFree(GetProcessHeap(), 0, encryptedBytes);
                } else {
                    send(ConnectSocket, statusMsg, lstrlenA(statusMsg), 0);
                }
            }
            else if (iResult == 0) {
                break;
            }
            else {
                break;
            }
        }
        
        closesocket(ConnectSocket);
    }
    
    WSACleanup();
    return 0;
}

void StartAntiCheat() {
    CreateThread(NULL, 0, AntiCheatMonitor, NULL, 0, NULL);
    CreateThread(NULL, 0, ServerCommunicationThread, NULL, 0, NULL);
}

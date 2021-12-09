#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <vector>
#include <string>

void NopEx(BYTE* dest, int length) {
    DWORD protect;
    VirtualProtect(dest, length, PAGE_EXECUTE_READWRITE, &protect);
    memset(dest, 0x90, length);
    VirtualProtect(dest, length, protect, &protect);
}

void PatchEx(BYTE* dest, BYTE* src, int length) {
    DWORD protect;
    VirtualProtect(dest, length, PAGE_EXECUTE_READWRITE, &protect);
    memcpy(dest, src, length);
    VirtualProtect(dest, length, protect, &protect);
}

namespace Offsets {
    DWORD playerPointer = 0x02020F38;
    DWORD batteriesPointer = 0xAA0;
}


DWORD WINAPI HackThread(HMODULE hModule) {

    //create console
    AllocConsole();
    FILE* main;
    freopen_s(&main, "CONOUT$", "w", stdout);

    //get base module address and create player pointer
    uintptr_t baseModule = (uintptr_t)GetModuleHandle(L"OLGame.exe");
    uintptr_t plrPointer = *(uintptr_t*)(baseModule + Offsets::playerPointer);

    //some settings
    bool changed = true;
    bool bateries = false;

    while (true) {
        int batteries = *(int*)plrPointer + 0xAA0;

        if (changed) {
            system("cls");
            std::cout << "Successfully injected krjdN++'s cheat \n";
            std::cout << "Game: Outlast \n";
            std::cout << "Type: Internal\n";
            std::cout << "[NUMPAD1] Infinite batteries: " << ((bateries) ? "Enabled" : "Disabled") << "\n";
            changed = false;
        }

        if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
            bateries = !bateries;
            changed = true;
            continue;
        }

        //infinite ammo
        if (bateries) {
            *(int*)(plrPointer + Offsets::batteriesPointer) = 10;
            NopEx((BYTE*)(baseModule + 0xCC914F), 2);
        }
        else {
            PatchEx((BYTE*)(baseModule + 0xCC914F), (BYTE*)"\xFF\xC8", 2);
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
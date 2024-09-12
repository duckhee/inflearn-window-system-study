// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <stdio.h>

BOOL APIENTRY DllMain(
        HMODULE hModule,
        DWORD ul_reason_for_call,
        LPVOID lpReserved
) {
    /** DLL 호출이 되는 것에 따라서 특정 동작을 하기 위한 case 문*/
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}


void WINAPI DLLTestFunction(int nParam) {
    printf("DLLTestFunction(int nParam:%d)\r\n", nParam);
}
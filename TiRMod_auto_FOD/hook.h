#pragma once

#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void* create_hook(
    BYTE* src,       //Set up hook address
    const BYTE* dst, //Address of block with dublicate
    const int len)   //Your hook size
{
    BYTE* jmp;
    DWORD dwback;
    DWORD jumpto, newjump;
    VirtualProtect(src, len, PAGE_READWRITE, &dwback);
    if (src[0] == 0xE9)
    {
        jmp = (BYTE*)malloc(10);
        jumpto = (*(DWORD*)(src + 1)) + ((DWORD)src) + 5;
        newjump = (jumpto - (DWORD)(jmp + 5));
        jmp[0] = 0xE9;
        *(DWORD*)(jmp + 1) = newjump;
        jmp += 5;
        jmp[0] = 0xE9;
        *(DWORD*)(jmp + 1) = (DWORD)(src - jmp);
    }
    else
    {
        jmp = (BYTE*)malloc(5 + len);
        memcpy(jmp, src, len);
        jmp += len;
        jmp[0] = 0xE9;
        *(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
    }
    src[0] = 0xE9;
    *(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;
    for (int i = 5; i < len; i++)
        src[i] = 0x90;
    VirtualProtect(src, len, dwback, &dwback);
    return (jmp - len);
}
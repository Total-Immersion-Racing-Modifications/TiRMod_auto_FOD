#include "pch.h"
#include "hook.h"
#include "auto_fod.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {

        create_hook((BYTE*)read_cfg_hook_address, (BYTE*)calculate_fod, read_cfg_hook_size);
        create_hook((BYTE*)use_ar_1_hook_address, (BYTE*)use_ar_1, fmul_hook_size);
        create_hook((BYTE*)use_ar_2_hook_address, (BYTE*)use_ar_2, fmul_hook_size);
        create_hook((BYTE*)use_FOV_hook_address, (BYTE*)use_FOV, fmul_hook_size);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


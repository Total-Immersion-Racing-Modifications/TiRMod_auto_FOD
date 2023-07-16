#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

DWORD read_cfg_hook_address = 0x44B9BD;
DWORD read_cfg_hook_size = 0x8;
DWORD read_cfg_hook_return_address = 0x44B9C5;

DWORD fmul_hook_size = 0x6;

DWORD use_ar_1_hook_address = 0x4AC24D;
DWORD use_ar_1_hook_return_address = 0x4AC253;
DWORD use_ar_2_hook_address = 0x4ACAC9;
DWORD use_ar_2_hook_return_address = 0x4ACACF;
DWORD use_FOV_hook_address = 0x4AC67B;
DWORD use_FOV_hook_return_address = 0x4AC681;

int* FOV_value_address = reinterpret_cast<int*>(0x0051A3EC);
double* ar_1_value_address = reinterpret_cast<double*>(0x0051A3B8);
double* ar_2_value_address = reinterpret_cast<double*>(0x0051A408);

double game_ar = 0.75;
float fov = 0.017453292f;

float f = 0.017453292f;

_declspec(naked) int ConvertIntToFloat(int value)
{
	__asm
	{
		mov eax, [esp + 4]
		fild dword ptr[eax]
		fstp dword ptr[esp]
		ret
	}
}

_declspec(naked)int calculate_fod(void)
{
	int screen_width;
	int screen_height;
	float ar;
	_asm mov screen_width, [0x005569F0]
	_asm mov screen_height, [0x005569F4]

	__asm
	{
		push eax

		mov eax, screen_width
		fild dword ptr[eax]
		mov eax, screen_height
		fild dword ptr[eax]
		fdiv
		fstp dword ptr[ar]

		; calculate game ar, for example 9:16 and 3:4
		mov eax, screen_height
		fild dword ptr[eax]
		mov eax, screen_width
		fild dword ptr[eax]
		fdiv
		fstp qword ptr[game_ar]

		pop eax
	}

	// calculate FOV and write it
	__asm
	{
		fld1
		fstp st(1)
		fld1
		fadd st(0), st(0)
		fadd st(1), st(0)
		; 2.0 in st(0) and 3.0 in st(1)
		; 4.0 in st(0) and 3.0 in st(1)
		fadd st(0), st(0)
		fdivp st(1), st(0)
		; 0.75 in st(0)

		; ar * 0.75
		fstp st(1)
		fld dword ptr[ar]
		fmul st(1), st(0)
		fstp st(0)

		; calculate atan
		fstp st(1)
		fld1
		fpatan

		; create 4.0 in st(0)
		fstp st(1)
		fld1
		fadd st(0), st(0)
		fadd st(0), st(0)

		; 4.0 * atan_result
		fmul st(1), st(0)
		fstp st(0)

		; 4.0 * f * atan_result
		fstp st(1)
		fld dword ptr[f]
		fmul st(1), st(0)
		fstp st(0)

		; 4.0 * f * atan_result / PI
		fstp st(1)
		fldpi
		fdivp st(1), st(0)

		fstp dword ptr[fov]
	}

	// from game, don't touch
	_asm add esp, 10h
	_asm push 0x00868650
	_asm jmp dword ptr[read_cfg_hook_return_address]
}

_declspec(naked)int use_ar_1(void)
{
	__asm
	{
		fmul ds : game_ar
		jmp dword ptr[use_ar_1_hook_return_address]
	}
}

_declspec(naked)int use_ar_2(void)
{
	__asm
	{
		fmul ds : game_ar
		jmp dword ptr[use_ar_2_hook_return_address]
	}
}

_declspec(naked)int use_FOV(void)
{
	__asm
	{
		fmul ds : fov
		jmp dword ptr[use_FOV_hook_return_address]
	}
}
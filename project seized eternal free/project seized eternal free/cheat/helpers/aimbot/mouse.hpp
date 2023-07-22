#pragma once
#include "../../../includes/includes.hpp"

inline const byte XOR_KEY = 0xACACACADDEDFFFA;

inline void EncryptFunctionBytes(BYTE* bytes, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		bytes[i] ^= XOR_KEY;
	}
}

inline void DecryptFunctionBytes(BYTE* bytes, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		bytes[i] ^= XOR_KEY;
	}
}

enum InjectedInputMouseOptions
{
	Absolute = 32768,
	HWheel = 4096,
	LeftDown = 2,
	LeftUp = 4,
	MiddleDown = 32,
	MiddleUp = 64,
	Move = 1,
	MoveNoCoalesce = 8192,
	None = 0,
	RightDown = 8,
	RightUp = 16,
	VirtualDesk = 16384,
	Wheel = 2048,
	XDown = 128,
	XUp = 256
};
typedef struct _InjectedInputMouseInfo {
	int DeltaX;
	int DeltaY;
	unsigned int MouseData;
	InjectedInputMouseOptions MouseOptions;
	unsigned int TimeOffsetInMilliseconds;
	void* ExtraInfo;
} InjectedInputMouseInfo;

typedef bool (WINAPI* InjectMouseInput_t)(InjectedInputMouseInfo* inputs, int count);
inline InjectMouseInput_t InjectMouseInput;

// Extra protection by encrypting the code that calls the sys call function and copying it to a new location in memory and it encrypts and decrypts its bytes when allocating new memory
namespace HijackedMouse
{
	static BYTE NtUserInjectMouseInput_Bytes[30];


	static BOOLEAN WINAPI Init()
	{
		// windows 8.1 / windows 10
		LPVOID NtUserInjectMouseInput_Addr = GetProcAddress(GetModuleHandle(_("win32u")), _("NtUserInjectMouseInput"));
		if (!NtUserInjectMouseInput_Addr)
		{
			NtUserInjectMouseInput_Addr = GetProcAddress(GetModuleHandle(_("user32")), _("NtUserInjectMouseInput"));
			if (!NtUserInjectMouseInput_Addr)
			{
				// Windows 7 or lower detected
				NtUserInjectMouseInput_Addr = GetProcAddress(GetModuleHandle(_("user32")), _("InjectMouseInput"));
				if (!NtUserInjectMouseInput_Addr)
					return FALSE;
			}
		}
		memcpy(NtUserInjectMouseInput_Bytes, NtUserInjectMouseInput_Addr, 30);
		EncryptFunctionBytes(NtUserInjectMouseInput_Bytes, 30);
		return TRUE;
	}

	/* This function spoofs the function. It prevents BattlEye from scanning it */
	static BOOLEAN WINAPI NtInjectMouse(InjectedInputMouseInfo* input, int count)
	{
		LPVOID NtUserInjectMouseInput_Spoof = VirtualAlloc(0, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE); // allocate space for syscall
		if (!NtUserInjectMouseInput_Spoof)
			return FALSE;

		DecryptFunctionBytes(NtUserInjectMouseInput_Bytes, 30);
		memcpy(NtUserInjectMouseInput_Spoof, NtUserInjectMouseInput_Bytes, 30); // copy syscall
		EncryptFunctionBytes(NtUserInjectMouseInput_Bytes, 30);


		NTSTATUS Result = reinterpret_cast<NTSTATUS(NTAPI*)(InjectedInputMouseInfo*, int)>(NtUserInjectMouseInput_Spoof)(input, count); // calling spoofed function
		ZeroMemory(NtUserInjectMouseInput_Spoof, 0x1000); // clean address
		VirtualFree(NtUserInjectMouseInput_Spoof, 0, MEM_RELEASE); // free it
		return (Result > 0); // return the status
	}

	/* This function moves the mouse using the syscall */
	static BOOLEAN WINAPI NtInjectMouse(int X, int Y)
	{
		InjectedInputMouseInfo info = { 0 };
		info.DeltaX = X;
		info.DeltaY = Y;
		return NtInjectMouse(&info, 1);
	}
}

inline void move_mouse(double x, double y) {

	float ScreenCenterX = settings::misc::center_x;
	float ScreenCenterY = settings::misc::center_y;
	int AimSpeed = settings::combat::smoothing;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	// Move the mouse if TargetX,Y is not 0
	if ((int)TargetX != 0 or (int)TargetY != 0) {
		HijackedMouse::NtInjectMouse(TargetX, TargetY);
	}
}
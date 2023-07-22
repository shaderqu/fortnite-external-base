#include <Windows.h>
#include <iostream>
#include "driver.h"
#include <mutex>

kernel::driver::driver() : pid(0u) {}

kernel::driver::~driver() {}

typedef __int64(__fastcall* pfunc_hk_t)(uintptr_t a1);
pfunc_hk_t pHookFunc = (pfunc_hk_t)NULL;
pfunc_hk_t pHookFunc2 = (pfunc_hk_t)NULL;

std::mutex invokeuwu;
template<bool debug>
bool invoke_driver(fptr_data::kernel_com* com, fptr_data::kernel_opr op) {

	invokeuwu.lock();

	if (!pHookFunc) return false;

	com->error = fptr_data::kernel_err::unset_err;
	com->opr = op;
	if (!pHookFunc((uintptr_t)com) &&
		com->error == fptr_data::kernel_err::unset_err) {
		printf((" [ - ] Driver not loaded!\n"));
		invokeuwu.unlock();
		return false;
	}

	invokeuwu.unlock();

	if (com->success) return true && com->error == fptr_data::kernel_err::no_error;
	if (!debug) return false;

	switch (com->error) {
		case fptr_data::kernel_err::check_fail:
		{
			printf((" [ - ] Security check failure!\n"));
			break;
		}
		case fptr_data::kernel_err::invalid_data: {
			//printf((" [ - ] Invalid data!\n"));
			break;
		}
		case fptr_data::kernel_err::invalid_process: {
			printf((" [ - ] Invalid process!\n"));
			break;
		}
		case fptr_data::kernel_err::no_operation: {
			printf((" [ - ] Invalid funciton operation sent to driver!\n"));
			break;
		}
	}

	return false;
}

HMODULE ensure_dll_load() {
#define LOAD_DLL(str) LoadLibrary((str))

	LOAD_DLL(_("user32.dll"));

#undef LOAD_DLL
	return LoadLibrary((_("win32u.dll")));
}

bool kernel::driver::init() {

	if (!pHookFunc) {
		HMODULE hDll = GetModuleHandle((_("win32u.dll")));
		if (!hDll) {
			hDll = ensure_dll_load();
			if (!hDll) return false;
		}

		pHookFunc = (pfunc_hk_t)GetProcAddress(hDll, (_("NtUserWOWCleanup")));
		if (!pHookFunc) {
			pHookFunc = (pfunc_hk_t)NULL;
			return false;
		}
		// printf(_(" [ i ] drevor: %p"), pHookFunc);

	}

	return true;
}

uintptr_t kernel::driver::get_process_module(const char* name) {
	fptr_data::kernel_com com{};
	com.target_pid = this->pid;
	com.name = name;

	if (!invoke_driver<true>(&com, fptr_data::kernel_opr::get_process_module))
		return 0;
	return com.buffer;
}

uintptr_t kernel::driver::get_process_base(uint32_t _pid) {
	fptr_data::kernel_com com{};
	com.target_pid = _pid ? _pid : this->pid;

	if (invoke_driver<true>(&com, fptr_data::kernel_opr::get_process_base))
		return com.buffer;
	return 0;
}

std::mutex readuwu;
bool kernel::driver::read_buffer(uintptr_t addr, uint8_t* buffer, size_t size, size_t* transfer) {

	readuwu.lock();

	fptr_data::kernel_com com{};
	com.target_pid = this->pid;
	com.user_pid = GetCurrentProcessId();

	com.address = addr;
	com.buffer = (uintptr_t)buffer;
	com.size = size;

	if (!invoke_driver<true>(&com, fptr_data::kernel_opr::read))
	{
		readuwu.unlock();
		return false;
	}

	if (transfer)
		*transfer = com.transfer;
	readuwu.unlock();
	return true;
}

std::mutex writeuwu;
bool kernel::driver::write_buffer(uintptr_t addr, uint8_t* buffer, size_t size, size_t* transfer) {

	writeuwu.lock();

	fptr_data::kernel_com com{};
	com.target_pid = this->pid;
	com.user_pid = GetCurrentProcessId();

	com.address = addr;
	com.buffer = (uintptr_t)buffer;
	com.size = size;

	if (!invoke_driver<true>(&com, fptr_data::kernel_opr::write))
	{
		writeuwu.unlock();
		return false;
	}

	if (transfer)
		*transfer = com.transfer;
	writeuwu.unlock();
	return true;
}

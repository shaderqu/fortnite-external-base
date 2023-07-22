#include "helpers.h"

int helpers::setup_communication() {
	if (!driver.init()) {
		printf(_(" [ - ] Failed to initialize communication!\nPlease make sure driver is loaded.\n"));
		PSEF_IMPORT(Sleep)(3500);
		return psef_failed_driver;
	}
	uint32_t f_pid = FindProcessID(EncryptString("FortniteClient-Win64-Shipping.exe"));
	driver.attach(f_pid);
	defines::base_address = driver.get_process_base(f_pid);
	std::cout << _("\n PID: ") << f_pid << _("\n");
	std::cout << _("\n Base Address: 0x") << defines::base_address << _("\n");

	return psef_success;
}
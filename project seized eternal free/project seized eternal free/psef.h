#pragma once
#include <Windows.h>
#include <cstdint>

#define STATUS_SUCCESS   ((NTSTATUS)0x00000000L) 

namespace psef {

	enum status : int32_t {
		successful_action = STATUS_SUCCESS,
		failed_hijack = 0,
		failed_intialization = 0,
		failed_communication = 0,
		failed_window = 0,
		failed_to_get_pid = 0,
		failed_to_get_image_base = 0,
	};
}

#define psef_success psef::status::successful_action
#define psef_failed_driver psef::status::failed_communication
#define psef_failed_overlay psef::status::failed_hijack
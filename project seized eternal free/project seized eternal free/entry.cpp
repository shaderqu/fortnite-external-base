#include "includes/includes.hpp"
#include "cheat/loop.h"
#include "cheat/helpers/aimbot/aimbot.hpp"

/*
				  _                __        __                        __
	   ________  (_)___  ___  ____/ /  ___  / /____  _________  ____ _/ /
	  / ___/ _ \/ /_  / / _ \/ __  /  / _ \/ __/ _ \/ ___/ __ \/ __ `/ /
	 (__  )  __/ / / /_/  __/ /_/ /  /  __/ /_/  __/ /  / / / / /_/ / /
	/____/\___/_/ /___/\___/\__,_/   \___/\__/\___/_/  /_/ /_/\__,_/_/

	project seized eternal free (PSEF) fortnite cheat - usermode - developed by seized1019
*/

Overlay::DX11Overlay* PSEFOverlay = nullptr;

// Function to initialize the client and set the console title accordingly
__declspec(noinline) auto initialize_client() -> psef::status {
	// Create the console title string based on whether it's a debug build or not
	char title[256];
	if (defines::debug) {
		sprintf_s(title, _("project seized eternal | DEVELOPER MODE | Last build: %s, %s MET)"), __DATE__, __TIME__);
	}
	else {
		sprintf_s(title, _("project seized eternal | Last build: %s, %s MET)"), __DATE__, __TIME__);
	}

	PSEF_IMPORT(SetConsoleTitleA)(title);

	return psef_success;
}

// Function to wait for the Fortnite game window to be available
__declspec(noinline) auto wait_for_window() -> psef::status {
	cout << _("\n Waiting for Fortnite game window");

	// Keep searching for the Fortnite game window until it's found
	defines::game_window = FindWindow(0, (_("Fortnite  ")));
	while (!defines::game_window) {
		defines::game_window = FindWindow(0, (_("Fortnite  ")));
		PSEF_IMPORT(Sleep)(500);
	}
	cout << _("\n Found Fortnite game window");
	cout << _("\n Press F3 in lobby");
	while (true) {
		if (GetAsyncKeyState(VK_F3) & 1) {
			break;
		}

		PSEF_IMPORT(Sleep)(50);
	}

	return psef_success;
}

// Function to initialize the overlay and start the main loop
__declspec(noinline) auto init_overlay() -> psef::status {
	// Get the window of the Fortnite game and initialize the overlay
	Overlay::DX11Overlay::GetWindow(FindWindow(0, (_("Fortnite  "))));
	auto overlay = PSEFOverlay->InitOverlay();

	// Start a separate thread to cache players
	thread t(PlayerCache);
	t.detach();

	// Based on the result of overlay initialization, proceed accordingly
	switch (overlay) {
	case 0: {
		return psef_failed_overlay; // Overlay initialization failed
		break;
	}
	case 1: {
		HijackedMouse::Init(); // Initialize mouse hijacking
		while (PSEFOverlay->MainLoop(ActorLoop)) {
			// Main loop of the overlay, calling the actor_loop function
		}
		break;
	}
	}

	return psef_success;
}

// Main function of the cheat program
int main() {
	// Call functions to initialize the client, wait for the game window, setup communication with the driver, and initialize the overlay
	if (initialize_client() != psef_success)
		return psef::status::failed_intialization;

	if (wait_for_window() != psef_success)
		return psef::status::failed_window;

	if (helpers::setup_communication() != psef_success)
		return psef::status::failed_communication;

	if (init_overlay() != psef_success)
		return psef::status::failed_hijack;

	// If everything went well, display a message indicating successful cheat loading
	cout << _("\n Cheat loaded successfully");

	return 0;
}
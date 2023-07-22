#pragma once

namespace settings {

	namespace combat {
		inline bool aimbot = true;
		inline bool show_fov = true;
		inline int smoothing = 18.0f;
		inline int hitbox = 1;
		inline int fov = 11;
	}

	namespace visuals {
		inline bool esp = true;
		inline bool draw_box = true;
	}

	namespace misc {
		inline bool draw_fps = true;
		inline bool vsync = true;
		inline bool show_menu = false;
		inline int center_x;
		inline int center_y;
	}
}
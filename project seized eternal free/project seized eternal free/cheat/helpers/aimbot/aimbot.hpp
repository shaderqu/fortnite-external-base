#pragma once
void preform_aimbot(double closest_distance, uintptr_t closest_player) {
	// Check if the closest_distance is valid and aimbot is enabled in settings
	if (closest_distance != DBL_MAX && settings::combat::aimbot) {

		// If closest_player is not valid (NULL), return without doing anything
		if (!closest_player) return;

		// Read the mesh address of the closest player
		uintptr_t mesh = driver.read<uintptr_t>(closest_player + 0x310);
		if (!mesh) {
			closest_distance = FLT_MAX;
			closest_player = NULL;
		}

		int hitboxid = 106;
		switch (settings::combat::hitbox)
		{
		case 0:
			hitboxid = 106; // Head
			break;
		case 1:
			hitboxid = 67; // Neck
			break;
		case 2:
			hitboxid = 7; // Chest
			break;
		}

		vector3 head3d = get_bone(mesh, hitboxid);
		vector3 head2d = w2s(head3d);
		// Calculate the distance between the crosshair and the target hitbox
		double dx = head2d.x - (screen_width / 2);
		double dy = head2d.y - (screen_height / 2);
		float dist = sqrtf(dx * dx + dy * dy);

		// If the target is out of theFOV or head2d.x,y are invalid,
		// reset closest_distance and closest_player.
		if (dist > settings::combat::fov or head2d.x == 0 or head2d.y == 0) {
			closest_distance = FLT_MAX;
			closest_player = NULL;
		}

		if (is_visible(mesh))
			move_mouse(head2d.x, head2d.y);
	}
}
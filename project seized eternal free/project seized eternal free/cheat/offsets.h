#include <cstddef>

enum offsets : ptrdiff_t {
	PersistentLevel = 0x30, // almost never changes
	Uworld = 0xEE2F8E8, // updated
	OwningGameInstance = 0x1b8, // updated
	PlayerCameraManager = 0x348, // updated
	LocalPlayers = 0x38, // almost never change
	AcknowledgedPawn = 0x338, //updated
	PlayerState = 0x2B0, // updated
	PlayerController = 0x30, // updated
	PlayerArray = 0x2A8, // updated
	RootComponent = 0x198, // updated
	RelativeLocation = 0x128, // updated
	AActor = 0x98, // almost never changes
	CurrentWeapon = 0x948, // updated
	WeaponData = 0x438, // updated
	Mesh = 0x318, // updated
	Levels = 0x170, // updated
	Tier = 0x73, // updated
	TeamID = 0x10D8, // updated
	GameState = 0x158, // updated
	ReviveFromDBNOTime = 0x46b8, // updated
	PawnPrivate = 0x308, // updated
};

#include <cstddef>

enum offsets : ptrdiff_t {
	PersistentLevel = 0x30, // almost never changes
	Uworld = 0xEA84A78, // updated
	OwningGameInstance = 0x1b8, // updated
	PlayerCameraManager = 0x340, // updated
	LocalPlayers = 0x38, // almost never change
	AcknowledgedPawn = 0x330, //updated
	PlayerState = 0x2A8, // updated
	PlayerController = 0x30, // updated
	PlayerArray = 0x2a0, // updated
	RootComponent = 0x190, // updated
	RelativeLocation = 0x128, // updated
	AActor = 0x98, // almost never changes
	CurrentWeapon = 0x948, // updated
	WeaponData = 0x438, // updated
	Mesh = 0x310, // updated
	Levels = 0x170, // updated
	Tier = 0x73, // updated
	TeamID = 0x10C8, // updated
	GameState = 0x158, // updated
	ReviveFromDBNOTime = 0x4698, // updated
	PawnPrivate = 0x300, // updated
};
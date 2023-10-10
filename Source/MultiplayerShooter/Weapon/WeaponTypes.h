#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),
	EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"),
	EWT_LaserPistol UMETA(DisplayName = "Laser Pistol"),
	EWT_LaserAssaultRifle UMETA(DisplayName = "Laser Assault Rifle"),
	EWT_LaserSubmachineGun UMETA(DisplayName = "Laser SubmachineGun"),
	EWT_LaserShotgun UMETA(DisplayName = "Laser Shotgun"),
	EWT_LaserSniperRifle UMETA(DisplayName = "Laser Sniper Rifle"),
	EWT_LaserGrenadeLauncher UMETA(DisplayName = "Laser GrenadeLauncher"),
	EWT_LaserRocketLauncher UMETA(DisplayName = "Laser RocketLauncher"),
	EWT_Flag UMETA(DisplayName = "Flag"),
	EWT_Sword UMETA(DisplayName = "Sword"),


	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};
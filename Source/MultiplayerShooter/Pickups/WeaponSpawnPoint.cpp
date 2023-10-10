// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSpawnPoint.h"
#include "MultiplayerShooter/Weapon/Weapon.h"

AWeaponSpawnPoint::AWeaponSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnWeaponTimer((AActor*)nullptr);
}

void AWeaponSpawnPoint::SpawnWeapon()
{
	int32 NumWeaponClasses = WeaponClasses.Num();
	if (NumWeaponClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumWeaponClasses - 1);
		SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClasses[Selection], GetActorTransform());

		if (HasAuthority() && SpawnedWeapon)
		{
			SpawnedWeapon->OnDestroyed.AddDynamic(this, &AWeaponSpawnPoint::StartSpawnWeaponTimer);
		}
	}
}

void AWeaponSpawnPoint::SpawnWeaponTimerFinished()
{
	if (HasAuthority())
	{
		SpawnWeapon();
	}
}

void AWeaponSpawnPoint::StartSpawnWeaponTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::RandRange(SpawnWeaponTimeMin, SpawnWeaponTimeMax);
	GetWorldTimerManager().SetTimer(SpawnWeaponTimer, this, &AWeaponSpawnPoint::SpawnWeaponTimerFinished, SpawnTime);
}

void AWeaponSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


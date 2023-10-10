// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveSpawnPoint.h"
#include "MultiplayerShooter/Miscellaneous/Explosives.h"

AExplosiveSpawnPoint::AExplosiveSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void AExplosiveSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnExplosiveTimer((AActor*)nullptr);

}

void AExplosiveSpawnPoint::SpawnExplosive()
{
	int32 NumExplosiveClasses = ExplosiveClasses.Num();
	if (NumExplosiveClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumExplosiveClasses - 1);
		SpawnedExplosive = GetWorld()->SpawnActor<AExplosives>(ExplosiveClasses[Selection], GetActorTransform());

		if (HasAuthority() && SpawnedExplosive)
		{
			SpawnedExplosive->OnDestroyed.AddDynamic(this, &AExplosiveSpawnPoint::StartSpawnExplosiveTimer);
		}
	}
}

void AExplosiveSpawnPoint::SpawnExplosiveTimerFinished()
{
	if (HasAuthority())
	{
		SpawnExplosive();
	}
}

void AExplosiveSpawnPoint::StartSpawnExplosiveTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::RandRange(SpawnExplosiveTimeMin, SpawnExplosiveTimeMax);
	GetWorldTimerManager().SetTimer(SpawnExplosiveTimer, this, &AExplosiveSpawnPoint::SpawnExplosiveTimerFinished, SpawnTime);
}

void AExplosiveSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




#include "ActorSpawnPoint.h"

AActorSpawnPoint::AActorSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

}

void AActorSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnActorTimer((AActor*)nullptr);
}
void AActorSpawnPoint::SpawnActor()
{
	int32 NumActorClasses = ActorClasses.Num();
	if (NumActorClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumActorClasses - 1);
		SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorClasses[Selection], GetActorTransform());

		if (HasAuthority() && SpawnedActor)
		{
			SpawnedActor->OnDestroyed.AddDynamic(this, &AActorSpawnPoint::StartSpawnActorTimer);
		}
	}
}

void AActorSpawnPoint::SpawnActorTimerFinished()
{
	if (HasAuthority())
	{
		SpawnActor();
	}
}

void AActorSpawnPoint::StartSpawnActorTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::RandRange(SpawnActorTimeMin, SpawnActorTimeMax);
	GetWorldTimerManager().SetTimer(SpawnActorTimer, this, &AActorSpawnPoint::SpawnActorTimerFinished, SpawnTime);
}

void AActorSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


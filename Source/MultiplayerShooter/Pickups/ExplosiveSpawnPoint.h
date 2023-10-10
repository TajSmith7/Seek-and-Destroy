// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveSpawnPoint.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AExplosiveSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AExplosiveSpawnPoint();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AExplosives>> ExplosiveClasses;
	UPROPERTY()
	AExplosives* SpawnedExplosive;
	void SpawnExplosive();
	void SpawnExplosiveTimerFinished();

	UFUNCTION()
	void StartSpawnExplosiveTimer(AActor* DestroyedActor);
private:

	FTimerHandle SpawnExplosiveTimer;

	UPROPERTY(EditAnywhere)
		float SpawnExplosiveTimeMin;

	UPROPERTY(EditAnywhere)
		float SpawnExplosiveTimeMax;
public:	
	virtual void Tick(float DeltaTime) override;

};

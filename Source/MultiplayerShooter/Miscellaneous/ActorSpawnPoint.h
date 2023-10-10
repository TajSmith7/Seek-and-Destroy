// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawnPoint.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AActorSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AActorSpawnPoint();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AActor>> ActorClasses;
	UPROPERTY()
		AActor* SpawnedActor;
	void SpawnActor();
	void SpawnActorTimerFinished();

	UFUNCTION()
		void StartSpawnActorTimer(AActor* DestroyedActor);
private:

	FTimerHandle SpawnActorTimer;

	UPROPERTY(EditAnywhere)
		float SpawnActorTimeMin;

	UPROPERTY(EditAnywhere)
		float SpawnActorTimeMax;
public:	
	virtual void Tick(float DeltaTime) override;

};

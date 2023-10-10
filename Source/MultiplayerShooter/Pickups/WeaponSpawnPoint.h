// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawnPoint.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AWeaponSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponSpawnPoint();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadOnly)
		AWeapon* SpawnedWeapon;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;

	
	void SpawnWeapon();
	void SpawnWeaponTimerFinished();
	class AShooterCharacter* Character;
	UFUNCTION()
	void StartSpawnWeaponTimer(AActor* DestroyedActor);
private:

	FTimerHandle SpawnWeaponTimer;

	UPROPERTY(EditAnywhere)
	float SpawnWeaponTimeMin;

	UPROPERTY(EditAnywhere)
	float SpawnWeaponTimeMax;
public:

};

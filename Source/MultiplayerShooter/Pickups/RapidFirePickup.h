// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "RapidFirePickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ARapidFirePickup : public APickup
{
	GENERATED_BODY()
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere)
		float RapidFireBuff = 2.f;

	UPROPERTY(EditAnywhere)
		float RapidFireBuffTime = 30.f;

};

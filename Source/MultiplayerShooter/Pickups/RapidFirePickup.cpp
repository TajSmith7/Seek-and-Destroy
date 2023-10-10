// Fill out your copyright notice in the Description page of Project Settings.


#include "RapidFirePickup.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "MultiplayerShooter/ShooterComponents/BuffComponent.h"



void ARapidFirePickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		if (ShooterCharacter->GetPickupSound())
		{
			ShooterCharacter->PlayPickupSound();
		}

		UBuffComponent* Buff = ShooterCharacter->GetBuff();
		if (Buff)
		{
			Buff->BuffRapidFire(RapidFireBuff, RapidFireBuffTime);
		}
	}
	Destroy();
}
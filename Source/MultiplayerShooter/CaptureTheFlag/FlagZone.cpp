// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"
#include "Components/SphereComponent.h"
#include "MultiplayerShooter/Weapon/Flag.h"
#include "MultiplayerShooter/GameMode/CaptureTheFlagGameMode.h"
#include "GameFramework/Actor.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"



AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	SetRootComponent(ZoneSphere);
}

void AFlagZone::BeginPlay()
{
	Super::BeginPlay();
	
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnSphereOverlap);
}

void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFlag* OverlappingFlag = Cast<AFlag>(OtherActor);
	if (OverlappingFlag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Flag Valid"));

	}
	if (OverlappingFlag  && OverlappingFlag->GetTeam() != Team)
	{
		UE_LOG(LogTemp, Warning, TEXT("Flag Is In Sphere"));

		ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
		if (GameMode)
		{
			GameMode->FlagCaptured(OverlappingFlag, this);
			UE_LOG(LogTemp, Warning, TEXT("Flag Captured"));

		}
		OverlappingFlag->ResetFlag();
		UE_LOG(LogTemp, Warning, TEXT("Flag Reset"));

	}
}



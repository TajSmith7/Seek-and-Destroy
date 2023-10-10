// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/Weapon/Weapon.h"
#include "MultiplayerShooter/ShooterComponents/CombatComponent.h"
#include "MultiplayerShooter/ShooterComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "ShooterAnimInstance.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"
#include "MultiplayerShooter/GameMode/ShooterGameMode.h"
#include "MultiplayerShooter/GameMode/LobbyGameMode.h"
#include "MultiplayerShooter/GameState/ShooterGameState.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"
#include "MultiplayerShooter/Weapon/WeaponTypes.h"
#include "Components/AudioComponent.h"
#include "MultiplayerShooter/Pickups/Pickup.h"
#include "Components/BoxComponent.h"
#include "MultiplayerShooter/ShooterComponents/LagCompensationComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "MultiplayerShooter/Miscellaneous/Explosives.h"
#include "MultiplayerShooter/PlayerStart/TeamPlayerStart.h"
#include "MultiplayerShooter/Weapon/Flag.h"




// Sets default values
AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	//GetCharacterMovement()->SetIsReplicated(true);

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT(" Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Hit boxes for Server-side rewind

	Head = CreateDefaultSubobject<UBoxComponent>(TEXT("Head"));
	Head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), Head);

	Pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("Pelvis"));
	Pelvis->SetupAttachment(GetMesh(), FName("Pelvis"));
	HitCollisionBoxes.Add(FName("Pelvis"), Pelvis);


	Spine_01 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_01"));
	Spine_01->SetupAttachment(GetMesh(), FName("spine_01"));
	HitCollisionBoxes.Add(FName("spine_01"), Spine_01);


	Spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_02"));
	Spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), Spine_02);


	Spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("Spine_03"));
	Spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), Spine_03);


	JetpackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("JetpackBox"));
	JetpackBox->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("jetpack"), JetpackBox);


	UpperArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArm_L"));
	UpperArm_L->SetupAttachment(GetMesh(), FName("UpperArm_L"));
	HitCollisionBoxes.Add(FName("UpperArm_L"), UpperArm_L);


	LowerArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArm_L"));
	LowerArm_L->SetupAttachment(GetMesh(), FName("lowerArm_L"));
	HitCollisionBoxes.Add(FName("lowerArm_L"), LowerArm_L);


	Hand_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_L"));
	Hand_L->SetupAttachment(GetMesh(), FName("Hand_L"));
	HitCollisionBoxes.Add(FName("Hand_L"), Hand_L);


	UpperArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperArm_R"));
	UpperArm_R->SetupAttachment(GetMesh(), FName("UpperArm_R"));
	HitCollisionBoxes.Add(FName("UpperArm_R"), UpperArm_R);


	LowerArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerArm_R"));
	LowerArm_R->SetupAttachment(GetMesh(), FName("lowerArm_R"));
	HitCollisionBoxes.Add(FName("lowerArm_R"), LowerArm_R);


	Hand_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Hand_R"));
	Hand_R->SetupAttachment(GetMesh(), FName("Hand_R"));
	HitCollisionBoxes.Add(FName("Hand_R"), Hand_R);


	Thigh_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_L"));
	Thigh_L->SetupAttachment(GetMesh(), FName("Thigh_L"));
	HitCollisionBoxes.Add(FName("Thigh_L"), Thigh_L);


	Calf_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Calf_L"));
	Calf_L->SetupAttachment(GetMesh(), FName("calf_L"));
	HitCollisionBoxes.Add(FName("calf_L"), Calf_L);


	Foot_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_L"));
	Foot_L->SetupAttachment(GetMesh(), FName("Foot_L"));
	HitCollisionBoxes.Add(FName("Foot_L"), Foot_L);


	Thigh_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Thigh_R"));
	Thigh_R->SetupAttachment(GetMesh(), FName("Thigh_R"));
	HitCollisionBoxes.Add(FName("Thigh_R"), Thigh_R);


	Calf_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Calf_R"));
	Calf_R->SetupAttachment(GetMesh(), FName("calf_R"));
	HitCollisionBoxes.Add(FName("calf_R"), Calf_R);

	Foot_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Foot_R"));
	Foot_R->SetupAttachment(GetMesh(), FName("Foot_R"));
	HitCollisionBoxes.Add(FName("Foot_R"), Foot_R);

	
	Antenna_R = CreateDefaultSubobject<UBoxComponent>(TEXT("Antenna_R"));
	Antenna_R->SetupAttachment(GetMesh(), FName("antenna_02_r"));
	HitCollisionBoxes.Add(FName("antenna_R"), Antenna_R);


	Antenna_L = CreateDefaultSubobject<UBoxComponent>(TEXT("Antenna_L"));
	Antenna_L->SetupAttachment(GetMesh(), FName("antenna_02_l"));
	HitCollisionBoxes.Add(FName("antenna_L"), Antenna_L);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, OverlappingItem, COND_OwnerOnly);
	DOREPLIFETIME(AShooterCharacter, Health);
	DOREPLIFETIME(AShooterCharacter, Shield);
	DOREPLIFETIME(AShooterCharacter, bDisableGameplay);

}



void AShooterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();

}

void AShooterCharacter::RotateInPlace(float DeltaTime)
{
	if (Combat && (Combat->bHoldingTheFlag || Combat->bHoldingSword))
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (Combat && Combat->EquippedWeapon && Combat->bHoldingSword == false)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void AShooterCharacter::FinishTalking()
{
	bIsTalking = false;
}

void AShooterCharacter::Eliminate(APlayerController* AttackerController, bool bPlayerLeftGame)
{
	FString AttackerName = FString();
	AShooterPlayerController* AttackerShooterController = Cast<AShooterPlayerController>(AttackerController);
	if (AttackerShooterController)
	{
		AShooterPlayerState* AttackerShooterPlayerState = Cast<AShooterPlayerState>(AttackerShooterController->PlayerState);
		if (AttackerShooterPlayerState)
		{
			AttackerName = AttackerShooterPlayerState->GetPlayerName();
		}
	}
	DropOrDestroyWeapons();
	MulticastEliminate(AttackerName, bPlayerLeftGame);
}

void AShooterCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->GetWeaponType() == EWeaponType::EWT_Flag)
	{
		AFlag* Flag = Cast<AFlag>(Weapon);
		Flag->Dropped();
	}
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void AShooterCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		if (Combat->bHoldingTheFlag)
		{
			DropOrDestroyWeapon(Combat->EquippedFlag);
		}
	}
}

void AShooterCharacter::OnPlayerStateInitialized()
{
	ShooterPlayerState->AddToScore(0.f);
	ShooterPlayerState->AddToDefeats(0);
	SetTeamColor(ShooterPlayerState->GetTeam());
	SetSpawnPoint();
}

void AShooterCharacter::SetSpawnPoint()
{
	if (HasAuthority() && ShooterPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		TArray<AActor*>PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);

		TArray<AActor*> Shooters;
		UGameplayStatics::GetAllActorsOfClass(this, AShooterCharacter::StaticClass(), Shooters);

		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == ShooterPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		if (TeamPlayerStarts.Num() > 0)
		{
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			SetActorLocationAndRotation(ChosenPlayerStart->GetActorLocation(), ChosenPlayerStart->GetActorRotation());
		}
	}
}

void AShooterCharacter::Destroyed()
{
	Super::Destroyed();

	if (EliminationBotComponent)
	{
		EliminationBotComponent->DestroyComponent();
	}

	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	bool bMatchNotInProgress = ShooterGameMode && ShooterGameMode->GetMatchState() != MatchState::InProgress;
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void AShooterCharacter::MulticastPlayEliminatorSound_Implementation()
{
	if (bIsTalking ) return;
	switch (bIsWinning)
	{
	case true:
		if (TopScorerSound)
		{
			bIsTalking = true;
			UGameplayStatics::SpawnSoundAtLocation(this, TopScorerSound, GetActorLocation());
		}
		break;
	case false:
		if (EliminatorSound)
		{
			bIsTalking = true;
			UGameplayStatics::SpawnSoundAtLocation(this, EliminatorSound, GetActorLocation());
		}
		break;
	}
	FinishTalking();
}

void AShooterCharacter::MulticastPlayTopScorerSound_Implementation()
{
	if (bIsTalking) return;

	if (TopScorerSound)
	{
		bIsTalking = true;
		UGameplayStatics::SpawnSoundAtLocation(this, TopScorerSound, GetActorLocation());
	}
	FinishTalking();
}

void AShooterCharacter::MulticastEliminate_Implementation(const FString& AttackerName, bool bPlayerLeftGame)
{
	bEliminated = true;
	bLeftGame = bPlayerLeftGame;
	PlayEliminationMontage();

	//Start to Dissolve Character
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	if (FaceDissolveMaterialInstance)
	{
		DynamicFaceDissolveMaterialInstance = UMaterialInstanceDynamic::Create(FaceDissolveMaterialInstance, this);
		GetMesh()->SetMaterial(1, DynamicFaceDissolveMaterialInstance);
		DynamicFaceDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicFaceDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();
	
	//Disable Character Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	if (ShooterPlayerController)
	{
		DisableInput(ShooterPlayerController);
		ShooterPlayerController->SetHUDWeaponAmmo(0);
		ShooterPlayerController->SetHUDWeaponName("");
	}

	//Disable Character Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Spawn Elimination bot
	if (EliminationBotEffect)
	{
		FVector EliminationBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		EliminationBotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EliminationBotEffect, EliminationBotSpawnPoint, GetActorRotation());
	}
	if (EliminationBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, EliminationBotSound, GetActorLocation());
	}
	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	GetWorldTimerManager().SetTimer(EliminateTimer, this, &AShooterCharacter::EliminateTimerFinished, EliminateDelay);

}

void AShooterCharacter::EliminateTimerFinished()
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	if (ShooterGameMode && !bLeftGame)
	{
		ShooterGameMode->RequestRespawn(this, Controller);
	}
	LobbyGameMode = LobbyGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : LobbyGameMode;
	if (LobbyGameMode && !bLeftGame)
	{
		LobbyGameMode->RequestRespawn(this, Controller);
	}
	if (bLeftGame && IsLocallyControlled())
	{
		OnLeftGame.Broadcast();
	}
	
}

void AShooterCharacter::ServerLeaveGame_Implementation()
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	ShooterPlayerState = ShooterPlayerState == nullptr ? GetPlayerState<AShooterPlayerState>() : ShooterPlayerState;
	if (ShooterGameMode && ShooterPlayerState)
	{
		ShooterGameMode->PlayerLeftGame(ShooterPlayerState);
	}
}

void AShooterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
		DynamicFaceDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);

	}
}

void AShooterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &AShooterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}



void AShooterCharacter::MulticastGainedTheLead_Implementation()
{
	if (CrownSystem == nullptr) return;
	bIsWinning = true;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 50.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}

}

void AShooterCharacter::MulticastLostTheLead_Implementation()
{
	bIsWinning = false;
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}

}

void AShooterCharacter::SetTeamColor(ETeam Team)
{
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_NoTeam:
		GetMesh()->SetMaterial(0,OriginalMaterial);
		DissolveMaterialInstance = OriginalDissolveMaterialInstance;
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMaterialInstance;
		break;
	case ETeam::ET_RedTeam:
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMaterialInstance;
		break;
	}
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	UpdateHUDShield();

	PlaySpawnMontage();
	if (SpawnSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, SpawnSound, GetActorLocation(), GetActorRotation());
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AShooterCharacter::ReceiveDamage);
	}
	if (AttachedGrenade)
	{
		AttachedGrenade->SetVisibility(false);
	}
	if (Buff && GetMesh() && GetMesh()->GetMaterial(0))
	{
		Buff->SetInitialMaterial(GetMesh()->GetMaterial(0), GetMesh()->GetMaterial(1));
	}
	
}



void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(GetCharacterMovement()->MaxWalkSpeed, GetCharacterMovement()->MaxWalkSpeedCrouched);
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
		//if (GetEquippedWeapon()) Buff->SetInitialFireRate(GetEquippedWeapon()->GetFireRate());

	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AShooterPlayerController>(Controller);
		}
	}
}

void AShooterCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (Combat->EquippedWeapon->GetWeaponType() != EWeaponType::EWT_Sword && FireWeaponMontage)
		{
			AnimInstance->Montage_Play(FireWeaponMontage);
			FName SectionName;
			SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
		if (Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Sword && SwordMontage)
		{
			AnimInstance->Montage_Play(SwordMontage);
			FName SectionName;
			SectionName = bAiming ? FName("SwingAim") : FName("SwingNormal");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
}

void AShooterCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		case EWeaponType::EWT_LaserPistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_LaserSubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_LaserAssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_LaserShotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_LaserSniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_LaserGrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		case EWeaponType::EWT_LaserRocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::PlayEliminationMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EliminationMontage)
	{
		AnimInstance->Montage_Play(EliminationMontage);
	}
}

void AShooterCharacter::PlaySpawnMontage()
{

	if (EliminationBotEffect)
	{
		FVector EliminationBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		EliminationBotComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EliminationBotEffect, EliminationBotSpawnPoint, GetActorRotation());
	}
	if (EliminationBotSpawnSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, EliminationBotSpawnSound, GetActorLocation());
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SpawnMontage)
	{
		AnimInstance->Montage_Play(SpawnMontage);
	}
}

void AShooterCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

void AShooterCharacter::PlaySwapWeaponsMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapWeaponsMontage)
	{
		AnimInstance->Montage_Play(SwapWeaponsMontage);
	}
}

void AShooterCharacter::PlayPickupSound()
{
	if (bIsTalking) return;
	if (PickupSound)
	{
		bIsTalking = true;
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
		
	}
	FinishTalking();
}

void AShooterCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AShooterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("SwapWeapons", IE_Pressed, this, &AShooterCharacter::SwapWeaponsButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShooterCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AShooterCharacter::GrenadeButtonPressed);



	PlayerInputComponent->BindAxis("MoveForwardBackward", this, &AShooterCharacter::MoveForwardBackward);
	PlayerInputComponent->BindAxis("MoveRightLeft", this, &AShooterCharacter::MoveRightLeft);
	PlayerInputComponent->BindAxis("TurnRightLeft", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUpDown", this, &AShooterCharacter::LookUpDown);

}



void AShooterCharacter::MoveForwardBackward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRightLeft(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::LookUpDown(float Value)
{
	AddControllerPitchInput(Value);
}

void AShooterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AShooterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat->bHoldingTheFlag) return;
	if (Combat)
	{
			ServerEquipButtonPressed();
			
	}
}

void AShooterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
	}
}

void AShooterCharacter::SwapWeaponsButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat->bHoldingTheFlag) return;

	if (Combat)
	{
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerSwapWeaponsButtonPressed();
		if (Combat->ShouldSwapWeapons() && !HasAuthority() && Combat->CombatState == ECombatState::ECS_Unoccupied)
		{
			PlaySwapWeaponsMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}

void AShooterCharacter::ServerSwapWeaponsButtonPressed_Implementation()
{
	if (Combat && Combat->ShouldSwapWeapons())
	{
		Combat->SwapWeapons();
	}
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (Combat && (Combat->bHoldingTheFlag || Combat->bHoldingSword)) return;

	if (bDisableGameplay) return;


	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	if (Combat && (Combat->bHoldingTheFlag || Combat->bHoldingSword)) return;

	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->Reload();
	}
}

void AShooterCharacter::AimButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag ) return;

	if (bDisableGameplay) return;

	if (Combat)
	{
		bAimButtonPressed = true;
		Combat->SetAiming(true);
	}
}

void AShooterCharacter::AimButtonReleased()
{
	if (Combat && Combat->bHoldingTheFlag ) return;

	if (bDisableGameplay) return;

	if (Combat)
	{
		bAimButtonPressed = false;
		Combat->SetAiming(false);
	}
}

void AShooterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // Standing still and not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // get yaw
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //  running or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // get yaw
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
	
}

void AShooterCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//Map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

//Function to stop simulated proxies from turning due to jittering effect
void AShooterCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else 
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AShooterCharacter::Jump()
{
	if (Combat && Combat->bHoldingTheFlag) return;

	if (bDisableGameplay) return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AShooterCharacter::FireButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;

	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->FireButtonPressed(true);

	}
	
	//else if (!bRapidFireActive && GetEquippedWeapon()) GetEquippedWeapon()->SetFireRate(GunFireRate);

}

void AShooterCharacter::FireButtonReleased()
{
	if (Combat && Combat->bHoldingTheFlag) return;

	if (bDisableGameplay) return;

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AShooterCharacter::GrenadeButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;

	if (Combat)
	{
		Combat->ThrowGrenade();
	}
}

void AShooterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	LobbyGameMode = LobbyGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : LobbyGameMode;

	if (bEliminated || bCantTakeDamage || (ShooterGameMode == nullptr && LobbyGameMode == nullptr))return;


	if (InstigatorController)
	{
		Damage = ShooterGameMode->CalculateDamage(InstigatorController, Controller, Damage);
	}

	float DamageToHealth = Damage;
	if (Shield > 0)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;

		}
		
	}
	AExplosives* Explosive = Cast<AExplosives>(DamageCauser);
	if (Explosive)
	{
		Health = FMath::Clamp(Health - DamageToHealth, 1.f, MaxHealth);
	}
	else
	{
		Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	}

	UpdateHUDHealth();
	UpdateHUDShield();
	if (DamageToHealth > 0) PlayHitReactMontage();
	
	if (Health == 0.f)
	{
		if (ShooterGameMode)
		{
			ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
			AShooterPlayerController* AttackerController = Cast<AShooterPlayerController>(InstigatorController);
			ShooterGameMode->PlayerEliminated(this, ShooterPlayerController, AttackerController);
		}
	}
	
}

void AShooterCharacter::UpdateHUDHealth()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AShooterCharacter::UpdateHUDShield()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

void AShooterCharacter::UpdateHUDAmmo()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if (ShooterPlayerController && Combat && Combat->EquippedWeapon)
	{
		ShooterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		ShooterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void AShooterCharacter::SpawnDefaultWeapon()
{
	ShooterGameMode = ShooterGameMode == nullptr ? GetWorld()->GetAuthGameMode<AShooterGameMode>() : ShooterGameMode;
	UWorld* World = GetWorld();
	if (ShooterGameMode && World && !bEliminated && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void AShooterCharacter::PollInit()
{
	if (ShooterPlayerState == nullptr)
	{
		ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
		if (ShooterPlayerState)
		{
			OnPlayerStateInitialized();

			AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
			if (ShooterGameState && ShooterGameState->TopScoringPlayers.Contains(ShooterPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
}

void AShooterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void AShooterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
		GetMesh()->SetVisibility(true);
	if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	{
		Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
	}
	if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
	{
		Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
	}
}

float AShooterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void AShooterCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void AShooterCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		PlayHitReactMontage();
	}
}



void AShooterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false); // When player character is the server host, hide pickupwidget for weapon
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled()) // Locally controlled on the server
	{
		if (OverlappingWeapon && OverlappingWeapon->GetWeaponState() != EWeaponState::EWS_Equipped && OverlappingWeapon->GetWeaponState() != EWeaponState::EWS_EquippedSecondary)
		{
			OverlappingWeapon->ShowPickupWidget(true); // When player character is the server host, show pickupwidget for weapon
		} 
	}
}

void AShooterCharacter::SetOverlappingItem(APickup* Item)
{
	if (OverlappingItem)
	{
		OverlappingItem->ShowPickupWidget(false); // When player character is the server host, hide pickupwidget for Pickup Item
	}
	OverlappingItem = Item;
	if (IsLocallyControlled()) // Locally controlled on the server
	{
		if (OverlappingItem)
		{
			OverlappingItem->ShowPickupWidget(true); // When player character is the server host, show pickupwidget for Pickup Item
		}
	}
}

bool AShooterCharacter::bIsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AShooterCharacter::bIsAiming()
{
	return  (Combat && Combat->bAiming);
}

AWeapon* AShooterCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}


FVector AShooterCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState AShooterCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}



void AShooterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon && OverlappingWeapon->GetWeaponState() != EWeaponState::EWS_Equipped && OverlappingWeapon->GetWeaponState() != EWeaponState::EWS_EquippedSecondary)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AShooterCharacter::OnRep_OverlappingItem(APickup* LastItem)
{
	if (OverlappingItem)
	{
		OverlappingItem->ShowPickupWidget(true);
	}
	if (LastItem)
	{
		LastItem->ShowPickupWidget(false);
	}
}

bool AShooterCharacter::IsLocallyReloading()
{
	
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

bool AShooterCharacter::IsHoldingTheFlag() const
{
	if (Combat == nullptr) return false;
	return Combat->bHoldingTheFlag;
}

bool AShooterCharacter::IsHoldingSword() const
{
	if (Combat == nullptr) return false;
	return Combat->bHoldingSword;
}

ETeam AShooterCharacter::GetTeam()
{
	ShooterPlayerState = ShooterPlayerState == nullptr ? GetPlayerState<AShooterPlayerState>() : ShooterPlayerState;
	if (ShooterPlayerState == nullptr) return ETeam::ET_NoTeam;
	return ShooterPlayerState->GetTeam();
}

void AShooterCharacter::SetHoldingTheFlag(bool bHolding)
{
	if (Combat == nullptr) return;
	Combat->bHoldingTheFlag = bHolding;
}
 
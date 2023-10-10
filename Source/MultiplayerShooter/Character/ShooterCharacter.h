// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/ShooterTypes/TurningInPlace.h"
#include "MultiplayerShooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "MultiplayerShooter/ShooterTypes/CombatState.h"
#include "MultiplayerShooter/ShooterTypes/Team.h"
#include "ShooterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class MULTIPLAYERSHOOTER_API AShooterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

//Play Montages
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayEliminationMontage();
	void PlaySpawnMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapWeaponsMontage();

	void PlayPickupSound();
	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(BlueprintCallable)
	void Eliminate(APlayerController* AttackerController, bool bPlayerLeftGame );
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminate(const FString& AttackerName, bool bPlayerLeftGame);
	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayEliminatorSound();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayTopScorerSound();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	bool bIsTalking = false;
	bool bBecameTopPlayer = false;
	float GunFireRate;

	bool bIsWinning = false;
	UPROPERTY(BlueprintReadWrite)
	bool bCanBeEliminated = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCantTakeDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SelectedCheckpoint;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();


	bool bRapidFireActive = false;
	void SpawnDefaultWeapon();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping = false;

	UFUNCTION(Server, Reliable)
		void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	UFUNCTION(BlueprintCallable)
	void SetTeamColor(ETeam Team);

protected:
	virtual void BeginPlay() override;

	void MoveForwardBackward(float Value);
	void MoveRightLeft(float Value);
	void LookUpDown(float Value);
	void Turn(float Value);
	void EquipButtonPressed();
	void SwapWeaponsButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;

	void FireButtonPressed();
	void FireButtonReleased();
	void GrenadeButtonPressed();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();
	void SetSpawnPoint();
	void OnPlayerStateInitialized();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
	//Poll for any relevant classes and initalize the HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);
	void FinishTalking();

	UPROPERTY()
	UAudioComponent* EliminatorSoundComponent;

	//Hit boxes used for server side rewind
	UPROPERTY(EditAnywhere)
	UBoxComponent* Head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_01;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* JetpackBox;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArm_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* UpperArm_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* LowerArm_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Hand_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Calf_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Thigh_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Calf_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Foot_R;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Antenna_L;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Antenna_R;
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AWeapon* OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_OverlappingItem)
	class APickup* OverlappingItem;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION()
	void OnRep_OverlappingItem(APickup* LastItem);


	//Shooter Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerSwapWeaponsButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	
	/* Animation Montages*/
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;


	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwordMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* EliminationMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* SpawnMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapWeaponsMontage;

	void PlayHitReactMontage();

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	//Player Health

	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;
	
	UFUNCTION()
	void OnRep_Health(float LastHealth);

	//Player Shield
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 100.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class AShooterPlayerController* ShooterPlayerController;

	bool bEliminated = false;

	FTimerHandle EliminateTimer;

	UPROPERTY(EditDefaultsOnly)
	float EliminateDelay = 2.75f;

	void EliminateTimerFinished();

	bool bLeftGame = false;

	
	

	AShooterCharacter* Attacker;

	//Dissolve Effect
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	//Dynamic instance that can be changed at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	//Dynamic instance that can be changed at runtime for face
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicFaceDissolveMaterialInstance;

	//Material Instance that is set on the BP, used with the dynamic material Instance
	UPROPERTY(EditAnywhere, Category = Elim, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* DissolveMaterialInstance;

	//Material Instance for bot face that is set on the BP, used with the dynamic face material Instance
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* FaceDissolveMaterialInstance;


	/**
	* Team Colors
	*/
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* RedDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* RedMaterial;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* BlueDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Elim, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* OriginalMaterial;

	UPROPERTY(EditAnywhere, Category = Elim, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* OriginalDissolveMaterialInstance;
	/*
	* Elimination Effects
	*/
	UPROPERTY(EditAnywhere)
	UParticleSystem* EliminationBotEffect;
	
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* EliminationBotComponent;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* EliminationBotSound;

	UPROPERTY(EditAnywhere)
	USoundCue* EliminationBotSpawnSound;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY()
	class UNiagaraComponent* CrownComponent;

	//Sound Variables

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundCue* EliminatorSound;

	/*UPROPERTY(EditAnywhere, Category = Combat)
	USoundCue* EliminatedSound;*/

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundCue* SpawnSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundCue* TopScorerSound;

	UPROPERTY()
	class AShooterPlayerState* ShooterPlayerState;

	//Grenade

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* AttachedGrenade;

	//DefaultWeapon
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY()
	class AShooterGameMode* ShooterGameMode;

	UPROPERTY()
	class ALobbyGameMode* LobbyGameMode;

public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	void SetOverlappingItem(APickup* Item);
	bool bIsWeaponEquipped();
	bool bIsAiming();
	bool bAimButtonPressed;

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	
	UFUNCTION(BlueprintCallable)
	AWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;

	UPROPERTY(EditAnywhere, Category = "WeaponRotationCorrection")
		float RightHandRotationRoll = 180;
	UPROPERTY(EditAnywhere, Category = "WeaponRotationCorrection")
		float RightHandRotationYaw = 1;
	UPROPERTY(EditAnywhere, Category = "WeaponRotationCorrection")
		float RightHandRotationPitch = -21;
		
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bEliminated; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE APickup* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	bool IsHoldingTheFlag() const;
	bool IsHoldingSword() const;
	ETeam GetTeam();
	void SetHoldingTheFlag(bool bHolding);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuffComponent();
	friend class AShooterCharacter;
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void BuffRapidFire(float BuffFireRate, float BuffTime);
	void BuffInvisibility(UMaterialInterface* BuffInvisibilityMaterial, UMaterialInterface* BuffInvisibilityFaceMaterial, float BuffTime);
	void BuffInvincibility(UMaterialInterface* BuffInvincibilityMaterial, float BuffTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);
private:
	UPROPERTY()
	class AShooterCharacter* Character;

	//Health Buff
	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	//Speed Buff
	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	//Shield Buff
	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0;
	float ShieldReplenishAmount = 0.f;

	UFUNCTION (NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	//Jump Buff
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

	//Rapid Fire Buff
	FTimerHandle RapidFireBuffTimer;
	void ResetRapidFire();
	float InitialFireRate;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRapidFireBuff(float FireRate);

	//Invisibility Buff
	FTimerHandle InvisibilityBuffTimer;
	void ResetInvisibility();

	UMaterialInterface* InitialMaterial;
	UMaterialInterface* InvisibilityMaterial;
	UMaterialInterface* InitialFaceMaterial;
	UMaterialInterface* InvisibilityFaceMaterial;


	UFUNCTION(NetMulticast, Reliable)
	void MulticastInvisibilityBuff(UMaterialInterface* InvisibleMaterial, UMaterialInterface* InvisibleFaceMaterial);

	//Invincibility Buff
	FTimerHandle InvincibilityBuffTimer;
	void ResetInvincibility();
	UMaterialInterface* InvincibilityMaterial;
	UFUNCTION(NetMulticast, Reliable)
		void MulticastInvincibilityBuff(UMaterialInterface* InvincibleMaterial);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float Velocity);
	void SetInitialFireRate(float FireRate);
	void SetInitialMaterial(UMaterialInterface* StarterMaterial, UMaterialInterface* StarterFaceMaterial);
};

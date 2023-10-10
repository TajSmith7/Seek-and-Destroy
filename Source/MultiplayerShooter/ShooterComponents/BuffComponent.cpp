// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MultiplayerShooter/Weapon/Weapon.h"



UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}





void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
	ShieldRampUp(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsEliminated()) return;

	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;

	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime)
{
	bReplenishingShield = true;
	ShieldReplenishRate = ShieldAmount / ReplenishTime;
	ShieldReplenishAmount += ShieldAmount;
}

void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsEliminated()) return;

	const float ShieldReplenishThisFrame = ShieldReplenishRate * DeltaTime;
	Character->SetShield(FMath::Clamp(Character->GetShield() + ShieldReplenishThisFrame, 0.f, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	ShieldReplenishAmount -= ShieldReplenishThisFrame;

	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield())
	{
		bReplenishingShield = false;
		ShieldReplenishAmount = 0.f;
	}
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (Character == nullptr) return;
	
	Character->GetWorldTimerManager().SetTimer(SpeedBuffTimer, this, &UBuffComponent::ResetSpeeds, BuffTime);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	}
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);

}

void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(JumpBuffTimer, this, &UBuffComponent::ResetJump, BuffTime);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpVelocity;
	}
	MulticastJumpBuff(BuffJumpVelocity);
}

void UBuffComponent::MulticastJumpBuff_Implementation(float JumpVelocity)
{
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	}
}

void UBuffComponent::SetInitialJumpVelocity(float Velocity)
{
	InitialJumpVelocity = Velocity;
}

void UBuffComponent::ResetJump()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity;
	MulticastJumpBuff(InitialJumpVelocity);
}

void UBuffComponent::BuffRapidFire(float BuffFireRate, float BuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(RapidFireBuffTimer, this, &UBuffComponent::ResetRapidFire, BuffTime);
	BuffFireRate = InitialFireRate;
	if (Character->GetEquippedWeapon())
	{
		Character->bRapidFireActive = true;
		BuffFireRate = BuffFireRate / 2;
		Character->GetEquippedWeapon()->SetFireRate(BuffFireRate);
	}
	MulticastRapidFireBuff(BuffFireRate);
}

void UBuffComponent::MulticastRapidFireBuff_Implementation(float FireRate)
{
	if (Character && Character->GetEquippedWeapon())
	{
		FireRate = InitialFireRate;
		if (Character->GetEquippedWeapon())
		{
			Character->bRapidFireActive = true;
			FireRate = FireRate / 2;
			Character->GetEquippedWeapon()->SetFireRate(FireRate);
		}
	}

}

void UBuffComponent::SetInitialFireRate(float FireRate)
{
	InitialFireRate = FireRate;
}

void UBuffComponent::ResetRapidFire()
{
	if (Character == nullptr || Character->GetEquippedWeapon() == nullptr) return;
	Character->bRapidFireActive = false;
	InitialFireRate = InitialFireRate * 2;
	Character->GetEquippedWeapon()->SetFireRate(InitialFireRate);
	MulticastRapidFireBuff(InitialFireRate);
}

void UBuffComponent::BuffInvisibility(UMaterialInterface* BuffInvisibilityMaterial, UMaterialInterface* BuffInvisibilityFaceMaterial, float BuffTime)
{
	if (Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(InvisibilityBuffTimer, this, &UBuffComponent::ResetInvisibility, BuffTime);

	if (Character->GetMesh())
	{
		Character->GetMesh()->SetMaterial(0,BuffInvisibilityMaterial);
		Character->GetMesh()->SetMaterial(1, BuffInvisibilityFaceMaterial);

	}
	MulticastInvisibilityBuff(BuffInvisibilityMaterial, BuffInvisibilityFaceMaterial);
}



void UBuffComponent::MulticastInvisibilityBuff_Implementation(UMaterialInterface* InvisibleMaterial, UMaterialInterface* InvisibleFaceMaterial)
{
	if (Character && Character->GetMesh())
	{
		Character->GetMesh()->SetMaterial(0, InvisibleMaterial);
		Character->GetMesh()->SetMaterial(1, InvisibleFaceMaterial);

	}
}

void UBuffComponent::SetInitialMaterial(UMaterialInterface* StarterMaterial, UMaterialInterface* StarterFaceMaterial)
{
	InitialMaterial = StarterMaterial;
	InitialFaceMaterial = StarterFaceMaterial;
}

void UBuffComponent::ResetInvisibility()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;

	Character->GetMesh()->SetMaterial(0, InitialMaterial);
	Character->GetMesh()->SetMaterial(1, InitialFaceMaterial);

	MulticastInvisibilityBuff(InitialMaterial, InitialFaceMaterial);
}

void UBuffComponent::BuffInvincibility(UMaterialInterface* BuffInvincibilityMaterial, float BuffTime)
{
	if (Character == nullptr) return;

	Character->bCanBeEliminated = false;
	Character->bCantTakeDamage = true;
	Character->GetWorldTimerManager().SetTimer(InvincibilityBuffTimer, this, &UBuffComponent::ResetInvincibility, BuffTime);

	if (Character->GetMesh())
	{
		Character->GetMesh()->SetMaterial(0, BuffInvincibilityMaterial);
	}
	MulticastInvincibilityBuff(BuffInvincibilityMaterial);
}

void UBuffComponent::MulticastInvincibilityBuff_Implementation(UMaterialInterface* InvincibleMaterial)
{
	if (Character && Character->GetMesh())
	{
		Character->GetMesh()->SetMaterial(0, InvincibleMaterial);
	}
}

void UBuffComponent::ResetInvincibility()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->bCanBeEliminated = true;
	Character->bCantTakeDamage = false;

	Character->GetMesh()->SetMaterial(0, InitialMaterial);
	MulticastInvincibilityBuff(InitialMaterial);
}
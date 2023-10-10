// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/ShooterTypes/CombatState.h"
#include "MultiplayerShooter/Weapon/Weapon.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter == nullptr) return;

	FVector Velocity = ShooterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false; // if CurrentAcceleration > 0 bIsAccelerating = true. Else false
	bWeaponEquipped = ShooterCharacter->bIsWeaponEquipped();
	EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	bIsCrouched = ShooterCharacter->bIsCrouched;
	bAiming = ShooterCharacter->bIsAiming();
	TurningInPlace = ShooterCharacter->GetTurningInPlace();
	bRotateRootBone = ShooterCharacter->ShouldRotateRootBone();
	bEliminated = ShooterCharacter->IsEliminated();
	bHoldingTheFlag = ShooterCharacter->IsHoldingTheFlag();
	bHoldingSword = ShooterCharacter->IsHoldingSword();

	//Offset Yaw for Strafing
	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90, 90.f);

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	//Setup for Fabrik IK. Setup left hand socket
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && ShooterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		ShooterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation );
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (ShooterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = ShooterCharacter->GetMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), ShooterCharacter->GetHitTarget());
			LookAtRotation.Roll += ShooterCharacter->RightHandRotationRoll;
			LookAtRotation.Pitch += ShooterCharacter->RightHandRotationPitch;
			LookAtRotation.Yaw += ShooterCharacter->RightHandRotationYaw;
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 100.f);
		}
		
		//Draw Debug line to check if Weapon and crosshairs line up
		/*FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), ShooterCharacter->GetHitTarget(), FColor::Blue);*/
	}
	//Can use fabrik if not reloading
	bUseFABRIK = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	bool bFABRIKOverride = ShooterCharacter->IsLocallyControlled() && ShooterCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade && ShooterCharacter->bFinishedSwapping ;
	if (bFABRIKOverride)
	{
		bUseFABRIK = !ShooterCharacter->IsLocallyReloading() ;
	}
	bUseAimOffsets = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShooterCharacter->GetDisableGameplay();
	bTransformRightHand = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !ShooterCharacter->GetDisableGameplay();
}

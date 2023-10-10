// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"
#include "MultiplayerShooter/ShooterComponents/LagCompensationComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "MultiplayerShooter/Interfaces/BulletHitInterface.h"
#include "Kismet/KismetMathLibrary.h"


void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		//Maps hit character to number of times hit
		TMap<AShooterCharacter*, uint32> HitMap;
		TMap<AShooterCharacter*, uint32> HeadshotHitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(FireHit.GetActor());
			if (ShooterCharacter)
			{

				const bool bHeadshot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadshot)
				{
					if (HeadshotHitMap.Contains(ShooterCharacter)) HeadshotHitMap[ShooterCharacter]++;
					else HeadshotHitMap.Emplace(ShooterCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(ShooterCharacter)) HitMap[ShooterCharacter]++;	
					else HitMap.Emplace(ShooterCharacter, 1);
				}
				
				


				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
				}
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint, 0.5f, FMath::FRandRange(-0.5f, 0.5f));
				}
			}
			if (FireHit.GetActor() != ShooterCharacter && HasAuthority() && InstigatorController && FireHit.GetActor() != OwnerPawn)
			{
				UGameplayStatics::ApplyDamage(FireHit.GetActor(), Damage, InstigatorController, this, UDamageType::StaticClass());
			}
		}
		TArray<AShooterCharacter*> HitCharacters;

		//Maps Character hit to total damage
		TMap<AShooterCharacter*, float> DamageMap;

		//Calculate body shot damage by multiplying times hit * Damage - Store in DamageMap
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		//Calculate Headshot damage by multiplying times hit * HeadshotDamage - Store in DamageMap
		for (auto HeadshotHitPair : HeadshotHitMap)
		{
			if (HeadshotHitPair.Key)
			{
				if (DamageMap.Contains(HeadshotHitPair.Key)) DamageMap[HeadshotHitPair.Key] += HeadshotHitPair.Value * HeadshotDamage;
				else DamageMap.Emplace(HeadshotHitPair.Key, HeadshotHitPair.Value * HeadshotDamage);

				HitCharacters.AddUnique(HeadshotHitPair.Key);
			}
		}

		//Loop through DamageMap to get total damage for Character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthoritativeDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthoritativeDamage && DamagePair.Key != OwnerPawn)
				{
					UGameplayStatics::ApplyDamage(
						DamagePair.Key, //Character that was hit
						DamagePair.Value,  //Damage calculated from the for loops above for body/headshots
						InstigatorController,
						this,
						UDamageType::StaticClass());
				}
			}
		}

		


		if (!HasAuthority() && bUseServerSideRewind)
		{
			ShooterOwnerCharacter = ShooterOwnerCharacter == nullptr ? Cast<AShooterCharacter>(OwnerPawn) : ShooterOwnerCharacter;
			ShooterOwnerController = ShooterOwnerController == nullptr ? Cast<AShooterPlayerController>(InstigatorController) : ShooterOwnerController;
			if (ShooterOwnerCharacter && ShooterOwnerController && ShooterOwnerCharacter->GetLagCompensation() && ShooterOwnerCharacter->IsLocallyControlled())
			{
				ShooterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(HitCharacters, Start, HitTargets, ShooterOwnerController->GetServerTime() - ShooterOwnerController->SingleTripTime);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLocation = SphereCenter + RandomVector;
		 FVector ToEndLocation = EndLocation - TraceStart;
		 ToEndLocation = TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size();

		HitTargets.Add(ToEndLocation);
	}

}

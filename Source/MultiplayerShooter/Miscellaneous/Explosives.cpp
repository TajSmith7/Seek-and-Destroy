// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosives.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"

// Sets default values
AExplosives::AExplosives()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

void AExplosives::Destroyed()
{
	if (HasAuthority())
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			this, //World context object
			ExplosiveDamage, //Base damage
			ExplosiveDamage / 4.f, //minimum damage
			GetActorLocation(), //Origin
			DamageInnerRadius, //Damage Inner Radius
			DamageOuterRadius, //Damage Outer Radius
			1.f, //Damage Falloff
			UDamageType::StaticClass(), //Damage Type Class
			TArray<AActor*>(), //Ignore Actors
			this
			);
	}
	Explode();
	
	Super::Destroyed();

	
}

void AExplosives::Explode()
{
	
	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	}
	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, GetActorLocation(), FRotator(0.f), true);
	}
	

}

// Called when the game starts or when spawned
void AExplosives::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AExplosives::ReceiveDamage);
	}
	
}

void AExplosives::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (Health > 0 && bDestroyed  == false)
	{
		Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	}

	if (Health <= 0.f)
	{
		OwnerCharacter = Cast<AShooterCharacter>(DamageCauser);
		if (OwnerCharacter) OwnerController = Cast<AShooterPlayerController>(OwnerCharacter->GetController());
		bDestroyed = true;
		Destroy();
	}
}

void AExplosives::ExplodeDamage()
{
	if (OwnerCharacter && HasAuthority())
	{
		if (OwnerController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this, //World context object
				ExplosiveDamage, //Base damage
				ExplosiveDamage / 4.f, //minimum damage
				GetActorLocation(), //Origin
				DamageInnerRadius, //Damage Inner Radius
				DamageOuterRadius, //Damage Outer Radius
				1.f, //Damage Falloff
				UDamageType::StaticClass(), //Damage Type Class
				TArray<AActor*>(), //Ignore Actors
				OwnerCharacter,//Damage Causer
				OwnerController // InstigatorController
			);
		}
	}
	
}

// Called every frame
void AExplosives::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*void AExplosives::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	}
	if (ExplodeParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticles, HitResult.Location, FRotator(0.f), true);
	}

	//Apply explosive damage
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(Actor, ExplosiveDamage, ShooterController, Shooter, UDamageType::StaticClass());
		UE_LOG(LogTemp, Warning, TEXT("Actor damaged by explosive: %s"), *Actor->GetName());
	}

	Destroy();
}*/

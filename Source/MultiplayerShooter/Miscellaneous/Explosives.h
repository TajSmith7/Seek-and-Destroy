// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiplayerShooter/Interfaces/InteractWithCrosshairsInterface.h"
#include "MultiplayerShooter/Interfaces/BulletHitInterface.h"
#include "Explosives.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AExplosives : public AActor, public IInteractWithCrosshairsInterface, public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosives();
	UPROPERTY(EditAnywhere)
	float ExplosiveDamage = 100;
	virtual void Destroyed() override;
	void Explode();
	
	UPROPERTY()
	class AShooterCharacter* OwnerCharacter;

	UPROPERTY()
	class AShooterPlayerController* OwnerController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void ExplodeDamage();
private:
	//Particles to spawn when hit by bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UParticleSystem* ExplodeParticles;

	//Sound to play when hit by bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class USoundCue* ExplodeSound;

	//Used to determine what actors overlap during explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class USphereComponent* OverlapSphere;

	//Mesh for the explosive
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ExplosiveMesh;

	//Damage Radius for explosive
	UPROPERTY(EditAnywhere)
		float DamageInnerRadius = 200.f;

	//Damage Radius for explosive
	UPROPERTY(EditAnywhere)
		float DamageOuterRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float Health = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		float MaxHealth = 1;


	bool bDestroyed = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) override;
};

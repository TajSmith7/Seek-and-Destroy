// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

namespace MatchState
{
	extern MULTIPLAYERSHOOTER_API const FName Cooldown; //Match duration has been reached. Display winner and start cooldown timer
}
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AShooterGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void PlayerEliminated(class AShooterCharacter* EliminatedCharacter, class AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, AController* ElimmedController);
	void PlayerLeftGame(class AShooterPlayerState* PlayerLeaving);

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	float LevelStartingTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchWarningTime = 10.f;

	bool bTeamsMatch = false;
protected:
	virtual void OnMatchStateSet() override;
private:
	UPROPERTY(VisibleAnywhere, meta =(AllowPrivateAccess = "true"))
	AShooterCharacter* ElimCharacter;

	float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};

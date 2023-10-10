// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ShooterGameState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AShooterGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AShooterPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray< AShooterPlayerState*> TopScoringPlayers;

	/**
	* Teams
	*/
		TArray< AShooterPlayerState*> RedTeam;
		TArray< AShooterPlayerState*> BlueTeam;

		void RedTeamScores();
		void BlueTeamScores();

		UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
		float RedTeamScore;

		UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
		float BlueTeamScore;

		UFUNCTION()
		void OnRep_RedTeamScore();

		UFUNCTION()
		void OnRep_BlueTeamScore();
private:

	float TopScore = 0.f;

	UPROPERTY()
	class AShooterCharacter* Character;

	UPROPERTY()
	AShooterCharacter* PreviousTopCharacter;

	
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameMode.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Gameframework/PlayerStart.h"
#include "MultiplayerShooter/PlayerState/ShooterPlayerState.h"
#include "MultiplayerShooter/GameState/ShooterGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AShooterGameMode::AShooterGameMode()
{
	bDelayedStart = true;
}

void AShooterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

float AShooterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AShooterGameMode::PlayerEliminated(AShooterCharacter* EliminatedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;


	AShooterPlayerState* AttackerPlayerState = AttackerController ? Cast<AShooterPlayerState>(AttackerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState = VictimController ? Cast<AShooterPlayerState>(VictimController->PlayerState) : nullptr;
	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && ShooterGameState)
	{
		TArray<AShooterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : ShooterGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		ShooterGameState->UpdateTopScore(AttackerPlayerState);
		if (ShooterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			AShooterCharacter* Leader = Cast<AShooterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
				//Leader->MulticastPlayTopScorerSound();
			}
		}


		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!ShooterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				AShooterCharacter* Loser = Cast<AShooterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (EliminatedCharacter)
	{
		ElimCharacter = EliminatedCharacter;
		EliminatedCharacter->Eliminate(AttackerController, false);
	}

	//Loops through all players controllers and calls broadcast elimination to display elimination text to all screens
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AShooterPlayerController* ShooterPlayer = Cast<AShooterPlayerController>(*It);
		if (ShooterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			ShooterPlayer->BroadcastElimination(AttackerPlayerState, VictimPlayerState); 
		}
	}
}


void AShooterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

		TArray<AActor*> Shooters;
		UGameplayStatics::GetAllActorsOfClass(this, AShooterCharacter::StaticClass(), Shooters);

		// Get the player start the furthest distance from other Shooters
		TArray<float> StockedDistances;

		for (int i = 0; i < PlayerStarts.Num(); i++)
		{
			float MinDistance = (PlayerStarts[i]->GetActorLocation() - Shooters[0]->GetActorLocation()).Size();
			for (int j = 1; j < Shooters.Num(); j++)
			{
				float Distance = (PlayerStarts[i]->GetActorLocation() - Shooters[j]->GetActorLocation()).Size();
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
				}
			}
			StockedDistances.Add(MinDistance);
		}

		float MaxDistance = StockedDistances[0];
		int32 Selection = 0;

		for (int i = 1; i < StockedDistances.Num(); i++)
		{
			if (MaxDistance < StockedDistances[i])
			{
				MaxDistance = StockedDistances[i];
				Selection = i;
			}
		}


		//int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

		//Respawn Player at Furthest player start from other players
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);

	}
}



void AShooterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AShooterPlayerController* ShooterPlayer = Cast<AShooterPlayerController>(*It);
		if (ShooterPlayer)
		{
			ShooterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}

	}
}

void AShooterGameMode::PlayerLeftGame(AShooterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();
	if (ShooterGameState && ShooterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		ShooterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AShooterCharacter* CharacterLeaving = Cast<AShooterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Eliminate(Cast<AShooterPlayerController>(PlayerLeaving->GetOwningController()), true);
	}
}



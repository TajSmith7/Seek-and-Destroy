// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagGameMode.h"
#include "MultiplayerShooter/Weapon/Flag.h"
#include "MultiplayerShooter/CaptureTheFlag/FlagZone.h"
#include "MultiplayerShooter/GameState/ShooterGameState.h"

void ACaptureTheFlagGameMode::PlayerEliminated(AShooterCharacter* EliminatedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	AShooterGameMode::PlayerEliminated(EliminatedCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	AShooterGameState* SGameState = Cast<AShooterGameState>(GameState);
	if (SGameState)
	{
		if (Zone->Team == ETeam::ET_BlueTeam)
		{
			SGameState->BlueTeamScores();
		}
		if (Zone->Team == ETeam::ET_RedTeam)
		{
			SGameState->RedTeamScores();
		}
	}
}

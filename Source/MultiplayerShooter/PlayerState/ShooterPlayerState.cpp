// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerState.h"
#include "MultiplayerShooter/Character/ShooterCharacter.h"
#include "MultiplayerShooter/PlayerController/ShooterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"



void AShooterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterPlayerState, Defeats);
	DOREPLIFETIME(AShooterPlayerState, Team);

}

void AShooterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (ScoreAmount > 0.f)
		{
			Character->MulticastPlayEliminatorSound();
		}
	}
	if (Controller)
	{
		Controller->SetHUDScore(GetScore());
	}
}

void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		//Character->MulticastPlayEliminatorSound();
	}
	if (Controller)
	{
		Controller->SetHUDScore(GetScore());

	}
}

void AShooterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}



void AShooterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
	}
	if (Controller)
	{
		Controller->SetHUDDefeats(Defeats);
	}
}

void AShooterPlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;
	AShooterCharacter* SCharacter = Cast<AShooterCharacter>(GetPawn());
	if (SCharacter)
	{
		SCharacter->SetTeamColor(Team);
	}
}

void AShooterPlayerState::OnRep_Team()
{
	AShooterCharacter* SCharacter = Cast<AShooterCharacter>(GetPawn());
	if (SCharacter)
	{
		SCharacter->SetTeamColor(Team);
	}
}



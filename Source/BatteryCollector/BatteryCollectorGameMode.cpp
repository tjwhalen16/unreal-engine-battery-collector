// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolume.h"

ABatteryCollectorGameMode::ABatteryCollectorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set base decay rate
	decay_rate_ = 0.01f;
}

void ABatteryCollectorGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Find and sotre all spawn volume actors before setting game state to EPlaying
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}

	// Start game
	SetCurrentState(EBatteryPlayState::EPlaying);

	// Set the score to beat
	ABatteryCollectorCharacter* player = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)); // Get the character
	if (player)
	{
		power_to_win_ = (player->get_initial_power()) * 1.25f; // Need 1.25 times intial power to win
	}

	if (HudWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ABatteryCollectorGameMode::Tick(float delta_time) 
{
	Super::Tick(delta_time);

	// Confirm player is ABatteryCollectorCharacter
	ABatteryCollectorCharacter* player = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
	if (player) {
		// If power is greater than needed to win, set game state to EWon
		if (player->get_current_power() > power_to_win_)
		{
			SetCurrentState(EBatteryPlayState::EWon);
		}
		else if (player->get_current_power() > 0)
		{ // If power is positive
			player->UpdateCurrentPower(-delta_time * decay_rate_ * player->get_initial_power()); // Update power based on frame rate and decay rate and initial power
		}
		else // Power is negative, game is over
		{
			SetCurrentState(EBatteryPlayState::EGameOver);
		}
	}
}

float ABatteryCollectorGameMode::GetPowerToWin() const
{
	return power_to_win_;
}

EBatteryPlayState ABatteryCollectorGameMode::GetCurrentState() const
{
	return CurrentState;
}

void ABatteryCollectorGameMode::SetCurrentState(EBatteryPlayState NewState)
{
	CurrentState = NewState; // Set state
	HandleNewState(CurrentState); // Do things that need to be done when switching to NewState
}

void ABatteryCollectorGameMode::HandleNewState(EBatteryPlayState NewState)
{
	switch (NewState)
	{
		case EBatteryPlayState::EPlaying:
		{
			// Start spawning 
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(true);
			}
		}
		break;
		case EBatteryPlayState::EWon:
		{
			// Stop spawning
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(false);
			}
		}
		break;
		case EBatteryPlayState::EGameOver:
		{
			UE_LOG(LogClass, Log, TEXT("Game Over"))
			// Stop spawning
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(false);
			}
			// Stop player input
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetCinematicMode(true, false, false, true, true);
			}
			// Destroy player animation
			ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
			if (Character)
			{
				Character->GetMesh()->SetSimulatePhysics(true);
				Character->GetMovementComponent()->MovementState.bCanJump = false;
			}
		}
		break;
		case EBatteryPlayState::EUnknown:
			break;
		default:
			break;
	}
}
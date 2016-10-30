// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "BatteryCollectorGameMode.generated.h"

// Enum to store current gamestate
UENUM(BlueprintType)
enum class EBatteryPlayState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

UCLASS(minimalapi)
class ABatteryCollectorGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABatteryCollectorGameMode();

	virtual void Tick(float delta_time) override;

	// Returns power needed to win - needed for HUD
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWin() const;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryPlayState GetCurrentState() const;
	void SetCurrentState(EBatteryPlayState NewState);

protected:
	// The rate of decay of the character's power level
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Power", Meta = (BlueprintProtected = "true"))
	float decay_rate_;

	// The power needed to win the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Power", Meta = (BlueprintProtected = "true"))
	float power_to_win_;

	// The widget class to use for our HUD screen
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, category = "Power", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> HudWidgetClass;

	// The instance based on HudWidgetClass
	UPROPERTY()
	class UUserWidget* CurrentWidget;

private:
	// Keeps track of current gamestate
	EBatteryPlayState CurrentState;

	TArray<class ASpawnVolume*> SpawnVolumeActors;

	// Handle any function calls that happen when gamestate changes
	void HandleNewState(EBatteryPlayState NewState);
};




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BATTERYCOLLECTOR_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Public accessor function for private component where_to_spawn_
	FORCEINLINE class UBoxComponent* get_where_to_spawn_() const { return where_to_spawn_; }

	// Returns a random point in the spawn volume
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	// Toggles wether or not spawn volume should spawn pickups
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetSpawningActive(bool bShouldSpawn);

protected:
	// The pickup to spawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class APickup> what_to_spawn_;

	FTimerHandle spawn_timer_;

	// Minimum spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float spawn_delay_range_low_;
	// Maximum spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float spawn_delay_range_high_;

private:
	// This is an invisible box that spawns pickups
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* where_to_spawn_;	

	// Spawns a pickup
	void SpawnPickup();

	// The current spawn delay
	float spawn_delay_;
};

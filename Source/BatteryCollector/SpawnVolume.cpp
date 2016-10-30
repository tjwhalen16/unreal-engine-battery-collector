// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "SpawnVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickup.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create spawn volume
	where_to_spawn_ = CreateDefaultSubobject<UBoxComponent>(TEXT("where_to_spawn"));
	RootComponent = where_to_spawn_;

	// Set the spawn delay range
	spawn_delay_range_low_ = 1.0f;
	spawn_delay_range_high_ = 4.5f;

}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ASpawnVolume::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FVector ASpawnVolume::GetRandomPointInVolume() {
	FVector spawn_origin = where_to_spawn_->Bounds.Origin;
	FVector spawn_extent = where_to_spawn_->Bounds.BoxExtent;
	
	return UKismetMathLibrary::RandomPointInBoundingBox(spawn_origin, spawn_extent);
}

void ASpawnVolume::SetSpawningActive(bool bShouldSpawn)
{
	if (bShouldSpawn)
	{
		// Set the timer on spawn pickup to a non-zero interval
		spawn_delay_ = FMath::FRandRange(spawn_delay_range_low_, spawn_delay_range_high_);
		GetWorldTimerManager().SetTimer(spawn_timer_, this, &ASpawnVolume::SpawnPickup, spawn_delay_, false);
	}
	else
	{
		// Stop the timer
		GetWorldTimerManager().ClearTimer(spawn_timer_);
	}
}

void ASpawnVolume::SpawnPickup() {
	// If we have something to spawn
	if (what_to_spawn_ != NULL) {
		// Check for a valid world
		UWorld* const world = GetWorld();
		if (world) {
			// Set the spawn parameters
			FActorSpawnParameters spawn_params;
			spawn_params.Owner = this;
			spawn_params.Instigator = Instigator;

			// Get a random location to spawn at
			FVector spawn_location = GetRandomPointInVolume();
			// Get a random rotation
			FRotator spawn_rotation;
			spawn_rotation.Yaw = FMath::FRand() * 360.0f;
			spawn_rotation.Pitch = FMath::FRand() * 360.0f;
			spawn_rotation.Roll = FMath::FRand() * 360.0f;

			// spawn the pickup
			APickup* const spawned_pickup = world->SpawnActor<APickup>(what_to_spawn_, spawn_location, spawn_rotation, spawn_params);

			// After spawning a pickup, reset the timer
			spawn_delay_ = FMath::FRandRange(spawn_delay_range_low_, spawn_delay_range_high_);
			GetWorldTimerManager().SetTimer(spawn_timer_, this, &ASpawnVolume::SpawnPickup, spawn_delay_, false);
		}
	}
}


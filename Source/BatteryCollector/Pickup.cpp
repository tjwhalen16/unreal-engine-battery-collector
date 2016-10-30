// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "Pickup.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //won't tick in this example
	// All pickups start active
	b_is_active_ = true;

	// Create static mesh component
	pickup_mesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = pickup_mesh_;

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay(); //Calls parent's BeginPlay()
	
}

// Called every frame
void APickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Returns active state
bool APickup::is_active() {
	return b_is_active_;
}

// Changes active state
void APickup::set_active(bool new_pickup_state) {
	b_is_active_ = new_pickup_state;
}

void APickup::WasCollected_Implementation() {
	// Log a debug message
	FString pickup_debug_string = GetName();
	UE_LOG(LogClass, Log, TEXT("You have collected %s"), *pickup_debug_string)
}


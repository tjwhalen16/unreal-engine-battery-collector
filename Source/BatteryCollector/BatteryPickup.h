// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYCOLLECTOR_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABatteryPickup();

	// Override was collected function - use implemenation because it's a Bluprint native event
	void WasCollected_Implementation() override;

	// Public accessor
	float get_battery_power();

protected:
	// The amount of power the battery gives to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BluprintProtected = "true"))
	float battery_power_;
	
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryCollector.h"
#include "BatteryPickup.h"


// Set default values
ABatteryPickup::ABatteryPickup() {
	get_pickup_mesh()->SetSimulatePhysics(true);

	// Set the base power level
	battery_power_ = 150;
}

void ABatteryPickup::WasCollected_Implementation() {
	// Use parent's pickup behavior
	Super::WasCollected_Implementation();

	//Destroy the battery
	Destroy();
}

float ABatteryPickup::get_battery_power() {
	return battery_power_;
}




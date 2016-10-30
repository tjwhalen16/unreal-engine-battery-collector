// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h" // Build tool looks for this as last in the list

UCLASS() // Lets editor know about this class, GC, keywords
class BATTERYCOLLECTOR_API APickup : public AActor
{
	GENERATED_BODY() // Engine linking
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Accessor for private mesh
	FORCEINLINE class UStaticMeshComponent* get_pickup_mesh() const { return pickup_mesh_; } //const = not allowed to modify mesh, just returns it

	/* Return wether or not pickup is active */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool is_active();

	/* Allows other classes to safely change pickup's active state */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void set_active(bool new_pickup_state);

	// Functionto call when the pickup is collected
	UFUNCTION(BlueprintNativeEvent)
	void WasCollected();
	virtual void WasCollected_Implementation();

protected: // Can only be changed by Pickup classes
	/* True when the pickup can be used, else false */
	bool b_is_active_;

private:
	// Static mesh to represent the pickup in the level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true")) //can see it exists, but can't change it
	class UStaticMeshComponent* pickup_mesh_;
	
};

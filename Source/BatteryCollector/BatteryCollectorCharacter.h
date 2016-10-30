// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "BatteryCollectorCharacter.generated.h"

UCLASS(config=Game)
class ABatteryCollectorCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* collection_sphere_;
public:
	ABatteryCollectorCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// Access intial power
	UFUNCTION(BlueprintPure, Category = "Power")
	float get_initial_power();

	// Access current power
	UFUNCTION(BlueprintPure, Category = "Power")
	float get_current_power();

	// Update character's power
	// Power_change is amount to change power by
	// > 0 increase power
	// < 0 decreases power
	UFUNCTION(BlueprintCallable, Category = "Power")
	void UpdateCurrentPower(float power_change);

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

private:
	void UpdateCurrentPowerIfBattery(class APickup* test_pickup);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	// Called when we press a pickup key to collect pickups inside the collection sphere
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void CollectPickups();

	// Starting power level of character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Power", Meta = (BlueprintProtected = "true"))
	float initial_power_;

	// How much to modify base speed... multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Power", Meta = (BlueprintProtected = "true"))
	float speed_factor_;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Power", Meta = (BlueprintProtected = "true"))
	float base_speed_;

	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
	void PowerChangeEffect();

private:
	// Current power level of character
	UPROPERTY(VisibleAnywhere, category = "Power")
	float current_power_;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* get_collection_sphere() const { return collection_sphere_; }
};


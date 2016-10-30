// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "BatteryCollectorCharacter.h"
#include "Pickup.h"
#include "BatteryPickup.h"

//////////////////////////////////////////////////////////////////////////
// ABatteryCollectorCharacter

ABatteryCollectorCharacter::ABatteryCollectorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a collection sphere
	collection_sphere_ = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	collection_sphere_->AttachTo(RootComponent);
	collection_sphere_->SetSphereRadius(200.f);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Set base power level
	initial_power_ = 2000.0f;
	current_power_ = initial_power_;

	// Set speed fields
	speed_factor_ = 0.75f;
	base_speed_ = 10.0f;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryCollectorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ABatteryCollectorCharacter::CollectPickups);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABatteryCollectorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABatteryCollectorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABatteryCollectorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABatteryCollectorCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABatteryCollectorCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABatteryCollectorCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABatteryCollectorCharacter::OnResetVR);
}


void ABatteryCollectorCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABatteryCollectorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void ABatteryCollectorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void ABatteryCollectorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::CollectPickups() {
	// Get all overlapping pickups
	TArray<AActor*> collected_actors;
	collection_sphere_->GetOverlappingActors(collected_actors);

	// For each pickup, collect it
	for (int i = 0; i < collected_actors.Num(); i++) {
		// Try to cast the actor to a APickup cast
		APickup* const test_pickup = Cast<APickup>(collected_actors[i]);
		// If cast is successful and pickup is active
		if (test_pickup && !test_pickup->IsPendingKill() && test_pickup->is_active()) {
			// Consume its power if it is a battery
			UpdateCurrentPowerIfBattery(test_pickup);
			// Collect the pickup
			test_pickup->WasCollected();
			//Deacivate the pickup
			test_pickup->set_active(false);
		}
	}
}

// Access intial power
float ABatteryCollectorCharacter::get_initial_power() {
	return initial_power_;
}

// Access current power
float ABatteryCollectorCharacter::get_current_power() {
	return current_power_;
}

// Update character's power
// Power_change is amount to change power by
// > 0 increase power
// < 0 decreases power
void ABatteryCollectorCharacter::UpdateCurrentPower(float power_change) {
	current_power_ += power_change;
	// Change speed based on the power
	GetCharacterMovement()->MaxWalkSpeed = base_speed_ + speed_factor_ * current_power_;
	// Call visual effect
	PowerChangeEffect();
}

void ABatteryCollectorCharacter::UpdateCurrentPowerIfBattery(APickup* test_pickup) {
	ABatteryPickup* const test_battery = Cast<ABatteryPickup>(test_pickup);

	if (test_battery) {
		UpdateCurrentPower(test_battery->get_battery_power());
	}
}
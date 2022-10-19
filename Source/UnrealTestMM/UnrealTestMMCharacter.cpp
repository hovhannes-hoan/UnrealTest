// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealTestMMCharacter.h"
#include "MainPlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AUnrealTestMMCharacter

AUnrealTestMMCharacter::AUnrealTestMMCharacter()
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	PlayerNameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameWidget"));
	PlayerNameWidget->SetupAttachment(GetCapsuleComponent());
	float height = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float offset = 30.f;
	PlayerNameWidget->SetRelativeLocation(FVector(0.f, 0.f, height + offset));
}

void AUnrealTestMMCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUnrealTestMMCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnrealTestMMCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AUnrealTestMMCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AUnrealTestMMCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AUnrealTestMMCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AUnrealTestMMCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AUnrealTestMMCharacter::OnResetVR);
}
void AUnrealTestMMCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("BeginPlay invoked on player character %s"), *GetName());
	HandlePlayerNameUpdate();
}

void AUnrealTestMMCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Display, TEXT("PossessedBy invoked on player character %s"), *GetName());
	HandlePlayerNameUpdate();

	PlayerNameWidget->SetVisibility(true);
}
void AUnrealTestMMCharacter::UnPossessed()
{
	UE_LOG(LogTemp, Display, TEXT("UnPossessed invoked on player character %s"), *GetName());
	UUserWidget* w = PlayerNameWidget->GetUserWidgetObject();
	ULabelWidget* labelWidget = Cast< ULabelWidget>(w);
	if (labelWidget) {
		labelWidget->SetPlayerName("UNPOSSESSED");
	}
	PlayerNameWidget->SetVisibility(false);
}

void AUnrealTestMMCharacter::OnRep_PlayerState()
{
	UE_LOG(LogTemp, Display, TEXT("OnRep_PlayerState invoked on player character %s"), *GetName());
	HandlePlayerNameUpdate();
}

void AUnrealTestMMCharacter::HandlePlayerNameUpdate()
{
	APlayerState* state = GetPlayerState();
	FString name = "INVALID_C";
	if (state)
	{
		name = state->GetPlayerName();

		UE_LOG(LogTemp, Display, TEXT("HandlePlayerNameUpdate Player name of character %s is : %s"), *GetName(), *name);
		UE_LOG(LogTemp, Display, TEXT("Setting name on player name widget..."));
		UUserWidget* w = PlayerNameWidget->GetUserWidgetObject();
		ULabelWidget* labelWidget = Cast< ULabelWidget>(w);
		if (labelWidget) {
			labelWidget->SetPlayerName(name);
			UE_LOG(LogTemp, Display, TEXT("Player name is applied."));
		}
		else {
			UE_LOG(LogTemp, Display, TEXT("Player name widget returned %s user widget object: "), w ? TEXT("valid") : TEXT("invalid"));
			if (w) {
				UE_LOG(LogTemp, Display, TEXT("WidgetClassName: %s"), *w->GetClass()->GetName());
			}
		}
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("PlayerState is null for character: %s"), *GetName());
	}
}


// Called every frame
void AUnrealTestMMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller)
	{
		APawn* playerPawn = controller->GetPawn();
		if (playerPawn) {
			FRotator rotator = UKismetMathLibrary::FindLookAtRotation(
				playerPawn->GetActorLocation(),
				UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation());
			rotator.Pitch = 0.f;
			rotator.Roll = 0.f;
			PlayerNameWidget->SetWorldRotation(rotator);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("ULabelWidget GetFirstPlayerController returned null"));
		}
	}
	else {

		UE_LOG(LogTemp, Error, TEXT("ULabelWidget GetFirstPlayerController is null"));
	}
}

//////////////////////////////////////////////////////////////////////////
/// Below are auto Generated methods, I haven't made any changes there

void AUnrealTestMMCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AUnrealTestMMCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AUnrealTestMMCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AUnrealTestMMCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealTestMMCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealTestMMCharacter::MoveForward(float Value)
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

void AUnrealTestMMCharacter::MoveRight(float Value)
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

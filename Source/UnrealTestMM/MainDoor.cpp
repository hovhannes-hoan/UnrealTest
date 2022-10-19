// Fill out your copyright notice in the Description page of Project Settings.


#include "MainDoor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMainDoor::AMainDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bTriggered = false;
	bDoorOpen = false;

	doorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = doorFrame;
	door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	door->SetupAttachment(doorFrame);
}

// Called when the game starts or when spawned
void AMainDoor::BeginPlay()
{
	Super::BeginPlay();
	// Setting the door to Closed position at the start of the game
	FRotator leverRotation = door->GetRelativeRotation();
	leverRotation.Yaw = doorRotationYaw_Closed;
	door->SetRelativeRotation(leverRotation);
}

void AMainDoor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AMainDoor, bTriggered);
	DOREPLIFETIME(AMainDoor, targetRotationValue);
	DOREPLIFETIME(AMainDoor, rotationAtServer);
}

// Called every frame
void AMainDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bTriggered) {
		FRotator currentRotation = door->GetRelativeRotation();
		currentRotation.Yaw = FMath::FInterpConstantTo(currentRotation.Yaw,
			targetRotationValue, DeltaTime, doorInteractionSpeed);
		door->SetRelativeRotation(currentRotation);
		if (HasAuthority()) {
			rotationAtServer = currentRotation;
			if (currentRotation.Yaw == targetRotationValue)
			{
				OnTriggerCompleted();
			}
		}
	}
}

void AMainDoor::OnServerRotationUpdate()
{
	UE_LOG(LogTemp, Log, TEXT("OnServerRotationUpdate is invoked in lever: %s"), *GetWorld()->GetFirstPlayerController()->GetName());
	door->SetRelativeRotation(rotationAtServer);
}

void AMainDoor::OpenDoor()
{
	UE_LOG(LogTemp, Log, TEXT("Triggering door invoked"));
	targetRotationValue = doorRotationYaw_FullyOpen;
	bTriggered = true;
}

void AMainDoor::CloseDoor()
{
	UE_LOG(LogTemp, Log, TEXT("Triggering door invoked"));
	targetRotationValue = doorRotationYaw_Closed;
	bTriggered = true;
}

void AMainDoor::OnTriggerCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("Triggering door completed"));
	bDoorOpen = !bDoorOpen;
	bTriggered = false;
}
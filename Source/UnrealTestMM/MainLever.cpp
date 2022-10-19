// Fill out your copyright notice in the Description page of Project Settings.


#include "MainLever.h"
#include "UnrealTestMMCharacter.h"
#include "MainPlayerController.h"
#include "MainDoor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMainLever::AMainLever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// the idea is that at OFF position lever's world rotation will be at 45 degrees pitch 
	// and at ON position it will be at 135 degrees pitch.
	// As the required lever relative rotation is just 90% and there is no need to be able to rotate it more 
	// (e.g. within 0 - 360 degrees), thus I am just setting a dummy parent scene component at 45 degrees pitch
	// and rotating the lever relatively by 90 degress on and off.
	leverBasePitchValue = 45.f;
	leverPitchValue_OFFPosition = 0.f;
	leverPitchValue_OnPosition = 90.f;
	interactionSpeed = 10.f;
	bReplicates = true;
	bTriggered = false;
	bTurnedOn = false;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	interactionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionArea"));
	interactionArea->SetupAttachment(RootComponent);
	interactionArea->OnComponentBeginOverlap.AddDynamic(this, &AMainLever::OnBeginOverlap);
	interactionArea->OnComponentEndOverlap.AddDynamic(this, &AMainLever::OnEndOverlap);

	leverCollisionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("LeverCollisionArea"));
	leverCollisionArea->SetupAttachment(RootComponent);

	leverAttachmentBaseAngle = CreateDefaultSubobject<USceneComponent>(TEXT("LeverAttachmentBaseAngle"));
	leverAttachmentBaseAngle->SetupAttachment(leverCollisionArea);
	FRotator leverRotation = leverAttachmentBaseAngle->GetRelativeRotation();
	leverRotation.Pitch = leverBasePitchValue;
	leverAttachmentBaseAngle->SetRelativeRotation(leverRotation);

	leverAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeverAttachmentPoint"));
	leverAttachmentPoint->SetupAttachment(leverAttachmentBaseAngle);

	leverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	leverMesh->SetupAttachment(leverAttachmentPoint);
}

void AMainLever::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AMainLever, bTriggered);
	DOREPLIFETIME(AMainLever, targetRotationValue);
	DOREPLIFETIME(AMainLever, rotationAtServer);
}

// Called when the game starts or when spawned
void AMainLever::BeginPlay()
{
	Super::BeginPlay();
	// Setting the lever at OFF position at the start of the game
	FRotator leverRotation = leverAttachmentPoint->GetRelativeRotation();
	leverRotation.Pitch = leverPitchValue_OFFPosition;
	leverAttachmentPoint->SetRelativeRotation(leverRotation);
}

// Called every frame
void AMainLever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bTriggered) {
		FRotator currentRotation = leverAttachmentPoint->GetRelativeRotation();
		currentRotation.Pitch = FMath::FInterpConstantTo(currentRotation.Pitch,
			targetRotationValue, DeltaTime, interactionSpeed);
		leverAttachmentPoint->SetRelativeRotation(currentRotation);
		if (HasAuthority()) {
			rotationAtServer = currentRotation;
			if (currentRotation.Pitch == targetRotationValue)
			{
				OnTriggerCompleted();
			}
		}
	}
}

void AMainLever::OnRotationUpdateNotify()
{
	UE_LOG(LogTemp, Log, TEXT("OnRotationUpdateNotify is invoked in lever: %s"), *GetWorld()->GetFirstPlayerController()->GetName());
	leverAttachmentPoint->SetRelativeRotation(rotationAtServer);
}

void AMainLever::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AUnrealTestMMCharacter* character = Cast<AUnrealTestMMCharacter>(OtherActor);
	if (character && OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Pawn)
	{
		UE_LOG(LogTemp, Log, TEXT("OnBeginOverlap is invoked in lever"));
		if (GEngine) {
			FString message = TEXT("Player character interaction with lever is enabled (Press E to trigger the lever");
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, message);
		}
		AController* controller = character->GetController();
		AMainPlayerController* PC = Cast<AMainPlayerController>(controller);
		if (PC) {
			EnableInteraction(PC, true);
		}
	}
}

void AMainLever::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AUnrealTestMMCharacter* character = Cast<AUnrealTestMMCharacter>(OtherActor);
	if (character && OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_Pawn)
	{
		UE_LOG(LogTemp, Log, TEXT("OnEndOverlap is invoked in lever"));
		if (GEngine) {
			FString message = TEXT("Player character interaction with lever is disabled");
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, message);
		}
		AController* controller = character->GetController();
		AMainPlayerController* PC = Cast<AMainPlayerController>(controller);
		if (PC) {
			EnableInteraction(PC, false);
		}
	}
}

void AMainLever::EnableInteraction(AMainPlayerController* PC, bool enable)
{
	PC->SetInteractionEnabled(enable);
}

void AMainLever::TriggerLever()
{
	UE_LOG(LogTemp, Log, TEXT("Triggering lever invoked"));
	if (bTurnedOn) {
		targetRotationValue = leverPitchValue_OFFPosition;
	}
	else {
		targetRotationValue = leverPitchValue_OnPosition;
	}
	bTurnedOn = !bTurnedOn;
	bTriggered = true;
}

void AMainLever::OnTriggerCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("Triggering lever completed"));
	bTriggered = false;

	// A better way of triggering through interaction would be through broadcasting an event through a delegate
	// There is no need for the lever itself to be aware of what it triggers (door or smth else)
	// The door on it's turn can subscribe to that delegate.
	// However for sake of simplicity I am getting the door actor directly here
	TArray<AActor*> FoundActors;
	AMainDoor* door;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainDoor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		door = Cast<AMainDoor>(FoundActors[0]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no actor of AMainDoor class placed in the level"));
		return;
	}
	if (bTurnedOn) {
		door->OpenDoor();
	}
	else {
		door->CloseDoor();
	}
}
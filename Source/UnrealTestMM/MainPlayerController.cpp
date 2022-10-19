// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "MainLever.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"


void AMainPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("Interact", IE_Pressed, this, &AMainPlayerController::OnInteractButtonPressed);
}

void AMainPlayerController::HostLocalGame()
{
	FString URL = TEXT("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
	URL.Append("?Name=");
	URL.Append(GetPlayerName());
	// Not sure why but for some reason the value provided in Name option is being properly set as the player name
	// So instead I am using a custom option called Param1 to specify the player name during non seemless travel
	URL.Append("?Param1=");
	URL.Append(GetPlayerName());
	// for sake of simplicity I am using non seemless travel here, however the best practice
	// is to use seemless travels
	GetWorld()->ServerTravel(URL, true);
}

void AMainPlayerController::JoinLocalGame(FString serverAddress)
{
	FString message = "Joining server: ";
	message.Append(serverAddress);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, message);
	FString URL = serverAddress;
	URL.Append("?Name=");
	URL.Append(GetPlayerName());
	URL.Append("?Param1=");
	URL.Append(GetPlayerName());
	ClientTravel(URL, ETravelType::TRAVEL_Absolute);
}

void AMainPlayerController::SetInteractionEnabled(bool enabled)
{
	interactionEnabled = enabled;
}

void AMainPlayerController::OnInteractButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Interact button pressed for player %s"), *GetName());
	FString message = "Interact button pressed: leverTriggered=";
	message.AppendInt(interactionEnabled);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, message);
	if (interactionEnabled) {
		Server_TriggerLever();
	}
}


void AMainPlayerController::Server_TriggerLever_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("TriggerLever has been invoked on server for player %s"), *GetName());

	TArray<AActor*> FoundActors;
	AMainLever* lever;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainLever::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		lever = Cast<AMainLever>(FoundActors[0]);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no actor of AMainDoor class placed in the level"));
		return;
	}

	lever->TriggerLever();
}

void AMainPlayerController::SetPlayerName(const FString& name)
{
	APlayerState* state = GetPlayerState<APlayerState>();
	if (state)
	{
		state->SetPlayerName(name);
	}
}

FString AMainPlayerController::GetPlayerName() const
{
	FString name = "INVALID";
	APlayerState* state = GetPlayerState<APlayerState>();
	if (state)
	{
		name = state->GetPlayerName();
	}
	return name;
}
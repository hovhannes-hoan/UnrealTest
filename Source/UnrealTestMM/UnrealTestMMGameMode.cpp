// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealTestMMGameMode.h"
#include "UnrealTestMMCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "HUD/MainHUD.h"
#include "Kismet/GameplayStatics.h"

AUnrealTestMMGameMode::AUnrealTestMMGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	HUDClass = AMainHUD::StaticClass();
}

FString AUnrealTestMMGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString Name = UGameplayStatics::ParseOption(Options, TEXT("Name"));
	FString Param1 = UGameplayStatics::ParseOption(Options, TEXT("Param1"));
	UE_LOG(LogTemp, Display, TEXT("InitNewPlayer PC=%s, Name=%s, Param1=%s"), *NewPlayerController->GetName(), *Name, *Param1);
	FString result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if (result.IsEmpty()) {
		ChangeName(NewPlayerController, Param1, false);
	}
	return result;
}


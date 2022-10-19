// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealTestMMGameMode.generated.h"

UCLASS(minimalapi)
class AUnrealTestMMGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUnrealTestMMGameMode();

protected:
	// Not sure why but for some reason the value provided in Name option during travel API call 
	// is NOT being properly set as the player name in player state.
	// So instead I am using a custom option called Param1 to provide the player name during travel API calls 
	// This method is overriden to get the value of Param1 option and to set it in player state
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
};




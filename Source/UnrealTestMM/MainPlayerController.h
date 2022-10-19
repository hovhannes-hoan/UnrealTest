// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTESTMM_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Hosts a server game and invokes servertravel
	UFUNCTION(BlueprintCallable)
	void HostLocalGame();

	// Joins a game as a client by invoking clienttravel
	UFUNCTION(BlueprintCallable)
	void JoinLocalGame(FString serverAddress);

	// Represents whether player input (keypresses) should be accepted or ignored
	void SetInteractionEnabled(bool enabled);

	// An API for HUD blueprints to set the player name
	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& name);

	// An API to get the player name
	UFUNCTION(BlueprintCallable)
	FString GetPlayerName() const;
protected:
	virtual void SetupInputComponent() override;

	// RPC method that can be triggered by clients to trigger the lever.
	// Is invoked on server so that the server can replicate the action to other clients
	UFUNCTION(Server, Reliable)
	void Server_TriggerLever();

private:
	// Is invoked then the player presses interaction key (E)
	void OnInteractButtonPressed();

	bool interactionEnabled;
};

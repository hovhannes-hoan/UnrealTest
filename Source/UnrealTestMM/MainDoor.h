// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainDoor.generated.h"

UCLASS()
class UNREALTESTMM_API AMainDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OpenDoor();
	void CloseDoor();

private:
	void OnTriggerCompleted();

	UFUNCTION()
	void OnServerRotationUpdate();

	// Mesh of the door frame. Is a static object and doesn't move when the door it triggered
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* doorFrame;

	// Mesh of the door. Gets rotated through yaw when the door triggered
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* door;

	UPROPERTY(EditAnywhere)
	float doorRotationYaw_Closed = 0.f;
	UPROPERTY(EditAnywhere)
	float doorRotationYaw_FullyOpen = 120.f;

	UPROPERTY(EditAnywhere)
	float doorInteractionSpeed = 2.f;

	UPROPERTY(ReplicatedUsing = OnServerRotationUpdate, Replicated)
	// represents door's current relative rotation on server node.
	FRotator rotationAtServer;

	// represents whether the door has been triggered (by the lever)
	UPROPERTY(Replicated)
	bool bTriggered;

	// represents the current state of the lever, Open or Closed
	bool bDoorOpen;

	// represents door's target relative rotation yaw (based on whether it is going to be Open or get Closed)
	UPROPERTY(Replicated)
	float targetRotationValue;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ShapeComponent.h"
#include "MainLever.generated.h"

// forward declarations
class AMainPlayerController;


UCLASS()
class UNREALTESTMM_API AMainLever : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainLever();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TriggerLever();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:
	// tells the player controller to accept or ignore trigger inputs 
	// based on whether the player is within the interaction area or not.
	void EnableInteraction(AMainPlayerController* PC, bool enable);

	// is invoked once the trigger reaches its target position (ON or OFF).
	// On it's turn this triggers the door opening/closing
	void OnTriggerCompleted();

	// repnotify method which gets called on clients once server replicates the rotationAtServer property
	UFUNCTION()
	void OnRotationUpdateNotify();

	// dummy root component used for offseting other components
	UPROPERTY(EditAnywhere)
	USceneComponent* root;

	// the area overlapping with which the player will be able to trigger the lever
	UPROPERTY(EditAnywhere)
	UShapeComponent* interactionArea;

	// dummy scene component with rotation pitch set to leverBasePitchValue
	UPROPERTY(EditAnywhere)
	USceneComponent* leverAttachmentBaseAngle;

	// component that is being rotated once the player triggers the lever
	UPROPERTY(EditAnywhere)
	USceneComponent* leverAttachmentPoint;

	// mesh of the lever
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* leverMesh;

	// a narrow area around the lever mesh used for collision and blocking the user from
	// getting too close to the lever. This area is expected to be set smaller than the 
	// interaction area, otherwise the player won't be able to trigger the lever
	UPROPERTY(EditAnywhere)
	UShapeComponent* leverCollisionArea;

	UPROPERTY(EditAnywhere)
	float leverBasePitchValue;

	UPROPERTY(EditAnywhere)
	float leverPitchValue_OFFPosition;

	UPROPERTY(EditAnywhere)
	float leverPitchValue_OnPosition;

	UPROPERTY(EditAnywhere)
	float interactionSpeed;

	// represents lever's current relative rotation on server node.
	UPROPERTY(ReplicatedUsing = OnRotationUpdateNotify, Replicated)
	FRotator rotationAtServer;

	// represents whether the player has interacted with the lever
	UPROPERTY(Replicated)
	bool bTriggered;

	// represents the current state of the lever, ON or OFF
	bool bTurnedOn;

	// represents lever's target relative rotation pitch (based on whether it is going to turn ON or OFF)
	UPROPERTY(Replicated)
	float targetRotationValue;
};

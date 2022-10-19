// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTESTMM_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	/** Remove the current menu widget and create a new one from the specified class, if provided. */
	UFUNCTION(BlueprintCallable)
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UFUNCTION(BlueprintGetter)
	UUserWidget* GetCurrentWidget() const;

	UFUNCTION(BlueprintSetter)
	void SetCurrentWidget(UUserWidget* CurrentWidget);

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** The widget class we will use as our menu when the game starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> StartingWidgetClass;

private:
	/** The widget instance that we are using as our menu. */
	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetCurrentWidget, BlueprintSetter = SetCurrentWidget)
	UUserWidget* CurrentWidget;
};

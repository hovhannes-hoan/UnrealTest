// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LabelWidget.generated.h"

/**
 * A widget that is used to draw the player name above the player character
 */
UCLASS()
class UNREALTESTMM_API ULabelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULabelWidget(const FObjectInitializer& ObjectInitializer);

	void SetPlayerName(const FString& name);

protected:
	UPROPERTY(BlueprintReadWrite, meta =(BindWidget))
	UTextBlock* PlayerNameText;
};

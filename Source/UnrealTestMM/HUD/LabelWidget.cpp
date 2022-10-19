// Fill out your copyright notice in the Description page of Project Settings.


#include "LabelWidget.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

ULabelWidget::ULabelWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerNameText = CreateDefaultSubobject<UTextBlock>(TEXT("PlayerNameTextBlock"));
}

void ULabelWidget::SetPlayerName(const FString& name)
{
	if (PlayerNameText) {
		PlayerNameText->SetText(FText::FromString(name));
	}
}
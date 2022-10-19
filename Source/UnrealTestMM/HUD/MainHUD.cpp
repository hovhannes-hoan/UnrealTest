// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "Blueprint/UserWidget.h"


void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void AMainHUD::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		APlayerController* PC = GetOwningPlayerController();
		CurrentWidget = CreateWidget<UUserWidget>(PC, NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

UUserWidget* AMainHUD::GetCurrentWidget() const
{
	return CurrentWidget;
}

void AMainHUD::SetCurrentWidget(UUserWidget* CurrentWidget1)
{
	this->CurrentWidget = CurrentWidget1;
}
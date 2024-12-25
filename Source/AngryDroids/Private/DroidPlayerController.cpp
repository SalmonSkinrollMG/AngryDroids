// Fill out your copyright notice in the Description page of Project Settings.


#include "DroidPlayerController.h"

void ADroidPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(PlayerWidgetClass))
	{
		CreatePlayerwidget();
	}
}

void ADroidPlayerController::CreatePlayerwidget()
{
	if(IsValid(PlayerWidget))
	{
		PlayerWidget->AddToViewport(1);
		return;
	}
	PlayerWidget = CreateWidget<UPlayerWidget>(this , PlayerWidgetClass);
	PlayerWidget->AddToViewport(0);
}

void ADroidPlayerController::UpdateHealth(const float CurrentHealth) const
{
	if(IsValid(PlayerWidget))
	{
		PlayerWidget->SetDamageProgressBar(CurrentHealth);
	}
}

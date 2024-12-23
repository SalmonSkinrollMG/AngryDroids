// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/GameHud.h"

void UGameHud::StartWaveTimer(uint8 MaxTime)
{
	CurrentTime = MaxTime;
	GetWorld()->GetTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&UGameHud::UpdateTimeInHud,
		1.0f,
		true
	);
}

void UGameHud::StopWaveTimer()
{
	if(WaveTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
	}
}

void UGameHud::UpdateTimeInHud()
{
	CurrentTime--;
	WaveTime->SetText(FText::AsNumber(CurrentTime));
}

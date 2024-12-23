// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameHud.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYDROIDS_API UGameHud : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentWave;

public:
	void StartWaveTimer(uint8 MaxTime);
	void StopWaveTimer();

private:
	UFUNCTION()
	void UpdateTimeInHud();

	uint8 CurrentTime{0};

	FTimerHandle WaveTimerHandle; 
};

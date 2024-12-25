// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "GameHud.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYDROIDS_API UGameHud : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentWave;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* HomeButton;



public:
	
	void StartWaveTimer(uint8 MaxTime);
	void StopWaveTimer();

	void SwitchWidgetWithIndex(uint8 index) const;

	void SetCurrentWaveText(uint8 wave) const;

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerWaveWithLoadingTime(float speed);


private:
	UFUNCTION()
	void UpdateTimeInHud();

	uint8 CurrentTime{0};

	FTimerHandle WaveTimerHandle; 
};

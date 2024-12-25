// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawner.h"
#include "HeroDroid.h"
#include "GameFramework/GameMode.h"
#include "Widget/GameHud.h"
#include "DroidGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYDROIDS_API ADroidGameMode : public AGameMode
{
	GENERATED_BODY()

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void SetActivePlayer();

	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	void StartWaveTimer();

public:
	UPROPERTY(EditAnywhere , Category="GameModeSetup")
	TSubclassOf<AEnemySpawner> EnemySpawnerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	UDataTable* WaveDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<UGameHud> GameHudClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float LoadingTime{1};

private:
	UPROPERTY()
	AEnemySpawner* EnemySpawner;

	uint8 CurrentWave = 0;

	uint8 WaveTime = 30;

	TTuple<uint8, uint8> GetDataFromTable(uint8 Wave) const;

	void TriggerNextWave();


	FTimerHandle WaveHandler;
	FTimerHandle LoadingTimeHandle;

	UPROPERTY()
	UGameHud* GameHud;

public:
	void StartGame();
	void EndWave();
	void ClearWaveTimer();
	void ClearLoadingTimer();

	void EndGame();

	UFUNCTION()
	void LoadedNextWave();
	void InitializeNextWave();

	UFUNCTION()
	void StartGameButtonClicked();

	UFUNCTION()
	void ExitGameButtonClicked();

	UFUNCTION()
	void HomeButtonClicked();

	UPROPERTY()
	AHeroDroid* HeroDroid;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawner.h"
#include "GameFramework/GameMode.h"
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
	void StartWaveTimer();

public:
	UPROPERTY(EditAnywhere , Category="GameModeSetup")
	TSubclassOf<AEnemySpawner> EnemySpawnerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	UDataTable* WaveDataTable;

private:
	UPROPERTY()
	AEnemySpawner* EnemySpawner;

	uint8 CurrentWave = 0;

	uint8 WaveTime = 30;

	TTuple<uint8, uint8> GetDataFromTable(uint8 Wave) const;

	void TriggerNextWave();
	void EndWave();

	FTimerHandle WaveHandler;

public:
	void StartGame();
	void ClearWaveTimer();

	void EndGame();
	
	
};

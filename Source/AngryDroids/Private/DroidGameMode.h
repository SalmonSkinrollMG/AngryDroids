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

	virtual void BeginPlay() override;


public:
	UPROPERTY(EditAnywhere , Category="GameModeSetup")
	TSubclassOf<AEnemySpawner> EnemySpawnerClass;

private:
	UPROPERTY()
	AEnemySpawner* EnemySpawner;
};

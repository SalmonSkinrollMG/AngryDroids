// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AngryBot.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class ANGRYDROIDS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	uint8 GetLargestEnemyLevel();
	// Sets default values for this actor's properties
	
	AEnemySpawner();
	
	void StartWave(uint8 WaveCount, uint8 MaximumWeight);
	void SetActivePlayer(AActor* Player);

protected:
	virtual void BeginPlay() override;
	
private:

	FVector GetRandomPointInVolume() const;
	void GetEnemiesBasedOnLevel(uint8 WaveCount, uint8 MaximumWeight ,TArray<uint8>& EnemyLevels);
	void SpawnEnemies(TArray<uint8>& EnemyLevels);

	uint8 MaxEnemyLevel{1};

	UPROPERTY()
	TArray<AAngryBot*> SpawnedEnemies;

	UPROPERTY()
	AActor* ActivePlayer{nullptr};

public:


	UPROPERTY(EditAnywhere, Category = "Spawning")
	UBoxComponent* SpawnVolume;

	/*
	 * Map of Enemy Class and its corresponding Weight
	 */
	UPROPERTY(EditAnywhere, Category = "Spawning") 
	TMap<int , TSubclassOf<AAngryBot>> EnemyMap;

	void CheckForWaveComplete();
	
	UFUNCTION()
	void EnemyDiedEvent(AAngryBot* AngryBot);

	void DestroyALlEnemies();

public:    
	virtual void Tick(float DeltaTime) override;

	
};


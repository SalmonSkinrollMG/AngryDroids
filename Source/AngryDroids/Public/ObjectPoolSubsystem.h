// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class ANGRYDROIDS_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	virtual void Deinitialize() override;
	bool FetchAndRemoveFromInActivePool(TSubclassOf<AActor> ActorClass, AActor*& FetchedActor);
	void SpawnNewActorToPool(TSubclassOf<AActor> ActorClass, AActor*& SpawnedActor);
	void DeactivateActor(AActor* Actor);
	void ActivateActor(AActor* Actor);

	TMap<TSubclassOf<AActor>,TArray<AActor*>> InActivePoolMap;
	
	TMap<TSubclassOf<AActor>,TArray<AActor*>> ActivePoolMap;
	
	TMap<TSubclassOf<AActor>, uint8 > PoolSize;

	

public:
	AActor* GetActorFromPool(TSubclassOf<AActor> ActorClass);
	void ReturnActorToPool(AActor* ReturningActor);
};

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
	
	virtual void Deinitialize() override;

	// Manager Functions for polling
	AActor* GetPooledObject(UWorld* World, TSubclassOf<AActor> ActorClass);
	void ReturnPooledObject(AActor* PooledActor);

	static void DestroyActorsFromMap(TMap<TSubclassOf<AActor>, TArray<AActor*>> ActorMap);

private:
	// Main storage to STore all the pooled actors
	TMap<TSubclassOf<AActor>, TArray<AActor*>> ActivePool;

	TMap<TSubclassOf<AActor>, TArray<AActor*>> InactivePool;


	AActor* CreateNewActor(UWorld* World, TSubclassOf<AActor> ActorClass);
	static void ActivateActor(AActor* Actor , bool bShouldActivate);

};

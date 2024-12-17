// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"


void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UObjectPoolSubsystem::DestroyActorsFromMap(TMap<TSubclassOf<AActor>, TArray<AActor*>> ActorMap)
{
	// Clean up all pooled objects
	for (auto& Pool : ActorMap)
	{
		for (AActor* Actor : Pool.Value)
		{
			if (Actor)
			{
				Actor->Destroy();
			}
		}
	}
}

void UObjectPoolSubsystem::Deinitialize()
{

	DestroyActorsFromMap(InactivePool);
	DestroyActorsFromMap(ActivePool);
	
	InactivePool.Empty();
	ActivePool.Empty();

	UE_LOG(LogTemp, Log, TEXT("Object Pooling Subsystem Deinitialized"));
	Super::Deinitialize();
}

AActor* UObjectPoolSubsystem::CreateNewActor(UWorld* World, TSubclassOf<AActor> ActorClass)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoFail = true;

	AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FTransform::Identity, SpawnParams);
	if (NewActor)
	{
		ReturnPooledObject(NewActor);
	}
	return NewActor;
}

void UObjectPoolSubsystem::ActivateActor(AActor* Actor , bool bShouldActivate)
{
	Actor->SetActorHiddenInGame(!bShouldActivate);
	Actor->SetActorEnableCollision(bShouldActivate); 
	Actor->SetActorTickEnabled(bShouldActivate);
	
}

AActor* UObjectPoolSubsystem::GetPooledObject(UWorld* World, TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass || !World) return nullptr;

	// Check the inactive pool for a free actor
	TArray<AActor*>& InactiveActors = InactivePool.FindOrAdd(ActorClass);

	for (auto& CurrentActor : InactiveActors)
	{
		AActor* Actor = CurrentActor;
		if (Actor && !Actor->IsActorBeingDestroyed() && !Actor->IsPendingKillPending())
		{
			InactiveActors.Remove(CurrentActor);
			
			TArray<AActor*>& ActiveActors = ActivePool.FindOrAdd(ActorClass);
			ActiveActors.Add(Actor);
			
			ActivateActor(Actor, true);

			UE_LOG(LogTemp, Log, TEXT("Activated Pooled Actor"));
			return Actor;
		}
	}

	// If no actor is available, create a new one
	return CreateNewActor(World, ActorClass);
}

void UObjectPoolSubsystem::ReturnPooledObject(AActor* PooledActor)
{
	if (PooledActor)
	{
		TSubclassOf<AActor> ActorClass = PooledActor->GetClass();

		// Deactivate the actor
		ActivateActor(PooledActor, false);

		// Move the actor back to the inactive pool
		TArray<AActor*>& ActiveActors = ActivePool.FindOrAdd(ActorClass);
		TArray<AActor*>& InactiveActors = InactivePool.FindOrAdd(ActorClass);

		if (ActiveActors.Remove(PooledActor)) // Remove from active pool
		{
			InactiveActors.Add(PooledActor); // Add to inactive pool
			UE_LOG(LogTemp, Log, TEXT("Actor returned to inactive pool"));
		}
	}
}


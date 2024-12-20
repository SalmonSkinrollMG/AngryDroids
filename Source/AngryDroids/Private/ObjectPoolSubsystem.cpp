// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"


void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}


void UObjectPoolSubsystem::Deinitialize()
{

	Super::Deinitialize();
}

bool UObjectPoolSubsystem::FetchAndRemoveFromInActivePool(TSubclassOf<AActor> ActorClass, AActor*& FetchedActor)
{
	if(InActivePoolMap.Contains(ActorClass))
	{
		TArray<AActor*>* Inactive_ActorClassArray = InActivePoolMap.Find(ActorClass);
		if(Inactive_ActorClassArray && Inactive_ActorClassArray->Num() > 0)
		{
			FetchedActor = (*Inactive_ActorClassArray)[0];
			Inactive_ActorClassArray->RemoveAt(0);
			TArray<AActor*>& Active_ActorClassArray = ActivePoolMap.FindOrAdd(ActorClass);
			Active_ActorClassArray.Add(FetchedActor);
			ActivateActor(FetchedActor);
			FString ClassName = FetchedActor->GetClass()->GetDefaultObjectName().ToString();
			UE_LOG(LogTemp , Warning , TEXT("[ObjectPool] Fetched actor from inactive pool of class %s"),*ClassName);
			return true;
		}
		FetchedActor = nullptr;
		return false;
	}
	return false;
}

void UObjectPoolSubsystem::SpawnNewActorToPool(TSubclassOf<AActor> ActorClass, AActor*& SpawnedActor)
{
	TArray<AActor*>& Active_ActorClassArray = ActivePoolMap.FindOrAdd(ActorClass);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ActorClass, 
		FVector::ZeroVector, 
		FRotator::ZeroRotator, 
		SpawnParams
	);
	Active_ActorClassArray.Add(SpawnedActor);
	ActivateActor(SpawnedActor); // Not necessary
	FString ClassName = SpawnedActor->GetClass()->GetDefaultObjectName().ToString();
	UE_LOG(LogTemp , Warning , TEXT("[ObjectPool] Spawned New Actor of class %s"),*ClassName);
}

void UObjectPoolSubsystem::DeactivateActor(AActor* Actor)
{

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
}

void UObjectPoolSubsystem::ActivateActor(AActor* Actor)
{
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetActorTickEnabled(true);
}


AActor* UObjectPoolSubsystem::GetActorFromPool(TSubclassOf<AActor> ActorClass)
{
	AActor* PooledActor = nullptr;
	
	if(!IsValid(GetWorld()))
	{
		UE_LOG(LogTemp , Warning , TEXT("[ObjectPool] World is not valid , Couldn't get an actor"));
		return nullptr;
	}
	
	if (FetchAndRemoveFromInActivePool(ActorClass, PooledActor))
	{
		return PooledActor;
	}
	
	SpawnNewActorToPool(ActorClass, PooledActor);
	
	return PooledActor;
}

void UObjectPoolSubsystem::ReturnActorToPool(AActor* ReturningActor)
{
	if (IsValid(ReturningActor))
	{
		TArray<AActor*>* Active_ActorClassArray = ActivePoolMap.Find(ReturningActor->GetClass());
		if (Active_ActorClassArray && Active_ActorClassArray->Num() > 0)
		{
			int32 Index = Active_ActorClassArray->Find(ReturningActor);
			if (Index != INDEX_NONE)
			{
				Active_ActorClassArray->RemoveAt(Index);
				TArray<AActor*>& InactiveArray = InActivePoolMap.FindOrAdd(ReturningActor->GetClass());
				InactiveArray.Add(ReturningActor);
				DeactivateActor(ReturningActor);
				FString ClassName = ReturningActor->GetClass()->GetDefaultObjectName().ToString();
				UE_LOG(LogTemp , Warning , TEXT("[ObjectPool] Returned Actor to the  pool of class %s"),*ClassName);
			}
		}
	}
}

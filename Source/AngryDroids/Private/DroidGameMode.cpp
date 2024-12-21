// Fill out your copyright notice in the Description page of Project Settings.


#include "DroidGameMode.h"

void ADroidGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ADroidGameMode::BeginPlay()
{
	Super::BeginPlay();
	if(EnemySpawnerClass)
	{
		EnemySpawner = GetWorld()->SpawnActor<AEnemySpawner>(EnemySpawnerClass, FVector::ZeroVector, FRotator::ZeroRotator);
		EnemySpawner->StartWave(1 , 5);
	}
}

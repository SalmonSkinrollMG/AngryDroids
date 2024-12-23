// Fill out your copyright notice in the Description page of Project Settings.


#include "DroidGameMode.h"

#include "AudioDeviceManager.h"
#include "HeroDroid.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/WaveInfo.h"

void ADroidGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ADroidGameMode::SetActivePlayer()
{
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , AHeroDroid::StaticClass() , Players);
	if(Players.IsValidIndex(0))
	{
		EnemySpawner->SetActivePlayer(Players[0]);
	}
}

void ADroidGameMode::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(GameHudClass))
	{
		if(IsValid(GameHud))
		{
			GameHud->AddToViewport(0);
			return;
		}
		GameHud = CreateWidget<UGameHud>(GetWorld(), GameHudClass);
		GameHud->AddToViewport(0);
	}
	if(EnemySpawnerClass)
	{
		EnemySpawner = GetWorld()->SpawnActor<AEnemySpawner>(EnemySpawnerClass, FVector::ZeroVector, FRotator::ZeroRotator);
		SetActivePlayer(); // Sends the reference of the heroActor to the Enemy Spawner. All the enemies spawned will have this reference.
		TriggerNextWave();
	}
	
}

void ADroidGameMode::StartWaveTimer()
{
	
	GetWorld()->GetTimerManager().SetTimer(
		WaveHandler,
		this,
		&ADroidGameMode::EndWave,
		WaveTime,
		false
	);
	if(IsValid(GameHud))
	{
		GameHud->StartWaveTimer(WaveTime);
	}
}

TTuple<uint8, uint8> ADroidGameMode::GetDataFromTable(uint8 Wave) const
{
	const FString ContextString(TEXT("Wave Data Context"));
	TArray<FWaveInfo*> AllRows;
	WaveDataTable->GetAllRows(ContextString, AllRows);

	if(AllRows.IsValidIndex(Wave))
	{
		return TTuple<uint8, uint8>(AllRows[Wave]->WaveActorsToSpawn, AllRows[Wave]->WaveWeight);
	}
	return TTuple<uint8, uint8>(0, 0);
}

void ADroidGameMode::TriggerNextWave()
{
	StartWaveTimer();
	TTuple<uint8 , uint8> WaveData = GetDataFromTable(CurrentWave);

	if(EnemySpawner)
	{
		EnemySpawner->StartWave( CurrentWave, WaveData.Get<0>());
	}
	WaveData.Get<0>();
}

void ADroidGameMode::EndWave()
{
	ClearWaveTimer();
	if(IsValid(GameHud))
	{
		GameHud->StopWaveTimer();
	}
}

void ADroidGameMode::StartGame()
{
	
}

void ADroidGameMode::ClearWaveTimer()
{
	if(WaveHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveHandler);
	}
}

void ADroidGameMode::EndGame()
{
	ClearWaveTimer();
}

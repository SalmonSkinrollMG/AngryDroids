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
		HeroDroid = Cast<AHeroDroid>(Players[0]);
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
		GameHud->AddToViewport(1);
	}
	if(GameHud)
	{
		GameHud->StartGameButton->OnClicked.AddDynamic(this , &ADroidGameMode::StartGameButtonClicked);
		GameHud->ExitGameButton->OnClicked.AddDynamic(this , &ADroidGameMode::ExitGameButtonClicked);
		GameHud->HomeButton->OnClicked.AddDynamic(this , &ADroidGameMode::HomeButtonClicked);
	}
	if(EnemySpawnerClass)
	{
		EnemySpawner = GetWorld()->SpawnActor<AEnemySpawner>(EnemySpawnerClass, FVector::ZeroVector, FRotator::ZeroRotator);
		SetActivePlayer(); // Sends the reference of the heroActor to the Enemy Spawner. All the enemies spawned will have this reference.
	}
	
}

void ADroidGameMode::Destroyed()
{
	Super::Destroyed();
	if(IsValid(GameHud))
	{
		GameHud->StartGameButton->OnClicked.RemoveAll(this);
		GameHud->ExitGameButton->OnClicked.RemoveAll(this);
		GameHud->HomeButton->OnClicked.RemoveAll(this);
	}
}

void ADroidGameMode::StartWaveTimer()
{
	
	GetWorld()->GetTimerManager().SetTimer(
		WaveHandler,
		this,
		&ADroidGameMode::EndGame,
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
	if(HeroDroid)
	{
		HeroDroid->ResetCharacterProperties();
	}
	

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
	CurrentWave++;
	InitializeNextWave();
}

void ADroidGameMode::StartGame()
{
	TriggerNextWave();
}

void ADroidGameMode::ClearWaveTimer()
{
	if(WaveHandler.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveHandler);
	}
}

void ADroidGameMode::ClearLoadingTimer()
{
	if(LoadingTimeHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(LoadingTimeHandle);
	}
}

void ADroidGameMode::EndGame()
{
	ClearWaveTimer();
	if(EnemySpawner)
	{
		EnemySpawner->DestroyALlEnemies();
	}
	if(IsValid(GameHud))
	{
		GameHud->StopWaveTimer();
		GameHud->SwitchWidgetWithIndex(2);
		if(HeroDroid)
		{
			HeroDroid->EnableMouse(true);
		}
	}
	
}

void ADroidGameMode::LoadedNextWave()
{
	ClearLoadingTimer();
	GameHud->SwitchWidgetWithIndex(1);
	if(HeroDroid)
	{
		HeroDroid->EnableMouse(false);
	}
}

void ADroidGameMode::InitializeNextWave()
{
	GameHud->SetCurrentWaveText(CurrentWave+1);
	GameHud->SwitchWidgetWithIndex(3);
	GameHud->TriggerWaveWithLoadingTime(1 / LoadingTime);
	TriggerNextWave();
	GetWorld()->GetTimerManager().SetTimer(
		LoadingTimeHandle,
		this,
		&ADroidGameMode::LoadedNextWave,
		LoadingTime,
		false
	);
}

void ADroidGameMode::StartGameButtonClicked()
{
	InitializeNextWave();
}

void ADroidGameMode::ExitGameButtonClicked()
{
	if(IsValid(HeroDroid))
	{
		HeroDroid->QuitGame();
	}
}

void ADroidGameMode::HomeButtonClicked()
{
	GameHud->SwitchWidgetWithIndex(0);
}

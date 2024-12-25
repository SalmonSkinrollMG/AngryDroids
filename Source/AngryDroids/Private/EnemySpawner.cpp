// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "DroidGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEnemySpawner::AEnemySpawner()
{

	PrimaryActorTick.bCanEverTick = true;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Volume"));
	MaxEnemyLevel = GetLargestEnemyLevel();;

}

uint8 AEnemySpawner::GetLargestEnemyLevel()
{
	uint8 MaxLevel = 0;
	for (const TPair<int, TSubclassOf<AAngryBot>>& SubClass : EnemyMap)
	{
		if (SubClass.Key > MaxLevel)
		{
			MaxLevel = SubClass.Key;
		}
	}
	return MaxLevel;
}


// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}


void AEnemySpawner::StartWave(uint8 WaveCount, uint8 MaximumWeight)
{
	TArray<uint8> EnemyLevels;
	GetEnemiesBasedOnLevel(WaveCount , MaximumWeight , EnemyLevels);
	SpawnEnemies(EnemyLevels);
}

void AEnemySpawner::SetActivePlayer(AActor* Player)
{
	ActivePlayer = Player;
}


FVector AEnemySpawner::GetRandomPointInVolume() const
{
	if (!SpawnVolume) return FVector::ZeroVector;

	const FVector Origin = SpawnVolume->GetComponentLocation();;
	const FVector BoxExtent = SpawnVolume->GetScaledBoxExtent();

	// Random point within the box
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);
	return RandomPoint;
}

void AEnemySpawner::GetEnemiesBasedOnLevel(uint8 WaveCount, uint8 MaximumWeight, TArray<uint8>& EnemyLevels)
{
	uint8 CurrentWeight = MaximumWeight;

	//Max level allowed on this wave
	uint8 MaxLevel = FMath::Clamp(WaveCount, 1,3);

	/*
	 * Start assigning levels from the highest possible level (MaxLevel).
	 * Subtract the weight used for each level from CurrentWeight.
	 */
	while (CurrentWeight > 0)
	{

		uint8 LevelToAdd = FMath::Min(MaxLevel, CurrentWeight);
		EnemyLevels.Add(LevelToAdd);
		CurrentWeight -= LevelToAdd;
	}


	/*
	 * Adjust distribution for optimal diversity
	 * For example, [3, 3, 1] may be adjusted to [3, 2, 2] for diversity.
	 */
	for (uint8 i = 0; i < EnemyLevels.Num() - 1; ++i)
	{
		if (EnemyLevels[i] > EnemyLevels[i + 1] + 1)
		{
			EnemyLevels[i]--;
			EnemyLevels[i + 1]++;
		}
	}
	
}

void AEnemySpawner::SpawnEnemies(TArray<uint8>& EnemyLevels)
{
	for (uint8 Level : EnemyLevels)
	{
		FVector SpawnLocation = GetRandomPointInVolume(); // Get a random spawn point

		AAngryBot* SpawnedEnemy = GetWorld()->SpawnActor<AAngryBot>(*EnemyMap.Find(Level), SpawnLocation, FRotator::ZeroRotator);
		SpawnedEnemy->AssignAndActivateEnemy(ActivePlayer);
		SpawnedEnemy->OnAngryBotDied.AddDynamic(this , &AEnemySpawner::EnemyDiedEvent);
		SpawnedEnemies.Add(SpawnedEnemy);
		
	}
}

void AEnemySpawner::CheckForWaveComplete()
{
	if(SpawnedEnemies.Num() == 0)
	{
		ADroidGameMode* DroidGameMode = Cast<ADroidGameMode>(GetWorld()->GetAuthGameMode());
		DroidGameMode->EndWave();
	}
}

void AEnemySpawner::EnemyDiedEvent(AAngryBot* AngryBot)
{
	for(uint8 i = 0 ; i<SpawnedEnemies.Num() ; i++)
	{
		if(SpawnedEnemies[i] == AngryBot)
		{
			SpawnedEnemies.RemoveAt(i);
			CheckForWaveComplete();
			AngryBot->Destroy();
		}
	}
}

void AEnemySpawner::DestroyALlEnemies()
{
	if(SpawnedEnemies.Num() > 0)
	{
		for (AAngryBot* AngryBot : SpawnedEnemies)
		{
			if (AngryBot && !AngryBot->IsPendingKillPending())
			{
				AngryBot->Destroy();
			}
		}

		// Clear the array after destroying the actors
		SpawnedEnemies.Empty();
	}
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaveInfo.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FWaveInfo : public FTableRowBase
{
	GENERATED_BODY()

	// Wave number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	uint8 WaveNumber;

	// Actors to spawn during the wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	uint8 WaveActorsToSpawn;

	// Total weight or difficulty of the wave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	uint8 WaveWeight;

	FWaveInfo()
		: WaveNumber(0), WaveActorsToSpawn(0),WaveWeight(0) {}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroidBullets.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "AngryBot.generated.h"

UCLASS()
class ANGRYDROIDS_API AAngryBot : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAngryBot();

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RotateTowardsPlayer(float DeltaTime);
	void SpawnProjectile(FTransform SpawnTransform, FActorSpawnParameters SpawnInfo);
	void FireBullets();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
	UPROPERTY()
	AActor* Player;

	UFUNCTION()
	void StartTimer();

public:
	bool bFlipFlop{false};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	float InterpolationSpeed;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	FName RightFirearmName{"None"};
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	FName LeftFirearmName{"None"};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	TSubclassOf<ADroidBullets> BulletClass;
	
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bAllowFire{true};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bAlteredFire{true};

	/*
	 * The Rate at which the bullets are fired in seconds.
	 * if bAlteredfire is disabled , the interval is applied at the same time for both.
	 */
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	float FireInterval{1.0}; 
};

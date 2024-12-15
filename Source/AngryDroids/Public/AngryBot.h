// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroidBullets.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DamageInterface.h"
#include "AngryBot.generated.h"

UCLASS()
class ANGRYDROIDS_API AAngryBot : public AActor,public IDamageInterface
{
	GENERATED_BODY()

public:

	AAngryBot();
	
	bool CheckForExceptionsInFire() const;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

protected:
	
	virtual void BeginPlay() override;
	void RotateTowardsPlayer(float DeltaTime);
	void SpawnNiagaraSystem(const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn) const;
	void PlaySound(const FTransform& SpawnTransform, USoundBase* SoundToPlay) const;
	void SpawnProjectile(const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnInfo);
	void FireBullets();
	void SelfDestruct();

public:

	virtual void Tick(float DeltaTime) override;

	virtual void ApplyDamage(float Damage , AActor* DamageCause) override;

private:
	
	UPROPERTY()
	AActor* Player;

	UFUNCTION()
	void StartTimerForFire();

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

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	UNiagaraSystem* FireFX;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	UNiagaraSystem* DestructionFX;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Bot Value")
	USoundBase* FireSound;
	
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bAllowFire{true};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bAlteredFire{true};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	float MaxHealth{100.0f};
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	float FireInterval{1.0}; 

private:
	float CurrentHealth{0.0f};

	/*
	 * The Rate at which the bullets are fired in seconds.
	 * if bAlteredfire is disabled , the interval is applied at the same time for both.
	 */
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroidBullets.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "HeroDroid.generated.h"

UCLASS()
class ANGRYDROIDS_API AHeroDroid : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHeroDroid();
	bool CheckForExceptionsInFire() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY()
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere)
	UFloatingPawnMovement* FloatingPawnMovement;

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SpawnNiagaraSystemAtLocation( const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn);
	void PlaySound(const FTransform& SpawnTransform, USoundBase* SoundToPlay) const;
	void SpawnProjectileAtLocation(const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnInfo);
	void TriggerFire();
	void Fire(const FInputActionValue& Value);

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireButton;

	bool bFlipFlop{false};
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Initialization Hero")
	FName RightFirearmName{"None"};
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Initialization Hero")
	FName LeftFirearmName{"None"};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Initialization Hero")
	TSubclassOf<ADroidBullets> BulletClass;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Initialization Hero")
	UNiagaraSystem* FireFX;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Initialization Hero")
	UNiagaraSystem* DestructionFX;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Initialization Hero")
	USoundBase* FireSound;
	
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bAllowFire{false};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bAlteredFire{true};

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	float MaxHealth{100.0f};
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	float FireInterval{1.0}; 


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	float CurrentHealth{0.0f};
};

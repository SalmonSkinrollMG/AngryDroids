// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DroidBullets.h"
#include "DroidPlayerController.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Interfaces/DamageInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "HeroDroid.generated.h"

UCLASS()
class ANGRYDROIDS_API AHeroDroid : public APawn,public IDamageInterface
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
	

	UPROPERTY(EditAnywhere)
	UFloatingPawnMovement* FloatingPawnMovement;

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SpawnNiagaraSystemAtLocation( const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn);
	void PlaySound(const FTransform& SpawnTransform, USoundBase* SoundToPlay) const;
	bool SpawnProjectileAtLocationFromPool(FTransform SpawnPosition);
	void SpawnProjectileAtLocation(const FTransform& SpawnTransform);
	void TriggerFire();
	void Fire(const FInputActionValue& Value);
	bool CalculateDirectionFromHit(FHitResult HitResult, bool bHit, FVector& Value1);
	FVector GetTargetAndDirection();
	void QuitGame();
	void EnableMouse(bool bEnable) const;
	void ResetCharacterProperties();

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
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	float TraceDistance{1000.0f};

//Debug variables
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="BotConfig")
	bool bEnableFireTrace{false};
	

	virtual void ApplyDamage(float Damage, AActor* DamageCauser) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	float CurrentHealth{0.0f};

	UPROPERTY()
	ADroidPlayerController* OwningController;

	
};


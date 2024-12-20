// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DroidBullets.generated.h"

UCLASS()
class ANGRYDROIDS_API ADroidBullets : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADroidBullets();
	void ReturnToPool();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere ,BlueprintReadWrite)
	UProjectileMovementComponent* ProjectileComponent;

	
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category="Hit Particles")
	UNiagaraSystem* HitFX;

protected:
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ActivateBullet();

	UPROPERTY(EditAnywhere, Blueprintable , Category = "BulletsProperties")
	float BulletDamage{10.0f};


	UPROPERTY(EditAnywhere, Blueprintable , Category = "BulletsProperties")
	float BulletLifeSpan{3.0};

	UPROPERTY(EditAnywhere, Blueprintable , Category = "BulletsProperties")
	float BulletSpeed{3000.0};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UFUNCTION()
	void ApplyDamageToActor(AActor* OtherActor);
	void SpawnNiagaraSystem(const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn) const;

	void EndBulletLife();
	void ClearBulletLifeHandle();

private:
	FTimerHandle BulletLifeHandle; 
};

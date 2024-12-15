// Fill out your copyright notice in the Description page of Project Settings.


#include "AngryDroids/Public/DroidBullets.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Interfaces/DamageInterface.h"


// Sets default values
ADroidBullets::ADroidBullets()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	RootComponent = StaticMesh;
	
	ProjectileComponent =  CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
}


void ADroidBullets::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ApplyDamageToActor(OtherActor);
	FTransform HitTransform;
	HitTransform.SetLocation(Hit.Location);
	HitTransform.SetRotation(FQuat::Identity);
	SpawnNiagaraSystem(HitTransform,HitFX);
	Destroy();
}


void ADroidBullets::BeginPlay()
{
	Super::BeginPlay();
	StaticMesh->OnComponentHit.AddDynamic(this, &ADroidBullets::OnComponentHit);
}

void ADroidBullets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADroidBullets::ApplyDamageToActor(AActor* OtherActor)
{
	if(OtherActor && OtherActor-> Implements<UDamageInterface>())
	{
		if (IDamageInterface* DamageableActor = Cast<IDamageInterface>(OtherActor))
		{
			DamageableActor->ApplyDamage(BulletDamage, this);
		}
	}
}

void ADroidBullets::SpawnNiagaraSystem(const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn) const
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EffectToSpawn, SpawnTransform.GetTranslation(), SpawnTransform.GetRotation().Rotator());
}


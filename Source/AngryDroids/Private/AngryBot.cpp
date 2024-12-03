// Fill out your copyright notice in the Description page of Project Settings.

#include "AngryDroids/Public/AngryBot.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AAngryBot::AAngryBot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	
	SphereComponent->SetupAttachment(Root);
	StaticMesh->SetupAttachment(Root);
}

void AAngryBot::ExceptionHandlingForFire()
{
	if(!IsValid(FireFX))
	{
		bAllowFire = false;
		UE_LOG(LogTemp , Warning , TEXT("Some important attributes in the class config is not assigned -- FireFX "));
	}
	if(!IsValid(FireSound))
	{
		bAllowFire = false;
		UE_LOG(LogTemp , Warning , TEXT("Some important attributes in the class config is not assigned  -- FireSound"));
	}
	
}

// Called when the game starts or when spawned
void AAngryBot::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , APawn::StaticClass() , Players);
	Player = Players[0];

	if(IsValid(BulletClass))
	{
		StartTimer();
	}
	ExceptionHandlingForFire();
}

void AAngryBot::RotateTowardsPlayer(float DeltaTime)
{
	if (Player)
	{
		FVector TargetLocation = Player->GetActorLocation();
		const FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() , TargetLocation);
		const FRotator InterpolatedRotation = FMath::RInterpTo(GetActorRotation() , DesiredRotation, DeltaTime, InterpolationSpeed);
		SetActorRotation(InterpolatedRotation);
	}
}

void AAngryBot::SpawnNiagaraSystem(FTransform SpawnTransform, UNiagaraSystem* EffectToSpawn) const
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EffectToSpawn, SpawnTransform.GetTranslation(), SpawnTransform.GetRotation().Rotator());
}

void AAngryBot::PlaySound(FTransform SpawnTransform, USoundBase* SoundToPlay)
{
	UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, SpawnTransform.GetTranslation());
}

void AAngryBot::SpawnProjectile(FTransform SpawnTransform, const FActorSpawnParameters SpawnInfo)
{
	GetWorld()->SpawnActor<ADroidBullets>(BulletClass, SpawnTransform.GetTranslation(),SpawnTransform.GetRotation().Rotator() , SpawnInfo);

	SpawnNiagaraSystem(SpawnTransform, FireFX);
	
	PlaySound(SpawnTransform, FireSound);
	
	bFlipFlop = !bFlipFlop;
}

void AAngryBot::FireBullets()
{
	if(bAllowFire)
	{
		FTransform SpawnTransform = FTransform::Identity;
		SpawnTransform.SetTranslation(FVector(1, 1, 1));
		SpawnTransform =  bFlipFlop ? StaticMesh->GetSocketTransform(RightFirearmName) : StaticMesh->GetSocketTransform(LeftFirearmName);

		const FActorSpawnParameters SpawnInfo;
	
		SpawnProjectile(SpawnTransform, SpawnInfo);
	
		if(!bAlteredFire)
		{
			SpawnTransform =  bFlipFlop ? StaticMesh->GetSocketTransform(RightFirearmName) : StaticMesh->GetSocketTransform(LeftFirearmName);
			SpawnProjectile(SpawnTransform , SpawnInfo);
		}
	}
}

// Called every frame
void AAngryBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateTowardsPlayer(DeltaTime);
	
}

void AAngryBot::StartTimer()
{
	FTimerHandle TimerHandle; 

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&AAngryBot::FireBullets,
		FireInterval,
		true
	);
}


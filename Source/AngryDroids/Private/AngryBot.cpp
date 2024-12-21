// Fill out your copyright notice in the Description page of Project Settings.

#include "AngryDroids/Public/AngryBot.h"

#include "ObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/MapErrors.h"


AAngryBot::AAngryBot()
{
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(StaticMesh);
	
	FollowingPlayerLocation = FVector::ZeroVector;
	CurrentHealth  = MaxHealth;
}

void AAngryBot::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , APawn::StaticClass() , Players);
	if(Players.IsValidIndex(0))
	{
		PlayerToFollow = Players[0];
		FollowingPlayerLocation = PlayerToFollow->GetActorLocation();
		StartTimerToMove();
	}
	if(CheckForExceptionsInFire())
	{
		StartTimerForFire();
	}
}

bool AAngryBot::CheckForExceptionsInFire() const
{
	bool bSafetoFire{true};
	if(!IsValid(BulletClass))
	{
		bSafetoFire = false;
		UE_LOG(LogTemp , Warning , TEXT("[ExceptionForFire] Some important attributes in the class config is not assigned -- BulletClass "));
	}
	if(!IsValid(FireFX))
	{
		bSafetoFire = false;
		UE_LOG(LogTemp , Warning , TEXT("[ExceptionForFire] Some important attributes in the class config is not assigned -- FireFX "));
	}
	if(!IsValid(FireSound))
	{
		bSafetoFire = false;
		UE_LOG(LogTemp , Warning , TEXT("[ExceptionForFire] Some important attributes in the class config is not assigned  -- FireSound"));
	}
	if(!IsValid(DestructionFX))
	{
		bSafetoFire = false;
		UE_LOG(LogTemp , Warning , TEXT("[ExceptionForFire] Some important attributes in the class config is not assigned  -- FireSound"));
	}
	return bSafetoFire;
}

void AAngryBot::StartTimerForFire()
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

void AAngryBot::StartTimerToMove()
{
	FTimerHandle TimerHandle; 

	GetWorld()->GetTimerManager().SetTimer(
		   TimerHandle,
		   [this]() 
		   {
			   FollowingPlayerLocation = PlayerToFollow->GetActorLocation();
		   },
		   UpdatePlayerLocationFrequency ,
		   true
		   );
		   
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

void AAngryBot::SelfDestruct()
{
	SpawnNiagaraSystem(GetActorTransform(),DestructionFX);
	GetWorld()->DestroyActor(this);
}

void AAngryBot::SpawnProjectileFromPool(const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnInfo)
{
	UWorld* World = GetWorld();
	if (!World || !BulletClass) return ;

	if (UObjectPoolSubsystem* PoolingSubsystem = World->GetGameInstance()->GetSubsystem<UObjectPoolSubsystem>())
	{
		if (AActor* PooledActor = PoolingSubsystem->GetActorFromPool(BulletClass))
		{
			// Move the actor to a desired location and reactivate
			PooledActor->SetActorLocation(SpawnTransform.GetTranslation());
			PooledActor->SetActorRotation(SpawnTransform.GetRotation().Rotator());
			PooledActor->SetActorHiddenInGame(false);
			ADroidBullets* Bullet  = Cast<ADroidBullets>(PooledActor);
			if (Bullet)
			{
				FVector ShootDirection = GetActorForwardVector();
				Bullet->ProjectileComponent->Velocity = ShootDirection * Bullet->BulletSpeed;
				Bullet->ActivateBullet();
			}
			UE_LOG(LogTemp, Log, TEXT("Spawned Pooled Object for enimies."));
		}
	}
}

void AAngryBot::MoveTowardPlayer(FVector Destination, float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, Destination, DeltaTime, MoveInterpSpeed);
	
	SetActorLocation(NewLocation , true);
}

void AAngryBot::SpawnProjectile(const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnInfo)
{
	SpawnProjectileFromPool(SpawnTransform, SpawnInfo);

	SpawnNiagaraSystem(SpawnTransform, FireFX);
	
	PlaySound(SpawnTransform, FireSound);
	
	bFlipFlop = !bFlipFlop;
}


void AAngryBot::SpawnNiagaraSystem(const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn) const
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EffectToSpawn, SpawnTransform.GetTranslation(), SpawnTransform.GetRotation().Rotator());
}

void AAngryBot::PlaySound(const FTransform& SpawnTransform, USoundBase* SoundToPlay) const
{
	UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, SpawnTransform.GetTranslation());
}

void AAngryBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateTowardsPlayer(DeltaTime);
	MoveTowardPlayer(FollowingPlayerLocation , DeltaTime);
}

void AAngryBot::ApplyDamage(float Damage, AActor* DamageCause)
{
	CurrentHealth = CurrentHealth - Damage;
	if(CurrentHealth <= 0)
	{
		SpawnNiagaraSystem(GetActorTransform(), DestructionFX);
		Destroy();
	}
}

void AAngryBot::RotateTowardsPlayer(float DeltaTime)
{
	if (PlayerToFollow)
	{
		FVector TargetLocation = PlayerToFollow->GetActorLocation();
		const FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() , TargetLocation);
		const FRotator InterpolatedRotation = FMath::RInterpTo(GetActorRotation() , DesiredRotation, DeltaTime, InterpolationSpeed);
		SetActorRotation(InterpolatedRotation);
	}
}

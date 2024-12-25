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

void AAngryBot::AssignAndActivateEnemy(AActor* PlayerRef)
{
	PlayerToFollow = PlayerRef;
	FollowingPlayerLocation = PlayerToFollow->GetActorLocation();
	StartTimerToMove();

	if(CheckForExceptionsInFire())
	{
		StartTimerForFire();
	}
}

void AAngryBot::StartTimerForFire()
{
	FTimerHandle TimerHandle; 

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&AAngryBot::FireBullets,
		FireInterval,
		true,
		ActivationDelay
	);
}

void AAngryBot::ClearTimerForMove()
{
	if(MoveTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
	}
}

void AAngryBot::StartTimerToMove()
{

	GetWorld()->GetTimerManager().SetTimer(
		   MoveTimerHandle,
		   [this]() 
		   {
		   		FollowingPlayerLocation = CalculateRandomPositionAroundActor(PlayerToFollow);
		   },
		   UpdatePlayerLocationFrequency ,
		   true,
		   ActivationDelay
		   );
		   
}
FVector AAngryBot::CalculateRandomPositionAroundActor(AActor* Actor)
{
	FVector ActorPosition = Actor->GetActorLocation();
	float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * EffectiveFireRange,
							  FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * EffectiveFireRange,
							  0.0f);
	return ActorPosition + Offset;
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

auto AAngryBot::MoveTowardPlayer(FVector Destination, float DeltaTime) -> void
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
		ClearTimerForMove();
		if(OnAngryBotDied.IsBound())
		{
			OnAngryBotDied.Broadcast(this);
		}
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

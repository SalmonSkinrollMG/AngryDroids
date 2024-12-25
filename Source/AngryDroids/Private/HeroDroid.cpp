// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroDroid.h"
#include "AngryBot.h"
#include "DroidGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraFunctionLibrary.h"
#include "ObjectPoolSubsystem.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AHeroDroid::AHeroDroid()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create the StaticMeshComponent and set it as the root component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	// Create the SphereComponent and attach it to the StaticMesh (child)
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(StaticMesh);

	// Create the FloatingPawnMovement component
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	
	CurrentHealth  = MaxHealth;
	if(CheckForExceptionsInFire())
	{
		bAllowFire = true;
	}
}

bool AHeroDroid::CheckForExceptionsInFire() const
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

// Called when the game starts or when spawned
void AHeroDroid::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get the Enhanced Input Subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add the default input mapping context
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	OwningController = Cast<ADroidPlayerController>(GetController());
	
}

// Called every frame
void AHeroDroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHeroDroid::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void AHeroDroid::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AHeroDroid::SpawnNiagaraSystemAtLocation(const FTransform& SpawnTransform, UNiagaraSystem* EffectToSpawn)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EffectToSpawn, SpawnTransform.GetTranslation(), SpawnTransform.GetRotation().Rotator());
}

void AHeroDroid::PlaySound(const FTransform& SpawnTransform, USoundBase* SoundToPlay) const
{
	UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, SpawnTransform.GetTranslation());
}

bool AHeroDroid::SpawnProjectileAtLocationFromPool(FTransform SpawnTransform)
{
	UWorld* World = GetWorld();
	if (!World || !BulletClass) return false;

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
				FVector ShootDirection = GetTargetAndDirection();
				Bullet->ProjectileComponent->Velocity = ShootDirection * Bullet->BulletSpeed;
				Bullet->ActivateBullet();
			}
			UE_LOG(LogTemp, Log, TEXT("Spawned Pooled Object"));
		}
	}
	return true;
}

void AHeroDroid::SpawnProjectileAtLocation(const FTransform& SpawnTransform)
{
	if (!SpawnProjectileAtLocationFromPool(SpawnTransform))
	{
		return;
	}
	
	SpawnNiagaraSystemAtLocation(SpawnTransform, FireFX);
	
	PlaySound(SpawnTransform, FireSound);
	
	bFlipFlop = !bFlipFlop;
}


void AHeroDroid::TriggerFire()
{
	if(bAllowFire)
	{
		FTransform SpawnTransform = FTransform::Identity;
		SpawnTransform.SetTranslation(FVector(1, 1, 1));
		SpawnTransform =  bFlipFlop ? StaticMesh->GetSocketTransform(RightFirearmName) : StaticMesh->GetSocketTransform(LeftFirearmName);

	
		SpawnProjectileAtLocation(SpawnTransform);
	
		if(!bAlteredFire)
		{
			SpawnTransform =  bFlipFlop ? StaticMesh->GetSocketTransform(RightFirearmName) : StaticMesh->GetSocketTransform(LeftFirearmName);
			SpawnProjectileAtLocation(SpawnTransform);
		}
	}
}

void AHeroDroid::Fire(const FInputActionValue& Value)
{
	TriggerFire();
}

bool AHeroDroid::CalculateDirectionFromHit(FHitResult HitResult, bool bHit, FVector& Value1)
{
	if (bHit && HitResult.GetActor())
	{
		FVector HitActorLocation = HitResult.GetActor()->GetActorLocation();
		FVector PawnLocation = GetActorLocation();

		// Calculate the direction vector (Target - Pawn)
		FVector Direction = HitActorLocation - PawnLocation;

		// Normalize the direction vector
		Direction.Normalize();

		Value1 = Direction;
		return true;
	}
	return false;
}

FVector AHeroDroid::GetTargetAndDirection()
{
	FVector TargetDirection = GetActorForwardVector();
	if (!OwningController)
	{
		return TargetDirection;
	}

	//View Point of the player
	FVector ViewLocation;
	FRotator ViewRotation;
	OwningController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// Start location + ForwardVector from rotation + Distance
	FVector TraceEnd = ViewLocation + (ViewRotation.Vector() * TraceDistance);
	
	FHitResult HitResult;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ViewLocation,
		TraceEnd,
		ECC_GameTraceChannel1
	);

	if(bEnableFireTrace)
	{
		DrawDebugLine(
		GetWorld(),
		ViewLocation,
		TraceEnd,
		bHit ? FColor::Green : FColor::Red,
		false,
		1.0f,
		0, 
		1.0f);
	}
	

	CalculateDirectionFromHit(HitResult, bHit, TargetDirection);
	return TargetDirection;
}

void AHeroDroid::QuitGame()
{
	if (OwningController)
	{
		UKismetSystemLibrary::QuitGame(OwningController, nullptr, EQuitPreference::Quit, true);
	}
}

void AHeroDroid::EnableMouse(bool bEnable) const
{
	if (IsValid(OwningController))
	{
		// Show or hide the mouse cursor
		OwningController->SetShowMouseCursor(bEnable);

		if (!bEnable)
		{
			// Set input mode to game only to focus the game viewport
			FInputModeGameOnly InputMode;
			OwningController->SetInputMode(InputMode);
		}
		else
		{
			// Set input mode to game and UI if mouse cursor is enabled
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			OwningController->SetInputMode(InputMode);
		}
	}
}

void AHeroDroid::ResetCharacterProperties()
{
	CurrentHealth = 100;
	OwningController->UpdateHealth(CurrentHealth);
}


void AHeroDroid::ApplyDamage(float Damage, AActor* DamageCauser)
{
	CurrentHealth = CurrentHealth - Damage;
	OwningController->UpdateHealth(CurrentHealth);
	if(CurrentHealth <= 0)
	{
		SpawnNiagaraSystemAtLocation(GetActorTransform(), DestructionFX);
		ADroidGameMode* GameMode = Cast<ADroidGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->EndGame();
	}
}

// Called to bind functionality to input
void AHeroDroid::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHeroDroid::Move);
		
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHeroDroid::Look);

		EnhancedInputComponent->BindAction(FireButton, ETriggerEvent::Triggered, this, &AHeroDroid::Fire);
		
	}
}


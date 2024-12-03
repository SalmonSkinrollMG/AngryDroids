// Fill out your copyright notice in the Description page of Project Settings.


#include "AngryDroids/Public/DroidBullets.h"


// Sets default values
ADroidBullets::ADroidBullets()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMesh->SetupAttachment(Root);
	
	ProjectileComponent =  CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
}

// Called when the game starts or when spawned
void ADroidBullets::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADroidBullets::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


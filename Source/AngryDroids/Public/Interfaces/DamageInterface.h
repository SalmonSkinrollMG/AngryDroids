// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

class ANGRYDROIDS_API IDamageInterface
{
	GENERATED_BODY()

public:
	virtual void ApplyDamage(float Damage , AActor* DamageCauser)=0;
};

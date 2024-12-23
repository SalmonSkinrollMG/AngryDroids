// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYDROIDS_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	UPROPERTY(meta = (BindWidget))
	UImage* HealthImage;


private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;
public:
	void SetDamageProgressBar(float Value) const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Widget/PlayerWidget.h"
#include "DroidPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYDROIDS_API ADroidPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	void CreatePlayerwidget();

	void UpdateHealth(float CurrentHealth) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UPlayerWidget> PlayerWidgetClass;
private:
	UPROPERTY()
	UPlayerWidget* PlayerWidget;
	
};

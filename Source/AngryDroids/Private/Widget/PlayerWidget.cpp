// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerWidget.h"
#include "Materials/MaterialInstanceDynamic.h"


void UPlayerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!HealthImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProgressImage is null!"));
		return;
	}

	// Try to get the current Material Instance Dynamic from the ProgressImage
	FSlateBrush Brush = HealthImage->GetBrush();
	DynamicMaterialInstance = Cast<UMaterialInstanceDynamic>(Brush.GetResourceObject());
	if (!DynamicMaterialInstance)
	{
		
		// If it's still not a Dynamic Material, create one from the brush's material
		if (UMaterialInterface* Material = Cast<UMaterialInterface>(Brush.GetResourceObject()))
		{
			DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);

			Brush.SetResourceObject(DynamicMaterialInstance);

			HealthImage->SetBrush(Brush);
		}
	}
}

void UPlayerWidget::SetDamageProgressBar(float Value) const
{
	if(IsValid(DynamicMaterialInstance))
	{
		DynamicMaterialInstance->SetScalarParameterValue(FName("Progress"), Value);
	}
}

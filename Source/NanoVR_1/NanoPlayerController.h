// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "XRayComponent.h"
#include <vector>
#include "NanoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NANOVR_1_API ANanoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANanoPlayerController(const FObjectInitializer& ObjectInitializer);

	void Rotate(float value);

	const FName VerticalRotationAxis();

	const UInputComponent* GetInputComponent();
	
protected:
	virtual void SetupInputComponent() override;
	
	UInputComponent* InputComponent;
};

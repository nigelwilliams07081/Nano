// Fill out your copyright notice in the Description page of Project Settings.

#include "NanoPlayerController.h"

ANanoPlayerController::ANanoPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ANanoPlayerController::Rotate(float value)
{
	
}

const FName ANanoPlayerController::VerticalRotationAxis()
{
	return FName("LookUp");
}

const UInputComponent * ANanoPlayerController::GetInputComponent()
{
	return InputComponent;
}

void ANanoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	if (InputComponent != NULL)
	{
		InputComponent->BindAxis(VerticalRotationAxis(), this, &ANanoPlayerController::Rotate);
	}
}


